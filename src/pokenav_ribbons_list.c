#include "global.h"
#include "constants/rgb.h"
#include "fork_run.h"
#include "region_map.h"
#include "constants/region_map_sections.h"
#include "pokenav.h"
#include "bg.h"
#include "menu.h"
#include "palette.h"
#include "window.h"
#include "sound.h"
#include "string_util.h"
#include "international_string_util.h"
#include "constants/songs.h"

enum
{
    RIBBONS_MON_LIST_FUNC_NONE,
    RIBBONS_MON_LIST_FUNC_MOVE_UP,
    RIBBONS_MON_LIST_FUNC_MOVE_DOWN,
    RIBBONS_MON_LIST_FUNC_PAGE_UP,
    RIBBONS_MON_LIST_FUNC_PAGE_DOWN,
    RIBBONS_MON_LIST_FUNC_EXIT,
    RIBBONS_MON_LIST_FUNC_OPEN_RIBBONS_SUMMARY
};


struct Pokenav_RibbonsMonList
{
    u32 (*callback)(struct Pokenav_RibbonsMonList *);
    u32 loopedTaskId;
    u16 winid;
    s32 boxId;
    s32 monId;
    u32 changeBgs;
    u32 saveMonList;
    struct PokenavMonList *monList;
};

struct Pokenav_RibbonsMonMenu
{
    bool32 (*callback)(void);
    u32 loopedTaskId;
    u16 winid;
    bool32 fromSummary;
    u8 buff[BG_SCREEN_SIZE];
};

static u32 HandleRibbonsMonListInput_WaitListInit(struct Pokenav_RibbonsMonList *);
static u32 HandleRibbonsMonListInput(struct Pokenav_RibbonsMonList *);
static u32 RibbonsMonMenu_ReturnToMainMenu(struct Pokenav_RibbonsMonList *);
static u32 BuildPartyMonRibbonList(s32);
static u32 InitBoxMonRibbonList(s32);
static u32 BuildBoxMonRibbonList(s32);
static u32 GetMonRibbonListLoopTaskFunc(s32);
static void InsertMonListItem(struct Pokenav_RibbonsMonList *, struct PokenavMonListItem *);
static u32 LoopedTask_OpenRibbonsMonList(s32);
static bool32 GetRibbonsMonCurrentLoopedTaskActive(void);
static u32 LoopedTask_RibbonsListMoveCursorUp(s32);
static u32 LoopedTask_RibbonsListMoveCursorDown(s32);
static u32 LoopedTask_RibbonsListMovePageUp(s32);
static u32 LoopedTask_RibbonsListMovePageDown(s32);
static u32 LoopedTask_RibbonsListReturnToMainMenu(s32);
static u32 LoopedTask_RibbonsListOpenSummary(s32);
static void DrawListIndexNumber(s32, s32);
static void ClearCatchLogRow(u16 windowId, u32 listItemId, u32 row);
static void AddRibbonsMonListWindow(struct Pokenav_RibbonsMonMenu *);
static void UpdateIndexNumberDisplay(struct Pokenav_RibbonsMonMenu *);
static void CreateRibbonMonsList(void);
static void BufferRibbonMonInfoText(struct PokenavListItem *, u8 *);

static const LoopedTask sMonRibbonListLoopTaskFuncs[] =
{
    BuildPartyMonRibbonList,
    InitBoxMonRibbonList,
    BuildBoxMonRibbonList
};

static const u16 sMonRibbonListFramePal[] = INCGFX_U16("graphics/pokenav/ribbons/list_bg.png", ".gbapal");
static const u32 sMonRibbonListFrameTiles[] = INCGFX_U32("graphics/pokenav/ribbons/list_bg.png", ".4bpp.smol");
static const u32 sMonRibbonListFrameTilemap[] = INCGFX_U32("graphics/pokenav/ribbons/list_bg.bin", ".smolTM");
static const u16 sMonRibbonListUi_Pal[] = INCGFX_U16("graphics/pokenav/ribbons/list_ui.pal", ".gbapal");

static const struct BgTemplate sMonRibbonListBgTemplates[] =
{
    {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 0x06,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 2,
        .baseTile = 0
    }, {
        .bg = 2,
        .charBaseIndex = 2,
        .mapBaseIndex = 0x07,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 3,
        .baseTile = 0
    }
};

