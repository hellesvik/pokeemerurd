#include "global.h"
#include "battle.h"
#include "party_menu.h"
#include "pokemon.h"
#include "test/test.h"
#include "battle_setup.h"
#include "caps.h"
#include "event_data.h"
#include "fieldmap.h"
#include "main.h"
#include "overworld.h"
#include "constants/items.h"
#include "constants/layouts.h"
#include "constants/moves.h"
#include "constants/opponents.h"
#include "constants/party_menu.h"
#include "menu.h"
#include "text.h"
#include "task.h"

#define TEST_MENU_DIR_DOWN     1
#define TEST_MENU_DIR_UP      -1
#define TEST_MENU_DIR_RIGHT    2

static void SetTestPartySize(enum BattleTrainer trainer, u8 partySize)
{
    for (u32 i = 0; i < PARTY_SIZE; i++)
        ZeroMonData(&gParties[trainer][i]);

    for (u32 i = 0; i < partySize; i++)
        CreateMon(&gParties[trainer][i], SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PRESET(0));

    gPartiesCount[trainer] = partySize;
}

TEST("Full multi partner party menu stops down navigation at partner party count")
{
    SetTestPartySize(B_TRAINER_PLAYER, PARTY_SIZE);
    SetTestPartySize(B_TRAINER_PARTNER, 2);
    gPartyMenu.layout = PARTY_LAYOUT_MULTI_FULL_PARTNER;

    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(1, TEST_MENU_DIR_DOWN, FALSE, 0), PARTY_SIZE + 1);
}

TEST("Full multi partner party menu allows down navigation through partner party count")
{
    SetTestPartySize(B_TRAINER_PLAYER, 2);
    SetTestPartySize(B_TRAINER_PARTNER, PARTY_SIZE);
    gPartyMenu.layout = PARTY_LAYOUT_MULTI_FULL_PARTNER;

    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(1, TEST_MENU_DIR_DOWN, FALSE, 0), 2);
}

TEST("Full multi partner party menu wraps cancel up to partner party count")
{
    SetTestPartySize(B_TRAINER_PLAYER, PARTY_SIZE);
    SetTestPartySize(B_TRAINER_PARTNER, 2);
    gPartyMenu.layout = PARTY_LAYOUT_MULTI_FULL_PARTNER;

    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(PARTY_SIZE + 1, TEST_MENU_DIR_UP, FALSE, 0), 1);
}


static void SetupPartyTraining(void)
{
    gSaveBlock3Ptr->forkGameplayOptionsConfigured = TRUE;
    gSaveBlock3Ptr->forkLevelCapEnabled = TRUE;
    ClearTrainerFlag(TRAINER_STEVEN);
    ClearTrainerFlag(TRAINER_ROXANNE_1);
    gMain.inBattle = FALSE;
    gMapHeader.mapLayoutId = LAYOUT_ROUTE101;
    gPartyMenu.menuType = PARTY_MENU_TYPE_FIELD;
    gPartyMenu.layout = PARTY_LAYOUT_SINGLE;
    gPartyMenu.action = PARTY_ACTION_CHOOSE_MON;
    gPartyMenu.slotId = 0;
    SetTestPartySize(B_TRAINER_PLAYER, 2);
}

