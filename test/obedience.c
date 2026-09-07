#include "global.h"
#include "battle.h"
#include "battle_controllers.h"
#include "battle_util.h"
#include "battle_util2.h"
#include "event_data.h"
#include "random.h"
#include "test/test.h"
#include "constants/battle_move_resolution.h"
#include "constants/flags.h"

TEST("Temporary test mode makes an overleveled outsider obey")
{
    gBattleTypeFlags = 0;
    gBattlerAttacker = B_BATTLER_0;
    gBattlersCount = 2;
    gBattlerBattleController[gBattlerAttacker] = BATTLE_CONTROLLER_PLAYER;
    gBattleMons[gBattlerAttacker].level = 100;
    gBattleMons[gBattlerAttacker].metLevel = 100;
    gBattleMons[gBattlerAttacker].otId = 1;
    gBattleMons[gBattlerAttacker].species = SPECIES_RAYQUAZA;
    gCurrentMove = MOVE_SPLASH;
    FlagClear(FLAG_BADGE01_GET);
    FlagClear(FLAG_BADGE02_GET);
    FlagClear(FLAG_BADGE03_GET);
    FlagClear(FLAG_BADGE04_GET);
    FlagClear(FLAG_BADGE05_GET);
    FlagClear(FLAG_BADGE06_GET);
    FlagClear(FLAG_BADGE07_GET);
    FlagClear(FLAG_BADGE08_GET);
    SeedRng(1);
    AllocateBattleResources();

    EXPECT_EQ(GetAttackerObedienceForAction(), OBEYS);

    FreeBattleResources();
}