static const LoopedTask sRibbonsMonMenuLoopTaskFuncs[] =
{
    [RIBBONS_MON_LIST_FUNC_NONE]                 = NULL,
    [RIBBONS_MON_LIST_FUNC_MOVE_UP]              = LoopedTask_RibbonsListMoveCursorUp,
    [RIBBONS_MON_LIST_FUNC_MOVE_DOWN]            = LoopedTask_RibbonsListMoveCursorDown,
    [RIBBONS_MON_LIST_FUNC_PAGE_UP]              = LoopedTask_RibbonsListMovePageUp,
    [RIBBONS_MON_LIST_FUNC_PAGE_DOWN]            = LoopedTask_RibbonsListMovePageDown,
    [RIBBONS_MON_LIST_FUNC_EXIT]                 = LoopedTask_RibbonsListReturnToMainMenu,
    [RIBBONS_MON_LIST_FUNC_OPEN_RIBBONS_SUMMARY] = LoopedTask_RibbonsListOpenSummary
};

static const struct WindowTemplate sRibbonsMonListWindowTemplate =
{
    .bg = 1,
    .tilemapLeft = 1,
    .tilemapTop = 6,
    .width = 7,
    .height = 10,
    .paletteNum = 1,
    .baseBlock = 20
};

static const u8 sText_MaleSymbol[] = _("{TEXT_COLORS LIGHT_RED GREEN WHITE}{BACKGROUND WHITE}♂{TEXT_COLORS DARK_GRAY LIGHT_GRAY WHITE}{BACKGROUND WHITE}");
static const u8 sText_FemaleSymbol[] = _("{TEXT_COLORS LIGHT_GREEN BLUE WHITE}{BACKGROUND WHITE}♀{TEXT_COLORS DARK_GRAY LIGHT_GRAY WHITE}{BACKGROUND WHITE}");
static const u8 sText_NoGenderSymbol[] = _("{UNK_SPACER}");


static const mapsec_u8_t sCatchLogAreas[] = {
    MAPSEC_LITTLEROOT_TOWN,
    MAPSEC_ROUTE_101,
    MAPSEC_OLDALE_TOWN,
    MAPSEC_ROUTE_103,
    MAPSEC_ROUTE_102,
    MAPSEC_PETALBURG_CITY,
    MAPSEC_ROUTE_104,
    MAPSEC_PETALBURG_WOODS,
    MAPSEC_RUSTBORO_CITY,
    MAPSEC_ROUTE_116,
    MAPSEC_RUSTURF_TUNNEL,
    MAPSEC_ROUTE_105,
    MAPSEC_ISLAND_CAVE,
    MAPSEC_ROUTE_106,
    MAPSEC_DEWFORD_TOWN,
    MAPSEC_GRANITE_CAVE,
    MAPSEC_ROUTE_107,
    MAPSEC_ROUTE_108,
    MAPSEC_ABANDONED_SHIP,
    MAPSEC_ROUTE_109,
    MAPSEC_SLATEPORT_CITY,
    MAPSEC_ROUTE_110,
    MAPSEC_MAUVILLE_CITY,
    MAPSEC_NEW_MAUVILLE,
    MAPSEC_ROUTE_117,
    MAPSEC_VERDANTURF_TOWN,
    MAPSEC_ROUTE_111,
    MAPSEC_DESERT_RUINS,
    MAPSEC_MIRAGE_TOWER,
    MAPSEC_ROUTE_112,
    MAPSEC_FIERY_PATH,
    MAPSEC_MT_CHIMNEY,
    MAPSEC_JAGGED_PASS,
    MAPSEC_MAGMA_HIDEOUT,
    MAPSEC_LAVARIDGE_TOWN,
    MAPSEC_ROUTE_113,
    MAPSEC_FALLARBOR_TOWN,
    MAPSEC_ROUTE_114,
    MAPSEC_METEOR_FALLS,
    MAPSEC_ROUTE_115,
    MAPSEC_ROUTE_118,
    MAPSEC_ROUTE_119,
    MAPSEC_FORTREE_CITY,
    MAPSEC_ROUTE_120,
    MAPSEC_ANCIENT_TOMB,
    MAPSEC_SCORCHED_SLAB,
    MAPSEC_ROUTE_121,
    MAPSEC_SAFARI_ZONE,
    MAPSEC_LILYCOVE_CITY,
    MAPSEC_ROUTE_122,
    MAPSEC_MT_PYRE,
    MAPSEC_ROUTE_123,
    MAPSEC_AQUA_HIDEOUT,
    MAPSEC_ROUTE_124,
    MAPSEC_MOSSDEEP_CITY,
    MAPSEC_ROUTE_125,
    MAPSEC_SHOAL_CAVE,
    MAPSEC_ROUTE_127,
    MAPSEC_ROUTE_128,
    MAPSEC_SEAFLOOR_CAVERN,
    MAPSEC_UNDERWATER_SEAFLOOR_CAVERN,
    MAPSEC_ROUTE_126,
    MAPSEC_UNDERWATER_SOOTOPOLIS,
    MAPSEC_SOOTOPOLIS_CITY,
    MAPSEC_CAVE_OF_ORIGIN,
    MAPSEC_ROUTE_129,
    MAPSEC_ROUTE_130,
    MAPSEC_ROUTE_131,
    MAPSEC_PACIFIDLOG_TOWN,
    MAPSEC_ROUTE_132,
    MAPSEC_ROUTE_133,
    MAPSEC_ROUTE_134,
    MAPSEC_SEALED_CHAMBER,
    MAPSEC_UNDERWATER_SEALED_CHAMBER,
    MAPSEC_SKY_PILLAR,
    MAPSEC_EVER_GRANDE_CITY,
    MAPSEC_VICTORY_ROAD,
    MAPSEC_SECRET_BASE,
};
static u32 CountUsedCatchLogAreas(void)
{
    u32 used = 0;
    for (u32 i = 0; i < ARRAY_COUNT(sCatchLogAreas); i++)
        used += ForkIsAreaEncounterSpent(sCatchLogAreas[i]);
    return used;
}

