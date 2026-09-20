#include "global.h"
#include "pokemon.h"
#include "test/test.h"
#include "wild_encounter.h"
#include "fork_encounter_randomizer.h"
#include "fork_run.h"
#include "constants/map_groups.h"

enum ForkEncounterBiomeForTest
{
    FORK_BIOME_CITY,
    FORK_BIOME_GRASSL,
    FORK_BIOME_FOREST,
    FORK_BIOME_MOUNTAIN,
    FORK_BIOME_CAVE,
    FORK_BIOME_DESERT,
    FORK_BIOME_VOLCANIC,
    FORK_BIOME_SNOW__ICE,
    FORK_BIOME_MARSH__SWAMP,
    FORK_BIOME_FRESHWATER,
    FORK_BIOME_OCEAN,
    FORK_BIOME_BEACH__COAST,
};

struct ForkEncounterAssignment
{
    u8 mapGroup;
    u8 mapNum;
    u8 area;
    u8 biome;
    u16 minBst;
    u16 maxBst;
    u8 slots;
};

#include "../src/data/fork_biome_encounter_data.h"

static u16 GetSpeciesBst(enum Species species)
{
    return gSpeciesInfo[species].baseHP
         + gSpeciesInfo[species].baseAttack
         + gSpeciesInfo[species].baseDefense
         + gSpeciesInfo[species].baseSpeed
         + gSpeciesInfo[species].baseSpAttack
         + gSpeciesInfo[species].baseSpDefense;
}