TEST("Party training sets exact cap experience for all growth rates and preserves Pokemon data")
{
    const u16 species[] = {SPECIES_BULBASAUR, SPECIES_CLEFAIRY, SPECIES_MAGIKARP, SPECIES_MAKUHITA, SPECIES_NINCADA, SPECIES_PIKACHU};
    SetupPartyTraining();
    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        struct Pokemon mon;
        u32 value;
        CreateMon(&mon, species[i], 5, 0, OTID_STRUCT_PRESET(0));
        SetMonMoveSlot(&mon, MOVE_TACKLE, 0);
        value = 2; SetMonData(&mon, MON_DATA_PP1, &value);
        value = ITEM_ORAN_BERRY; SetMonData(&mon, MON_DATA_HELD_ITEM, &value);
        value = STATUS1_POISON; SetMonData(&mon, MON_DATA_STATUS, &value);
        value = 123; SetMonData(&mon, MON_DATA_FRIENDSHIP, &value);
        value = 32; SetMonData(&mon, MON_DATA_HP_EV, &value);
        CalculateMonStats(&mon);
        value = GetMonData(&mon, MON_DATA_HP) - 3; SetMonData(&mon, MON_DATA_HP, &value);
        struct Pokemon before = mon;
        EXPECT(Test_LevelPartyMonToCap(&mon));
        EXPECT_EQ(GetMonData(&mon, MON_DATA_LEVEL), GetCurrentLevelCap());
        EXPECT_EQ(GetMonData(&mon, MON_DATA_EXP), gExperienceTables[gSpeciesInfo[species[i]].growthRate][GetCurrentLevelCap()]);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_MAX_HP) - GetMonData(&mon, MON_DATA_HP), 3);
        const u8 fields[] = {MON_DATA_SPECIES, MON_DATA_HELD_ITEM, MON_DATA_STATUS, MON_DATA_FRIENDSHIP,
            MON_DATA_MOVE1, MON_DATA_MOVE2, MON_DATA_MOVE3, MON_DATA_MOVE4,
            MON_DATA_PP1, MON_DATA_PP2, MON_DATA_PP3, MON_DATA_PP4, MON_DATA_PP_BONUSES,
            MON_DATA_HP_EV, MON_DATA_ATK_EV, MON_DATA_DEF_EV, MON_DATA_SPEED_EV, MON_DATA_SPATK_EV, MON_DATA_SPDEF_EV,
            MON_DATA_HP_IV, MON_DATA_ATK_IV, MON_DATA_DEF_IV, MON_DATA_SPEED_IV, MON_DATA_SPATK_IV, MON_DATA_SPDEF_IV};
        for (u32 j = 0; j < ARRAY_COUNT(fields); j++)
            EXPECT_EQ(GetMonData(&mon, fields[j]), GetMonData(&before, fields[j]));
        before = mon;
        EXPECT(!Test_LevelPartyMonToCap(&mon));
        EXPECT_EQ(memcmp(&before, &mon, sizeof(mon)), 0);
    }
}

TEST("Party training preserves fainting and never lowers or changes disabled-cap Pokemon")
{
    struct Pokemon mon, before;
    u32 hp = 0;
    SetupPartyTraining();
    CreateMon(&mon, SPECIES_BULBASAUR, 5, 0, OTID_STRUCT_PRESET(0));
    CalculateMonStats(&mon);
    SetMonData(&mon, MON_DATA_HP, &hp);
    EXPECT(Test_LevelPartyMonToCap(&mon));
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HP), 0);
    CreateMon(&mon, SPECIES_BULBASAUR, 80, 0, OTID_STRUCT_PRESET(0));
    before = mon;
    EXPECT(!Test_LevelPartyMonToCap(&mon));
    EXPECT_EQ(memcmp(&before, &mon, sizeof(mon)), 0);
    CreateMon(&mon, SPECIES_BULBASAUR, 5, 0, OTID_STRUCT_PRESET(0));
    before = mon;
    gSaveBlock3Ptr->forkLevelCapEnabled = FALSE;
    EXPECT(!Test_LevelPartyMonToCap(&mon));
    EXPECT_EQ(memcmp(&before, &mon, sizeof(mon)), 0);
    gSaveBlock3Ptr->forkLevelCapEnabled = TRUE;
    SetTrainerFlag(TRAINER_STEVEN);
    EXPECT(Test_LevelPartyMonToCap(&mon));
    EXPECT_EQ(GetMonData(&mon, MON_DATA_LEVEL), 100);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_BULBASAUR);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_EXP), gExperienceTables[GROWTH_MEDIUM_SLOW][100]);
}