#if TESTING
u32 Test_GetCatchLogAreaCount(void) { return ARRAY_COUNT(sCatchLogAreas); }
mapsec_u8_t Test_GetCatchLogArea(u32 index) { return sCatchLogAreas[index]; }
u32 Test_CountUsedCatchLogAreas(void) { return CountUsedCatchLogAreas(); }
#endif

static void GetCatchLogAreaName(u8 *dest, mapsec_u8_t area)
{
    switch (area)
    {
    case MAPSEC_UNDERWATER_SEALED_CHAMBER: StringCopy(dest, COMPOUND_STRING("SEALED CHAMBER DIVE")); break;
    case MAPSEC_UNDERWATER_SEAFLOOR_CAVERN: StringCopy(dest, COMPOUND_STRING("SEAFLOOR CAVERN DIVE")); break;
    case MAPSEC_UNDERWATER_SOOTOPOLIS: StringCopy(dest, COMPOUND_STRING("SOOTOPOLIS DIVE")); break;
    default: GetMapName(dest, area, 0); break;
    }
}

bool32 PokenavCallback_Init_MonRibbonList(void)
{
    struct Pokenav_RibbonsMonList *list = AllocSubstruct(POKENAV_SUBSTRUCT_RIBBONS_MON_LIST, sizeof(struct Pokenav_RibbonsMonList));
    if (list == NULL)
        return FALSE;

    list->monList = AllocSubstruct(POKENAV_SUBSTRUCT_MON_LIST, sizeof(struct PokenavMonList));
    if (list->monList == NULL)
        return FALSE;

    list->callback = HandleRibbonsMonListInput_WaitListInit;
    list->monList->listCount = ARRAY_COUNT(sCatchLogAreas);
    list->monList->currIndex = 0;
    list->saveMonList = FALSE;
    for (u32 i = 0; i < ARRAY_COUNT(sCatchLogAreas); i++)
    {
        list->monList->monData[i].data = sCatchLogAreas[i];
        if (sCatchLogAreas[i] == ForkGetCurrentCatchArea())
            list->monList->currIndex = i;
    }
    list->callback = HandleRibbonsMonListInput;
    list->loopedTaskId = CreateLoopedTask(GetMonRibbonListLoopTaskFunc, 1);
    list->changeBgs = 1;
    return TRUE;
}

bool32 PokenavCallback_Init_RibbonsMonListFromSummary(void)
{
    struct Pokenav_RibbonsMonList *list = AllocSubstruct(POKENAV_SUBSTRUCT_RIBBONS_MON_LIST, sizeof(struct Pokenav_RibbonsMonList));
    if (list == NULL)
        return FALSE;

    list->monList = GetSubstructPtr(POKENAV_SUBSTRUCT_MON_LIST);
    list->callback = HandleRibbonsMonListInput;
    list->changeBgs = 1;
    return TRUE;
}

u32 GetRibbonsMonListCallback(void)
{
    struct Pokenav_RibbonsMonList *list = GetSubstructPtr(POKENAV_SUBSTRUCT_RIBBONS_MON_LIST);
    return list->callback(list);
}

