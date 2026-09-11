#include "global.h"
#include "event_data.h"
#include "item_ball.h"
#include "item.h"
#include "malloc.h"
#include "random.h"
#include "script_menu.h"
#include "test/test.h"
#include "constants/items.h"
#include "constants/moves.h"

TEST("Fork item randomizer preserves protected items")
{
    gSaveBlock3Ptr->forkItemRandomizerSeed = 12345;
    ResetForkItemRandomizerState();

    EXPECT_EQ(ResolveForkRandomizedItem(ITEM_HM01, 7), ITEM_HM01);
    EXPECT_EQ(ResolveForkRandomizedItem(ITEM_DEVON_SCOPE, 9), ITEM_DEVON_SCOPE);
}

TEST("Fork item-ball resolver safely rejects an invalid object context")
{
    gSpecialVar_LastTalked = 0;
    gSpecialVar_Result = ITEM_POTION;
    gSpecialVar_0x8009 = 1;

    GetItemBallIdAndAmountFromTemplate();

    EXPECT_EQ(gSpecialVar_Result, ITEM_NONE);
    EXPECT_EQ(gSpecialVar_0x8009, 0);
}

TEST("Fork item randomizer is stable per source and seed")
{
    enum Item item1;
    enum Item item2;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 54321;
    ResetForkItemRandomizerState();
    item1 = ResolveForkRandomizedItem(ITEM_POTION, 11);
    item2 = ResolveForkRandomizedItem(ITEM_POTION, 11);

    EXPECT_EQ(item1, item2);
}

TEST("Fork item ball randomization is consumed only once")
{
    enum Item displayedItem;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 54321;
    ResetForkItemRandomizerState();
    displayedItem = ResolveForkRandomizedItemBall(ITEM_POTION, 11);

    EXPECT_EQ(displayedItem, ResolveForkRandomizedItem(ITEM_POTION, 11));
    EXPECT_EQ(ConsumeForkItemBallRandomizationGuard(), TRUE);
    EXPECT_EQ(ConsumeForkItemBallRandomizationGuard(), FALSE);
}

TEST("Fork item randomizer varies by source")
{
    gSaveBlock3Ptr->forkItemRandomizerSeed = 54321;
    ResetForkItemRandomizerState();

    EXPECT_NE(ResolveForkRandomizedItem(ITEM_POTION, 11), ResolveForkRandomizedItem(ITEM_POTION, 12));
}

TEST("Game Corner sells every evolution stone and the Linking Cord")
{
    static const enum Item expectedPrizes[] =
    {
        ITEM_FIRE_STONE,
        ITEM_WATER_STONE,
        ITEM_THUNDER_STONE,
        ITEM_LEAF_STONE,
        ITEM_ICE_STONE,
        ITEM_SUN_STONE,
        ITEM_MOON_STONE,
        ITEM_SHINY_STONE,
        ITEM_DUSK_STONE,
        ITEM_DAWN_STONE,
        ITEM_LINKING_CORD,
    };

    for (u32 i = 0; i < ARRAY_COUNT(expectedPrizes); i++)
    {
        EXPECT_EQ(GetForkGameCornerPrizeItem(i), expectedPrizes[i]);
    }
    EXPECT_EQ(GetForkGameCornerPrizeItem(ARRAY_COUNT(expectedPrizes)), ITEM_NONE);
}

TEST("Game Corner prize menu names use static storage")
{
    struct ListMenuItem *menuItem;

    ScrCmd_BuildForkGameCornerPrizeMenu(NULL);
    for (u32 i = 0; i < FORK_GAME_CORNER_MENU_PRIZE_COUNT + 1; i++)
    {
        menuItem = MultichoiceDynamic_PopElement();
        EXPECT(menuItem != NULL);
        EXPECT((const u8 *)menuItem->name < gHeap || (const u8 *)menuItem->name >= gHeap + HEAP_SIZE);
        if ((const u8 *)menuItem->name >= gHeap && (const u8 *)menuItem->name < gHeap + HEAP_SIZE)
            Free((void *)menuItem->name);
    }
    MultichoiceDynamic_DestroyStack();
}

TEST("Fork item randomizer hidden items are stable by hidden-item flag")
{
    enum Item item1;
    enum Item item2;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 98765;
    ResetForkItemRandomizerState();
    item1 = ResolveForkRandomizedHiddenItem(ITEM_POTION, FLAG_HIDDEN_ITEMS_START + 3);
    item2 = ResolveForkRandomizedHiddenItem(ITEM_POTION, FLAG_HIDDEN_ITEMS_START + 3);

    EXPECT_EQ(item1, item2);
}

TEST("Fork hidden item randomization is consumed only once")
{
    enum Item displayedItem;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 24680;
    ResetForkItemRandomizerState();
    displayedItem = ResolveForkRandomizedHiddenItem(ITEM_POTION, FLAG_HIDDEN_ITEMS_START + 4);

    EXPECT_EQ(displayedItem, ResolveForkRandomizedHiddenItem(ITEM_POTION, FLAG_HIDDEN_ITEMS_START + 4));
    EXPECT_EQ(ConsumeForkHiddenItemRandomizationGuard(), TRUE);
    EXPECT_EQ(ConsumeForkHiddenItemRandomizationGuard(), FALSE);
}

