#include "global.h"
#include "fork_run.h"
#include "battle.h"
#include "daycare.h"
#include "event_data.h"
#include "item.h"
#include "overworld.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "region_map.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/vars.h"
#include "config/item.h"
#include "config/wild_encounter.h"

#define FORK_AREA_STATE_INIT_FLAG FLAG_SPECIAL_FLAG_UNUSED_0x4003
#define FORK_AREA_STATE_VAR_COUNT 14

static const u16 sAreaStateVars[FORK_AREA_STATE_VAR_COUNT] =
{
    VAR_UNUSED_0x40F7,
    VAR_UNUSED_0x40F8,
    VAR_UNUSED_0x40F9,
    VAR_UNUSED_0x40FA,
    VAR_UNUSED_0x40FB,
    VAR_UNUSED_0x40FC,
    VAR_UNUSED_0x40FD,
    VAR_UNUSED_0x40FE,
    VAR_UNUSED_0x40FF,
    VAR_UNUSED_0x40DB,
    VAR_UNUSED_0x40DC,
    VAR_UNUSED_0x40E5,
    VAR_UNUSED_0x40A1,
    VAR_UNUSED_0x409D,
};

static EWRAM_DATA bool8 sForkEncounterActive = FALSE;
static EWRAM_DATA bool8 sForkEncounterResolved = FALSE;
static EWRAM_DATA bool8 sForkEncounterShouldSpend = FALSE;
static EWRAM_DATA bool8 sForkEncounterCatchable = FALSE;
static EWRAM_DATA mapsec_u8_t sForkEncounterMapSec = 0;
static EWRAM_DATA bool8 sOwnedFamilyCacheBuilt = FALSE;
static EWRAM_DATA u8 sOwnedFamilyRoots[(NUM_SPECIES + 7) / 8];

bool32 ForkIsAreaEncounterSpent(mapsec_u8_t mapSecId);
bool32 ForkPlayerOwnsSpeciesFamily(enum Species species);

static mapsec_u8_t ForkGetCurrentMapSec(void)
{
    // Keep separate encounter areas distinct. CorrectSpecialMapSecId maps
    // Petalburg Woods to Route 104 for region-map display purposes.
    return GetCurrentRegionMapSectionId();
}

bool32 ForkIsAreaEncounterRuleActive(void)
{
    // This flag is set immediately after Birch's successful five-Poké-Ball
    // gift, so early forced encounters never consume an area.
    return FlagGet(FLAG_ADVENTURE_STARTED) && ForkIsCatchLimitEnabled();
}

bool32 ForkIsBattleStyleLocked(void)
{
    return gSaveBlock3Ptr->forkGameplayOptionsConfigured && gSaveBlock3Ptr->forkBattleStyleLocked;
}

void ForkSetBattleStyleLocked(bool32 locked)
{
    gSaveBlock3Ptr->forkBattleStyleLocked = locked;
    if (locked)
        gSaveBlock2Ptr->optionsBattleStyle = OPTIONS_BATTLE_STYLE_SET;
}

bool32 ForkIsCatchLimitEnabled(void)
{
    return !gSaveBlock3Ptr->forkGameplayOptionsConfigured || gSaveBlock3Ptr->forkCatchLimitEnabled;
}

bool32 ForkIsLevelCapEnabled(void)
{
    return !gSaveBlock3Ptr->forkGameplayOptionsConfigured || gSaveBlock3Ptr->forkLevelCapEnabled;
}

u8 ForkGetFaintRule(void)
{
    if (!gSaveBlock3Ptr->forkGameplayOptionsConfigured)
        return FORK_FAINT_RULE_WHITEOUT;
    return gSaveBlock3Ptr->forkFaintRule;
}

bool32 ForkAreBattleItemsEnabled(void)
{
    return !gSaveBlock3Ptr->forkGameplayOptionsConfigured || gSaveBlock3Ptr->forkItemsInBattleEnabled;
}

bool32 ForkHasInfiniteRareCandy(void)
{
    return !gSaveBlock3Ptr->forkGameplayOptionsConfigured || gSaveBlock3Ptr->forkInfiniteRareCandyEnabled;
}

bool32 ForkHasInfiniteRepel(void)
{
    return !gSaveBlock3Ptr->forkGameplayOptionsConfigured || gSaveBlock3Ptr->forkInfiniteRepelEnabled;
}