TEST("Every biome assignment has enough distinct candidates in every generation")
{
    for (u8 generation = GEN_3; generation <= GEN_9; generation++)
    {
        ForkConfigureGameplayOptions(FALSE, FALSE, FORK_FAINT_RULE_WHITEOUT,
            FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, generation, TRUE, FALSE);
        gSaveBlock3Ptr->forkEncounterRandomizerSeed = 0x4153534E;

        for (u32 assignmentIndex = 0; assignmentIndex < ARRAY_COUNT(sForkEncounterAssignments); assignmentIndex++)
        {
            const struct ForkEncounterAssignment *assignment = &sForkEncounterAssignments[assignmentIndex];
            enum Species selected[FORK_FISHING_RANDOMIZED_SLOT_COUNT];

            EXPECT_LE(assignment->slots, ARRAY_COUNT(selected));
            for (u8 slot = 0; slot < assignment->slots; slot++)
            {
                u8 encounterSlot = slot;

                if (assignment->area == WILD_AREA_FISHING && slot >= 4)
                    encounterSlot++;
                selected[slot] = ResolveForkRandomizedEncounterSpecies(
                    assignment->mapGroup, assignment->mapNum, assignment->area,
                    encounterSlot, SPECIES_NONE);
                if (selected[slot] == SPECIES_NONE)
                {
                    Test_MgbaPrintf("Empty biome assignment: gen=%d index=%d map=%d/%d area=%d slot=%d\n",
                        generation, assignmentIndex, assignment->mapGroup, assignment->mapNum, assignment->area, slot);
                    for (u8 previous = 0; previous < slot; previous++)
                        Test_MgbaPrintf("  selected[%d]=%S (%d)\n", previous,
                            gSpeciesInfo[selected[previous]].speciesName, selected[previous]);
                }
                EXPECT_NE(selected[slot], SPECIES_NONE);
                for (u8 previous = 0; previous < slot; previous++)
                {
                    if (selected[slot] == selected[previous])
                        Test_MgbaPrintf("Duplicate biome assignment: gen=%d index=%d map=%d/%d area=%d slots=%d/%d species=%d\n",
                            generation, assignmentIndex, assignment->mapGroup, assignment->mapNum,
                            assignment->area, previous, slot, selected[slot]);
                    EXPECT_NE(selected[slot], selected[previous]);
                }
            }
        }
    }
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

TEST("Biome encounter randomizer assigns BST ranges to newly grassy cities")
{
    EXPECT_EQ(GetForkEncounterRandomizerBstMin(MAP_GROUP(MAP_LITTLEROOT_TOWN), MAP_NUM(MAP_LITTLEROOT_TOWN), WILD_AREA_LAND), 150);
    EXPECT_EQ(GetForkEncounterRandomizerBstCap(MAP_GROUP(MAP_LITTLEROOT_TOWN), MAP_NUM(MAP_LITTLEROOT_TOWN), WILD_AREA_LAND), 250);
    EXPECT_EQ(GetForkEncounterRandomizerBstMin(MAP_GROUP(MAP_OLDALE_TOWN), MAP_NUM(MAP_OLDALE_TOWN), WILD_AREA_LAND), 150);
    EXPECT_EQ(GetForkEncounterRandomizerBstCap(MAP_GROUP(MAP_OLDALE_TOWN), MAP_NUM(MAP_OLDALE_TOWN), WILD_AREA_LAND), 250);
    EXPECT_EQ(GetForkEncounterRandomizerBstMin(MAP_GROUP(MAP_RUSTBORO_CITY), MAP_NUM(MAP_RUSTBORO_CITY), WILD_AREA_LAND), 180);
    EXPECT_EQ(GetForkEncounterRandomizerBstCap(MAP_GROUP(MAP_RUSTBORO_CITY), MAP_NUM(MAP_RUSTBORO_CITY), WILD_AREA_LAND), 280);
    EXPECT_EQ(GetForkEncounterRandomizerBstMin(MAP_GROUP(MAP_MAUVILLE_CITY), MAP_NUM(MAP_MAUVILLE_CITY), WILD_AREA_LAND), 250);
    EXPECT_EQ(GetForkEncounterRandomizerBstCap(MAP_GROUP(MAP_MAUVILLE_CITY), MAP_NUM(MAP_MAUVILLE_CITY), WILD_AREA_LAND), 350);
    EXPECT_EQ(GetForkEncounterRandomizerBstMin(MAP_GROUP(MAP_VERDANTURF_TOWN), MAP_NUM(MAP_VERDANTURF_TOWN), WILD_AREA_LAND), 250);
    EXPECT_EQ(GetForkEncounterRandomizerBstCap(MAP_GROUP(MAP_VERDANTURF_TOWN), MAP_NUM(MAP_VERDANTURF_TOWN), WILD_AREA_LAND), 350);
    EXPECT_EQ(GetForkEncounterRandomizerBstMin(MAP_GROUP(MAP_FALLARBOR_TOWN), MAP_NUM(MAP_FALLARBOR_TOWN), WILD_AREA_LAND), 270);
    EXPECT_EQ(GetForkEncounterRandomizerBstCap(MAP_GROUP(MAP_FALLARBOR_TOWN), MAP_NUM(MAP_FALLARBOR_TOWN), WILD_AREA_LAND), 370);
    EXPECT_EQ(GetForkEncounterRandomizerBstMin(MAP_GROUP(MAP_LAVARIDGE_TOWN), MAP_NUM(MAP_LAVARIDGE_TOWN), WILD_AREA_LAND), 300);
    EXPECT_EQ(GetForkEncounterRandomizerBstCap(MAP_GROUP(MAP_LAVARIDGE_TOWN), MAP_NUM(MAP_LAVARIDGE_TOWN), WILD_AREA_LAND), 400);
    EXPECT_EQ(GetForkEncounterRandomizerBstMin(MAP_GROUP(MAP_FORTREE_CITY), MAP_NUM(MAP_FORTREE_CITY), WILD_AREA_LAND), 320);
    EXPECT_EQ(GetForkEncounterRandomizerBstCap(MAP_GROUP(MAP_FORTREE_CITY), MAP_NUM(MAP_FORTREE_CITY), WILD_AREA_LAND), 420);
}

TEST("Surf randomizer can select non-Water Flying Pokémon")
{
    bool8 foundFlyingNonWater = FALSE;

    for (u32 seed = 1; seed <= 512 && !foundFlyingNonWater; seed++)
    {
        gSaveBlock3Ptr->forkEncounterRandomizerSeed = seed;
        for (u8 slot = 0; slot < 3; slot++)
        {
            enum Species species = ResolveForkRandomizedEncounterSpecies(
                MAP_GROUP(MAP_ROUTE118), MAP_NUM(MAP_ROUTE118), WILD_AREA_WATER, slot, SPECIES_TENTACOOL);
            bool8 isWater = gSpeciesInfo[species].types[0] == TYPE_WATER || gSpeciesInfo[species].types[1] == TYPE_WATER;
            bool8 isFlying = gSpeciesInfo[species].types[0] == TYPE_FLYING || gSpeciesInfo[species].types[1] == TYPE_FLYING;

            if (isFlying && !isWater)
                foundFlyingNonWater = TRUE;
        }
    }

    EXPECT(foundFlyingNonWater);
}

TEST("Fishing randomizer remains Water-only")
{
    for (u32 seed = 1; seed <= 32; seed++)
    {
        gSaveBlock3Ptr->forkEncounterRandomizerSeed = seed;
        for (u8 slot = 0; slot < FISH_WILD_COUNT; slot++)
        {
            enum Species species = ResolveForkRandomizedEncounterSpecies(
                MAP_GROUP(MAP_ROUTE118), MAP_NUM(MAP_ROUTE118), WILD_AREA_FISHING, slot, SPECIES_TENTACOOL);

            EXPECT(gSpeciesInfo[species].types[0] == TYPE_WATER || gSpeciesInfo[species].types[1] == TYPE_WATER);
        }
    }
}

TEST("Biome encounter randomizer avoids duplicate species within one land table")
{
    enum Species species[3];
    u32 i;
    u32 j;

    gSaveBlock3Ptr->forkEncounterRandomizerSeed = 0x0BADF00D;
    for (i = 0; i < ARRAY_COUNT(species); i++)
        species[i] = ResolveForkRandomizedEncounterSpecies(MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101), WILD_AREA_LAND, i, SPECIES_ZIGZAGOON);
    for (i = 0; i < ARRAY_COUNT(species); i++)
        for (j = i + 1; j < ARRAY_COUNT(species); j++)
            EXPECT_NE(species[i], species[j]);
}

