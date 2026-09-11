#include "global.h"
#include "fork_ability_randomizer.h"
#include "fork_encounter_randomizer.h"
#include "fork_run.h"
#include "pokemon.h"
#include "random.h"

#define FORK_ABILITY_RANDOMIZER_SALT 0x4142494C

EWRAM_DATA static enum Species sFamilyRoots[NUM_SPECIES];
EWRAM_DATA static bool8 sExcludedFamilies[NUM_SPECIES];
EWRAM_DATA static enum Ability sAvailableAbilities[ABILITIES_COUNT];
static u16 sAvailableAbilityCount;
static u32 sPoolSeed;
static u16 sPoolMaxNationalDex;
static bool8 sPoolInitialized;

static bool8 IsExcludedAbility(enum Ability ability)
{
    switch (ability)
    {
    case ABILITY_WONDER_GUARD:
    case ABILITY_FORECAST:
    case ABILITY_MULTITYPE:
    case ABILITY_RKS_SYSTEM:
    case ABILITY_SCHOOLING:
    case ABILITY_DISGUISE:
    case ABILITY_SHIELDS_DOWN:
    case ABILITY_POWER_CONSTRUCT:
    case ABILITY_STANCE_CHANGE:
    case ABILITY_BATTLE_BOND:
    case ABILITY_ZEN_MODE:
    case ABILITY_FLOWER_GIFT:
    case ABILITY_GULP_MISSILE:
    case ABILITY_ICE_FACE:
    case ABILITY_HUNGER_SWITCH:
    case ABILITY_ZERO_TO_HERO:
    case ABILITY_COMMANDER:
    case ABILITY_AS_ONE_ICE_RIDER:
    case ABILITY_AS_ONE_SHADOW_RIDER:
    case ABILITY_TERA_SHIFT:
    case ABILITY_TERAFORM_ZERO:
        return TRUE;
    default:
        return FALSE;
    }
}

static bool8 IsExcludedAbilityPair(enum Species species, enum Ability ability)
{
    return species == SPECIES_SHEDINJA && ability == ABILITY_WONDER_GUARD;
}

static enum Species GetFamilyRoot(enum Species species)
{
    enum Species root;
    enum Species previous;

    species = SanitizeSpeciesId(species);
    if (species == SPECIES_NONE)
        return SPECIES_NONE;
    if (sFamilyRoots[species] != SPECIES_NONE)
        return sFamilyRoots[species];

    root = species;
    while ((previous = GetSpeciesPreEvolution(root)) != SPECIES_NONE)
        root = previous;

    sFamilyRoots[species] = root;
    return root;
}

static u16 GetSpeciesBst(enum Species species)
{
    const struct SpeciesInfo *speciesInfo = &gSpeciesInfo[SanitizeSpeciesId(species)];

    return speciesInfo->baseHP
         + speciesInfo->baseAttack
         + speciesInfo->baseDefense
         + speciesInfo->baseSpeed
         + speciesInfo->baseSpAttack
         + speciesInfo->baseSpDefense;
}

static bool8 IsSpecialStaticAbilitySpecies(enum Species species)
{
    const struct SpeciesInfo *speciesInfo = &gSpeciesInfo[SanitizeSpeciesId(species)];
    u16 bst = GetSpeciesBst(species);

    return speciesInfo->natDexNum <= GetForkMaxNationalDex()
        && bst >= 550
        && bst <= 600
        && (speciesInfo->isRestrictedLegendary
         || speciesInfo->isSubLegendary
         || speciesInfo->isMythical
         || speciesInfo->isUltraBeast
         || speciesInfo->isParadox);
}

static bool8 IsEligibleAbilitySpecies(enum Species species)
{
    const struct SpeciesInfo *speciesInfo;

    if (!IsSpeciesEnabled(species))
        return FALSE;

    speciesInfo = &gSpeciesInfo[SanitizeSpeciesId(species)];

    return (speciesInfo->natDexNum <= GetForkMaxNationalDex()
         && !speciesInfo->isRestrictedLegendary
         && !speciesInfo->isSubLegendary
         && !speciesInfo->isMythical
         && !speciesInfo->isUltraBeast
         && !speciesInfo->isParadox)
        || IsSpecialStaticAbilitySpecies(species);
}

static bool8 SpeciesHasExcludedAbility(enum Species species)
{
    u8 slot;

    for (slot = 0; slot < NUM_ABILITY_SLOTS; slot++)
        if (IsExcludedAbility(GetSpeciesAbility(species, slot))
         || IsExcludedAbilityPair(species, GetSpeciesAbility(species, slot)))
            return TRUE;
    return FALSE;
}

static void AddAvailableAbility(enum Ability ability)
{
    u16 i;

    if (ability == ABILITY_NONE || IsExcludedAbility(ability))
        return;

    for (i = 0; i < sAvailableAbilityCount; i++)
        if (sAvailableAbilities[i] == ability)
            return;

    sAvailableAbilities[sAvailableAbilityCount++] = ability;
}

static void BuildAbilityPool(void)
{
    enum Species species;
    u16 maxNationalDex = GetForkMaxNationalDex();

    if (sPoolInitialized
     && sPoolSeed == gSaveBlock3Ptr->forkItemRandomizerSeed
     && sPoolMaxNationalDex == maxNationalDex)
        return;

    // This cache can be rebuilt for a different save or configured generation.
    // Clear both lookup tables so excluded families from an earlier pool cannot
    // influence the new one.
    memset(sFamilyRoots, 0, sizeof(sFamilyRoots));
    memset(sExcludedFamilies, 0, sizeof(sExcludedFamilies));
    sAvailableAbilityCount = 0;
    for (species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        if (IsEligibleAbilitySpecies(species) && SpeciesHasExcludedAbility(species))
            sExcludedFamilies[GetFamilyRoot(species)] = TRUE;
    }

    for (species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        u8 slot;

        if (!IsEligibleAbilitySpecies(species) || sExcludedFamilies[GetFamilyRoot(species)])
            continue;
        for (slot = 0; slot < NUM_ABILITY_SLOTS; slot++)
            AddAvailableAbility(GetSpeciesAbility(species, slot));
    }

    sPoolSeed = gSaveBlock3Ptr->forkItemRandomizerSeed;
    sPoolMaxNationalDex = maxNationalDex;
    sPoolInitialized = TRUE;
}

bool8 IsForkAbilityRandomizedSpecies(enum Species species)
{
    if (!ForkAreRandomAbilitiesEnabled())
        return FALSE;
    BuildAbilityPool();
    return IsEligibleAbilitySpecies(species) && !sExcludedFamilies[GetFamilyRoot(species)];
}

enum Ability GetForkRandomizedAbility(enum Species species)
{
    enum Species root;
    rng_value_t rng;

    if (!IsForkAbilityRandomizedSpecies(species))
        return GetSpeciesAbility(species, 0);

    root = GetFamilyRoot(species);
    rng = LocalRandomSeed(gSaveBlock3Ptr->forkItemRandomizerSeed ^ FORK_ABILITY_RANDOMIZER_SALT ^ root);
    if (sAvailableAbilityCount == 0)
        return GetSpeciesAbility(species, 0);
    return sAvailableAbilities[LocalRandom(&rng) % sAvailableAbilityCount];
}
