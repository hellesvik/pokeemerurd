#include "global.h"
#include "fork_ability_randomizer.h"
#include "pokemon.h"
#include "test/test.h"

TEST("Fork ability randomizer assigns one ability to an evolutionary family")
{
    gSaveBlock3Ptr->forkItemRandomizerSeed = 0xA11B17E5;

    EXPECT_EQ(GetForkRandomizedAbility(SPECIES_CHARMANDER), GetForkRandomizedAbility(SPECIES_CHARMELEON));
    EXPECT_EQ(GetForkRandomizedAbility(SPECIES_CHARMANDER), GetForkRandomizedAbility(SPECIES_CHARIZARD));
}

TEST("Castform keeps Forecast and Forecast is not randomized onto other families")
{
    gSaveBlock3Ptr->forkItemRandomizerSeed = 0xA11B17E5;

    EXPECT_EQ(GetForkRandomizedAbility(SPECIES_CASTFORM), ABILITY_FORECAST);
    EXPECT_NE(GetForkRandomizedAbility(SPECIES_CHARMANDER), ABILITY_FORECAST);
}

TEST("Species with form-specific excluded abilities keep their original abilities")
{
    EXPECT_EQ(GetForkRandomizedAbility(SPECIES_ARCEUS), GetSpeciesAbility(SPECIES_ARCEUS, 0));
}

TEST("Shedinja keeps Wonder Guard")
{
    EXPECT_EQ(GetSpeciesAbility(SPECIES_SHEDINJA, 0), ABILITY_WONDER_GUARD);
    EXPECT_EQ(IsForkAbilityRandomizedSpecies(SPECIES_SHEDINJA), FALSE);
    EXPECT_EQ(GetForkRandomizedAbility(SPECIES_SHEDINJA), ABILITY_WONDER_GUARD);
}

TEST("Special-static legendary families receive randomized abilities")
{
    gSaveBlock3Ptr->forkItemRandomizerSeed = 0xA11B17E5;

    EXPECT_EQ(IsForkAbilityRandomizedSpecies(SPECIES_LATIAS), TRUE);
    EXPECT_NE(GetForkRandomizedAbility(SPECIES_LATIAS), ABILITY_LEVITATE);
}

TEST("Wonder Guard is not assigned by the ability randomizer")
{
    gSaveBlock3Ptr->forkItemRandomizerSeed = 0xA11B17E5;

    for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        if (IsForkAbilityRandomizedSpecies(species))
            EXPECT_NE(GetForkRandomizedAbility(species), ABILITY_WONDER_GUARD);
    }
}