void FreeRibbonsMonList(void)
{
    struct Pokenav_RibbonsMonList *list = GetSubstructPtr(POKENAV_SUBSTRUCT_RIBBONS_MON_LIST);
    if (!list->saveMonList)
        FreePokenavSubstruct(POKENAV_SUBSTRUCT_MON_LIST);
    FreePokenavSubstruct(POKENAV_SUBSTRUCT_RIBBONS_MON_LIST);
}

static u32 HandleRibbonsMonListInput_WaitListInit(struct Pokenav_RibbonsMonList *list)
{
    if (!IsLoopedTaskActive(list->loopedTaskId))
        list->callback = HandleRibbonsMonListInput;
    return 0;
}

static u32 HandleRibbonsMonListInput(struct Pokenav_RibbonsMonList *list)
{
    if (JOY_REPEAT(DPAD_UP))
        return RIBBONS_MON_LIST_FUNC_MOVE_UP;
    if (JOY_REPEAT(DPAD_DOWN))
        return RIBBONS_MON_LIST_FUNC_MOVE_DOWN;
    if (JOY_NEW(DPAD_LEFT))
        return RIBBONS_MON_LIST_FUNC_PAGE_UP;
    if (JOY_NEW(DPAD_RIGHT))
        return RIBBONS_MON_LIST_FUNC_PAGE_DOWN;
    if (JOY_NEW(B_BUTTON))
    {
        list->saveMonList = 0;
        list->callback = RibbonsMonMenu_ReturnToMainMenu;
        return RIBBONS_MON_LIST_FUNC_EXIT;
    }
    return RIBBONS_MON_LIST_FUNC_NONE;
}

static u32 RibbonsMonMenu_ReturnToMainMenu(struct Pokenav_RibbonsMonList *list)
{
    return POKENAV_MAIN_MENU_CURSOR_ON_RIBBONS;
}



static u32 UpdateMonListBgs(void)
{
    struct Pokenav_RibbonsMonList *list = GetSubstructPtr(POKENAV_SUBSTRUCT_RIBBONS_MON_LIST);
    return list->changeBgs;
}

static struct PokenavMonListItem *GetMonRibbonMonListData(void)
{
    struct Pokenav_RibbonsMonList *list = GetSubstructPtr(POKENAV_SUBSTRUCT_RIBBONS_MON_LIST);
    return list->monList->monData;
}

static s32 GetRibbonsMonListCount(void)
{
    struct Pokenav_RibbonsMonList *list = GetSubstructPtr(POKENAV_SUBSTRUCT_RIBBONS_MON_LIST);
    return list->monList->listCount;
}

static s32 UNUSED GetMonRibbonSelectedMonData(void)
{
    struct Pokenav_RibbonsMonList *list = GetSubstructPtr(POKENAV_SUBSTRUCT_RIBBONS_MON_LIST);
    s32 idx = PokenavList_GetSelectedIndex();
    return list->monList->monData[idx].data;
}

static s32 GetRibbonListMenuCurrIndex(void)
{
    struct Pokenav_RibbonsMonList *list = GetSubstructPtr(POKENAV_SUBSTRUCT_RIBBONS_MON_LIST);
    return list->monList->currIndex;
}

static u32 GetMonRibbonListLoopTaskFunc(s32 state)
{
    (void)sMonRibbonListLoopTaskFuncs;
    return LT_FINISH;
}

static u32 BuildPartyMonRibbonList(s32 state)
{
    s32 i;
    struct PokenavMonListItem item;
    struct Pokenav_RibbonsMonList *list = GetSubstructPtr(POKENAV_SUBSTRUCT_RIBBONS_MON_LIST);

    list->monList->listCount = 0;
    list->monList->currIndex = 0;
    item.boxId = TOTAL_BOXES_COUNT;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *pokemon = &gParties[B_TRAINER_PLAYER][i];
        if (!GetMonData(pokemon, MON_DATA_SANITY_HAS_SPECIES))
            return LT_INC_AND_CONTINUE;
        if (!GetMonData(pokemon, MON_DATA_SANITY_IS_EGG) && !GetMonData(pokemon, MON_DATA_SANITY_IS_BAD_EGG))
        {
            u32 ribbonCount = GetMonData(pokemon, MON_DATA_RIBBON_COUNT);
            if (ribbonCount != 0)
            {
                item.monId = i;
                item.data = ribbonCount;
                InsertMonListItem(list, &item);
            }
        }
    }

    return LT_INC_AND_CONTINUE;
}

