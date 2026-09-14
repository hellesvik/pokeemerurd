#include "global.h"
#include "fork_encounter_randomizer.h"
#include "fork_randomizer_catalog.h"
#include "fork_run.h"
#include "pokemon.h"
#include "constants/form_change_types.h"

#define SPECIES_BITSET_SIZE ((NUM_SPECIES + 7) / 8)
#define ABILITY_BITSET_SIZE ((ABILITIES_COUNT + 7) / 8)
#define ITEM_BITSET_SIZE ((ITEMS_COUNT + 7) / 8)

EWRAM_DATA static enum Species sFamilies[NUM_SPECIES];
EWRAM_DATA static u8 sAvailableSpecies[SPECIES_BITSET_SIZE];
EWRAM_DATA static u8 sExcludedFamilies[SPECIES_BITSET_SIZE];
EWRAM_DATA static enum Ability sAbilities[ABILITIES_COUNT];
EWRAM_DATA static u8 sSeenAbilities[ABILITY_BITSET_SIZE];
EWRAM_DATA static u8 sRelevantEvolutionItems[ITEM_BITSET_SIZE];
EWRAM_DATA static u8 sRelevantMegaStones[ITEM_BITSET_SIZE];
static u16 sAbilityCount;
static u8 sMaxGeneration;
static bool8 sInitialized;

static bool8 GetBit(const u8 *bits, u16 index)
{
    return (bits[index / 8] >> (index % 8)) & 1;
}

static void SetBit(u8 *bits, u16 index)
{
    bits[index / 8] |= 1 << (index % 8);
}

static bool8 IsWithinGeneration(enum Species species, u8 maxGeneration, u16 maxNationalDex)
{
    const struct SpeciesInfo *info = &gSpeciesInfo[species];

    if (!IsSpeciesEnabled(species)
     || info->natDexNum == NATIONAL_DEX_NONE
     || info->natDexNum > maxNationalDex)
        return FALSE;
    if (info->isAlolanForm && maxGeneration < GEN_7)
        return FALSE;
    if ((info->isGalarianForm || info->isHisuianForm) && maxGeneration < GEN_8)
        return FALSE;
    if (info->isPaldeanForm && maxGeneration < GEN_9)
        return FALSE;
    return TRUE;
}

static bool8 IsOrdinaryCandidate(enum Species species)
{
    const struct SpeciesInfo *info = &gSpeciesInfo[species];
    u16 bst = info->baseHP + info->baseAttack + info->baseDefense
            + info->baseSpeed + info->baseSpAttack + info->baseSpDefense;

    return (info->formSpeciesIdTable == NULL || info->formSpeciesIdTable[0] == species)
        && bst >= 100 && bst <= 550
        && !info->isRestrictedLegendary
        && !info->isSubLegendary
        && !info->isMythical
        && !info->isUltraBeast
        && !info->isParadox;
}

static bool8 IsSpecialStaticCandidate(enum Species species)
{
    const struct SpeciesInfo *info = &gSpeciesInfo[species];
    u16 bst = info->baseHP + info->baseAttack + info->baseDefense
            + info->baseSpeed + info->baseSpAttack + info->baseSpDefense;

    return (info->formSpeciesIdTable == NULL || info->formSpeciesIdTable[0] == species)
        && bst >= 550 && bst <= 600
        && (info->isRestrictedLegendary
         || info->isSubLegendary
         || info->isMythical
         || info->isUltraBeast
         || info->isParadox);
}

static enum Species FindFamily(enum Species species)
{
    enum Species parent = sFamilies[species];

    if (parent != species)
        sFamilies[species] = FindFamily(parent);
    return sFamilies[species];
}

static void JoinFamilies(enum Species first, enum Species second)
{
    enum Species firstRoot = FindFamily(first);
    enum Species secondRoot = FindFamily(second);

    if (firstRoot == secondRoot)
        return;
    if (firstRoot < secondRoot)
        sFamilies[secondRoot] = firstRoot;
    else
        sFamilies[firstRoot] = secondRoot;
}

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

