#include "global.h"
#include "coins.h"
#include "event_data.h"
#include "item.h"
#include "item_ball.h"
#include "fork_randomizer_catalog.h"
#include "test/test.h"
#include "constants/flags.h"
#include "constants/items.h"

#if IS_FRLG
void CollectorTrade(void);
void Test_CollectorLeaveOnlyPotion(void);
void Test_CollectorLeaveOnlyEarlyBall(void);

TEST("FRLG collector refuses insufficient coins")
{
    Test_CollectorLeaveOnlyPotion();
    FlagClear(FLAG_COLLECTOR_TRADE_COMPLETE);
    SetCoins(3999);
    CollectorTrade();
    EXPECT_EQ(gSpecialVar_Result, 1);
    EXPECT_EQ(GetCoins(), 3999);
    EXPECT_EQ(FlagGet(FLAG_HIDE_VIRIDIAN_FOREST_POTION), FALSE);
}

TEST("FRLG collector grants remaining Kanto item ball once")
{
    gSaveBlock3Ptr->forkGameplayOptionsConfigured = TRUE;
    gSaveBlock3Ptr->forkItemRandomizerEnabled = FALSE;
    Test_CollectorLeaveOnlyPotion();
    FlagClear(FLAG_COLLECTOR_TRADE_COMPLETE);
    SetCoins(4000);
    CollectorTrade();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT_EQ(GetCoins(), 0);
    EXPECT_EQ(FlagGet(FLAG_HIDE_VIRIDIAN_FOREST_POTION), TRUE);
    EXPECT(CheckBagHasItem(ITEM_POTION, 1));
    CollectorTrade();
    EXPECT_EQ(gSpecialVar_Result, 4);
}

TEST("FRLG collector uses stable randomized reward")
{
    enum Item reward;
    gSaveBlock3Ptr->forkGameplayOptionsConfigured = TRUE;
    gSaveBlock3Ptr->forkItemRandomizerEnabled = TRUE;
    gSaveBlock3Ptr->forkItemRandomizerSeed = 12345;
    ResetForkItemRandomizerState();
    Test_CollectorLeaveOnlyPotion();
    FlagClear(FLAG_COLLECTOR_TRADE_COMPLETE);
    reward = ResolveForkRandomizedItem(ITEM_POTION, 0);
    SetCoins(4000);
    CollectorTrade();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT(CheckBagHasItem(reward, 1));
}

TEST("FRLG collector matches early item ball randomizer fallback")
{
    enum Item reward;
    gSaveBlock3Ptr->forkGameplayOptionsConfigured = TRUE;
    gSaveBlock3Ptr->forkItemRandomizerEnabled = TRUE;
    gSaveBlock3Ptr->forkItemRandomizerSeed = 4321;
    ResetForkItemRandomizerState();
    Test_CollectorLeaveOnlyEarlyBall();
    FlagClear(FLAG_COLLECTOR_TRADE_COMPLETE);
    reward = ResolveForkRandomizedItem(ITEM_POKE_BALL, FORK_ITEM_RANDOMIZER_SOURCE_COUNT - 1);
    SetCoins(4000);
    CollectorTrade();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT_EQ(FlagGet(FLAG_HIDE_VIRIDIAN_FOREST_POKE_BALL), TRUE);
    EXPECT(CheckBagHasItem(reward, 1));
}

TEST("FRLG collector leaves coins and pickups intact if Bag cannot fit reward")
{
    struct BagPocket *pocket = &gBagPockets[POCKET_ITEMS];
    gSaveBlock3Ptr->forkGameplayOptionsConfigured = TRUE;
    gSaveBlock3Ptr->forkItemRandomizerEnabled = FALSE;
    Test_CollectorLeaveOnlyPotion();
    FlagClear(FLAG_COLLECTOR_TRADE_COMPLETE);
    for (u32 i = 0; i < pocket->capacity; i++)
        BagPocket_SetSlotItemIdAndCount(pocket, i, ITEM_ANTIDOTE, MAX_BAG_ITEM_CAPACITY);
    SetCoins(4000);
    CollectorTrade();
    EXPECT_EQ(gSpecialVar_Result, 2);
    EXPECT_EQ(GetCoins(), 4000);
    EXPECT_EQ(FlagGet(FLAG_HIDE_VIRIDIAN_FOREST_POTION), FALSE);
    EXPECT_EQ(FlagGet(FLAG_COLLECTOR_TRADE_COMPLETE), FALSE);
}
#endif
