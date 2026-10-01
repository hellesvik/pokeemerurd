#include "global.h"
#include "battle_setup.h"
#include "caps.h"
#include "event_data.h"
#include "fork_run.h"
#include "test/battle.h"
#include "constants/flags.h"
#include "constants/map_groups.h"

static void ConfigureBattleRules(enum ForkFaintRule faintRule, bool32 levelCap, bool32 playerEvs)
{
    ForkConfigureGameplayOptions(FALSE, levelCap, faintRule,
                                 TRUE, FALSE, FALSE, playerEvs, FALSE, FALSE,
                                 FALSE, GEN_9, TRUE, FALSE);
}

SINGLE_BATTLE_TEST("Fork battle rules: faint penalty depends on selected rule and story start")
{
    enum ForkFaintRule faintRule = FORK_FAINT_RULE_OFF;
    bool32 adventureStarted = FALSE;
    PARAMETRIZE { faintRule = FORK_FAINT_RULE_OFF; adventureStarted = TRUE; }
    PARAMETRIZE { faintRule = FORK_FAINT_RULE_WHITEOUT; adventureStarted = TRUE; }
    PARAMETRIZE { faintRule = FORK_FAINT_RULE_ON_FAINT; adventureStarted = FALSE; }
    PARAMETRIZE { faintRule = FORK_FAINT_RULE_ON_FAINT; adventureStarted = TRUE; }
    GIVEN {
        ConfigureBattleRules(faintRule, FALSE, TRUE);
        if (adventureStarted)
            FlagSet(FLAG_ADVENTURE_STARTED);
        else
            FlagClear(FLAG_ADVENTURE_STARTED);
        PLAYER(SPECIES_EEVEE) { HP(1); }
        PLAYER(SPECIES_MUDKIP);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); SEND_OUT(player, 1); }
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SOFT_NUZLOCKE),
                  faintRule == FORK_FAINT_RULE_ON_FAINT && adventureStarted);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SOFT_NUZLOCKE), FALSE);
    }
}

SINGLE_BATTLE_TEST("Fork battle rules: poison faint also marks a permanently lost Pokemon")
{
    GIVEN {
        ConfigureBattleRules(FORK_FAINT_RULE_ON_FAINT, FALSE, TRUE);
        FlagSet(FLAG_ADVENTURE_STARTED);
        PLAYER(SPECIES_EEVEE) { HP(1); Status1(STATUS1_POISON); }
        PLAYER(SPECIES_MUDKIP);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SPLASH); SEND_OUT(player, 1); }
    } SCENE {
        MESSAGE("Eevee was hurt by its poisoning!");
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SOFT_NUZLOCKE), TRUE);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 0);
    }
}

SINGLE_BATTLE_TEST("Fork battle rules: self-inflicted faint also marks a permanently lost Pokemon")
{
    GIVEN {
        ConfigureBattleRules(FORK_FAINT_RULE_ON_FAINT, FALSE, TRUE);
        FlagSet(FLAG_ADVENTURE_STARTED);
        PLAYER(SPECIES_EEVEE);
        PLAYER(SPECIES_MUDKIP);
        OPPONENT(SPECIES_GASTLY);
    } WHEN {
        TURN { MOVE(player, MOVE_EXPLOSION); SEND_OUT(player, 1); }
    } SCENE {
        MESSAGE("Eevee fainted!");
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SOFT_NUZLOCKE), TRUE);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 0);
    }
}

DOUBLE_BATTLE_TEST("Fork battle rules: two player faints in one turn mark both party slots")
{
    GIVEN {
        ConfigureBattleRules(FORK_FAINT_RULE_ON_FAINT, FALSE, TRUE);
        FlagSet(FLAG_ADVENTURE_STARTED);
        PLAYER(SPECIES_EEVEE) { HP(1); }
        PLAYER(SPECIES_MUDKIP) { HP(1); }
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_GARCHOMP);
        OPPONENT(SPECIES_GASTLY);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_EARTHQUAKE); SEND_OUT(playerLeft, 2); SEND_OUT(playerRight, 3); }
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SOFT_NUZLOCKE), TRUE);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SOFT_NUZLOCKE), TRUE);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][2], MON_DATA_SOFT_NUZLOCKE), FALSE);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][3], MON_DATA_SOFT_NUZLOCKE), FALSE);
    }
}

MULTI_BATTLE_TEST("Fork battle rules: partner and opponent faints never mark a player's Pokemon")
{
    GIVEN {
        ConfigureBattleRules(FORK_FAINT_RULE_ON_FAINT, FALSE, TRUE);
        FlagSet(FLAG_ADVENTURE_STARTED);
        PLAYER(SPECIES_EEVEE);
        PARTNER(SPECIES_WOBBUFFET) { HP(1); }
        PARTNER(SPECIES_WYNAUT);
        OPPONENT_A(SPECIES_DRAGONITE) { Moves(MOVE_DRAGON_RAGE); }
        OPPONENT_B(SPECIES_GASTLY);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_DRAGON_RAGE, target:playerRight); SEND_OUT(playerRight, 1); }
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SOFT_NUZLOCKE), FALSE);
        EXPECT_GT(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 0);
    }
}