TEST("Party training menu holds four field moves and both shortcuts and hides excluded contexts")
{
    u8 count;
    u32 value;
    SetupPartyTraining();
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    CreateMon(mon, SPECIES_CATERPIE, 10, 0, OTID_STRUCT_PRESET(0));
    SetMonMoveSlot(mon, MOVE_CUT, 0);
    SetMonMoveSlot(mon, MOVE_SURF, 1);
    SetMonMoveSlot(mon, MOVE_STRENGTH, 2);
    SetMonMoveSlot(mon, MOVE_FLASH, 3);
    EXPECT_EQ(Test_PartyTrainingActions(&count), 3);
    EXPECT_EQ(count, 10);
    gSaveBlock3Ptr->forkLevelCapEnabled = FALSE;
    EXPECT_EQ(Test_PartyTrainingActions(&count), 2);
    gSaveBlock3Ptr->forkLevelCapEnabled = TRUE;
    const u8 types[] = {PARTY_MENU_TYPE_IN_BATTLE, PARTY_MENU_TYPE_UNION_ROOM_REGISTER, PARTY_MENU_TYPE_UNION_ROOM_TRADE, PARTY_MENU_TYPE_DAYCARE};
    for (u32 i = 0; i < ARRAY_COUNT(types); i++)
    {
        gPartyMenu.menuType = types[i];
        EXPECT_EQ(Test_PartyTrainingActions(&count), 0);
    }
    gPartyMenu.menuType = PARTY_MENU_TYPE_FIELD;
    gPartyMenu.layout = PARTY_LAYOUT_MULTI_FULL_PARTNER;
    EXPECT_EQ(Test_PartyTrainingActions(&count), 0);
    gPartyMenu.layout = PARTY_LAYOUT_SINGLE;
    value = TRUE; SetMonData(mon, MON_DATA_IS_EGG, &value);
    EXPECT_EQ(Test_PartyTrainingActions(&count), 0);
    value = FALSE; SetMonData(mon, MON_DATA_IS_EGG, &value);
    const u16 layouts[] = {LAYOUT_BATTLE_FRONTIER_BATTLE_PIKE_ROOM_FINAL, LAYOUT_BATTLE_FRONTIER_BATTLE_PYRAMID_FLOOR, LAYOUT_BATTLE_FRONTIER_BATTLE_FACTORY_BATTLE_ROOM};
    for (u32 i = 0; i < ARRAY_COUNT(layouts); i++)
    {
        gMapHeader.mapLayoutId = layouts[i];
        EXPECT_EQ(Test_PartyTrainingActions(&count), 0);
    }
}

TEST("Party Evolve checks are side effect free and honor thresholds and Everstone")
{
    u8 count;
    u32 item = ITEM_EVERSTONE;
    SetupPartyTraining();
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    for (u32 level = 6; level <= 10; level++)
    {
        CreateMon(mon, SPECIES_CATERPIE, level, 0, OTID_STRUCT_PRESET(0));
        struct Pokemon before = *mon;
        EXPECT_EQ(Test_PartyTrainingActions(&count) & 2, level >= 7 ? 2 : 0);
        EXPECT_EQ(memcmp(&before, mon, sizeof(*mon)), 0);
    }
    SetMonData(mon, MON_DATA_HELD_ITEM, &item);
    EXPECT_EQ(Test_PartyTrainingActions(&count) & 2, 0);
    CreateMon(mon, SPECIES_BUTTERFREE, 10, 0, OTID_STRUCT_PRESET(0));
    EXPECT_EQ(Test_PartyTrainingActions(&count) & 2, 0);
    CreateMon(mon, SPECIES_TOGEPI, 10, 0, OTID_STRUCT_PRESET(0));
    u32 friendship = 0;
    SetMonData(mon, MON_DATA_FRIENDSHIP, &friendship);
    EXPECT_EQ(Test_PartyTrainingActions(&count) & 2, 0);
    friendship = 255;
    SetMonData(mon, MON_DATA_FRIENDSHIP, &friendship);
    EXPECT_EQ(Test_PartyTrainingActions(&count) & 2, 2);
}