static u32 InitBoxMonRibbonList(s32 state)
{
    struct Pokenav_RibbonsMonList *list = GetSubstructPtr(POKENAV_SUBSTRUCT_RIBBONS_MON_LIST);
    list->monId = 0;
    list->boxId = 0;
    return LT_INC_AND_CONTINUE;
}

static u32 BuildBoxMonRibbonList(s32 state)
{
    struct Pokenav_RibbonsMonList *list = GetSubstructPtr(POKENAV_SUBSTRUCT_RIBBONS_MON_LIST);
    s32 boxId = list->boxId;
    s32 monId = list->monId;
    s32 boxCount = 0;
    struct PokenavMonListItem item;

    while (boxId < TOTAL_BOXES_COUNT)
    {
        while (monId < IN_BOX_COUNT)
        {
            if (CheckBoxMonSanityAt(boxId, monId))
            {
                u32 ribbonCount = GetBoxMonDataAt(boxId, monId, MON_DATA_RIBBON_COUNT);
                if (ribbonCount != 0)
                {
                    item.boxId = boxId;
                    item.monId = monId;
                    item.data = ribbonCount;
                    InsertMonListItem(list, &item);
                }
            }
            boxCount++;
            monId++;
            if (boxCount > TOTAL_BOXES_COUNT)
            {
                list->boxId = boxId;
                list->monId = monId;
                return LT_CONTINUE;
            }
        }
        monId = 0;
        boxId++;
    }

    list->changeBgs = 1;
    return LT_FINISH;
}

static void InsertMonListItem(struct Pokenav_RibbonsMonList *list, struct PokenavMonListItem *item)
{
    u32 left = 0;
    u32 right = list->monList->listCount;
    u32 insertionIdx = left + (right - left) / 2;

    while (right != insertionIdx)
    {
        if (item->data > list->monList->monData[insertionIdx].data)
            right = insertionIdx;
        else
            left = insertionIdx + 1;
        insertionIdx = left + (right - left) / 2;
    }
    for (right = list->monList->listCount; right > insertionIdx; right--)
        list->monList->monData[right] = list->monList->monData[right - 1];
    list->monList->monData[insertionIdx] = *item;
    list->monList->listCount++;
}

static bool32 UNUSED PlayerHasRibbonsMon(void)
{
    s32 i, j;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
        if (!GetMonData(mon, MON_DATA_SANITY_HAS_SPECIES))
            continue;
        if (GetMonData(mon, MON_DATA_SANITY_IS_EGG))
            continue;
        if (GetMonData(mon, MON_DATA_RIBBONS))
            return TRUE;
    }

    for (i = 0; i < TOTAL_BOXES_COUNT; i++)
    {
        for (j = 0; j < IN_BOX_COUNT; j++)
        {
            if (!CheckBoxMonSanityAt(i, j))
                continue;
            if (GetBoxMonDataAt(i, j, MON_DATA_RIBBONS))
                return TRUE;
        }
    }

    return FALSE;
}

bool32 OpenRibbonsMonList(void)
{
    struct Pokenav_RibbonsMonMenu *menu = AllocSubstruct(POKENAV_SUBSTRUCT_RIBBONS_MON_MENU, sizeof(struct Pokenav_RibbonsMonMenu));
    if (menu == NULL)
        return FALSE;
    menu->loopedTaskId = CreateLoopedTask(LoopedTask_OpenRibbonsMonList, 1);
    menu->callback = GetRibbonsMonCurrentLoopedTaskActive;
    menu->fromSummary = FALSE;
    return TRUE;
}

bool32 OpenRibbonsMonListFromRibbonsSummary(void)
{
    struct Pokenav_RibbonsMonMenu *menu = AllocSubstruct(POKENAV_SUBSTRUCT_RIBBONS_MON_MENU, sizeof(struct Pokenav_RibbonsMonMenu));
    if (menu == NULL)
        return FALSE;
    menu->loopedTaskId = CreateLoopedTask(LoopedTask_OpenRibbonsMonList, 1);
    menu->callback = GetRibbonsMonCurrentLoopedTaskActive;
    menu->fromSummary = TRUE;
    return TRUE;
}

void CreateRibbonsMonListLoopedTask(s32 idx)
{
    struct Pokenav_RibbonsMonMenu *menu = GetSubstructPtr(POKENAV_SUBSTRUCT_RIBBONS_MON_MENU);
    menu->loopedTaskId = CreateLoopedTask(sRibbonsMonMenuLoopTaskFuncs[idx], 1);
    menu->callback = GetRibbonsMonCurrentLoopedTaskActive;
}

