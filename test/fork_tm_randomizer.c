#include "global.h"
#include "item.h"
#include "pokemon.h"
#include "test/test.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/tms_hms.h"

void InitForkRandomizedTMMoves(void);

TEST("Lilycove TM shop sells each randomized TM only once")
{
    gSaveBlock3Ptr->forkLilycoveTmShopPurchases = 0;

    EXPECT(IsForkLilycoveTmShopItem(ITEM_TM51));
    EXPECT(!IsForkLilycoveTmShopItem(ITEM_TM59));
    EXPECT(!HasForkLilycoveTmShopItemBeenPurchased(ITEM_TM51));

    MarkForkLilycoveTmShopItemPurchased(ITEM_TM51);

    EXPECT(HasForkLilycoveTmShopItemBeenPurchased(ITEM_TM51));
    EXPECT(!HasForkLilycoveTmShopItemBeenPurchased(ITEM_TM52));
}

TEST("Fork TM randomizer assigns distinct normally teachable moves")
{
    enum Move moves[80];
    bool8 eligibleMoves[MOVES_COUNT] = {0};
    u16 eligibleCount = 0;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 12345;
    InitForkRandomizedTMMoves();
    EXPECT_EQ(GetForkRandomizedTMItem(131), ITEM_NONE);

    for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        const u16 *learnset;

        if (!IsSpeciesEnabled(species))
            continue;
        learnset = GetSpeciesTeachableLearnset(species);
        for (u32 i = 0; learnset[i] != MOVE_UNAVAILABLE; i++)
        {
            if (learnset[i] > MOVE_NONE
             && learnset[i] < MOVES_COUNT
             && learnset[i] != MOVE_CUT
             && learnset[i] != MOVE_FLY
             && learnset[i] != MOVE_SURF
             && learnset[i] != MOVE_STRENGTH
             && learnset[i] != MOVE_FLASH
             && learnset[i] != MOVE_ROCK_SMASH
             && learnset[i] != MOVE_WATERFALL
             && learnset[i] != MOVE_DIVE
             && CanLearnTeachableMove(species, learnset[i])
             && !eligibleMoves[learnset[i]])
            {
                eligibleMoves[learnset[i]] = TRUE;
                eligibleCount++;
            }
        }
    }
    EXPECT_EQ(eligibleCount, ARRAY_COUNT(moves));

    for (u32 i = 0; i < ARRAY_COUNT(moves); i++)
    {
        bool32 canBeTaught = FALSE;

        moves[i] = GetForkRandomizedTMMove(GetForkRandomizedTMItem(51 + i));
        EXPECT_NE(moves[i], MOVE_NONE);
        EXPECT_LT(moves[i], MOVES_COUNT);
        EXPECT_NE(moves[i], MOVE_CUT);
        EXPECT_NE(moves[i], MOVE_FLY);
        EXPECT_NE(moves[i], MOVE_SURF);
        EXPECT_NE(moves[i], MOVE_STRENGTH);
        EXPECT_NE(moves[i], MOVE_FLASH);
        EXPECT_NE(moves[i], MOVE_ROCK_SMASH);
        EXPECT_NE(moves[i], MOVE_WATERFALL);
        EXPECT_NE(moves[i], MOVE_DIVE);
        for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
        {
            if (IsSpeciesEnabled(species) && CanLearnTeachableMove(species, moves[i]))
            {
                canBeTaught = TRUE;
                break;
            }
        }
        EXPECT(canBeTaught);
        for (u32 j = 0; j < i && i < eligibleCount; j++)
            EXPECT_NE(moves[i], moves[j]);
    }
}