SINGLE_BATTLE_TEST("Fork battle rules: Revival Blessing skips lost Pokemon but revives another fainted teammate")
{
    u32 lost = TRUE;
    GIVEN {
        ConfigureBattleRules(FORK_FAINT_RULE_ON_FAINT, FALSE, TRUE);
        FlagSet(FLAG_ADVENTURE_STARTED);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_EEVEE) { HP(0); }
        PLAYER(SPECIES_MUDKIP) { HP(0); }
        SetMonData(&PLAYER_PARTY[1], MON_DATA_SOFT_NUZLOCKE, &lost);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_REVIVAL_BLESSING, partyIndex:2); }
    } SCENE {
        MESSAGE("Mudkip was revived and is ready to fight again!");
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HP), 0);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SOFT_NUZLOCKE), TRUE);
        EXPECT_GT(GetMonData(&gParties[B_TRAINER_PLAYER][2], MON_DATA_HP), 0);
    }
}

SINGLE_BATTLE_TEST("Fork battle rules: lost Pokemon cannot be revived by a bag item")
{
    u32 lost = TRUE;
    GIVEN {
        ConfigureBattleRules(FORK_FAINT_RULE_ON_FAINT, FALSE, TRUE);
        FlagSet(FLAG_ADVENTURE_STARTED);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_EEVEE) { HP(0); }
        SetMonData(&PLAYER_PARTY[1], MON_DATA_SOFT_NUZLOCKE, &lost);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { USE_ITEM(player, ITEM_REVIVE, partyIndex:1); }
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HP), 0);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SOFT_NUZLOCKE), TRUE);
    }
}

WILD_BATTLE_TEST("Fork battle rules: disabled player EVs block EV gain from battle")
{
    bool32 evsEnabled = FALSE;
    PARAMETRIZE { evsEnabled = FALSE; }
    PARAMETRIZE { evsEnabled = TRUE; }
    GIVEN {
        ConfigureBattleRules(FORK_FAINT_RULE_OFF, FALSE, evsEnabled);
        PLAYER(SPECIES_WOBBUFFET) { Level(20); }
        OPPONENT(SPECIES_ZUBAT) { Level(10); HP(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        EXPERIENCE_BAR(player);
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPEED_EV), evsEnabled ? 1 : 0);
    }
}

WILD_BATTLE_TEST("Fork battle rules: hard cap limits EXP earned from a defeated Pokemon", s32 earned)
{
    bool32 capped = FALSE;
    PARAMETRIZE { capped = FALSE; }
    PARAMETRIZE { capped = TRUE; }
    GIVEN {
        ConfigureBattleRules(FORK_FAINT_RULE_OFF, capped, TRUE);
        ClearTrainerFlag(TRAINER_ROXANNE_1);
        PLAYER(SPECIES_WOBBUFFET) { Level(15); }
        OPPONENT(SPECIES_BLISSEY) { Level(100); HP(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        EXPERIENCE_BAR(player, captureGainedExp: &results[i].earned);
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL) > 15, !capped);
        EXPECT_EQ(results[i].earned == 0, capped);
    }
}

WILD_BATTLE_TEST("Fork battle rules: a spent area's ball throw is rejected during battle")
{
    GIVEN {
        ForkConfigureGameplayOptions(TRUE, FALSE, FORK_FAINT_RULE_OFF,
                                     TRUE, FALSE, FALSE, TRUE, FALSE, FALSE,
                                     FALSE, GEN_9, TRUE, FALSE);
        FlagSet(FLAG_ADVENTURE_STARTED);
        ForkResetAreaEncounterState();
        gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROUTE101);
        gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROUTE101);
        gMapHeader.regionMapSectionId = MAPSEC_ROUTE_101;
        ForkSetAreaEncounterSpent(MAPSEC_ROUTE_101);
        ForkPrepareWildEncounter();
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_POOCHYENA);
    } WHEN {
        TURN { USE_ITEM(player, ITEM_POKE_BALL); }
    } SCENE {
        MESSAGE("Items can't be used now.{PAUSE 64}");
        NOT ANIMATION(ANIM_TYPE_SPECIAL, B_ANIM_BALL_THROW, player);
    } THEN {
        EXPECT_EQ(ForkCanCatchCurrentEncounter(), FALSE);
    }
}

WILD_BATTLE_TEST("Fork battle rules: a shiny remains catchable in a spent area")
{
    GIVEN {
        ForkConfigureGameplayOptions(TRUE, FALSE, FORK_FAINT_RULE_OFF,
                                     TRUE, FALSE, FALSE, TRUE, FALSE, FALSE,
                                     FALSE, GEN_9, TRUE, FALSE);
        FlagSet(FLAG_ADVENTURE_STARTED);
        ForkResetAreaEncounterState();
        gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROUTE101);
        gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROUTE101);
        gMapHeader.regionMapSectionId = MAPSEC_ROUTE_101;
        ForkSetAreaEncounterSpent(MAPSEC_ROUTE_101);
        ForkPrepareWildEncounter();
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_POOCHYENA) { Shiny(TRUE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPLASH); }
    } THEN {
        EXPECT_EQ(ForkCanCatchCurrentEncounter(), TRUE);
        EXPECT_EQ(ForkIsAreaEncounterSpent(MAPSEC_ROUTE_101), TRUE);
    }
}
