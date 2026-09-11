#include "global.h"
#include "event_data.h"
#include "fork_run.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Nuzlocke marks a player Pokemon when it faints in battle")
{
    GIVEN {
        ForkConfigureGameplayOptions(TRUE, FALSE, FORK_FAINT_RULE_ON_FAINT, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, GEN_3, TRUE, FALSE);
        FlagSet(FLAG_ADVENTURE_STARTED);
        PLAYER(SPECIES_EEVEE) { HP(1); }
        PLAYER(SPECIES_MUDKIP);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); SEND_OUT(player, 1); }
    } THEN {
        EXPECT_EQ(FlagGet(FLAG_ADVENTURE_STARTED), TRUE);
        EXPECT_EQ(ForkGetFaintRule(), FORK_FAINT_RULE_ON_FAINT);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SOFT_NUZLOCKE), TRUE);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 0);
    }
}