bool32 IsRibbonsMonListLoopedTaskActive(void)
{
    struct Pokenav_RibbonsMonMenu *menu = GetSubstructPtr(POKENAV_SUBSTRUCT_RIBBONS_MON_MENU);
    return menu->callback();
}

bool32 GetRibbonsMonCurrentLoopedTaskActive(void)
{
    struct Pokenav_RibbonsMonMenu *menu = GetSubstructPtr(POKENAV_SUBSTRUCT_RIBBONS_MON_MENU);
    return IsLoopedTaskActive(menu->loopedTaskId);
}

void FreeRibbonsMonMenu(void)
{
    struct Pokenav_RibbonsMonMenu *menu = GetSubstructPtr(POKENAV_SUBSTRUCT_RIBBONS_MON_MENU);
    DestroyPokenavList();
    RemoveWindow(menu->winid);
    FreePokenavSubstruct(POKENAV_SUBSTRUCT_RIBBONS_MON_MENU);
}

static u32 LoopedTask_OpenRibbonsMonList(s32 state)
{
    struct Pokenav_RibbonsMonMenu *menu = GetSubstructPtr(POKENAV_SUBSTRUCT_RIBBONS_MON_MENU);
    switch (state)
    {
    case 0:
        InitBgTemplates(sMonRibbonListBgTemplates, ARRAY_COUNT(sMonRibbonListBgTemplates));
        BgDmaFill(1, 0, 0, 1);
        SetBgTilemapBuffer(1, menu->buff);
        FillBgTilemapBufferRect(1, 0, 0, 0, 32, 32, 1);
        BgDmaFill(1, PIXEL_FILL(1), 1, 1);
        {
            const u16 colors[16] = {RGB_WHITE, RGB_WHITE, RGB(4, 6, 8), RGB(24, 25, 26), RGB(26, 3, 3), RGB_WHITE, RGB(2, 18, 5)};
            LoadPalette(colors, BG_PLTT_ID(1), sizeof(colors));
        }
        CopyBgTilemapBufferToVram(1);
        return LT_INC_AND_PAUSE;
    case 1:
        if (FreeTempTileDataBuffersIfPossible())
            return LT_PAUSE;
        if (!UpdateMonListBgs())
            return LT_PAUSE;
        ChangeBgX(1, 0, BG_COORD_SET);
        ChangeBgY(1, 0, BG_COORD_SET);
        ShowBg(1);
        return LT_INC_AND_PAUSE;
    case 2:
        if (FreeTempTileDataBuffersIfPossible())
            return LT_PAUSE;
        {
            const u16 colors[16] = {RGB_WHITE, RGB_WHITE, RGB(4, 6, 8), RGB(24, 25, 26), RGB(26, 3, 3), RGB_WHITE, RGB(2, 18, 5)};
            LoadPalette(colors, BG_PLTT_ID(2), sizeof(colors));
        }
        CreateRibbonMonsList();
        return LT_INC_AND_PAUSE;
    case 3:
        if (IsCreatePokenavListTaskActive())
            return LT_PAUSE;
        AddRibbonsMonListWindow(menu);
        return LT_INC_AND_PAUSE;
    case 4:
        if (FreeTempTileDataBuffersIfPossible())
            return LT_PAUSE;
        ShowBg(2);
        HideBg(3);
        PrintHelpBarText(HELPBAR_RIBBONS_MON_LIST);
        PokenavFadeScreen(POKENAV_FADE_FROM_BLACK);
        if (!menu->fromSummary)
        {
            LoadLeftHeaderGfxForIndex(POKENAV_GFX_RIBBONS_MENU);
            ShowLeftHeaderGfx(POKENAV_GFX_RIBBONS_MENU, TRUE, FALSE);
        }
        return LT_INC_AND_PAUSE;
    case 5:
        SetLeftHeaderSpritesInvisibility();
        if (IsPaletteFadeActive())
            return LT_PAUSE;
        if (AreLeftHeaderSpritesMoving())
            return LT_PAUSE;
        break;
    }
    return LT_FINISH;
}

