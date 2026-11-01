#include "global.h"
#include "event_data.h"
#include "new_game.h"
#include "overworld.h"
#include "battle_setup.h"
#include "constants/opponents.h"
#include "constants/game_stat.h"
#include "test/test.h"

u32 Test_GetKantoLevelCap(void);

TEST("Kanto caps follow story milestones and accept rival variants")
{
    static const u16 trainers[] = {
        TRAINER_LEADER_BROCK, TRAINER_LEADER_MISTY, TRAINER_LEADER_LT_SURGE,
        TRAINER_RIVAL_POKEMON_TOWER_BULBASAUR, TRAINER_BOSS_GIOVANNI,
        TRAINER_LEADER_ERIKA, TRAINER_RIVAL_SILPH_CHARMANDER,
        TRAINER_BOSS_GIOVANNI_2, TRAINER_LEADER_KOGA, TRAINER_LEADER_SABRINA,
        TRAINER_LEADER_BLAINE, TRAINER_LEADER_GIOVANNI,
        TRAINER_RIVAL_ROUTE22_LATE_BULBASAUR,
    };
    static const u8 caps[] = {14,21,24,25,29,30,40,41,43,44,47,50,53,63};
    NewGameInitData();
    for (u32 i = 0; i < ARRAY_COUNT(trainers); i++)
    {
        EXPECT_EQ(Test_GetKantoLevelCap(), caps[i]);
        SetTrainerFlag(trainers[i]);
    }
    EXPECT_EQ(Test_GetKantoLevelCap(), 63);
    FlagSet(FLAG_SYS_GAME_CLEAR);
    EXPECT_EQ(Test_GetKantoLevelCap(), 75);
    SetGameStat(GAME_STAT_ENTERED_HOF, 2);
    EXPECT_EQ(Test_GetKantoLevelCap(), MAX_LEVEL);
}