bool32 ForkArePlayerEvsEnabled(void)
{
    return !gSaveBlock3Ptr->forkGameplayOptionsConfigured || gSaveBlock3Ptr->forkPlayerEvsEnabled;
}

bool32 ForkIsItemRandomizerEnabled(void)
{
    return !gSaveBlock3Ptr->forkGameplayOptionsConfigured || gSaveBlock3Ptr->forkItemRandomizerEnabled;
}

bool32 ForkAreRandomEncountersEnabled(void)
{
    return !gSaveBlock3Ptr->forkGameplayOptionsConfigured || gSaveBlock3Ptr->forkRandomEncountersEnabled;
}

bool32 ForkAreRandomAbilitiesEnabled(void)
{
    return !gSaveBlock3Ptr->forkGameplayOptionsConfigured || gSaveBlock3Ptr->forkRandomAbilitiesEnabled;
}

u8 ForkGetRandomizerMaxGen(void)
{
    if (!gSaveBlock3Ptr->forkGameplayOptionsConfigured)
        return FORK_MAX_GEN_MONS;
    return gSaveBlock3Ptr->forkRandomizerMaxGen;
}

bool32 ForkAreMegaEvolutionsEnabled(void)
{
    return !gSaveBlock3Ptr->forkGameplayOptionsConfigured || gSaveBlock3Ptr->forkMegaEvolutionEnabled;
}

bool32 ForkAreTMsReusable(void)
{
    return !gSaveBlock3Ptr->forkGameplayOptionsConfigured ? I_REUSABLE_TMS : gSaveBlock3Ptr->forkReusableTMsEnabled;
}

void ForkConfigureGameplayOptions(bool32 catchLimitEnabled, bool32 levelCapEnabled, u8 faintRule, bool32 itemsInBattleEnabled, bool32 infiniteRareCandyEnabled, bool32 infiniteRepelEnabled, bool32 playerEvsEnabled, bool32 itemRandomizerEnabled, bool32 randomEncountersEnabled, bool32 randomAbilitiesEnabled, u8 randomizerMaxGen, bool32 megaEvolutionEnabled, bool32 reusableTMsEnabled)
{
    gSaveBlock3Ptr->forkGameplayOptionsConfigured = TRUE;
    gSaveBlock3Ptr->forkCatchLimitEnabled = catchLimitEnabled;
    gSaveBlock3Ptr->forkLevelCapEnabled = levelCapEnabled;
    gSaveBlock3Ptr->forkFaintRule = faintRule;
    gSaveBlock3Ptr->forkItemsInBattleEnabled = itemsInBattleEnabled;
    gSaveBlock3Ptr->forkInfiniteRareCandyEnabled = infiniteRareCandyEnabled;
    gSaveBlock3Ptr->forkInfiniteRepelEnabled = infiniteRepelEnabled;
    gSaveBlock3Ptr->forkPlayerEvsEnabled = playerEvsEnabled;
    gSaveBlock3Ptr->forkItemRandomizerEnabled = itemRandomizerEnabled;
    gSaveBlock3Ptr->forkRandomEncountersEnabled = randomEncountersEnabled;
    gSaveBlock3Ptr->forkRandomizerMaxGen = randomizerMaxGen;
    gSaveBlock3Ptr->forkRandomAbilitiesEnabled = randomAbilitiesEnabled;
    gSaveBlock3Ptr->forkMegaEvolutionEnabled = megaEvolutionEnabled;
    gSaveBlock3Ptr->forkReusableTMsEnabled = reusableTMsEnabled;
}

static void EnsureAreaStateInitialized(void)
{
    if (FlagGet(FORK_AREA_STATE_INIT_FLAG))
        return;

    for (u32 i = 0; i < ARRAY_COUNT(sAreaStateVars); i++)
        VarSet(sAreaStateVars[i], 0);
    FlagSet(FORK_AREA_STATE_INIT_FLAG);
}

static enum Species GetFamilyRootSpecies(enum Species species)
{
    if (species != SPECIES_NONE && !IsSpeciesEnabled(species))
        return SPECIES_NONE;
    while (species != SPECIES_NONE && GetSpeciesPreEvolution(species) != SPECIES_NONE)
        species = GetSpeciesPreEvolution(species);
    return species;
}

