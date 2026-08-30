#include "global.h"
#include "pokemon.h"
#include "test/test.h"
#include "wild_encounter.h"
#include "constants/map_groups.h"

// The public encounter-randomizer API is deliberately declared here before
// its production header exists. These tests specify the feature contract.
enum Species ResolveForkRandomizedEncounterSpecies(u8 mapGroup, u8 mapNum, enum WildPokemonArea area, u8 slot, enum Species fallback);
u16 GetForkEncounterRandomizerBstCap(u8 mapGroup, u8 mapNum, enum WildPokemonArea area);
u16 GetForkEncounterRandomizerBstMin(u8 mapGroup, u8 mapNum, enum WildPokemonArea area);

static u16 GetSpeciesBst(enum Species species)
{
    return gSpeciesInfo[species].baseHP
         + gSpeciesInfo[species].baseAttack
         + gSpeciesInfo[species].baseDefense
         + gSpeciesInfo[species].baseSpeed
         + gSpeciesInfo[species].baseSpAttack
         + gSpeciesInfo[species].baseSpDefense;
}

TEST("Biome encounter randomizer is stable for one save seed")
{
    enum Species first;
    enum Species second;

    gSaveBlock3Ptr->forkEncounterRandomizerSeed = 0x12345678;
    first = ResolveForkRandomizedEncounterSpecies(MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101), WILD_AREA_LAND, 0, SPECIES_ZIGZAGOON);
    second = ResolveForkRandomizedEncounterSpecies(MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101), WILD_AREA_LAND, 0, SPECIES_ZIGZAGOON);

    EXPECT_EQ(first, second);
}

TEST("Biome encounter randomizer obeys the configured place BST cap")
{
    enum Species species;
    u16 cap;

    gSaveBlock3Ptr->forkEncounterRandomizerSeed = 0x2468ACE0;
    cap = GetForkEncounterRandomizerBstCap(MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101), WILD_AREA_LAND);
    species = ResolveForkRandomizedEncounterSpecies(MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101), WILD_AREA_LAND, 3, SPECIES_ZIGZAGOON);

    EXPECT_NE(cap, 0);
    EXPECT_LE(GetSpeciesBst(species), cap);
}

TEST("Biome encounter randomizer uses route-specific BST ranges")
{
    EXPECT_EQ(GetForkEncounterRandomizerBstMin(MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101), WILD_AREA_LAND), 150);
    EXPECT_EQ(GetForkEncounterRandomizerBstCap(MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101), WILD_AREA_LAND), 250);
    EXPECT_EQ(GetForkEncounterRandomizerBstMin(MAP_GROUP(MAP_VICTORY_ROAD_1F), MAP_NUM(MAP_VICTORY_ROAD_1F), WILD_AREA_LAND), 490);
    EXPECT_EQ(GetForkEncounterRandomizerBstCap(MAP_GROUP(MAP_VICTORY_ROAD_1F), MAP_NUM(MAP_VICTORY_ROAD_1F), WILD_AREA_LAND), 560);
}

TEST("Biome encounter randomizer makes water tables water-compatible")
{
    enum Species species;

    gSaveBlock3Ptr->forkEncounterRandomizerSeed = 0x13579BDF;
    species = ResolveForkRandomizedEncounterSpecies(MAP_GROUP(MAP_ROUTE118), MAP_NUM(MAP_ROUTE118), WILD_AREA_WATER, 1, SPECIES_TENTACOOL);

    EXPECT(gSpeciesInfo[species].types[0] == TYPE_WATER || gSpeciesInfo[species].types[1] == TYPE_WATER);
}

TEST("Biome encounter randomizer avoids duplicate species within one land table")
{
    enum Species species[LAND_WILD_COUNT];
    u32 i;
    u32 j;

    gSaveBlock3Ptr->forkEncounterRandomizerSeed = 0x0BADF00D;
    for (i = 0; i < ARRAY_COUNT(species); i++)
        species[i] = ResolveForkRandomizedEncounterSpecies(MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101), WILD_AREA_LAND, i, SPECIES_ZIGZAGOON);
    for (i = 0; i < ARRAY_COUNT(species); i++)
        for (j = i + 1; j < ARRAY_COUNT(species); j++)
            EXPECT_NE(species[i], species[j]);
}

TEST("Biome encounter randomizer defaults to Generation 3 species")
{
    gSaveBlock3Ptr->forkEncounterRandomizerSeed = 0x10203040;

    for (u8 slot = 0; slot < LAND_WILD_COUNT; slot++)
    {
        enum Species species = ResolveForkRandomizedEncounterSpecies(
            MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101), WILD_AREA_LAND, slot, SPECIES_ZIGZAGOON);

        EXPECT_LE((u16)gSpeciesInfo[species].natDexNum, NATIONAL_DEX_DEOXYS);
    }
}
