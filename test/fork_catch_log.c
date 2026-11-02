#include "global.h"
#include "fork_run.h"
#include "test/test.h"
#include "constants/region_map_sections.h"

u32 Test_GetCatchLogAreaCount(void);
mapsec_u8_t Test_GetCatchLogArea(u32 index);
u32 Test_CountUsedCatchLogAreas(void);

TEST("Catch log lists unique canonical Hoenn areas in progression order")
{
    u32 safariCount = 0;
    u32 route = 101;
    EXPECT_EQ(Test_GetCatchLogArea(0), MAPSEC_LITTLEROOT_TOWN);
    EXPECT_EQ(Test_GetCatchLogArea(1), MAPSEC_ROUTE_101);
    for (u32 i = 0; i < Test_GetCatchLogAreaCount(); i++)
    {
        u32 area = Test_GetCatchLogArea(i);
        EXPECT(area < MAPSEC_COUNT);
        EXPECT(area != MAPSEC_NONE);
        EXPECT(area != MAPSEC_DYNAMIC);
        EXPECT(area != MAPSEC_UNDERWATER_124 && area != MAPSEC_UNDERWATER_125
            && area != MAPSEC_UNDERWATER_126 && area != MAPSEC_UNDERWATER_127
            && area != MAPSEC_UNDERWATER_128 && area != MAPSEC_UNDERWATER_129);
        safariCount += area == MAPSEC_SAFARI_ZONE;
        for (u32 j = 0; j < i; j++)
            EXPECT(area != Test_GetCatchLogArea(j));
    }
    // Every numbered Hoenn route appears once, including late ocean routes.
    for (; route <= 134; route++)
    {
        u32 count = 0;
        for (u32 i = 0; i < Test_GetCatchLogAreaCount(); i++)
            count += Test_GetCatchLogArea(i) == MAPSEC_ROUTE_101 + route - 101;
        EXPECT_EQ(count, 1);
    }
    EXPECT_EQ(safariCount, 1);
}

TEST("Catch log reads existing encounter bits without changing them")
{
    u8 before[FORK_AREA_ENCOUNTER_SPENT_BYTE_COUNT];
    // Initialize through the same migration path used by encounter checks.
    ForkIsAreaEncounterSpent(MAPSEC_ROUTE_101);
    memset(gSaveBlock3Ptr->forkAreaEncounterSpent, 0, sizeof(before));
    EXPECT_EQ(Test_CountUsedCatchLogAreas(), 0);
    ForkSetAreaEncounterSpent(MAPSEC_ROUTE_101);
    ForkSetAreaEncounterSpent(MAPSEC_SAFARI_ZONE);
    ForkSetAreaEncounterSpent(MAPSEC_SHOAL_CAVE);
    memcpy(before, gSaveBlock3Ptr->forkAreaEncounterSpent, sizeof(before));
    EXPECT_EQ(Test_CountUsedCatchLogAreas(), 3);
    EXPECT_EQ(memcmp(before, gSaveBlock3Ptr->forkAreaEncounterSpent, sizeof(before)), 0);
    gSaveBlock3Ptr->forkGameplayOptionsConfigured = TRUE;
    gSaveBlock3Ptr->forkCatchLimitEnabled = FALSE;
    EXPECT_EQ(Test_CountUsedCatchLogAreas(), 3);
    EXPECT_EQ(memcmp(before, gSaveBlock3Ptr->forkAreaEncounterSpent, sizeof(before)), 0);
}
