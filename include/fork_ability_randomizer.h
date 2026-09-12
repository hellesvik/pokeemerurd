#ifndef GUARD_FORK_ABILITY_RANDOMIZER_H
#define GUARD_FORK_ABILITY_RANDOMIZER_H

#include "global.h"

bool8 IsForkAbilityRandomizedSpecies(enum Species species);
enum Ability GetForkRandomizedAbility(enum Species species);
#if TESTING
u16 GetForkAbilityPoolCount(void);
enum Ability GetForkAbilityPoolEntry(u16 index);
bool8 IsForkAbilityPoolSource(enum Species species, enum Ability ability);
#endif

#endif // GUARD_FORK_ABILITY_RANDOMIZER_H
