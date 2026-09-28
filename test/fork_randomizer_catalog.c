#include "global.h"
#include "fork_randomizer_catalog.h"
#include "fork_run.h"
#include "fork_gift_pokemon_randomizer.h"
#include "item_ball.h"
#include "test/test.h"

TEST("Fork catalog follows encounter mode without manual invalidation")
{
    ForkConfigureGameplayOptions(FALSE, FALSE, 0, TRUE, FALSE, FALSE, TRUE, TRUE, TRUE, TRUE, GEN_3, TRUE, FALSE);
    EXPECT_EQ(ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_KYOGRE), FALSE);
    gSaveBlock3Ptr->forkRandomEncountersEnabled = FALSE;
    EXPECT_EQ(ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_KYOGRE), TRUE);
    EXPECT_EQ(IsForkItemEligibleForRandomizerPool(ITEM_BLUE_ORB), TRUE);
    EXPECT_EQ(ForkRandomizerCatalog_IsAbilitySource(SPECIES_RAYQUAZA, ABILITY_AIR_LOCK), TRUE);
    EXPECT_EQ(ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_ZIGZAGOON), TRUE);
    EXPECT_EQ(ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_RAYQUAZA_MEGA), FALSE);
    EXPECT_EQ(ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_ARCEUS), FALSE);
    gSaveBlock3Ptr->forkRandomEncountersEnabled = TRUE;
    EXPECT_EQ(ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_KYOGRE), FALSE);
    EXPECT_EQ(IsForkItemEligibleForRandomizerPool(ITEM_BLUE_ORB), FALSE);
}

TEST("Fork fixed encounter catalog includes evolutions beyond randomizer generation")
{
    ForkConfigureGameplayOptions(FALSE, FALSE, 0, TRUE, FALSE, FALSE, TRUE, TRUE, FALSE, TRUE, GEN_3, TRUE, FALSE);
    EXPECT_EQ(ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_RHYDON), TRUE);
    EXPECT_EQ(ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_RHYPERIOR), TRUE);
    EXPECT_EQ(ForkRandomizerCatalog_IsEvolutionItemRelevant(ITEM_PROTECTOR), TRUE);
    EXPECT_EQ(ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_BELDUM), TRUE);
    EXPECT_EQ(ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_METAGROSS), TRUE);
    EXPECT_EQ(ForkRandomizerCatalog_IsMegaStoneRelevant(ITEM_METAGROSSITE), TRUE);
}

TEST("Fork fixed encounter catalog follows seeded trade choices")
{
    ForkConfigureGameplayOptions(FALSE, FALSE, 0, TRUE, FALSE, FALSE, TRUE, TRUE, FALSE, TRUE, GEN_9, TRUE, FALSE);
    for (u32 seed = 0; seed < 16; seed++)
    {
        gSaveBlock3Ptr->forkItemRandomizerSeed = seed;
        EXPECT_EQ(ForkRandomizerCatalog_IsSpeciesAvailable(GetForkRandomizedFortreeTradeSpecies()), TRUE);
        EXPECT_EQ(ForkRandomizerCatalog_IsSpeciesAvailable(GetForkRandomizedRustboroTradeSpecies()), TRUE);
    }
}