static u32 LoopedTask_RibbonsListMoveCursorUp(s32 state)
{
    struct Pokenav_RibbonsMonMenu *menu = GetSubstructPtr(POKENAV_SUBSTRUCT_RIBBONS_MON_MENU);
    switch (state)
    {
    case 0:
        switch (PokenavList_MoveCursorUp())
        {
        case 0:
            return LT_FINISH;
        case 1:
            PlaySE(SE_SELECT);
            return LT_SET_STATE(2);
        case 2:
            PlaySE(SE_SELECT);
            break;
        }
        return LT_INC_AND_PAUSE;
    case 1:
        if (PokenavList_IsMoveWindowTaskActive())
            return LT_PAUSE;
        // fallthrough
    case 2:
        UpdateIndexNumberDisplay(menu);
        return LT_INC_AND_PAUSE;
    case 3:
        if (IsDma3ManagerBusyWithBgCopy())
            return LT_PAUSE;
        break;
    }
    return LT_FINISH;
}

static u32 LoopedTask_RibbonsListMoveCursorDown(s32 state)
{
    struct Pokenav_RibbonsMonMenu *menu = GetSubstructPtr(POKENAV_SUBSTRUCT_RIBBONS_MON_MENU);
    switch (state)
    {
    case 0:
        switch (PokenavList_MoveCursorDown())
        {
        case 0:
            return LT_FINISH;
        case 1:
            PlaySE(SE_SELECT);
            return LT_SET_STATE(2);
        case 2:
            PlaySE(SE_SELECT);
            break;
        }
        return LT_INC_AND_PAUSE;
    case 1:
        if (PokenavList_IsMoveWindowTaskActive())
            return LT_PAUSE;
        // fallthrough
    case 2:
        UpdateIndexNumberDisplay(menu);
        return LT_INC_AND_PAUSE;
    case 3:
        if (IsDma3ManagerBusyWithBgCopy())
            return LT_PAUSE;
        break;
    }
    return LT_FINISH;
}

static u32 LoopedTask_RibbonsListMovePageUp(s32 state)
{
    struct Pokenav_RibbonsMonMenu *menu = GetSubstructPtr(POKENAV_SUBSTRUCT_RIBBONS_MON_MENU);
    switch (state)
    {
    case 0:
        switch (PokenavList_PageUp())
        {
        case 0:
            return LT_FINISH;
        case 1:
            PlaySE(SE_SELECT);
            return LT_SET_STATE(2);
        case 2:
            PlaySE(SE_SELECT);
            break;
        }
        return LT_INC_AND_PAUSE;
    case 1:
        if (PokenavList_IsMoveWindowTaskActive())
            return LT_PAUSE;
        // fallthrough
    case 2:
        UpdateIndexNumberDisplay(menu);
        return LT_INC_AND_PAUSE;
    case 3:
        if (IsDma3ManagerBusyWithBgCopy())
            return LT_PAUSE;
        break;
    }
    return LT_FINISH;
}

static u32 LoopedTask_RibbonsListMovePageDown(s32 state)
{
    struct Pokenav_RibbonsMonMenu *menu = GetSubstructPtr(POKENAV_SUBSTRUCT_RIBBONS_MON_MENU);
    switch (state)
    {
    case 0:
        switch (PokenavList_PageDown())
        {
        case 0:
            return LT_FINISH;
        case 1:
            PlaySE(SE_SELECT);
            return LT_SET_STATE(2);
        case 2:
            PlaySE(SE_SELECT);
            break;
        }
        return LT_INC_AND_PAUSE;
    case 1:
        if (PokenavList_IsMoveWindowTaskActive())
            return LT_PAUSE;
        // fallthrough
    case 2:
        UpdateIndexNumberDisplay(menu);
        return LT_INC_AND_PAUSE;
    case 3:
        if (IsDma3ManagerBusyWithBgCopy())
            return LT_PAUSE;
        break;
    }
    return LT_FINISH;
}

static u32 LoopedTask_RibbonsListReturnToMainMenu(s32 state)
{
    switch (state)
    {
    case 0:
        PlaySE(SE_SELECT);
        PokenavFadeScreen(POKENAV_FADE_TO_BLACK);
        SlideMenuHeaderDown();
        return LT_INC_AND_PAUSE;
    case 1:
        if (IsPaletteFadeActive())
            return LT_PAUSE;
        if (MainMenuLoopedTaskIsBusy())
            return LT_PAUSE;
        SetLeftHeaderSpritesInvisibility();
        break;
    }
    return LT_FINISH;
}

static u32 LoopedTask_RibbonsListOpenSummary(s32 state)
{
    switch (state)
    {
    case 0:
        PlaySE(SE_SELECT);
        PokenavFadeScreen(POKENAV_FADE_TO_BLACK);
        return LT_INC_AND_PAUSE;
    case 1:
        if (IsPaletteFadeActive())
            return LT_PAUSE;
        break;
    }
    return LT_FINISH;
}