TEST("Biome encounter randomizer maps every land slot onto three randomized species")
{
    enum Species species[LAND_WILD_COUNT];
    u32 i;

    gSaveBlock3Ptr->forkEncounterRandomizerSeed = 0x0BADF00D;
    for (i = 0; i < ARRAY_COUNT(species); i++)
        species[i] = ResolveForkRandomizedEncounterSpecies(MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101), WILD_AREA_LAND, i, SPECIES_ZIGZAGOON);

    EXPECT_NE(species[0], species[1]);
    EXPECT_NE(species[0], species[2]);
    EXPECT_NE(species[1], species[2]);
    for (i = 3; i < ARRAY_COUNT(species); i++)
        EXPECT_EQ(species[i], species[i % 3]);
}

TEST("Biome encounter randomizer gives each fishing rod two randomized species")
{
    enum Species species[FISH_WILD_COUNT];
    const u8 representativeSlots[] = {0, 1, 2, 3, 5, 6};
    u32 i;
    u32 j;

    gSaveBlock3Ptr->forkEncounterRandomizerSeed = 0x0BADF00D;
    for (i = 0; i < ARRAY_COUNT(species); i++)
        species[i] = ResolveForkRandomizedEncounterSpecies(MAP_GROUP(MAP_ROUTE118), MAP_NUM(MAP_ROUTE118), WILD_AREA_FISHING, i, SPECIES_TENTACOOL);

    EXPECT_NE(species[0], species[1]);
    EXPECT_NE(species[2], species[3]);
    EXPECT_EQ(species[2], species[4]);
    EXPECT_NE(species[5], species[6]);
    EXPECT_EQ(species[5], species[7]);
    EXPECT_EQ(species[6], species[8]);
    EXPECT_EQ(species[5], species[9]);
    for (i = 0; i < ARRAY_COUNT(representativeSlots); i++)
        for (j = i + 1; j < ARRAY_COUNT(representativeSlots); j++)
            EXPECT_NE(species[representativeSlots[i]], species[representativeSlots[j]]);
}

TEST("Area encounter state is stored in the fork save block")
{
    mapsec_u8_t mapSecId = 42;

    ForkResetAreaEncounterState();
    ForkSetAreaEncounterSpent(mapSecId);

    EXPECT((gSaveBlock3Ptr->forkAreaEncounterSpent[mapSecId / 8] & (1 << (mapSecId % 8))) != 0);
}

TEST("Biome encounter randomizer defaults to Generation 3 species")
{
    gSaveBlock3Ptr->forkEncounterRandomizerSeed = 0x10203040;

    for (u8 slot = 0; slot < 3; slot++)
    {
        enum Species species = ResolveForkRandomizedEncounterSpecies(
            MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101), WILD_AREA_LAND, slot, SPECIES_ZIGZAGOON);

        EXPECT_LE((u16)gSpeciesInfo[species].natDexNum, NATIONAL_DEX_DEOXYS);
    }
}

TEST("Configured encounter generation can reach Generation 8")
{
    ForkConfigureGameplayOptions(FALSE, FALSE, FORK_FAINT_RULE_WHITEOUT, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, GEN_8, TRUE, FALSE);

    EXPECT_EQ(GetForkMaxNationalDex(), NATIONAL_DEX_ENAMORUS);
}

TEST("Egg randomizer selects a non-restricted species within the egg BST range")
{
    enum Species species;

    gSaveBlock3Ptr->forkEncounterRandomizerSeed = 0x55667788;
    species = ResolveForkRandomizedEggSpecies(SPECIES_WYNAUT);

    EXPECT_GE(GetSpeciesBst(species), 100);
    EXPECT_LE(GetSpeciesBst(species), 550);
    EXPECT(!gSpeciesInfo[species].isRestrictedLegendary);
    EXPECT(!gSpeciesInfo[species].isSubLegendary);
    EXPECT(!gSpeciesInfo[species].isMythical);
}

TEST("Static encounter randomizer uses its route BST range")
{
    enum Species species;

    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROUTE120);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROUTE120);
    gSaveBlock3Ptr->forkEncounterRandomizerSeed = 0x99AABBCC;
    species = ResolveForkRandomizedStaticEncounterSpecies(SPECIES_KECLEON);

    EXPECT_GE(GetSpeciesBst(species), 330);
    EXPECT_LE(GetSpeciesBst(species), 430);
}

TEST("Special static encounter randomizer permits legendary BST range")
{
    enum Species species;

    gSaveBlock3Ptr->forkEncounterRandomizerSeed = 0xDDEEFF00;
    species = ResolveForkRandomizedStaticEncounterSpecies(SPECIES_REGIROCK);

    EXPECT_GE(GetSpeciesBst(species), 550);
    EXPECT_LE(GetSpeciesBst(species), 600);
}
