#include "global.h"
#include "event_data.h"
#include "fork_fossil_randomizer.h"
#include "item_ball.h"
#include "pokemon.h"
#include "script.h"
#include "test/test.h"

enum Item GetForkMirageTowerFossil(u8 choice);
enum Species GetForkFossilRevivalSpecies(enum Item item);

static bool8 IsRevivalFossil(enum Item item)
{
    switch (item)
    {
    case ITEM_OLD_AMBER:
    case ITEM_HELIX_FOSSIL:
    case ITEM_DOME_FOSSIL:
    case ITEM_ROOT_FOSSIL:
    case ITEM_CLAW_FOSSIL:
    case ITEM_ARMOR_FOSSIL:
    case ITEM_SKULL_FOSSIL:
    case ITEM_COVER_FOSSIL:
    case ITEM_PLUME_FOSSIL:
    case ITEM_JAW_FOSSIL:
    case ITEM_SAIL_FOSSIL:
        return TRUE;
    default:
        return FALSE;
    }
}

static bool8 IsAnyFossilItem(enum Item item)
{
    return item >= ITEM_HELIX_FOSSIL && item <= ITEM_FOSSILIZED_DINO;
}

TEST("Mirage Tower offers two distinct revival fossils")
{
    enum Item first;
    enum Item second;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 0x12345678;
    first = GetForkMirageTowerFossil(0);
    second = GetForkMirageTowerFossil(1);

    EXPECT(IsRevivalFossil(first));
    EXPECT(IsRevivalFossil(second));
    EXPECT_NE(first, second);
}

TEST("Every Mirage Tower fossil has its matching revival Pokémon")
{
    EXPECT_EQ(GetForkFossilRevivalSpecies(ITEM_OLD_AMBER), SPECIES_AERODACTYL);
    EXPECT_EQ(GetForkFossilRevivalSpecies(ITEM_HELIX_FOSSIL), SPECIES_OMANYTE);
    EXPECT_EQ(GetForkFossilRevivalSpecies(ITEM_DOME_FOSSIL), SPECIES_KABUTO);
    EXPECT_EQ(GetForkFossilRevivalSpecies(ITEM_ROOT_FOSSIL), SPECIES_LILEEP);
    EXPECT_EQ(GetForkFossilRevivalSpecies(ITEM_CLAW_FOSSIL), SPECIES_ANORITH);
    EXPECT_EQ(GetForkFossilRevivalSpecies(ITEM_ARMOR_FOSSIL), SPECIES_SHIELDON);
    EXPECT_EQ(GetForkFossilRevivalSpecies(ITEM_SKULL_FOSSIL), SPECIES_CRANIDOS);
    EXPECT_EQ(GetForkFossilRevivalSpecies(ITEM_COVER_FOSSIL), SPECIES_TIRTOUGA);
    EXPECT_EQ(GetForkFossilRevivalSpecies(ITEM_PLUME_FOSSIL), SPECIES_ARCHEN);
    EXPECT_EQ(GetForkFossilRevivalSpecies(ITEM_JAW_FOSSIL), SPECIES_TYRUNT);
    EXPECT_EQ(GetForkFossilRevivalSpecies(ITEM_SAIL_FOSSIL), SPECIES_AMAURA);
}

TEST("Fossil special functions return their script values")
{
    gSpecialVar_0x8004 = ITEM_CLAW_FOSSIL;
    EXPECT_EQ(GetForkFossilRevivalSpeciesSpecial(), SPECIES_ANORITH);
}

TEST("Item randomizer does not award fossil items")
{
    gSaveBlock3Ptr->forkItemRandomizerSeed = 0x87654321;
    ResetForkItemRandomizerState();

    for (u16 source = 0; source < FORK_ITEM_RANDOMIZER_POOL_COUNT; source++)
        EXPECT(!IsAnyFossilItem(ResolveForkRandomizedItem(ITEM_POTION, source)));
}
