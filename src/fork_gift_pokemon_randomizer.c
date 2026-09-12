#include "global.h"
#include "fork_ability_randomizer.h"
#include "fork_encounter_randomizer.h"
#include "fork_gift_pokemon_randomizer.h"
#include "fork_run.h"
#include "pokemon.h"
#include "random.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/moves.h"

#define FORK_STARTER_CHOICE_COUNT 3
#define FORK_STARTER_IV_SALT 0x53544956

static const enum Species sPseudoLegendaryFirstStages[] =
{
    SPECIES_DRATINI, SPECIES_LARVITAR, SPECIES_BAGON,
    SPECIES_GIBLE, SPECIES_DEINO, SPECIES_GOOMY,
    SPECIES_JANGMO_O, SPECIES_DREEPY, SPECIES_FRIGIBAX,
};

struct FortreeTradeCandidate
{
    enum Species species;
    enum Ability ability;
    bool8 shiny;
};

struct RustboroTradeCandidate
{
    enum Species species;
    enum Ability ability;
    enum Item heldItem;
    enum Move moves[MAX_MON_MOVES];
    bool8 useRandomAbility;
};

static const struct RustboroTradeCandidate sRustboroTradeCandidates[] =
{
    {SPECIES_PSYDUCK,    ABILITY_DRIZZLE,       ITEM_EVIOLITE,    {MOVE_FOLLOW_ME, MOVE_CONFUSION, MOVE_CONFUSE_RAY, MOVE_EXPLOSION}, FALSE},
    {SPECIES_CHIKORITA,  ABILITY_NONE,          ITEM_MEGANIUMITE, {MOVE_NONE},                                           TRUE},
    {SPECIES_MAWILE,     ABILITY_NONE,          ITEM_MAWILITE,    {MOVE_NONE},                                           TRUE},
    {SPECIES_SPIRITOMB,  ABILITY_SHADOW_SHIELD, ITEM_NONE,       {MOVE_NONE},                                           FALSE},
    {SPECIES_EELEKTROSS, ABILITY_LEVITATE,      ITEM_NONE,       {MOVE_THUNDERBOLT},                                    FALSE},
    {SPECIES_MALAMAR,    ABILITY_CONTRARY,      ITEM_NONE,       {MOVE_NONE},                                           FALSE},
    {SPECIES_WISHIWASHI, ABILITY_SCHOOLING,     ITEM_NONE,       {MOVE_SHADOW_SNEAK},                                  FALSE},
    {SPECIES_CURSOLA,    ABILITY_PERISH_BODY,   ITEM_NONE,       {MOVE_NONE},                                           FALSE},
    {SPECIES_PALAFIN,    ABILITY_ZERO_TO_HERO,  ITEM_NONE,       {MOVE_NONE},                                           FALSE},
};

static const struct FortreeTradeCandidate sFortreeTradeCandidates[] =
{
    {SPECIES_BLASTOISE, ABILITY_MEGA_LAUNCHER, FALSE},
    {SPECIES_SCIZOR, ABILITY_TECHNICIAN, FALSE},
    {SPECIES_TORKOAL, ABILITY_DROUGHT, FALSE},
    {SPECIES_MAGMORTAR, ABILITY_FLASH_FIRE, FALSE},
    {SPECIES_GOLURK, ABILITY_UNSEEN_FIST, TRUE},
    {SPECIES_DRAGALGE, ABILITY_REGENERATOR, FALSE},
    {SPECIES_SHIINOTIC, ABILITY_POISON_HEAL, FALSE},
    {SPECIES_MORPEKO_FULL_BELLY, ABILITY_HUNGER_SWITCH, FALSE},
    {SPECIES_DACHSBUN, ABILITY_WELL_BAKED_BODY, FALSE},
};

static bool8 IsEligibleGiftSpecies(enum Species species)
{
    return IsSpeciesEnabled(species)
        && gSpeciesInfo[species].natDexNum <= GetForkMaxNationalDex();
}

static bool8 IsAlreadySelected(enum Species species, const enum Species *selected, u8 selectedCount)
{
    for (u8 i = 0; i < selectedCount; i++)
        if (selected[i] == species)
            return TRUE;
    return FALSE;
}

static bool8 IsEligibleStarterSpecies(enum Species species)
{
    return IsEligibleGiftSpecies(species)
        && species == GET_BASE_SPECIES_ID(species)
        && GetSpeciesBaseStatTotal(species) >= 275
        && GetSpeciesBaseStatTotal(species) <= 325;
}

static enum Species SelectRandomStarterSpecies(u8 selection)
{
    enum Species selected[FORK_STARTER_CHOICE_COUNT];
    rng_value_t rng = LocalRandomSeed(gSaveBlock3Ptr->forkItemRandomizerSeed ^ 0x53544152);

    if (selection >= ARRAY_COUNT(selected))
        return SPECIES_TREECKO;

    for (u8 selectedCount = 0; selectedCount <= selection; selectedCount++)
    {
        u16 candidateCount = 0;
        u16 candidateIndex;

        selected[selectedCount] = SPECIES_TREECKO;
        for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
            if (IsEligibleStarterSpecies(species) && !IsAlreadySelected(species, selected, selectedCount))
                candidateCount++;

        if (candidateCount == 0)
            continue;
        candidateIndex = LocalRandom(&rng) % candidateCount;

        for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
        {
            if (!IsEligibleStarterSpecies(species) || IsAlreadySelected(species, selected, selectedCount))
                continue;
            if (candidateIndex-- == 0)
            {
                selected[selectedCount] = species;
                break;
            }
        }
    }

    return selected[selection];
}

