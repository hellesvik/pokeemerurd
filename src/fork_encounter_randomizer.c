#include "global.h"
#include "fork_encounter_randomizer.h"
#include "fork_run.h"
#include "pokemon.h"
#include "random.h"

#define FORK_ENCOUNTER_RANDOMIZER_VERSION 1

enum ForkEncounterBiome
{
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

#include "data/fork_biome_encounter_data.h"

static u16 GetSpeciesBst(enum Species species)
{
    return gSpeciesInfo[species].baseHP
         + gSpeciesInfo[species].baseAttack
         + gSpeciesInfo[species].baseDefense
         + gSpeciesInfo[species].baseSpeed
         + gSpeciesInfo[species].baseSpAttack
         + gSpeciesInfo[species].baseSpDefense;
}

static const struct ForkEncounterAssignment *FindAssignment(u8 mapGroup, u8 mapNum, enum WildPokemonArea area)
{
    u32 i;
    for (i = 0; i < ARRAY_COUNT(sForkEncounterAssignments); i++)
    {
        const struct ForkEncounterAssignment *assignment = &sForkEncounterAssignments[i];
        if (assignment->mapGroup == mapGroup && assignment->mapNum == mapNum && assignment->area == area)
            return assignment;
    }
    return NULL;
}

static const enum Species *GetBiomePool(u8 biome, u16 *count)
{
    switch (biome)
    {
    case FORK_BIOME_GRASSL: *count = ARRAY_COUNT(sForkBiomePool_GRASSL); return sForkBiomePool_GRASSL;
    case FORK_BIOME_FOREST: *count = ARRAY_COUNT(sForkBiomePool_FOREST); return sForkBiomePool_FOREST;
    case FORK_BIOME_MOUNTAIN: *count = ARRAY_COUNT(sForkBiomePool_MOUNTAIN); return sForkBiomePool_MOUNTAIN;
    case FORK_BIOME_CAVE: *count = ARRAY_COUNT(sForkBiomePool_CAVE); return sForkBiomePool_CAVE;
    case FORK_BIOME_DESERT: *count = ARRAY_COUNT(sForkBiomePool_DESERT); return sForkBiomePool_DESERT;
    case FORK_BIOME_VOLCANIC: *count = ARRAY_COUNT(sForkBiomePool_VOLCANIC); return sForkBiomePool_VOLCANIC;
    case FORK_BIOME_SNOW__ICE: *count = ARRAY_COUNT(sForkBiomePool_SNOW__ICE); return sForkBiomePool_SNOW__ICE;
    case FORK_BIOME_MARSH__SWAMP: *count = ARRAY_COUNT(sForkBiomePool_MARSH__SWAMP); return sForkBiomePool_MARSH__SWAMP;
    case FORK_BIOME_FRESHWATER: *count = ARRAY_COUNT(sForkBiomePool_FRESHWATER); return sForkBiomePool_FRESHWATER;
    case FORK_BIOME_OCEAN: *count = ARRAY_COUNT(sForkBiomePool_OCEAN); return sForkBiomePool_OCEAN;
    default: *count = ARRAY_COUNT(sForkBiomePool_BEACH__COAST); return sForkBiomePool_BEACH__COAST;
    }
}

static bool8 IsMethodCompatible(enum Species species, enum WildPokemonArea area)
{
    enum Type type1 = gSpeciesInfo[species].types[0];
    enum Type type2 = gSpeciesInfo[species].types[1];
    if (area == WILD_AREA_WATER || area == WILD_AREA_FISHING)
        return type1 == TYPE_WATER || type2 == TYPE_WATER;
    if (area == WILD_AREA_ROCKS)
        return type1 == TYPE_ROCK || type2 == TYPE_ROCK || type1 == TYPE_GROUND || type2 == TYPE_GROUND || type1 == TYPE_STEEL || type2 == TYPE_STEEL;
    return TRUE;
}

static bool8 IsAlreadySelected(enum Species species, const enum Species *selected, u8 selectedCount)
{
    u8 i;
    for (i = 0; i < selectedCount; i++)
        if (selected[i] == species)
            return TRUE;
    return FALSE;
}

static u16 GetForkMaxNationalDexForGen(u8 maxGen)
{
    switch (maxGen)
    {
    case GEN_1:
        return NATIONAL_DEX_MEW;
    case GEN_2:
        return NATIONAL_DEX_CELEBI;
    case GEN_3:
        return NATIONAL_DEX_DEOXYS;
    case GEN_4:
        return NATIONAL_DEX_ARCEUS;
    case GEN_5:
        return NATIONAL_DEX_GENESECT;
    case GEN_6:
        return NATIONAL_DEX_VOLCANION;
    case GEN_7:
        return NATIONAL_DEX_MELMETAL;
    case GEN_8:
        return NATIONAL_DEX_ENAMORUS;
    default:
        return NATIONAL_DEX_PECHARUNT;
    }
}

static bool8 IsWithinForkMaxGeneration(enum Species species)
{
    u8 maxGen = ForkGetRandomizerMaxGen();
    if (maxGen > FORK_MAX_GEN_MONS)
        maxGen = FORK_MAX_GEN_MONS;
    return gSpeciesInfo[species].natDexNum <= GetForkMaxNationalDexForGen(maxGen);
}

static bool8 IsRegularEncounterSpecies(enum Species species)
{
    const struct SpeciesInfo *speciesInfo = &gSpeciesInfo[species];

    return IsWithinForkMaxGeneration(species)
        && !speciesInfo->isRestrictedLegendary
        && !speciesInfo->isSubLegendary
        && !speciesInfo->isMythical
        && !speciesInfo->isUltraBeast
        && !speciesInfo->isParadox;
}

u16 GetForkMaxNationalDex(void)
{
    return GetForkMaxNationalDexForGen(ForkGetRandomizerMaxGen());
}

static enum Species SelectEncounterSpecies(const struct ForkEncounterAssignment *assignment, u8 mapGroup, u8 mapNum, enum WildPokemonArea area, u8 slot, u8 randomSalt, const enum Species *selected, u8 selectedCount, enum Species fallback)
{
    const enum Species *pool;
    rng_value_t rng;
    u16 count;
    u16 i;
    u16 start;