static void AddRibbonsMonListWindow(struct Pokenav_RibbonsMonMenu *menu)
{
    s32 listCount;
    menu->winid = AddWindow(&sRibbonsMonListWindowTemplate);
    // AddWindow installs the window system's tilemap buffer. Mask the list's
    // circular scrolling rows in that buffer, after the window is allocated.
    FillBgTilemapBufferRect(1, 1, 13, 0, 17, 1, 1);
    FillBgTilemapBufferRect(1, 1, 13, 17, 17, 15, 1);
    PutWindowTilemap(menu->winid);
    listCount = GetRibbonsMonListCount();
    DrawListIndexNumber(menu->winid, listCount);
    CopyWindowToVram(menu->winid, COPYWIN_MAP);
    UpdateIndexNumberDisplay(menu);
}

static void UpdateIndexNumberDisplay(struct Pokenav_RibbonsMonMenu *menu)
{
    s32 listCount = GetRibbonsMonListCount();
    DrawListIndexNumber(menu->winid, listCount);
    CopyWindowToVram(menu->winid, COPYWIN_GFX);
}

static void DrawListIndexNumber(s32 windowId, s32 max)
{
    u8 text[128];
    u32 used = CountUsedCatchLogAreas();
    u8 *ptr;
    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    StringCopy(text, COMPOUND_STRING("CATCH LOG"));
    AddTextPrinterParameterized(windowId, FONT_SMALL_NARROWER, text, 0, 0, TEXT_SKIP_DRAW, NULL);
    ptr = ConvertIntToDecimalStringN(text, used, STR_CONV_MODE_LEFT_ALIGN, 3);
    *ptr++ = CHAR_SLASH;
    ConvertIntToDecimalStringN(ptr, max, STR_CONV_MODE_LEFT_ALIGN, 3);
    StringAppend(text, COMPOUND_STRING(" USED"));
    AddTextPrinterParameterized(windowId, FONT_SMALL_NARROWER, text, 0, 14, TEXT_SKIP_DRAW, NULL);
    if (!ForkIsCatchLimitEnabled())
        AddTextPrinterParameterized(windowId, FONT_SMALL_NARROWER, COMPOUND_STRING("CATCH LIMIT\nOFF"), 0, 32, TEXT_SKIP_DRAW, NULL);
}

static void CreateRibbonMonsList(void)
{
    struct PokenavListTemplate template;
    template.list = (struct PokenavListItem *)GetMonRibbonMonListData();
    template.count = GetRibbonsMonListCount();
    template.itemSize = sizeof(struct PokenavListItem);
    template.startIndex = GetRibbonListMenuCurrIndex();
    template.item_X = 13;
    template.windowWidth = 17;
    template.listTop = 1;
    template.maxShowed = 8;
    template.fillValue = 2;
    template.fontId = FONT_SMALL_NARROWER;
    template.bufferItemFunc = BufferRibbonMonInfoText;
    template.iconDrawFunc = ClearCatchLogRow;
    CreatePokenavList(&sMonRibbonListBgTemplates[1], &template, 0);
}

// The generic scrolling list reuses its tile rows. Clear the entire old row,
// including trailing status letters, before printing its replacement.
static void ClearCatchLogRow(u16 windowId, u32 listItemId, u32 row)
{
    FillWindowPixelRect(windowId, PIXEL_FILL(1), 0, row * 16, 17 * 8, 16);
}

// Keep the status column fixed, with enough room for either status label.
static void BufferRibbonMonInfoText(struct PokenavListItem *listItem, u8 *dest)
{
    struct PokenavMonListItem *item = (struct PokenavMonListItem *)listItem;
    u32 len;
    StringCopy(dest, COMPOUND_STRING("{COLOR 2}{SHADOW 3}{HIGHLIGHT 1}"));
    GetCatchLogAreaName(dest + StringLength(dest), item->data);
    len = StringLength(dest);
    while (len > 0 && GetStringWidth(FONT_SMALL_NARROWER, dest, 0) > 88)
        dest[--len] = EOS;
    StringAppend(dest, ForkIsAreaEncounterSpent(item->data)
        ? COMPOUND_STRING("{CLEAR_TO 94}{COLOR 4}USED") : COMPOUND_STRING("{CLEAR_TO 94}{COLOR 6}OPEN"));
}