static void SetOwnedFamilyRoot(enum Species species)
{
    enum Species rootSpecies = GetFamilyRootSpecies(species);

    if (rootSpecies == SPECIES_NONE)
        return;

    sOwnedFamilyRoots[rootSpecies / 8] |= 1 << (rootSpecies % 8);
}

static bool32 HasOwnedFamilyRoot(enum Species species)
{
    enum Species rootSpecies = GetFamilyRootSpecies(species);

    if (rootSpecies == SPECIES_NONE)
        return FALSE;

    return (sOwnedFamilyRoots[rootSpecies / 8] >> (rootSpecies % 8)) & 1;
}

static void BuildOwnedFamilyCache(void)
{
    struct Pokemon mon;

    memset(sOwnedFamilyRoots, 0, sizeof(sOwnedFamilyRoots));

    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES) == SPECIES_NONE)
            continue;
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_IS_EGG))
            continue;
        SetOwnedFamilyRoot(GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES));
    }

    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
    {
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
        {
            struct BoxPokemon *boxMon = GetBoxedMonPtr(box, slot);
            enum Species boxedSpecies = GetBoxMonData(boxMon, MON_DATA_SPECIES);

            if (boxedSpecies == SPECIES_NONE || GetBoxMonData(boxMon, MON_DATA_IS_EGG))
                continue;
            SetOwnedFamilyRoot(boxedSpecies);
        }
    }

    for (u32 i = 0; i < DAYCARE_MON_COUNT; i++)
    {
        if (GetBoxMonData(&gSaveBlock1Ptr->daycare.mons[i].mon, MON_DATA_SPECIES) == SPECIES_NONE)
            continue;
        BoxMonToMon(&gSaveBlock1Ptr->daycare.mons[i].mon, &mon);
        if (GetMonData(&mon, MON_DATA_IS_EGG))
            continue;
        SetOwnedFamilyRoot(GetMonData(&mon, MON_DATA_SPECIES));
    }

    for (u32 checkSpecies = SPECIES_BULBASAUR; checkSpecies < NUM_SPECIES; checkSpecies++)
    {
        if (!IsSpeciesEnabled(checkSpecies))
            continue;
        if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(checkSpecies), FLAG_GET_CAUGHT))
            SetOwnedFamilyRoot(checkSpecies);
    }

    sOwnedFamilyCacheBuilt = TRUE;
}

static void ZeroMonEvs(struct Pokemon *mon)
{
    u8 zero = 0;

    SetMonData(mon, MON_DATA_HP_EV, &zero);
    SetMonData(mon, MON_DATA_ATK_EV, &zero);
    SetMonData(mon, MON_DATA_DEF_EV, &zero);
    SetMonData(mon, MON_DATA_SPEED_EV, &zero);
    SetMonData(mon, MON_DATA_SPATK_EV, &zero);
    SetMonData(mon, MON_DATA_SPDEF_EV, &zero);
    CalculateMonStats(mon);
}

static void ResetCurrentEncounterState(void)
{
    sForkEncounterActive = FALSE;
    sForkEncounterResolved = FALSE;
    sForkEncounterShouldSpend = FALSE;
    sForkEncounterCatchable = TRUE;
    sForkEncounterMapSec = MAPSEC_NONE;
}

static void ResolveCurrentEncounterState(void)
{
    struct Pokemon *mon;
    enum Species species;
    bool32 isShiny;
    bool32 isDupe;
    bool32 areaSpent;

    if (!sForkEncounterActive || sForkEncounterResolved || sForkEncounterMapSec >= MAPSEC_COUNT)
        return;

    if (!ForkIsAreaEncounterRuleActive())
    {
        sForkEncounterResolved = TRUE;
        return;
    }

    mon = &gParties[B_TRAINER_OPPONENT_A][0];
    species = GetMonData(mon, MON_DATA_SPECIES);
    if (species == SPECIES_NONE)
        return;

    isShiny = GetMonData(mon, MON_DATA_IS_SHINY);
    isDupe = ForkPlayerOwnsSpeciesFamily(species);
    areaSpent = ForkIsAreaEncounterSpent(sForkEncounterMapSec);

    sForkEncounterResolved = TRUE;
    sForkEncounterCatchable = isShiny || (!areaSpent && !isDupe);
    sForkEncounterShouldSpend = !isShiny && !isDupe && !areaSpent;
}