    pool = GetBiomePool(assignment->biome, &count);
    rng = LocalRandomSeed(gSaveBlock3Ptr->forkEncounterRandomizerSeed ^ ((u32)mapGroup << 24) ^ ((u32)mapNum << 16) ^ ((u32)area << 8) ^ randomSalt);
    start = LocalRandom(&rng) % count;
    for (i = 0; i < count; i++)
    {
        enum Species species = pool[(start + i) % count];
        if (IsRegularEncounterSpecies(species)
         && GetSpeciesBst(species) >= assignment->minBst
         && GetSpeciesBst(species) <= assignment->maxBst
         && IsMethodCompatible(species, area)
         && !IsAlreadySelected(species, selected, selectedCount))
            return species;
    }
    return fallback;
}

static enum Species SelectSpeciesFromBstRange(u16 minBst, u16 maxBst, bool8 allowSpecial, u32 salt, enum Species fallback)
{
    enum Species selected = fallback;
    rng_value_t rng = LocalRandomSeed(gSaveBlock3Ptr->forkEncounterRandomizerSeed ^ salt);
    u16 candidateCount = 0;
    u16 candidateIndex;

    for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        // The expanded species enum contains disabled placeholder entries.
        // Check that a slot is enabled before asking form helpers to inspect it.
        if (!IsSpeciesEnabled(species)
         || species != GET_BASE_SPECIES_ID(species)
         || !IsWithinForkMaxGeneration(species)
         || GetSpeciesBst(species) < minBst
         || GetSpeciesBst(species) > maxBst)
            continue;
        if (!allowSpecial && !IsRegularEncounterSpecies(species))
            continue;
        candidateCount++;
    }

    if (candidateCount == 0)
        return fallback;
    candidateIndex = LocalRandom(&rng) % candidateCount;
    for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        if (!IsSpeciesEnabled(species)
         || species != GET_BASE_SPECIES_ID(species)
         || !IsWithinForkMaxGeneration(species)
         || GetSpeciesBst(species) < minBst
         || GetSpeciesBst(species) > maxBst
         || (!allowSpecial && !IsRegularEncounterSpecies(species)))
            continue;
        if (candidateIndex-- == 0)
        {
            selected = species;
            break;
        }
    }
    return selected;
}

static bool8 IsSpecialStaticEncounter(enum Species species)
{
    return species == SPECIES_REGIROCK
        || species == SPECIES_REGICE
        || species == SPECIES_REGISTEEL
        || species == SPECIES_GROUDON
        || species == SPECIES_KYOGRE
        || species == SPECIES_RAYQUAZA;
}

void InitForkEncounterRandomizerSeed(void)
{
    gSaveBlock3Ptr->forkEncounterRandomizerSeed = Random32() ^ 0xB10E5EED;
    gSaveBlock3Ptr->forkEncounterRandomizerVersion = FORK_ENCOUNTER_RANDOMIZER_VERSION;
}

u16 GetForkEncounterRandomizerBstCap(u8 mapGroup, u8 mapNum, enum WildPokemonArea area)
{
    const struct ForkEncounterAssignment *assignment = FindAssignment(mapGroup, mapNum, area);
    return assignment == NULL ? 0 : assignment->maxBst;
}

u16 GetForkEncounterRandomizerBstMin(u8 mapGroup, u8 mapNum, enum WildPokemonArea area)
{
    const struct ForkEncounterAssignment *assignment = FindAssignment(mapGroup, mapNum, area);
    return assignment == NULL ? 0 : assignment->minBst;
}

enum Species ResolveForkRandomizedEncounterSpecies(u8 mapGroup, u8 mapNum, enum WildPokemonArea area, u8 slot, enum Species fallback)
{
    if (!ForkAreRandomEncountersEnabled())
        return fallback;
    const struct ForkEncounterAssignment *assignment = FindAssignment(mapGroup, mapNum, area);
    enum Species selected[LAND_WILD_COUNT];
    u8 i;

    if (assignment == NULL || slot >= assignment->slots)
        return fallback;
    for (i = 0; i <= slot; i++)
        selected[i] = SelectEncounterSpecies(assignment, mapGroup, mapNum, area, i, i, selected, i, fallback);
    return selected[slot];
}

enum Species ResolveForkRandomizedEggSpecies(enum Species fallback)
{
    if (!ForkAreRandomEncountersEnabled())
        return fallback;
    return SelectSpeciesFromBstRange(100, 550, FALSE, 0x45474700 ^ fallback, fallback);
}

enum Species ResolveForkRandomizedStaticEncounterSpecies(enum Species fallback)
{
    const struct ForkEncounterAssignment *assignment;
    enum Species species;

    if (!ForkAreRandomEncountersEnabled())
        return fallback;
    if (IsSpecialStaticEncounter(fallback))
        return SelectSpeciesFromBstRange(550, 600, TRUE, 0x53544154 ^ fallback, fallback);

    assignment = FindAssignment(gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum, WILD_AREA_LAND);
    if (assignment == NULL)
        return fallback;
    species = SelectEncounterSpecies(assignment,
        gSaveBlock1Ptr->location.mapGroup,
        gSaveBlock1Ptr->location.mapNum,
        WILD_AREA_LAND,
        0,
        0x80,
        NULL,
        0,
        fallback);
    return species;
}
