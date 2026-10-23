#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Natural Cure cures the pivot user without curing the replacement")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_VOLT_SWITCH; }
    PARAMETRIZE { move = MOVE_U_TURN; }
    PARAMETRIZE { move = MOVE_FLIP_TURN; }
    PARAMETRIZE { move = MOVE_BATON_PASS; }
    GIVEN {
        gSaveBlock3Ptr->forkGameplayOptionsConfigured = TRUE;
        gSaveBlock3Ptr->forkRandomAbilitiesEnabled = TRUE;
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_POISON); }
        PLAYER(SPECIES_THUNDURUS) { Ability(ABILITY_NATURAL_CURE); Status1(STATUS1_BURN); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { MOVE(player, move); SEND_OUT(player, 0); }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_POISON);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_STATUS), STATUS1_POISON);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_STATUS), STATUS1_NONE);
    }
}
