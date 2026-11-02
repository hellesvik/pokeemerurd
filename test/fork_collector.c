#include "global.h"
#include "coins.h"
#include "event_data.h"
#include "item.h"
#include "test/test.h"
void CollectorTrade(void);
void CollectorNextReceiptItem(void);
TEST("Collector refuses insufficient coins without removing pickups")
{
    FlagClear(FLAG_COLLECTOR_TRADE_COMPLETE);
    FlagClear(FLAG_ITEM_ROUTE_102_POTION);
    SetCoins(3999);
    CollectorTrade();
    EXPECT_EQ(gSpecialVar_Result, 1);
    EXPECT_EQ(GetCoins(), 3999);
    EXPECT_EQ(FlagGet(FLAG_ITEM_ROUTE_102_POTION), FALSE);
    EXPECT_EQ(FlagGet(FLAG_COLLECTOR_TRADE_COMPLETE), FALSE);
}
TEST("Collector refuses a repeated purchase without charging coins")
{
    FlagSet(FLAG_COLLECTOR_TRADE_COMPLETE);
    SetCoins(4000);
    CollectorTrade();
    EXPECT_EQ(gSpecialVar_Result, 4);
    EXPECT_EQ(GetCoins(), 4000);
}

void Test_CollectorLeaveOnlyPotion(void);
TEST("Collector grants remaining pickups once and saves their collection flags")
{
    gSaveBlock3Ptr->forkGameplayOptionsConfigured = TRUE;
    gSaveBlock3Ptr->forkItemRandomizerEnabled = FALSE;
    Test_CollectorLeaveOnlyPotion();
    FlagClear(FLAG_COLLECTOR_TRADE_COMPLETE);
    SetCoins(4000);
    CollectorTrade();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT_EQ(GetCoins(), 0);
    EXPECT_EQ(FlagGet(FLAG_ITEM_ROUTE_102_POTION), TRUE);
    EXPECT_EQ(FlagGet(FLAG_COLLECTOR_TRADE_COMPLETE), TRUE);
    EXPECT(CheckBagHasItem(ITEM_POTION, 1));
    CollectorNextReceiptItem();
    EXPECT_EQ(gSpecialVar_Result, 1);
    CollectorNextReceiptItem();
    EXPECT_EQ(gSpecialVar_Result, 0);
}
TEST("Collector rolls back a full Items pocket without charging or collecting")
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
    CollectorNextReceiptItem();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT_EQ(GetCoins(), 4000);
    EXPECT_EQ(FlagGet(FLAG_ITEM_ROUTE_102_POTION), FALSE);
    EXPECT_EQ(FlagGet(FLAG_COLLECTOR_TRADE_COMPLETE), FALSE);
    for (u32 i = 0; i < pocket->capacity; i++)
        EXPECT_EQ(BagPocket_GetSlotData(pocket, i).itemId, ITEM_ANTIDOTE);
}