void ForkResetAreaEncounterState(void)
{
    for (u32 i = 0; i < ARRAY_COUNT(sAreaStateVars); i++)
        VarSet(sAreaStateVars[i], 0);
    FlagSet(FORK_AREA_STATE_INIT_FLAG);
}

bool32 ForkIsAreaEncounterSpent(mapsec_u8_t mapSecId)
{
    u32 word;
    u16 value;

    EnsureAreaStateInitialized();
    if (mapSecId >= MAPSEC_COUNT)
        return FALSE;

    word = mapSecId / 16;
    value = VarGet(sAreaStateVars[word]);
    return (value >> (mapSecId % 16)) & 1;
}

void ForkSetAreaEncounterSpent(mapsec_u8_t mapSecId)
{
    u32 word;
    u16 value;

    EnsureAreaStateInitialized();
    if (mapSecId >= MAPSEC_COUNT)
        return;

    word = mapSecId / 16;
    value = VarGet(sAreaStateVars[word]);
    value |= 1 << (mapSecId % 16);
    VarSet(sAreaStateVars[word], value);
}

bool32 ForkPlayerOwnsSpeciesFamily(enum Species species)
{
    if (species == SPECIES_NONE)
        return FALSE;
    if (!sOwnedFamilyCacheBuilt)
        BuildOwnedFamilyCache();
    return HasOwnedFamilyRoot(species);
}

bool32 ForkIsSoftNuzlockeMon(struct Pokemon *mon)
{
    return GetMonData(mon, MON_DATA_SOFT_NUZLOCKE);
}

void ForkEnsureKeyItemsPresent(void)
{
    if (ForkAreMegaEvolutionsEnabled() && !CheckBagHasItem(ITEM_MEGA_RING, 1))
        AddBagItem(ITEM_MEGA_RING, 1);
    if (ForkHasInfiniteRareCandy() && !CheckBagHasItem(ITEM_INFINITE_RARE_CANDY, 1))
        AddBagItem(ITEM_INFINITE_RARE_CANDY, 1);
    if (ForkHasInfiniteRepel() && !CheckBagHasItem(ITEM_INFINITE_REPEL, 1))
        AddBagItem(ITEM_INFINITE_REPEL, 1);
}

void ForkInvalidateOwnedFamilyCache(void)
{
    sOwnedFamilyCacheBuilt = FALSE;
}

void ForkRecordOwnedSpecies(enum Species species)
{
    if (species == SPECIES_NONE || !IsSpeciesEnabled(species))
        return;

    if (!sOwnedFamilyCacheBuilt)
        BuildOwnedFamilyCache();
    SetOwnedFamilyRoot(species);
}

void ForkNormalizePlayerMon(struct Pokemon *mon)
{
    if (GetMonData(mon, MON_DATA_SPECIES) == SPECIES_NONE)
        return;
    ZeroMonEvs(mon);
    if (!GetMonData(mon, MON_DATA_IS_EGG))
        ForkRecordOwnedSpecies(GetMonData(mon, MON_DATA_SPECIES));
}

void ForkScrubPlayerOwnedMons(void)
{
    struct Pokemon mon;

    ForkInvalidateOwnedFamilyCache();

    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES) != SPECIES_NONE)
            ZeroMonEvs(&gParties[B_TRAINER_PLAYER][i]);
    }

    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
    {
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
        {
            struct BoxPokemon *boxMon = GetBoxedMonPtr(box, slot);
            if (GetBoxMonData(boxMon, MON_DATA_SPECIES) == SPECIES_NONE)
                continue;
            BoxMonToMon(boxMon, &mon);
            ZeroMonEvs(&mon);
            CopyMon(boxMon, &mon.box, sizeof(mon.box));
        }
    }

    for (u32 i = 0; i < DAYCARE_MON_COUNT; i++)
    {
        if (GetBoxMonData(&gSaveBlock1Ptr->daycare.mons[i].mon, MON_DATA_SPECIES) == SPECIES_NONE)
            continue;
        BoxMonToMon(&gSaveBlock1Ptr->daycare.mons[i].mon, &mon);
        ZeroMonEvs(&mon);
        CopyMon(&gSaveBlock1Ptr->daycare.mons[i].mon, &mon.box, sizeof(mon.box));
    }
}

