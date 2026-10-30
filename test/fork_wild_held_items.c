#include "global.h"
#include "battle.h"
#include "pokemon.h"
#include "test/test.h"

TEST("Wild Pokemon start without held items")
{
    u32 flags;
    u32 i;
    PARAMETRIZE { flags = 0; }
    PARAMETRIZE { flags = BATTLE_TYPE_DOUBLE; }
    PARAMETRIZE { flags = BATTLE_TYPE_LEGENDARY; }
    PARAMETRIZE { flags = BATTLE_TYPE_PYRAMID; }
    PARAMETRIZE { flags = BATTLE_TYPE_PIKE; }

    gBattleTypeFlags = flags;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        u16 item = ITEM_ORAN_BERRY;
        CreateMon(&gParties[B_TRAINER_OPPONENT_A][i], SPECIES_CHANSEY, 10, 0, OTID_STRUCT_PLAYER_ID);
        SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_HELD_ITEM, &item);
    }

    SetWildMonHeldItem();

    for (i = 0; i < PARTY_SIZE; i++)
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_HELD_ITEM), ITEM_NONE);
}

TEST("Wild held item rule preserves trainer and multiplayer items")
{
    u32 flags;
    u16 item = ITEM_ORAN_BERRY;
    PARAMETRIZE { flags = BATTLE_TYPE_TRAINER; }
    PARAMETRIZE { flags = BATTLE_TYPE_LINK; }
    PARAMETRIZE { flags = BATTLE_TYPE_RECORDED; }

    gBattleTypeFlags = flags;
    CreateMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_CHANSEY, 10, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HELD_ITEM, &item);

    SetWildMonHeldItem();

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HELD_ITEM), item);
}
