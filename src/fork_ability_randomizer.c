#include "global.h"
#include "fork_ability_randomizer.h"
#include "fork_randomizer_catalog.h"
#include "fork_run.h"
#include "pokemon.h"
#include "random.h"

#define FORK_ABILITY_RANDOMIZER_SALT 0x4142494C

#if TESTING
u16 GetForkAbilityPoolCount(void)
{
    return ForkRandomizerCatalog_GetAbilityCount();
}

enum Ability GetForkAbilityPoolEntry(u16 index)
{
    return ForkRandomizerCatalog_GetAbility(index);
}

bool8 IsForkAbilityPoolSource(enum Species species, enum Ability ability)
{
    return ForkRandomizerCatalog_IsAbilitySource(species, ability);
}
#endif

bool8 IsForkAbilityRandomizedSpecies(enum Species species)
{
    return ForkAreRandomAbilitiesEnabled()
        && ForkRandomizerCatalog_IsFamilyAbilityRandomized(species);
}

enum Ability GetForkRandomizedAbility(enum Species species)
{
    enum Species family;
    rng_value_t rng;
    u16 abilityCount;

    if (!IsForkAbilityRandomizedSpecies(species))
        return GetSpeciesAbility(species, 0);

    family = ForkRandomizerCatalog_GetFamily(species);
    abilityCount = ForkRandomizerCatalog_GetAbilityCount();
    if (abilityCount == 0)
        return GetSpeciesAbility(species, 0);
    rng = LocalRandomSeed(gSaveBlock3Ptr->forkItemRandomizerSeed ^ FORK_ABILITY_RANDOMIZER_SALT ^ family);
    return ForkRandomizerCatalog_GetAbility(LocalRandom(&rng) % abilityCount);
}
