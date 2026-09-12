#include "global.h"
#include "data.h"
#include "fork_ability_randomizer.h"
#include "fork_run.h"
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

TEST("Mega Pokemon keep their native abilities")
{
    gSaveBlock3Ptr->forkItemRandomizerSeed = 0xA11B17E5;

    EXPECT(gSpeciesInfo[SPECIES_KANGASKHAN_MEGA].isMegaEvolution);
    EXPECT_EQ(IsForkAbilityRandomizedSpecies(SPECIES_KANGASKHAN_MEGA), FALSE);
    EXPECT_EQ(GetForkRandomizedAbility(SPECIES_KANGASKHAN_MEGA),
              GetSpeciesAbility(SPECIES_KANGASKHAN_MEGA, 0));
}

TEST("Mega Pokemon do not contribute abilities to the randomizer pool")
{
    ForkConfigureGameplayOptions(FALSE, FALSE, FORK_FAINT_RULE_WHITEOUT,
                                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                                 TRUE, GEN_4, TRUE, FALSE);
    gSaveBlock3Ptr->forkItemRandomizerSeed = 0xA11B17E5;

    for (u16 i = 0; i < GetForkAbilityPoolCount(); i++)
        EXPECT_NE(GetForkAbilityPoolEntry(i), ABILITY_PARENTAL_BOND);
}

TEST("Unavailable regional forms keep their native abilities")
{
    gSaveBlock3Ptr->forkItemRandomizerSeed = 0xA11B17E5;

    EXPECT_NE(SPECIES_GEODUDE_ALOLA, GET_BASE_SPECIES_ID(SPECIES_GEODUDE_ALOLA));
    EXPECT_EQ(IsForkAbilityRandomizedSpecies(SPECIES_GEODUDE_ALOLA), FALSE);
    EXPECT_EQ(GetForkRandomizedAbility(SPECIES_GEODUDE_ALOLA),
              GetSpeciesAbility(SPECIES_GEODUDE_ALOLA, 0));
}

TEST("Unavailable regional forms do not contribute abilities to the randomizer pool")
{
    ForkConfigureGameplayOptions(FALSE, FALSE, FORK_FAINT_RULE_WHITEOUT,
                                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                                 TRUE, GEN_4, TRUE, FALSE);
    gSaveBlock3Ptr->forkItemRandomizerSeed = 0xA11B17E5;

    for (u16 i = 0; i < GetForkAbilityPoolCount(); i++)
        EXPECT_NE(GetForkAbilityPoolEntry(i), ABILITY_GALVANIZE);
}

TEST("Generation 4 ability randomizer prints its finalized ability pool")
{
    ForkConfigureGameplayOptions(FALSE, FALSE, FORK_FAINT_RULE_WHITEOUT,
                                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                                 TRUE, GEN_4, TRUE, FALSE);
    gSaveBlock3Ptr->forkItemRandomizerSeed = 0xA11B17E5;

    // Resolve every eligible species so this represents a completed randomization pass.
    for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
        if (IsForkAbilityRandomizedSpecies(species))
            GetForkRandomizedAbility(species);

    u16 count = GetForkAbilityPoolCount();

    EXPECT_GT(count, 0);
    Test_MgbaPrintf("Generation 4 ability pool (%d abilities):\n", count);
    for (u16 i = 0; i < count; i++)
        Test_MgbaPrintf("%d: %S\n", i + 1, gAbilitiesInfo[GetForkAbilityPoolEntry(i)].name);
}

TEST("Generation 4 ability pool prints the Pokemon contributing each ability")
{
    ForkConfigureGameplayOptions(FALSE, FALSE, FORK_FAINT_RULE_WHITEOUT,
                                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                                 TRUE, GEN_4, TRUE, FALSE);
    gSaveBlock3Ptr->forkItemRandomizerSeed = 0xA11B17E5;

    u16 count = GetForkAbilityPoolCount();

    EXPECT_GT(count, 0);
    Test_MgbaPrintf("Generation 4 ability pool sources (%d abilities):\n", count);
    for (u16 i = 0; i < count; i++)
    {
        enum Ability ability = GetForkAbilityPoolEntry(i);
        u16 sourceCount = 0;

        Test_MgbaPrintf("%d: %S\n", i + 1, gAbilitiesInfo[ability].name);
        for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
        {
            if (IsForkAbilityPoolSource(species, ability))
            {
                Test_MgbaPrintf("  - %S (species ID %d)\n", gSpeciesInfo[species].speciesName, species);
                sourceCount++;
            }
        }
        EXPECT_GT(sourceCount, 0);
    }
}
