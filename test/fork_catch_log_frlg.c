#include "global.h"
#include "event_data.h"
#include "fork_run.h"
#include "test/test.h"
#include "constants/region_map_sections.h"

#if IS_FRLG
u32 ForkGetKantoCatchLogAreaCount(void);
mapsec_u8_t ForkGetKantoCatchLogArea(u32 index);
u32 ForkCountUsedKantoCatchLogAreas(void);
bool32 ForkIsKantoCatchLogAreaSpent(mapsec_u8_t area);

TEST("FRLG Catch Log lists pre-League Kanto and tracks spent areas")
{
    bool32 hasForest = FALSE;
    bool32 hasSevii = FALSE;
    bool32 hasCeruleanCave = FALSE;

    ForkResetAreaEncounterState();
    EXPECT_EQ(ForkGetKantoCatchLogArea(0), MAPSEC_PALLET_TOWN);
    for (u32 i = 0; i < ForkGetKantoCatchLogAreaCount(); i++)
    {
        mapsec_u8_t area = ForkGetKantoCatchLogArea(i);
        hasForest |= area == MAPSEC_VIRIDIAN_FOREST;
        hasSevii |= area == MAPSEC_ONE_ISLAND;
        hasCeruleanCave |= area == MAPSEC_CERULEAN_CAVE;
    }
    EXPECT(hasForest);
    EXPECT(!hasSevii);
    EXPECT(!hasCeruleanCave);
    EXPECT_EQ(ForkCountUsedKantoCatchLogAreas(), 0);
    ForkSetAreaEncounterSpent(MAPSEC_VIRIDIAN_FOREST);
    EXPECT_EQ(ForkCountUsedKantoCatchLogAreas(), 1);
    ForkSetAreaEncounterSpent(MAPSEC_SAFARI_ZONE);
    EXPECT(ForkIsKantoCatchLogAreaSpent(MAPSEC_KANTO_SAFARI_ZONE));
    EXPECT_EQ(ForkCountUsedKantoCatchLogAreas(), 2);
}
#endif
