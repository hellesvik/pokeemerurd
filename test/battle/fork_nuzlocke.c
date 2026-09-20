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

SINGLE_BATTLE_TEST("Revival Blessing cannot revive a permanently lost Pokemon")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_REVIVAL_BLESSING) == EFFECT_REVIVAL_BLESSING);
        ForkConfigureGameplayOptions(TRUE, FALSE, FORK_FAINT_RULE_ON_FAINT, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, GEN_3, TRUE, FALSE);
        FlagSet(FLAG_ADVENTURE_STARTED);
        PLAYER(SPECIES_EEVEE) { HP(1); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); SEND_OUT(player, 1); }
        TURN { MOVE(player, MOVE_REVIVAL_BLESSING); }
    } SCENE {
        MESSAGE("Wobbuffet used Revival Blessing!");
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SOFT_NUZLOCKE), TRUE);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 0);
    }
}

MULTI_BATTLE_TEST("Nuzlocke does not mark a player Pokemon when the AI partner faints")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_DRAGON_RAGE) == EFFECT_FIXED_HP_DAMAGE);
        ForkConfigureGameplayOptions(TRUE, FALSE, FORK_FAINT_RULE_ON_FAINT, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, GEN_3, TRUE, FALSE);
        FlagSet(FLAG_ADVENTURE_STARTED);
        PLAYER(SPECIES_EEVEE);
        PARTNER(SPECIES_WOBBUFFET) { HP(1); }
        PARTNER(SPECIES_MUDKIP);
        OPPONENT_A(SPECIES_DRAGONITE) { Moves(MOVE_DRAGON_RAGE); }
        OPPONENT_B(SPECIES_MAGIKARP) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_DRAGON_RAGE, target:playerRight); SEND_OUT(playerRight, 1); }
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SOFT_NUZLOCKE), FALSE);
        EXPECT_GT(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 0);
    }
}
