// Standalone flash-fixture test; run with an injected legacy save.
#include "global.h"
#include "caps.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "test/test.h"

#include "new_game.h"
#include "save.h"
#include "load_save.h"

TEST("Box compatibility train legacy fixture")
{
    CheckForFlashMemory();
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
    CopyPartyAndObjectsFromSave();
    EXPECT_EQ(GetLevelFromBoxMonExp(GetBoxedMonPtr(TOTAL_BOXES_COUNT - 1, IN_BOX_COUNT - 1)), 5);
    EXPECT_EQ(LevelAllBoxMonsToCap(), 1);
    EXPECT_EQ(GetLevelFromBoxMonExp(GetBoxedMonPtr(TOTAL_BOXES_COUNT - 1, IN_BOX_COUNT - 1)), 15);
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
}