static void BuildCatalog(void)
{
    enum Species species;
    u8 maxGeneration = ForkGetRandomizerMaxGen();
    u16 maxNationalDex = GetForkMaxNationalDex();

    if (sInitialized && sMaxGeneration == maxGeneration)
        return;

    memset(sAvailableSpecies, 0, sizeof(sAvailableSpecies));
    memset(sExcludedFamilies, 0, sizeof(sExcludedFamilies));
    memset(sSeenAbilities, 0, sizeof(sSeenAbilities));
    memset(sRelevantEvolutionItems, 0, sizeof(sRelevantEvolutionItems));
    memset(sRelevantMegaStones, 0, sizeof(sRelevantMegaStones));
    sAbilityCount = 0;
    for (species = SPECIES_NONE; species < NUM_SPECIES; species++)
        sFamilies[species] = species;

    for (species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        const struct Evolution *evolutions;

        if (!IsWithinGeneration(species, maxGeneration, maxNationalDex))
            continue;
        evolutions = gSpeciesInfo[species].evolutions;
        for (u16 i = 0; evolutions != NULL && evolutions[i].method != EVOLUTIONS_END; i++)
            if (IsWithinGeneration(evolutions[i].targetSpecies, maxGeneration, maxNationalDex))
                JoinFamilies(species, evolutions[i].targetSpecies);
    }

    for (species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
        if (IsWithinGeneration(species, maxGeneration, maxNationalDex))
            sFamilies[species] = FindFamily(species);

    for (species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
        if (IsWithinGeneration(species, maxGeneration, maxNationalDex)
         && (IsOrdinaryCandidate(species) || IsSpecialStaticCandidate(species)))
            SetBit(sAvailableSpecies, sFamilies[species]);

    // Availability is family-wide: every normal evolution reachable from a
    // candidate source is obtainable, while disconnected alternate forms stay out.
    for (species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
        if (IsWithinGeneration(species, maxGeneration, maxNationalDex)
         && GetBit(sAvailableSpecies, sFamilies[species]))
            SetBit(sAvailableSpecies, species);

    // Derive item relevance once instead of rescanning every evolution and
    // form-change table for each randomized item selection.
    for (species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        const struct Evolution *evolutions;
        const struct FormChange *changes;

        if (!GetBit(sAvailableSpecies, species))
            continue;
        evolutions = gSpeciesInfo[species].evolutions;
        for (u16 i = 0; evolutions != NULL && evolutions[i].method != EVOLUTIONS_END; i++)
            if (evolutions[i].method == EVO_ITEM
             && evolutions[i].param < ITEMS_COUNT
             && GetBit(sAvailableSpecies, evolutions[i].targetSpecies))
                SetBit(sRelevantEvolutionItems, evolutions[i].param);

        changes = gSpeciesInfo[species].formChangeTable;
        for (u16 i = 0; changes != NULL && changes[i].method != FORM_CHANGE_TERMINATOR; i++)
            if (changes[i].method == FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM
             && changes[i].param1 < ITEMS_COUNT)
                SetBit(sRelevantMegaStones, changes[i].param1);
    }

    for (species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        if (!GetBit(sAvailableSpecies, species))
            continue;
        for (u8 slot = 0; slot < NUM_ABILITY_SLOTS; slot++)
            if (IsExcludedAbility(gSpeciesInfo[species].abilities[slot]))
                SetBit(sExcludedFamilies, sFamilies[species]);
    }

    for (species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        if (!GetBit(sAvailableSpecies, species) || GetBit(sExcludedFamilies, sFamilies[species]))
            continue;
        for (u8 slot = 0; slot < NUM_ABILITY_SLOTS; slot++)
        {
            enum Ability ability = gSpeciesInfo[species].abilities[slot];

            if (ability != ABILITY_NONE && !IsExcludedAbility(ability) && !GetBit(sSeenAbilities, ability))
            {
                SetBit(sSeenAbilities, ability);
                sAbilities[sAbilityCount++] = ability;
            }
        }
    }

    sMaxGeneration = maxGeneration;
    sInitialized = TRUE;
}

bool8 ForkRandomizerCatalog_IsSpeciesAvailable(enum Species species)
{
    BuildCatalog();
    return species < NUM_SPECIES && GetBit(sAvailableSpecies, species);
}

enum Species ForkRandomizerCatalog_GetFamily(enum Species species)
{
    BuildCatalog();
    return species < NUM_SPECIES ? sFamilies[species] : SPECIES_NONE;
}

bool8 ForkRandomizerCatalog_IsFamilyAbilityRandomized(enum Species species)
{
    BuildCatalog();
    return species < NUM_SPECIES
        && GetBit(sAvailableSpecies, species)
        && !GetBit(sExcludedFamilies, sFamilies[species]);
}

u16 ForkRandomizerCatalog_GetAbilityCount(void)
{
    BuildCatalog();
    return sAbilityCount;
}

enum Ability ForkRandomizerCatalog_GetAbility(u16 index)
{
    BuildCatalog();
    return index < sAbilityCount ? sAbilities[index] : ABILITY_NONE;
}

bool8 ForkRandomizerCatalog_IsAbilitySource(enum Species species, enum Ability ability)
{
    BuildCatalog();
    if (!ForkRandomizerCatalog_IsFamilyAbilityRandomized(species))
        return FALSE;
    for (u8 slot = 0; slot < NUM_ABILITY_SLOTS; slot++)
        if (gSpeciesInfo[species].abilities[slot] == ability)
            return TRUE;
    return FALSE;
}

bool8 ForkRandomizerCatalog_IsEvolutionItemRelevant(enum Item item)
{
    BuildCatalog();
    return item < ITEMS_COUNT && GetBit(sRelevantEvolutionItems, item);
}

bool8 ForkRandomizerCatalog_IsMegaStoneRelevant(enum Item item)
{
    BuildCatalog();
    return item < ITEMS_COUNT && GetBit(sRelevantMegaStones, item);
}

void ForkRandomizerCatalog_Invalidate(void)
{
    sInitialized = FALSE;
}

void ForkRandomizerCatalog_EnsureInitialized(void)
{
    BuildCatalog();
}
