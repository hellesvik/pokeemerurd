#include "global.h"
#include "battle.h"
#include "battle_ai_main.h"
#include "battle_setup.h"
#include "data.h"
#include "test/test.h"
#include "constants/opponents.h"

TEST("Shelly's Weather Institute team is a four-Pokemon rain double battle")
{
    const struct Trainer *trainer = &gTrainers[DIFFICULTY_NORMAL][TRAINER_SHELLY_WEATHER_INSTITUTE];

    EXPECT_EQ((u32)trainer->battleType, TRAINER_BATTLE_TYPE_DOUBLES);
    EXPECT_EQ((u32)trainer->partySize, 4);

    EXPECT_EQ(trainer->party[0].species, SPECIES_POLITOED);
    EXPECT_EQ(trainer->party[0].lvl, 29);
    EXPECT_EQ(trainer->party[0].ability, ABILITY_DRIZZLE);
    EXPECT_EQ(trainer->party[0].heldItem, ITEM_FOCUS_SASH);
    EXPECT_EQ(trainer->party[0].moves[0], MOVE_WIDE_GUARD);
    EXPECT_EQ(trainer->party[0].moves[1], MOVE_WEATHER_BALL);
    EXPECT_EQ(trainer->party[0].moves[2], MOVE_ICY_WIND);
    EXPECT_EQ(trainer->party[0].moves[3], MOVE_HYPNOSIS);

    EXPECT_EQ(trainer->party[1].species, SPECIES_CASTFORM);
    EXPECT_EQ(trainer->party[1].lvl, 29);
    EXPECT_EQ(trainer->party[1].ability, ABILITY_FORECAST);
    EXPECT_EQ(trainer->party[1].heldItem, ITEM_MYSTIC_WATER);
    EXPECT_EQ(trainer->party[1].moves[0], MOVE_WEATHER_BALL);
    EXPECT_EQ(trainer->party[1].moves[1], MOVE_RAIN_DANCE);
    EXPECT_EQ(trainer->party[1].moves[2], MOVE_ICY_WIND);
    EXPECT_EQ(trainer->party[1].moves[3], MOVE_FREEZE_DRY);

    EXPECT_EQ(trainer->party[2].species, SPECIES_WHISCASH);
    EXPECT_EQ(trainer->party[2].lvl, 30);
    EXPECT_EQ(trainer->party[2].ability, ABILITY_HYDRATION);
    EXPECT_EQ(trainer->party[2].heldItem, ITEM_RINDO_BERRY);
    EXPECT_EQ(trainer->party[2].moves[0], MOVE_REST);
    EXPECT_EQ(trainer->party[2].moves[1], MOVE_AQUA_JET);
    EXPECT_EQ(trainer->party[2].moves[2], MOVE_MAGNITUDE);
    EXPECT_EQ(trainer->party[2].moves[3], MOVE_DIVE);

    EXPECT_EQ(trainer->party[3].species, SPECIES_DRAGONAIR);
    EXPECT_EQ(trainer->party[3].lvl, 32);
    EXPECT_EQ(trainer->party[3].ability, ABILITY_MARVEL_SCALE);
    EXPECT_EQ(trainer->party[3].heldItem, ITEM_AIR_BALLOON);
    EXPECT_EQ(trainer->party[3].moves[0], MOVE_DRAGON_RAGE);
    EXPECT_EQ(trainer->party[3].moves[1], MOVE_DRAGON_TAIL);
    EXPECT_EQ(trainer->party[3].moves[2], MOVE_AQUA_TAIL);
    EXPECT_EQ(trainer->party[3].moves[3], MOVE_THUNDER_WAVE);
}

TEST("Maxie's Mt. Chimney team uses the configured sun and ground lineup")
{
    const struct Trainer *trainer = &gTrainers[DIFFICULTY_NORMAL][TRAINER_MAXIE_MT_CHIMNEY];

    EXPECT_EQ((u32)trainer->partySize, 4);
    EXPECT_EQ(trainer->party[0].species, SPECIES_GLIGAR);
    EXPECT_EQ(trainer->party[0].lvl, 23);
    EXPECT_EQ(trainer->party[0].ability, ABILITY_INTIMIDATE);
    EXPECT_EQ(trainer->party[0].heldItem, ITEM_PASSHO_BERRY);
    EXPECT_EQ(trainer->party[0].moves[0], MOVE_STEALTH_ROCK);
    EXPECT_EQ(trainer->party[1].species, SPECIES_RHYHORN);
    EXPECT_EQ(trainer->party[2].species, SPECIES_SCOVILLAIN);
    EXPECT_EQ(trainer->party[2].ability, ABILITY_CHLOROPHYLL);
    EXPECT_EQ(trainer->party[3].species, SPECIES_CAMERUPT);
    EXPECT_EQ(trainer->party[3].lvl, 25);
    EXPECT_EQ(trainer->party[3].ability, ABILITY_SOLID_ROCK);
}

