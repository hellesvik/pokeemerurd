#include "global.h"
#include "battle_main.h"
#include "fork_ability_randomizer.h"
#include "fork_run.h"
#include "test/battle.h"

// Register overrides through the real trainer-party builder. The battle runner
// subsequently loads PLAYER/OPPONENT parties into these same slots. Do not use
// Ability() for these opponents: its forced-ability hook can hide regressions.
static void CreateTrainerAbilities(enum BattleTrainer trainer, enum Species species, enum Ability active, enum Ability bench)
{
    const struct TrainerMon mons[] =
    {
        { .species = species, .lvl = 50, .ability = active },
        { .species = SPECIES_WOBBUFFET, .lvl = 50, .ability = bench },
    };
    const struct Trainer data =
    {
        .party = mons,
        .partySize = ARRAY_COUNT(mons),
        .battleType = TRAINER_BATTLE_TYPE_SINGLES,
    };
    CreateNPCTrainerPartyFromTrainer(gParties[trainer], &data, FALSE, BATTLE_TYPE_TRAINER);
}

static void ConfigureAbilities(bool32 randomized)
{
    ForkConfigureGameplayOptions(FALSE, FALSE, FORK_FAINT_RULE_OFF,
                                 TRUE, FALSE, FALSE, TRUE, FALSE, TRUE,
                                 randomized, GEN_9, TRUE, FALSE);
    gSaveBlock3Ptr->forkItemRandomizerSeed = 0xA11B17E5;
    ClearEnemyTrainerMonAbilityOverrides();
}