TEST("Fork hidden items use repeatable ball and treasure rewards")
{
    enum Item firstVisibleItem;
    enum Item repeatedVisibleItem;
    enum Item hiddenItems[32];
    bool32 foundDuplicate = FALSE;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 24681;
    ResetForkItemRandomizerState();
    firstVisibleItem = ResolveForkRandomizedItem(ITEM_POTION, 1);

    for (u16 i = 0; i < ARRAY_COUNT(hiddenItems); i++)
    {
        bool32 allowed = FALSE;

        hiddenItems[i] = ResolveForkRandomizedHiddenItem(ITEM_POTION, FLAG_HIDDEN_ITEMS_START + i);
        switch (hiddenItems[i])
        {
        case ITEM_POKE_BALL:
        case ITEM_GREAT_BALL:
        case ITEM_ULTRA_BALL:
        case ITEM_PREMIER_BALL:
        case ITEM_HEAL_BALL:
        case ITEM_NET_BALL:
        case ITEM_NEST_BALL:
        case ITEM_DIVE_BALL:
        case ITEM_DUSK_BALL:
        case ITEM_TIMER_BALL:
        case ITEM_QUICK_BALL:
        case ITEM_REPEAT_BALL:
        case ITEM_LUXURY_BALL:
        case ITEM_TINY_MUSHROOM:
        case ITEM_BIG_MUSHROOM:
        case ITEM_NUGGET:
        case ITEM_BIG_NUGGET:
        case ITEM_BALM_MUSHROOM:
        case ITEM_PEARL:
        case ITEM_BIG_PEARL:
        case ITEM_PEARL_STRING:
        case ITEM_STARDUST:
        case ITEM_STAR_PIECE:
        case ITEM_COMET_SHARD:
        case ITEM_RARE_BONE:
            allowed = TRUE;
            break;
        default:
            break;
        }
        EXPECT(allowed);
        for (u16 j = 0; j < i; j++)
        {
            if (hiddenItems[i] == hiddenItems[j])
                foundDuplicate = TRUE;
        }
    }

    repeatedVisibleItem = ResolveForkRandomizedItem(ITEM_POTION, 1);
    EXPECT(foundDuplicate);
    EXPECT_EQ(firstVisibleItem, repeatedVisibleItem);
}

TEST("Fork item randomizer randomizes Brendans room hidden-item test source")
{
    enum Item item1;
    enum Item item2;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 13579;
    ResetForkItemRandomizerState();
    item1 = ResolveForkRandomizedHiddenItem(ITEM_POTION, FLAG_HIDDEN_ITEM_LITTLEROOT_TOWN_BRENDANS_HOUSE_2F_TEST);
    item2 = ResolveForkRandomizedHiddenItem(ITEM_POTION, FLAG_HIDDEN_ITEM_LITTLEROOT_TOWN_BRENDANS_HOUSE_2F_TEST);

    EXPECT_EQ(item1, item2);
    EXPECT_NE(item1, ITEM_POTION);
}

TEST("Fork item randomizer scripted poke balls stay vanilla")
{
    static const u8 sFakeScript[] = {0x00};

    gSaveBlock3Ptr->forkItemRandomizerSeed = 11111;
    ResetForkItemRandomizerState();

    EXPECT_EQ(ResolveForkRandomizedScriptItem(ITEM_POKE_BALL, sFakeScript), ITEM_POKE_BALL);
}

TEST("Fork item randomizer leaves ordinary scripted rewards vanilla")
{
    static const u8 sFakeScript[] = {0x00};

    gSaveBlock3Ptr->forkItemRandomizerSeed = 11112;
    ResetForkItemRandomizerState();

    EXPECT_EQ(ResolveForkRandomizedScriptItem(ITEM_POTION, sFakeScript), ITEM_POTION);
}

TEST("Fork item randomizer includes gym leader TM gifts")
{
    static const u8 sFakeScript[] = {0x00};
    enum Item randomizedItem;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 11113;
    ResetForkItemRandomizerState();
    randomizedItem = ResolveForkRandomizedScriptItem(ITEM_TM_ROCK_TOMB, sFakeScript);

    EXPECT_NE(randomizedItem, ITEM_TM_ROCK_TOMB);
}

TEST("Fork item randomizer does not duplicate early source assignments")
{
    enum Item items[8];

    gSaveBlock3Ptr->forkItemRandomizerSeed = 22222;
    ResetForkItemRandomizerState();

    for (u32 i = 0; i < ARRAY_COUNT(items); i++)
        items[i] = ResolveForkRandomizedItem(ITEM_POTION, i);

    for (u32 i = 0; i < ARRAY_COUNT(items); i++)
    {
        for (u32 j = i + 1; j < ARRAY_COUNT(items); j++)
            EXPECT_NE(items[i], items[j]);
    }
}