TEST("Maxie's Magma Hideout team uses the configured sand and mega lineup")
{
    const struct Trainer *trainer = &gTrainers[DIFFICULTY_NORMAL][TRAINER_MAXIE_MAGMA_HIDEOUT];

    EXPECT_EQ((u32)trainer->partySize, 5);
    EXPECT_EQ(trainer->party[0].species, SPECIES_SANDACONDA);
    EXPECT_EQ(trainer->party[0].lvl, 35);
    EXPECT_EQ(trainer->party[0].ability, ABILITY_SAND_SPIT);
    EXPECT_EQ(trainer->party[1].species, SPECIES_STOUTLAND);
    EXPECT_EQ(trainer->party[2].species, SPECIES_RHYDON);
    EXPECT_EQ(trainer->party[3].species, SPECIES_CRADILY);
    EXPECT_EQ(trainer->party[4].species, SPECIES_CAMERUPT);
    EXPECT_EQ(trainer->party[4].lvl, 39);
    EXPECT_EQ(trainer->party[4].ability, ABILITY_SOLID_ROCK);
    EXPECT_EQ(trainer->party[4].heldItem, ITEM_CAMERUPTITE);
}

TEST("Maxie and Tabitha's Mossdeep battle uses the configured six-Pokemon sand team")
{
    const struct Trainer *maxie = &gTrainers[DIFFICULTY_NORMAL][TRAINER_MAXIE_MOSSDEEP];
    const struct Trainer *tabitha = &gTrainers[DIFFICULTY_NORMAL][TRAINER_TABITHA_MOSSDEEP];

    EXPECT_EQ((u32)maxie->partySize, 3);
    EXPECT_EQ(maxie->party[0].species, SPECIES_GLISCOR);
    EXPECT_EQ(maxie->party[0].lvl, 42);
    EXPECT_EQ(maxie->party[0].ability, ABILITY_SAND_VEIL);
    EXPECT_EQ(maxie->party[1].species, SPECIES_STOUTLAND);
    EXPECT_EQ(maxie->party[2].species, SPECIES_CAMERUPT);
    EXPECT_EQ(maxie->party[2].lvl, 44);
    EXPECT_EQ(maxie->party[2].heldItem, ITEM_CAMERUPTITE);

    EXPECT_EQ((u32)tabitha->partySize, 3);
    EXPECT_EQ(tabitha->party[0].species, SPECIES_HIPPOWDON);
    EXPECT_EQ(tabitha->party[0].lvl, 42);
    EXPECT_EQ(tabitha->party[0].ability, ABILITY_SAND_STREAM);
    EXPECT_EQ(tabitha->party[1].species, SPECIES_GABITE);
    EXPECT_EQ(tabitha->party[2].species, SPECIES_VOLCANION);
    EXPECT_EQ(tabitha->party[2].lvl, 44);
    EXPECT_EQ(tabitha->party[2].ability, ABILITY_WATER_ABSORB);
}

TEST("Archie uses the configured six-Pokemon rain team")
{
    const struct Trainer *trainer = &gTrainers[DIFFICULTY_NORMAL][TRAINER_ARCHIE];
    EXPECT_EQ((u32)trainer->partySize, 6);
    EXPECT_EQ(trainer->party[0].species, SPECIES_WAILORD);
    EXPECT_EQ(trainer->party[0].ability, ABILITY_DRIZZLE);
    EXPECT_EQ(trainer->party[4].species, SPECIES_MASQUERAIN);
    EXPECT_EQ(trainer->party[5].species, SPECIES_SHARPEDO);
    EXPECT_EQ(trainer->party[5].heldItem, ITEM_SHARPEDONITE);
}

TEST("Steven uses the configured Regi and Mawile champion team")
{
    const struct Trainer *trainer = &gTrainers[DIFFICULTY_NORMAL][TRAINER_STEVEN];
    EXPECT_EQ((u32)trainer->partySize, 6);
    EXPECT_EQ(trainer->party[0].species, SPECIES_REGIROCK);
    EXPECT_EQ(trainer->party[3].species, SPECIES_DRIFBLIM);
    EXPECT_EQ(trainer->party[4].species, SPECIES_METAGROSS);
    EXPECT_EQ(trainer->party[4].heldItem, ITEM_LIECHI_BERRY);
    EXPECT_EQ(trainer->party[5].species, SPECIES_MAWILE);
    EXPECT_EQ(trainer->party[5].lvl, 80);
    EXPECT_EQ(trainer->party[5].heldItem, ITEM_MAWILITE);
}

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
