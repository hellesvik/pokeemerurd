#include "global.h"
#include "battle.h"
#include "battle_util.h"
#include "pokemon.h"
#include "test/test.h"
#include "constants/items.h"

TEST("Consumed held berries are restored after battle")
{
    u16 item = ITEM_ORAN_BERRY;
    u16 noItem = ITEM_NONE;

    gBattleStruct->itemLost[B_SIDE_PLAYER][0].originalItem = ITEM_ORAN_BERRY;
    gBattleStruct->itemLost[B_SIDE_PLAYER][0].stolen = FALSE;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &noItem);

    TryRestoreHeldItems();

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), item);
}