static enum Species SelectRandomGiftSpecies(const enum Species *candidates, u16 candidateCount, u32 salt, u8 selection, enum Species fallback)
{
    enum Species selected[FORK_STARTER_CHOICE_COUNT];
    rng_value_t rng = LocalRandomSeed(gSaveBlock3Ptr->forkItemRandomizerSeed ^ salt);

    if (selection >= ARRAY_COUNT(selected))
        return fallback;

    for (u8 selectedCount = 0; selectedCount <= selection; selectedCount++)
    {
        u16 start = LocalRandom(&rng) % candidateCount;

        selected[selectedCount] = fallback;

        for (u16 i = 0; i < candidateCount; i++)
        {
            enum Species species = candidates[(start + i) % candidateCount];

            if (IsEligibleGiftSpecies(species) && !IsAlreadySelected(species, selected, selectedCount))
            {
                selected[selectedCount] = species;
                break;
            }
        }
    }

    return selected[selection];
}

enum Species GetForkRandomizedStarterSpecies(u8 slot)
{
    if (!ForkAreRandomEncountersEnabled())
    {
        static const enum Species sVanillaStarters[] = {SPECIES_TREECKO, SPECIES_TORCHIC, SPECIES_MUDKIP};
        return slot < ARRAY_COUNT(sVanillaStarters) ? sVanillaStarters[slot] : SPECIES_TREECKO;
    }
    return SelectRandomStarterSpecies(slot);
}

enum Species GetForkRandomizedStevenGiftSpecies(void)
{
    if (!ForkAreRandomEncountersEnabled())
        return SPECIES_BELDUM;
    return SelectRandomGiftSpecies(sPseudoLegendaryFirstStages, ARRAY_COUNT(sPseudoLegendaryFirstStages), 0x53544556, 0, SPECIES_BELDUM);
}

static const struct FortreeTradeCandidate *GetFortreeTradeCandidate(void)
{
    rng_value_t rng = LocalRandomSeed(gSaveBlock3Ptr->forkItemRandomizerSeed ^ 0x46545244);
    u16 eligible[ARRAY_COUNT(sFortreeTradeCandidates)];
    u16 eligibleCount = 0;

    for (u16 i = 0; i < ARRAY_COUNT(sFortreeTradeCandidates); i++)
    {
        const struct FortreeTradeCandidate *candidate = &sFortreeTradeCandidates[i];
        if (IsSpeciesEnabled(candidate->species)
         && gSpeciesInfo[candidate->species].natDexNum <= GetForkMaxNationalDex())
            eligible[eligibleCount++] = i;
    }

    if (eligibleCount == 0)
        return &sFortreeTradeCandidates[0];
    return &sFortreeTradeCandidates[eligible[LocalRandom(&rng) % eligibleCount]];
}

static const struct RustboroTradeCandidate *GetRustboroTradeCandidate(void)
{
    rng_value_t rng = LocalRandomSeed(gSaveBlock3Ptr->forkItemRandomizerSeed ^ 0x52535452);
    u16 eligible[ARRAY_COUNT(sRustboroTradeCandidates)];
    u16 eligibleCount = 0;

    for (u16 i = 0; i < ARRAY_COUNT(sRustboroTradeCandidates); i++)
    {
        if (IsEligibleGiftSpecies(sRustboroTradeCandidates[i].species))
            eligible[eligibleCount++] = i;
    }

    if (eligibleCount == 0)
        return &sRustboroTradeCandidates[0];
    return &sRustboroTradeCandidates[eligible[LocalRandom(&rng) % eligibleCount]];
}

enum Species GetForkRandomizedFortreeTradeSpecies(void)
{
    return GetFortreeTradeCandidate()->species;
}

enum Ability GetForkRandomizedFortreeTradeAbility(void)
{
    return GetFortreeTradeCandidate()->ability;
}

bool8 GetForkRandomizedFortreeTradeIsShiny(void)
{
    return GetFortreeTradeCandidate()->shiny;
}

enum Species GetForkRandomizedRustboroTradeSpecies(void)
{
    return GetRustboroTradeCandidate()->species;
}

enum Ability GetForkRandomizedRustboroTradeAbility(void)
{
    const struct RustboroTradeCandidate *candidate = GetRustboroTradeCandidate();

    return candidate->useRandomAbility ? GetForkRandomizedAbility(candidate->species) : candidate->ability;
}

enum Item GetForkRandomizedRustboroTradeHeldItem(void)
{
    return GetRustboroTradeCandidate()->heldItem;
}

enum Move GetForkRandomizedRustboroTradeMove(u8 slot)
{
    const struct RustboroTradeCandidate *candidate = GetRustboroTradeCandidate();

    return slot < MAX_MON_MOVES ? candidate->moves[slot] : MOVE_NONE;
}

void ApplyForkStarterPerfectIvs(struct Pokemon *mon, u8 starterSlot)
{
    rng_value_t rng = LocalRandomSeed(gSaveBlock3Ptr->forkItemRandomizerSeed ^ FORK_STARTER_IV_SALT ^ starterSlot);
    u8 firstPerfectStat = LocalRandom(&rng) % NUM_STATS;
    u8 secondPerfectStat = LocalRandom(&rng) % (NUM_STATS - 1);
    u8 iv;
    u8 stat;

    if (secondPerfectStat >= firstPerfectStat)
        secondPerfectStat++;

    for (stat = 0; stat < NUM_STATS; stat++)
    {
        iv = LocalRandom(&rng) % (MAX_PER_STAT_IVS + 1);
        if (stat == firstPerfectStat || stat == secondPerfectStat)
            iv = MAX_PER_STAT_IVS;
        SetMonData(mon, MON_DATA_HP_IV + stat, &iv);
    }
    CalculateMonStats(mon);
}