TEST("Party Evolve respects day and night held-item conditions without consuming during checks")
{
    u8 count;
    u32 item = ITEM_RAZOR_CLAW;
    SetupPartyTraining();
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    CreateMon(mon, SPECIES_SNEASEL, 10, 0, OTID_STRUCT_PRESET(0));
    u16 oldHours = SetTimeOfDay(12);
    SetMonData(mon, MON_DATA_HELD_ITEM, &item);
    EXPECT_EQ(Test_PartyTrainingActions(&count) & 2, 0);
    SetTimeOfDay(23);
    EXPECT_EQ(Test_PartyTrainingActions(&count) & 2, 2);
    EXPECT_EQ(GetMonData(mon, MON_DATA_HELD_ITEM), ITEM_RAZOR_CLAW);
    bool32 canStop = TRUE;
    EXPECT_EQ(GetEvolutionTargetSpecies(mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, &canStop, DO_EVO), SPECIES_WEAVILE);
    EXPECT_EQ(GetMonData(mon, MON_DATA_HELD_ITEM), ITEM_NONE);
    EXPECT_EQ(canStop, TRUE);
    EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), SPECIES_SNEASEL); // Scene applies the single target.
    EXPECT_EQ(Test_PartyTrainingActions(&count) & 2, 0);
    SetTimeOfDay(oldHours);
}


TEST("Party action list scrolls ten entries, selects absolute indices and cancels with B")
{
    SetupPartyTraining();
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    CreateMon(mon, SPECIES_CATERPIE, 10, 0, OTID_STRUCT_PRESET(0));
    SetMonMoveSlot(mon, MOVE_CUT, 0);
    SetMonMoveSlot(mon, MOVE_SURF, 1);
    SetMonMoveSlot(mon, MOVE_STRENGTH, 2);
    SetMonMoveSlot(mon, MOVE_FLASH, 3);
    const u16 keys[] = {DPAD_DOWN, DPAD_DOWN, DPAD_DOWN, DPAD_DOWN, DPAD_DOWN, DPAD_DOWN, DPAD_DOWN, DPAD_DOWN, DPAD_DOWN, A_BUTTON};
    u16 scroll;
    s16 cursor;
    u32 tasksBefore = 0, tasksAfter = 0;
    for (u32 i = 0; i < NUM_TASKS; i++)
        tasksBefore += gTasks[i].isActive;
    EXPECT_EQ(Test_PartyActionInput(keys, ARRAY_COUNT(keys), &scroll, &cursor), 9);
    EXPECT_EQ(cursor, 9);
    EXPECT_EQ(scroll, 2);
    const u16 cancel[] = {DPAD_DOWN, B_BUTTON};
    EXPECT_EQ(Test_PartyActionInput(cancel, ARRAY_COUNT(cancel), &scroll, &cursor), MENU_B_PRESSED);
    const u16 idle[] = {0};
    EXPECT_EQ(Test_PartyActionInput(idle, ARRAY_COUNT(idle), &scroll, &cursor), MENU_NOTHING_CHOSEN);
    for (u32 i = 0; i < NUM_TASKS; i++)
        tasksAfter += gTasks[i].isActive;
    EXPECT_EQ(tasksBefore, tasksAfter);
    EXPECT(GetStringWidth(FONT_NORMAL, COMPOUND_STRING("LEVEL TO CAP"), GetFontAttribute(FONT_NORMAL, FONTATTR_LETTER_SPACING))
        + GetMenuCursorDimensionByFont(FONT_NORMAL, 0) <= 11 * 8);
}

TEST("Party Evolve respects known moves and party composition and excludes item-use triggers")
{
    u8 count;
    SetupPartyTraining();
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    CreateMon(mon, SPECIES_BONSLY, 5, 0, OTID_STRUCT_PRESET(0));
    EXPECT_EQ(Test_PartyTrainingActions(&count) & 2, 0);
    SetMonMoveSlot(mon, MOVE_MIMIC, 0);
    EXPECT_EQ(Test_PartyTrainingActions(&count) & 2, 2);
    CreateMon(mon, SPECIES_MANTYKE, 5, 0, OTID_STRUCT_PRESET(0));
    EXPECT_EQ(Test_PartyTrainingActions(&count) & 2, 0);
    CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_REMORAID, 5, 0, OTID_STRUCT_PRESET(0));
    EXPECT_EQ(Test_PartyTrainingActions(&count) & 2, 2);
    CreateMon(mon, SPECIES_PIKACHU, 50, 0, OTID_STRUCT_PRESET(0));
    EXPECT_EQ(Test_PartyTrainingActions(&count) & 2, 0);
}
