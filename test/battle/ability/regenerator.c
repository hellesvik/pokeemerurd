#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Regenerator heals the pivot user without overwriting the replacement's HP")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_VOLT_SWITCH; }
    PARAMETRIZE { move = MOVE_U_TURN; }
    PARAMETRIZE { move = MOVE_FLIP_TURN; }
    PARAMETRIZE { move = MOVE_BATON_PASS; }
    GIVEN {
        gSaveBlock3Ptr->forkGameplayOptionsConfigured = TRUE;
        gSaveBlock3Ptr->forkRandomAbilitiesEnabled = TRUE;
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(200); }
        PLAYER(SPECIES_THUNDURUS) { Ability(ABILITY_REGENERATOR); HP(30); MaxHP(150); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { MOVE(player, move); SEND_OUT(player, 0); }
    } THEN {
        EXPECT_EQ(player->hp, 100);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 100);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HP), 80);
    }
}

SINGLE_BATTLE_TEST("Regenerator heals 1/3 of max HP upon switching out")
{
    u32 currHP;
    PARAMETRIZE { currHP = 1; }
    PARAMETRIZE { currHP = 2; }
    PARAMETRIZE { currHP = 3; }
    GIVEN {
        gSaveBlock3Ptr->forkGameplayOptionsConfigured = TRUE;
        gSaveBlock3Ptr->forkRandomAbilitiesEnabled = FALSE;
        PLAYER(SPECIES_SLOWBRO) { Ability(ABILITY_REGENERATOR); HP(currHP); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
    } SCENE {
        SWITCH_OUT_MESSAGE("Slowbro");
        SEND_IN_MESSAGE("Wobbuffet");
        SWITCH_OUT_MESSAGE("Wobbuffet");
        SEND_IN_MESSAGE("Slowbro");
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP / 3 + currHP);
    }
}

SINGLE_BATTLE_TEST("Regenerator heals 1/3 of max HP upon switching out but doesn't surpass max HP")
{
    u32 currHP;
    PARAMETRIZE { currHP = 5; }
    PARAMETRIZE { currHP = 4; }
    PARAMETRIZE { currHP = 3; }
    PARAMETRIZE { currHP = 2; }
    PARAMETRIZE { currHP = 1; }
    GIVEN {
        gSaveBlock3Ptr->forkGameplayOptionsConfigured = TRUE;
        gSaveBlock3Ptr->forkRandomAbilitiesEnabled = FALSE;
        PLAYER(SPECIES_SLOWBRO) { Ability(ABILITY_REGENERATOR); HP(currHP); MaxHP(5); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
    } SCENE {
        SWITCH_OUT_MESSAGE("Slowbro");
        SEND_IN_MESSAGE("Wobbuffet");
        SWITCH_OUT_MESSAGE("Wobbuffet");
        SEND_IN_MESSAGE("Slowbro");
    } THEN {
        EXPECT_LE(player->hp, player->maxHP);
    }
}
