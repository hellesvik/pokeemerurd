#include "global.h"
#include "battle.h"
#include "daycare.h"
#include "pokemon.h"
#include "random.h"
#include "test/test.h"

static void SetBreedingParents(enum Species secondSpecies)
{
    struct Pokemon mon;
    memset(&gSaveBlock1Ptr->daycare, 0, sizeof(gSaveBlock1Ptr->daycare));
    gPartiesCount[B_TRAINER_PLAYER] = 0;
    CreateMon(&mon, SPECIES_DITTO, 5, 0, OTID_STRUCT_PRESET(0));
    gSaveBlock1Ptr->daycare.mons[0].mon = mon.box;
    CreateMon(&mon, secondSpecies, 5, 0, OTID_STRUCT_PRESET(0));
    gSaveBlock1Ptr->daycare.mons[1].mon = mon.box;
}

TEST("Egg production checks after 25 steps and preserves a pending Egg")
{
    struct DayCare *daycare = &gSaveBlock1Ptr->daycare;
    u32 personality;
    u32 seed;
    SetBreedingParents(SPECIES_PIKACHU);
    for (u32 i = 0; i < 24; i++)
        ShouldEggHatch();
    EXPECT_EQ(daycare->offspringPersonality, 0);
    // Choose a successful compatibility roll with the current RNG algorithm.
    for (seed = 0; seed < 100; seed++)
    {
        SeedRng(seed);
        if ((Random() * 100u) / USHRT_MAX < GetDaycareCompatibilityScore(daycare))
            break;
    }
    EXPECT(seed < 100);
    SeedRng(seed);
    ShouldEggHatch();
    EXPECT(daycare->offspringPersonality != 0);
    EXPECT_EQ(daycare->mons[0].steps, 25);
    EXPECT_EQ(daycare->mons[1].steps, 25);
    personality = daycare->offspringPersonality;
    for (u32 i = 0; i < 25; i++)
        ShouldEggHatch();
    EXPECT_EQ(daycare->offspringPersonality, personality);
    EXPECT_EQ(daycare->mons[1].steps, 50);
}

TEST("Egg production still rejects incompatible parents at the faster interval")
{
    SetBreedingParents(SPECIES_DITTO);
    for (u32 i = 0; i < 100; i++)
    {
        SeedRng(0);
        ShouldEggHatch();
    }
    EXPECT_EQ(gSaveBlock1Ptr->daycare.offspringPersonality, 0);
}