TEST("Fork item randomizer leaves later sources vanilla after the unique pool is exhausted")
{
    gSaveBlock3Ptr->forkItemRandomizerSeed = 22223;
    ResetForkItemRandomizerState();

    for (u16 sourceId = 0; sourceId < FORK_ITEM_RANDOMIZER_POOL_COUNT; sourceId++)
        ResolveForkRandomizedItem(ITEM_POTION, sourceId);

    EXPECT_EQ(ResolveForkRandomizedItem(ITEM_POTION, FORK_ITEM_RANDOMIZER_POOL_COUNT), ITEM_POTION);
}

TEST("Fork item randomizer protects hidden progression items")
{
    gSaveBlock3Ptr->forkItemRandomizerSeed = 33333;
    ResetForkItemRandomizerState();

    EXPECT_EQ(ResolveForkRandomizedHiddenItem(ITEM_KEY_TO_ROOM_1, FLAG_HIDDEN_ITEMS_START + 1), ITEM_KEY_TO_ROOM_1);
}

TEST("Fork item randomizer protects scripted HM gifts")
{
    static const u8 sFakeScript[] = {0x00};

    gSaveBlock3Ptr->forkItemRandomizerSeed = 44444;
    ResetForkItemRandomizerState();

    EXPECT_EQ(ResolveForkRandomizedScriptItem(ITEM_HM_FLY, sFakeScript), ITEM_HM_FLY);
}

TEST("Fork item randomizer scripted rewards are location-stable across call order")
{
    static const u8 sScriptA[] = {0x00};
    static const u8 sScriptB[] = {0x00};
    enum Item itemAFirst;
    enum Item itemBSecond;
    enum Item itemBFirst;
    enum Item itemASecond;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 55555;
    ResetForkItemRandomizerState();
    itemAFirst = ResolveForkRandomizedScriptItem(ITEM_POTION, sScriptA);
    itemBSecond = ResolveForkRandomizedScriptItem(ITEM_SUPER_POTION, sScriptB);

    gSaveBlock3Ptr->forkItemRandomizerSeed = 55555;
    ResetForkItemRandomizerState();
    itemBFirst = ResolveForkRandomizedScriptItem(ITEM_SUPER_POTION, sScriptB);
    itemASecond = ResolveForkRandomizedScriptItem(ITEM_POTION, sScriptA);

    EXPECT_EQ(itemAFirst, itemASecond);
    EXPECT_EQ(itemBFirst, itemBSecond);
}

TEST("Fork item randomizer direct item sources are location-stable across call order")
{
    enum Item item0First;
    enum Item item1Second;
    enum Item item1First;
    enum Item item0Second;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 66666;
    ResetForkItemRandomizerState();
    item0First = ResolveForkRandomizedItem(ITEM_POTION, 0);
    item1Second = ResolveForkRandomizedItem(ITEM_SUPER_POTION, 1);

    gSaveBlock3Ptr->forkItemRandomizerSeed = 66666;
    ResetForkItemRandomizerState();
    item1First = ResolveForkRandomizedItem(ITEM_SUPER_POTION, 1);
    item0Second = ResolveForkRandomizedItem(ITEM_POTION, 0);

    EXPECT_EQ(item0First, item0Second);
    EXPECT_EQ(item1First, item1Second);
}

TEST("Fork item randomizer does not yield placeholder TMs")
{
    gSaveBlock3Ptr->forkItemRandomizerSeed = 77777;
    ResetForkItemRandomizerState();

    for (u16 sourceId = 0; sourceId < FORK_ITEM_RANDOMIZER_SOURCE_COUNT; sourceId++)
    {
        enum Item item = ResolveForkRandomizedItem(ITEM_POTION, sourceId);
        if (GetItemPocket(item) == POCKET_TM_HM)
            EXPECT_NE(GetItemTMHMMoveId(item), MOVE_NONE);
    }
}

TEST("Fork item randomizer includes every randomized TM")
{
    u16 randomizedTMCount = 0;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 77778;
    ResetForkItemRandomizerState();

    for (u16 sourceId = 0; sourceId < FORK_ITEM_RANDOMIZER_POOL_COUNT; sourceId++)
    {
        enum Item item = ResolveForkRandomizedItem(ITEM_POTION, sourceId);

        if ((item >= ITEM_TM51 && item <= ITEM_TM100)
         || (item >= ITEM_TM101 && item <= ITEM_TM130))
            randomizedTMCount++;
    }
    EXPECT_EQ(randomizedTMCount, 80);
}

TEST("Fork item randomizer does not yield Plates or Incenses")
{
    gSaveBlock3Ptr->forkItemRandomizerSeed = 88888;
    ResetForkItemRandomizerState();

    for (u16 sourceId = 0; sourceId < FORK_ITEM_RANDOMIZER_SOURCE_COUNT; sourceId++)
    {
        enum Item item = ResolveForkRandomizedItem(ITEM_POTION, sourceId);
        EXPECT_NE(gItemsInfo[item].sortType, ITEM_TYPE_PLATE);
        EXPECT_NE(gItemsInfo[item].sortType, ITEM_TYPE_INCENSE);
    }
}
