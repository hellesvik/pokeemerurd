#include "global.h"
#include "battle.h"
#include "battle_ai_main.h"
#include "battle_setup.h"
#include "test/test.h"
#include "constants/opponents.h"

TEST("Story bosses are restricted from bringing battle items")
{
    static const enum TrainerClassID sBossClasses[] =
    {
        TRAINER_CLASS_LEADER,
        TRAINER_CLASS_ELITE_FOUR,
        TRAINER_CLASS_CHAMPION,
        TRAINER_CLASS_AQUA_LEADER,
        TRAINER_CLASS_MAGMA_LEADER,
        TRAINER_CLASS_AQUA_ADMIN,
        TRAINER_CLASS_MAGMA_ADMIN,
        TRAINER_CLASS_RIVAL,
    };
    for (u32 i = 0; i < ARRAY_COUNT(sBossClasses); i++)
        EXPECT(IsTrainerClassBossBattleItemRestricted(sBossClasses[i]));

    EXPECT(IsTrainerBossBattleItemRestricted(TRAINER_WALLY_VR_1));
}

TEST("Ordinary trainers are not restricted from bringing battle items")
{
    EXPECT(!IsTrainerClassBossBattleItemRestricted(TRAINER_CLASS_COOLTRAINER));
}