SINGLE_BATTLE_TEST("Fork battle: fixed trainer abilities override randomized species abilities")
{
    bool32 randomized;
    PARAMETRIZE { randomized = FALSE; }
    PARAMETRIZE { randomized = TRUE; }
    GIVEN {
        ConfigureAbilities(randomized);
        CreateTrainerAbilities(B_TRAINER_OPPONENT_A, SPECIES_WOBBUFFET, ABILITY_INTIMIDATE, ABILITY_NONE);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_RUN_AWAY); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SPLASH); }
    } THEN {
        EXPECT_EQ(opponent->ability, ABILITY_INTIMIDATE);
        EXPECT_EQ(GetMonAbility(&gParties[B_TRAINER_OPPONENT_A][0]), ABILITY_INTIMIDATE);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Fork battle: Mega Evolution replaces the trainer override and keeps the Mega ability after switching")
{
    enum Species species, mega;
    enum Ability baseAbility, megaAbility;
    enum Item stone;
    bool32 randomized;
    for (u32 j = 0; j < 2; j++)
    {
        PARAMETRIZE { randomized = j; species = SPECIES_MAWILE; mega = SPECIES_MAWILE_MEGA; baseAbility = ABILITY_INTIMIDATE; megaAbility = ABILITY_HUGE_POWER; stone = ITEM_MAWILITE; }
        PARAMETRIZE { randomized = j; species = SPECIES_METAGROSS; mega = SPECIES_METAGROSS_MEGA; baseAbility = ABILITY_CLEAR_BODY; megaAbility = ABILITY_TOUGH_CLAWS; stone = ITEM_METAGROSSITE; }
        PARAMETRIZE { randomized = j; species = SPECIES_GYARADOS; mega = SPECIES_GYARADOS_MEGA; baseAbility = ABILITY_INTIMIDATE; megaAbility = ABILITY_MOLD_BREAKER; stone = ITEM_GYARADOSITE; }
        PARAMETRIZE { randomized = j; species = SPECIES_RAYQUAZA; mega = SPECIES_RAYQUAZA_MEGA; baseAbility = ABILITY_INTIMIDATE; megaAbility = ABILITY_DELTA_STREAM; stone = ITEM_NONE; }
    }
    GIVEN {
        ConfigureAbilities(randomized);
        CreateTrainerAbilities(B_TRAINER_OPPONENT_A, species, baseAbility, ABILITY_NONE);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(species) { Item(stone); Moves(MOVE_DRAGON_ASCENT, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
        TURN { SWITCH(opponent, 1); }
        TURN { SWITCH(opponent, 0); }
    } SCENE {
        if (baseAbility == ABILITY_INTIMIDATE)
            ABILITY_POPUP(opponent, ABILITY_INTIMIDATE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, opponent);
        NOT ABILITY_POPUP(opponent, baseAbility);
    } THEN {
        EXPECT_EQ(opponent->species, mega);
        EXPECT_EQ(opponent->ability, megaAbility);
        EXPECT_EQ(GetMonAbility(&gParties[B_TRAINER_OPPONENT_A][0]), megaAbility);
        EXPECT_EQ(player->statStages[STAT_ATK], baseAbility == ABILITY_INTIMIDATE ? DEFAULT_STAT_STAGE - 1 : DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Fork battle: Skill Swap changes trainer abilities until switching out")
{
    bool32 switchBack;
    PARAMETRIZE { switchBack = FALSE; }
    PARAMETRIZE { switchBack = TRUE; }
    GIVEN {
        ConfigureAbilities(FALSE);
        CreateTrainerAbilities(B_TRAINER_OPPONENT_A, SPECIES_WOBBUFFET, ABILITY_LEVITATE, ABILITY_NONE);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SKILL_SWAP); }
        if (switchBack) {
            TURN { SWITCH(opponent, 1); }
            TURN { SWITCH(opponent, 0); }
        }
        TURN { MOVE(player, MOVE_EARTHQUAKE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKILL_SWAP, player);
        if (switchBack) {
            ABILITY_POPUP(opponent, ABILITY_LEVITATE);
            NOT HP_BAR(opponent);
        } else {
            HP_BAR(opponent);
        }
    } THEN {
        EXPECT_EQ(player->ability, ABILITY_LEVITATE);
        EXPECT_EQ(opponent->ability, switchBack ? ABILITY_LEVITATE : ABILITY_TELEPATHY);
        EXPECT_EQ(GetMonAbility(&gParties[B_TRAINER_OPPONENT_A][0]), ABILITY_LEVITATE);
    }
}

SINGLE_BATTLE_TEST("Fork battle: ability replacement moves override trainer abilities until switching out")
{
    enum Move move;
    enum Ability replacement;
    bool32 switchBack;
    for (u32 j = 0; j < 2; j++)
    {
        PARAMETRIZE { switchBack = j; move = MOVE_SIMPLE_BEAM; replacement = ABILITY_SIMPLE; }
        PARAMETRIZE { switchBack = j; move = MOVE_WORRY_SEED; replacement = ABILITY_INSOMNIA; }
        PARAMETRIZE { switchBack = j; move = MOVE_ENTRAINMENT; replacement = ABILITY_TELEPATHY; }
    }
    GIVEN {
        ConfigureAbilities(FALSE);
        CreateTrainerAbilities(B_TRAINER_OPPONENT_A, SPECIES_WOBBUFFET, ABILITY_LEVITATE, ABILITY_NONE);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
        if (switchBack) {
            TURN { SWITCH(opponent, 1); }
            TURN { SWITCH(opponent, 0); }
        }
        TURN { MOVE(player, MOVE_EARTHQUAKE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        if (switchBack) {
            ABILITY_POPUP(opponent, ABILITY_LEVITATE);
            NOT HP_BAR(opponent);
        } else {
            HP_BAR(opponent);
        }
    } THEN {
        EXPECT_EQ(opponent->ability, switchBack ? ABILITY_LEVITATE : replacement);
        EXPECT_EQ(GetMonAbility(&gParties[B_TRAINER_OPPONENT_A][0]), ABILITY_LEVITATE);
    }
}

SINGLE_BATTLE_TEST("Fork battle: Gastro Acid suppresses a non-native trainer ability until switching out")
{
    bool32 switchBack;
    PARAMETRIZE { switchBack = FALSE; }
    PARAMETRIZE { switchBack = TRUE; }
    GIVEN {
        ConfigureAbilities(FALSE);
        CreateTrainerAbilities(B_TRAINER_OPPONENT_A, SPECIES_WOBBUFFET, ABILITY_LEVITATE, ABILITY_NONE);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_GASTRO_ACID); }
        if (switchBack) {
            TURN { SWITCH(opponent, 1); }
            TURN { SWITCH(opponent, 0); }
        }
        TURN { MOVE(player, MOVE_EARTHQUAKE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GASTRO_ACID, player);
        if (switchBack) {
            ABILITY_POPUP(opponent, ABILITY_LEVITATE);
            NOT HP_BAR(opponent);
        } else {
            HP_BAR(opponent);
        }
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_1), switchBack ? ABILITY_LEVITATE : ABILITY_NONE);
    }
}

SINGLE_BATTLE_TEST("Fork battle: Mold Breaker bypasses a non-native trainer Levitate")
{
    GIVEN {
        ConfigureAbilities(FALSE);
        CreateTrainerAbilities(B_TRAINER_OPPONENT_A, SPECIES_WOBBUFFET, ABILITY_LEVITATE, ABILITY_NONE);
        PLAYER(SPECIES_HAXORUS) { Ability(ABILITY_MOLD_BREAKER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_EARTHQUAKE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->ability, ABILITY_LEVITATE);
    }
}

SINGLE_BATTLE_TEST("Fork battle: Trace copies a non-native trainer ability")
{
    GIVEN {
        ConfigureAbilities(FALSE);
        CreateTrainerAbilities(B_TRAINER_OPPONENT_A, SPECIES_WOBBUFFET, ABILITY_LEVITATE, ABILITY_NONE);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_PORYGON) { Ability(ABILITY_TRACE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { MOVE(opponent, MOVE_EARTHQUAKE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TRACE);
        MESSAGE("It traced the opposing Wobbuffet's Levitate!");
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->ability, ABILITY_LEVITATE);
    }
}

SINGLE_BATTLE_TEST("Fork battle: Heal Bell and Aromatherapy respect inactive trainer Soundproof and generation rules")
{
    enum Move move = MOVE_NONE;
    u32 generation = GEN_4;
    for (u32 j = GEN_4; j <= GEN_8; j++)
    {
        PARAMETRIZE { generation = j; move = MOVE_HEAL_BELL; }
        PARAMETRIZE { generation = j; move = MOVE_AROMATHERAPY; }
    }
    GIVEN {
        ConfigureAbilities(FALSE);
        WITH_CONFIG(B_HEAL_BELL_SOUNDPROOF, generation);
        CreateTrainerAbilities(B_TRAINER_OPPONENT_A, SPECIES_WOBBUFFET, ABILITY_NONE, ABILITY_SOUNDPROOF);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_BURN); }
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_STATUS),
                  move == MOVE_HEAL_BELL && generation == GEN_4 ? STATUS1_BURN : STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Fork battle: Poke Flute respects inactive trainer Soundproof")
{
    GIVEN {
        ConfigureAbilities(FALSE);
        CreateTrainerAbilities(B_TRAINER_OPPONENT_A, SPECIES_WOBBUFFET, ABILITY_NONE, ABILITY_SOUNDPROOF);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_SLEEP); }
        OPPONENT(SPECIES_WYNAUT) { Status1(STATUS1_SLEEP); }
    } WHEN {
        TURN { USE_ITEM(player, ITEM_POKE_FLUTE, partyIndex: 0); }
    } SCENE {
        MESSAGE("The Pokémon hearing the flute awoke!");
    } THEN {
        EXPECT_NE(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_STATUS) & STATUS1_SLEEP, 0);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][2], MON_DATA_STATUS), STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Fork battle: Poke Flute uses a suppressed trainer ability for active Pokemon")
{
    GIVEN {
        ConfigureAbilities(FALSE);
        CreateTrainerAbilities(B_TRAINER_OPPONENT_A, SPECIES_WOBBUFFET, ABILITY_SOUNDPROOF, ABILITY_NONE);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_SLEEP); }
    } WHEN {
        TURN { MOVE(player, MOVE_GASTRO_ACID); }
        TURN { USE_ITEM(player, ITEM_POKE_FLUTE, partyIndex: 0); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GASTRO_ACID, player);
        MESSAGE("The Pokémon hearing the flute awoke!");
    } THEN {
        EXPECT_EQ(opponent->status1 & STATUS1_SLEEP, 0);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_STATUS) & STATUS1_SLEEP, 0);
    }
}

MULTI_BATTLE_TEST("Fork battle: partner and both opponent trainers keep independent ability overrides")
{
    GIVEN {
        ConfigureAbilities(FALSE);
        CreateTrainerAbilities(B_TRAINER_PARTNER, SPECIES_WOBBUFFET, ABILITY_LEVITATE, ABILITY_NONE);
        CreateTrainerAbilities(B_TRAINER_OPPONENT_A, SPECIES_WOBBUFFET, ABILITY_SOUNDPROOF, ABILITY_NONE);
        CreateTrainerAbilities(B_TRAINER_OPPONENT_B, SPECIES_WOBBUFFET, ABILITY_WATER_ABSORB, ABILITY_NONE);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
        PARTNER(SPECIES_WOBBUFFET);
        OPPONENT_A(SPECIES_WOBBUFFET);
        OPPONENT_B(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SURF); }
    } THEN {
        EXPECT_EQ(playerLeft->ability, ABILITY_TELEPATHY);
        EXPECT_EQ(playerRight->ability, ABILITY_LEVITATE);
        EXPECT_EQ(opponentLeft->ability, ABILITY_SOUNDPROOF);
        EXPECT_EQ(opponentRight->ability, ABILITY_WATER_ABSORB);
    }
}