void ForkPrepareWildEncounter(void)
{
    ResetCurrentEncounterState();

    sForkEncounterMapSec = ForkGetCurrentMapSec();
    if (sForkEncounterMapSec >= MAPSEC_COUNT)
        return;

    sForkEncounterActive = TRUE;
}

void ForkResolveWildEncounter(void)
{
    ResolveCurrentEncounterState();
}

void ForkFinalizeWildEncounter(void)
{
    ResolveCurrentEncounterState();

    if (sForkEncounterActive && sForkEncounterShouldSpend && sForkEncounterMapSec < MAPSEC_COUNT)
        ForkSetAreaEncounterSpent(sForkEncounterMapSec);

    ResetCurrentEncounterState();
}

bool32 ForkCanCatchCurrentEncounter(void)
{
    ResolveCurrentEncounterState();
    return !sForkEncounterActive || sForkEncounterCatchable;
}

bool32 ForkShouldRejectSelectedBall(u16 itemId)
{
    return GetItemBattleUsage(itemId) == EFFECT_ITEM_THROW_BALL && !ForkCanCatchCurrentEncounter();
}

bool32 ForkShouldShowFirstEncounterIndicator(void)
{
    ResolveCurrentEncounterState();
    return sForkEncounterActive && sForkEncounterCatchable;
}

bool32 ForkShouldBlockEggHatchInCurrentArea(void)
{
    mapsec_u8_t mapSecId = ForkGetCurrentMapSec();

    return ForkIsAreaEncounterRuleActive() && mapSecId < MAPSEC_COUNT && ForkIsAreaEncounterSpent(mapSecId);
}

void ForkSpendCurrentAreaEncounter(void)
{
    mapsec_u8_t mapSecId = ForkGetCurrentMapSec();

    if (ForkIsAreaEncounterRuleActive() && mapSecId < MAPSEC_COUNT)
        ForkSetAreaEncounterSpent(mapSecId);
}

void ForkApplySoftNuzlockeWhiteOutPenalty(void)
{
    if (!ForkIsAreaEncounterRuleActive() || ForkGetFaintRule() != FORK_FAINT_RULE_WHITEOUT)
        return;

    u8 candidates[PARTY_SIZE];
    u8 candidateCount = 0;
    u32 softNuzlocke = TRUE;
    u32 exp;
    u8 level = 1;

    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES) == SPECIES_NONE)
            continue;
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_IS_EGG))
            continue;
        candidates[candidateCount++] = i;
    }

    if (candidateCount == 0)
        return;

    u8 slot = candidates[Random() % candidateCount];
    enum Species species = GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_SPECIES);

    SetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_SOFT_NUZLOCKE, &softNuzlocke);
    SetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_LEVEL, &level);
    exp = gExperienceTables[gSpeciesInfo[species].growthRate][level];
    SetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_EXP, &exp);
    CalculateMonStats(&gParties[B_TRAINER_PLAYER][slot]);
    SetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HP, &gParties[B_TRAINER_PLAYER][slot].maxHP);
}

void ForkApplySoftNuzlockeFaintPenalty(u8 partySlot)
{
    struct Pokemon *mon;
    enum Species species;
    u32 softNuzlocke = TRUE;
    u8 level = 1;
    u32 exp;

    if (!ForkIsAreaEncounterRuleActive()
     || ForkGetFaintRule() != FORK_FAINT_RULE_ON_FAINT
     || partySlot >= PARTY_SIZE)
        return;

    mon = &gParties[B_TRAINER_PLAYER][partySlot];
    species = GetMonData(mon, MON_DATA_SPECIES);
    if (species == SPECIES_NONE || GetMonData(mon, MON_DATA_IS_EGG))
        return;

    SetMonData(mon, MON_DATA_SOFT_NUZLOCKE, &softNuzlocke);
    SetMonData(mon, MON_DATA_LEVEL, &level);
    exp = gExperienceTables[gSpeciesInfo[species].growthRate][level];
    SetMonData(mon, MON_DATA_EXP, &exp);
    CalculateMonStats(mon);
    SetMonData(mon, MON_DATA_HP, &mon->maxHP);
}
