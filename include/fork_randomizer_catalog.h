#ifndef GUARD_FORK_RANDOMIZER_CATALOG_H
#define GUARD_FORK_RANDOMIZER_CATALOG_H

bool8 ForkRandomizerCatalog_IsSpeciesAvailable(enum Species species);
enum Species ForkRandomizerCatalog_GetFamily(enum Species species);
bool8 ForkRandomizerCatalog_IsFamilyAbilityRandomized(enum Species species);
u16 ForkRandomizerCatalog_GetAbilityCount(void);
enum Ability ForkRandomizerCatalog_GetAbility(u16 index);
bool8 ForkRandomizerCatalog_IsAbilitySource(enum Species species, enum Ability ability);
bool8 ForkRandomizerCatalog_IsEvolutionItemRelevant(enum Item item);
bool8 ForkRandomizerCatalog_IsMegaStoneRelevant(enum Item item);
void ForkRandomizerCatalog_Invalidate(void);
void ForkRandomizerCatalog_EnsureInitialized(void);

#endif
