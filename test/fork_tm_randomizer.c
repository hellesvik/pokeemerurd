#include "global.h"
#include "battle_main.h"
#include "item.h"
#include "item_icon.h"
#include "graphics.h"
#include "move.h"
#include "pokemon.h"
#include "test/test.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/tms_hms.h"

TEST("Gen9 TM catalog has fixed assignments and correct descriptions for every seed")
{
    EXPECT_EQ(NUM_TECHNICAL_MACHINES, 229);
    for (u32 seed = 1; seed < 4; seed++)
    {
        gSaveBlock3Ptr->forkItemRandomizerSeed = seed;
        for (u32 tm = 1; tm <= NUM_TECHNICAL_MACHINES; tm++)
        {
            enum Item item = GetTMHMItemId(tm);
            enum Move move = GetTMHMMoveId(tm);
            EXPECT_NE(item, ITEM_NONE);
            EXPECT_NE(move, MOVE_NONE);
            EXPECT_EQ(GetItemTMHMIndex(item), tm);
            EXPECT_EQ(GetItemTMHMMoveId(item), move);
            EXPECT_EQ(GetTMHMItemIdFromMoveId(move), item);
            EXPECT_EQ(GetItemDescription(item), GetMoveDescription(move));
            EXPECT_EQ(GetItemPocket(item), POCKET_TM_HM);
            EXPECT_EQ(GetItemIconPic(item), gItemIcon_TM);
            EXPECT_EQ(GetItemIconPalette(item), gTypesInfo[GetMoveType(move)].paletteTMHM);
        }
    }
    EXPECT_EQ(GetItemTMHMMoveId(ITEM_TM01), MOVE_TAKE_DOWN);
    EXPECT_EQ(GetItemTMHMMoveId(ITEM_TM50), MOVE_RAIN_DANCE);
    EXPECT_EQ(GetItemTMHMMoveId(ITEM_TM100), MOVE_DRAGON_DANCE);
    EXPECT_EQ(GetItemTMHMMoveId(ITEM_TM171), MOVE_TERA_BLAST);
    EXPECT_EQ(GetItemTMHMMoveId(ITEM_TM201), MOVE_MISTY_EXPLOSION);
    EXPECT_EQ(GetItemTMHMMoveId(ITEM_TM229), MOVE_UPPER_HAND);
    EXPECT_EQ(GetTMHMItemId(NUM_ALL_MACHINES + 1), ITEM_NONE);
    EXPECT_EQ(GetItemTMHMMoveId(ITEM_POTION), MOVE_NONE);
}

TEST("Gen9 TM catalog preserves HMs and prefers TMs for shared moves")
{
#define CHECK_HM(move) EXPECT_EQ(GetItemTMHMMoveId(ITEM_HM_##move), MOVE_##move); EXPECT_GT(GetItemTMHMIndex(ITEM_HM_##move), NUM_TECHNICAL_MACHINES);
    FOREACH_HM(CHECK_HM)
#undef CHECK_HM
    EXPECT_EQ(GetTMHMItemIdFromMoveId(MOVE_SURF), ITEM_TM123);
    EXPECT_EQ(GetTMHMItemIdFromMoveId(MOVE_FLY), ITEM_TM97);
    EXPECT_EQ(GetTMHMItemIdFromMoveId(MOVE_WATERFALL), ITEM_TM77);
    EXPECT(CanLearnTeachableMove(SPECIES_PIKACHU, GetItemTMHMMoveId(ITEM_TM126)));
    EXPECT(!CanLearnTeachableMove(SPECIES_MAGIKARP, GetItemTMHMMoveId(ITEM_TM126)));
}

TEST("Gen9 TM Lilycove shop sells each selected TM only once")
{
    enum Item stock[FORK_LILYCOVE_TM_SHOP_ITEM_COUNT];

    gSaveBlock3Ptr->forkItemRandomizerSeed = 12345;
    gSaveBlock3Ptr->forkLilycoveTmShopPurchases = 0;

    for (u32 i = 0; i < ARRAY_COUNT(stock); i++)
    {
        stock[i] = ResolveForkLilycoveTmShopItem(ITEM_TM51 + i);
        EXPECT(IsForkLilycoveTmShopItem(stock[i]));
        for (u32 j = 0; j < i; j++)
            EXPECT_NE(stock[i], stock[j]);
    }
    EXPECT_NE(stock[0], ITEM_TM51);
    EXPECT(!HasForkLilycoveTmShopItemBeenPurchased(stock[0]));

    MarkForkLilycoveTmShopItemPurchased(stock[0]);

    EXPECT(HasForkLilycoveTmShopItemBeenPurchased(stock[0]));
    EXPECT(!HasForkLilycoveTmShopItemBeenPurchased(stock[1]));
}
