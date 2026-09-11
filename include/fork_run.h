#ifndef GUARD_FORK_RUN_H
#define GUARD_FORK_RUN_H

#include "global.h"
#include "pokemon.h"

enum ForkFaintRule
{
    FORK_FAINT_RULE_OFF,
    FORK_FAINT_RULE_WHITEOUT,
    FORK_FAINT_RULE_ON_FAINT,
};

void ForkResetAreaEncounterState(void);
bool32 ForkIsBattleStyleLocked(void);
void ForkSetBattleStyleLocked(bool32 locked);
bool32 ForkIsAreaEncounterRuleActive(void);
bool32 ForkIsCatchLimitEnabled(void);
bool32 ForkIsLevelCapEnabled(void);
void ForkConfigureGameplayOptions(bool32 catchLimitEnabled, bool32 levelCapEnabled, u8 faintRule, bool32 itemsInBattleEnabled, bool32 infiniteRareCandyEnabled, bool32 infiniteRepelEnabled, bool32 playerEvsEnabled, bool32 itemRandomizerEnabled, bool32 randomEncountersEnabled, bool32 randomAbilitiesEnabled, u8 randomizerMaxGen, bool32 megaEvolutionEnabled, bool32 reusableTMsEnabled);
bool32 ForkAreBattleItemsEnabled(void);
bool32 ForkHasInfiniteRareCandy(void);
bool32 ForkHasInfiniteRepel(void);
bool32 ForkArePlayerEvsEnabled(void);
bool32 ForkIsItemRandomizerEnabled(void);
bool32 ForkAreRandomEncountersEnabled(void);
bool32 ForkAreRandomAbilitiesEnabled(void);
u8 ForkGetRandomizerMaxGen(void);
bool32 ForkAreMegaEvolutionsEnabled(void);
bool32 ForkAreTMsReusable(void);
u8 ForkGetFaintRule(void);
void ForkApplySoftNuzlockeFaintPenalty(u8 partySlot);
bool32 ForkIsAreaEncounterSpent(mapsec_u8_t mapSecId);
void ForkSetAreaEncounterSpent(mapsec_u8_t mapSecId);
bool32 ForkPlayerOwnsSpeciesFamily(enum Species species);
bool32 ForkIsSoftNuzlockeMon(struct Pokemon *mon);
void ForkEnsureKeyItemsPresent(void);
void ForkInvalidateOwnedFamilyCache(void);
void ForkRecordOwnedSpecies(enum Species species);
void ForkNormalizePlayerMon(struct Pokemon *mon);
void ForkPrepareWildEncounter(void);
void ForkResolveWildEncounter(void);
void ForkFinalizeWildEncounter(void);
bool32 ForkCanCatchCurrentEncounter(void);
bool32 ForkShouldRejectSelectedBall(u16 itemId);
bool32 ForkShouldShowFirstEncounterIndicator(void);
bool32 ForkShouldBlockEggHatchInCurrentArea(void);
void ForkSpendCurrentAreaEncounter(void);
void ForkApplySoftNuzlockeWhiteOutPenalty(void);

#endif
