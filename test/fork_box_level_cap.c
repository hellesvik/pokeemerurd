#include "global.h"
#include "battle.h"
#include "battle_setup.h"
#include "caps.h"
#include "save.h"
#include "data.h"
#include "event_data.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "test/test.h"
#include "constants/opponents.h"

static void SetupBoxTraining(void)
{
    gSaveBlock3Ptr->forkGameplayOptionsConfigured = TRUE;
    gSaveBlock3Ptr->forkLevelCapEnabled = TRUE;
    ClearTrainerFlag(TRAINER_STEVEN);
    ClearTrainerFlag(TRAINER_ROXANNE_1);
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
            ZeroBoxMonAt(box, slot);
}

TEST("Box training levels all boxes with correct growth curves and preserves Pokemon data")
{
    const u16 species[] = {SPECIES_BULBASAUR, SPECIES_CLEFAIRY, SPECIES_MAGIKARP, SPECIES_MAKUHITA, SPECIES_NINCADA, SPECIES_PIKACHU};
    const u8 boxes[] = {0, 2, 5, 7, 10, TOTAL_BOXES_COUNT - 1};
    struct BoxPokemon before[ARRAY_COUNT(species)];
    struct Pokemon partyBefore;
    SetupBoxTraining();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_PIKACHU, 5, 0, OTID_STRUCT_PRESET(0));
    partyBefore = gParties[B_TRAINER_PLAYER][0];
    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        struct Pokemon mon;
        u32 value = ITEM_ORAN_BERRY;
        CreateMon(&mon, species[i], 5, 12345, OTID_STRUCT_PRESET(0));
        SetMonMoveSlot(&mon, MOVE_TACKLE, 0);
        SetMonData(&mon, MON_DATA_HELD_ITEM, &value);
        value = 32; SetMonData(&mon, MON_DATA_HP_EV, &value);
        SetBoxMonAt(boxes[i], IN_BOX_COUNT - 1, &mon.box);
        before[i] = mon.box;
    }
    EXPECT_EQ(LevelAllBoxMonsToCap(), ARRAY_COUNT(species));
    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        struct BoxPokemon *mon = GetBoxedMonPtr(boxes[i], IN_BOX_COUNT - 1);
        struct Pokemon withdrawn, expected;
        u32 exp = gExperienceTables[gSpeciesInfo[species[i]].growthRate][GetCurrentLevelCap()];
        EXPECT_EQ(GetBoxMonData(mon, MON_DATA_EXP), exp);
        EXPECT_EQ(GetLevelFromBoxMonExp(mon), GetCurrentLevelCap());
        SetBoxMonData(&before[i], MON_DATA_EXP, &exp);
        EXPECT_EQ(memcmp(mon, &before[i], sizeof(*mon)), 0);
        BoxMonAtToMon(boxes[i], IN_BOX_COUNT - 1, &withdrawn);
        BoxMonToMon(&before[i], &expected);
        EXPECT_EQ(GetMonData(&withdrawn, MON_DATA_LEVEL), GetCurrentLevelCap());
        EXPECT_EQ(GetMonData(&withdrawn, MON_DATA_MAX_HP), GetMonData(&expected, MON_DATA_MAX_HP));
        EXPECT_EQ(GetMonData(&withdrawn, MON_DATA_ATK), GetMonData(&expected, MON_DATA_ATK));
    }
    EXPECT_EQ(memcmp(&partyBefore, &gParties[B_TRAINER_PLAYER][0], sizeof(partyBefore)), 0);
    EXPECT_EQ(LevelAllBoxMonsToCap(), 0);
}

TEST("Box training skips Eggs bad Eggs and capped Pokemon and requires enabled caps")
{
    struct Pokemon mon;
    struct BoxPokemon before[5];
    u32 value = TRUE;
    SetupBoxTraining();
    CreateMon(&mon, SPECIES_PIKACHU, 5, 0, OTID_STRUCT_PRESET(0));
    SetBoxMonAt(0, 0, &mon.box);
    SetBoxMonData(&mon.box, MON_DATA_IS_EGG, &value);
    SetBoxMonAt(0, 1, &mon.box);
    CreateMon(&mon, SPECIES_PIKACHU, 80, 0, OTID_STRUCT_PRESET(0));
    SetBoxMonAt(0, 2, &mon.box);
    CreateMon(&mon, SPECIES_PIKACHU, GetCurrentLevelCap(), 0, OTID_STRUCT_PRESET(0));
    SetBoxMonAt(0, 3, &mon.box);
    CreateMon(&mon, SPECIES_PIKACHU, 5, 0, OTID_STRUCT_PRESET(0));
    mon.box.isBadEgg = TRUE;
    SetBoxMonAt(0, 4, &mon.box);
    for (u32 i = 0; i < ARRAY_COUNT(before); i++)
        before[i] = *GetBoxedMonPtr(0, i);
    gSaveBlock3Ptr->forkLevelCapEnabled = FALSE;
    EXPECT_EQ(LevelAllBoxMonsToCap(), 0);
    for (u32 i = 0; i < ARRAY_COUNT(before); i++)
        EXPECT_EQ(memcmp(&before[i], GetBoxedMonPtr(0, i), sizeof(before[i])), 0);
    gSaveBlock3Ptr->forkLevelCapEnabled = TRUE;
    EXPECT_EQ(LevelAllBoxMonsToCap(), 1);
    for (u32 i = 1; i < ARRAY_COUNT(before); i++)
        EXPECT_EQ(memcmp(&before[i], GetBoxedMonPtr(0, i), sizeof(before[i])), 0);
    EXPECT_EQ(GetBoxMonDataAt(0, 5, MON_DATA_SPECIES), SPECIES_NONE);
    EXPECT_EQ(LevelAllBoxMonsToCap(), 0);
    SetTrainerFlag(TRAINER_STEVEN);
    EXPECT_EQ(LevelAllBoxMonsToCap(), 3);
    EXPECT_EQ(GetLevelFromBoxMonExp(GetBoxedMonPtr(0, 0)), MAX_LEVEL);
}

extern bool32 Test_PCBoxTrainingMenu(void);

TEST("Box training PC menu selects the action displays results and returns")
{
    struct Pokemon mon;
    SetupBoxTraining();
    CreateMon(&mon, SPECIES_PIKACHU, 5, 0, OTID_STRUCT_PRESET(0));
    SetBoxMonAt(0, 0, &mon.box);
    EXPECT(Test_PCBoxTrainingMenu());
    EXPECT_EQ(GetLevelFromBoxMonExp(GetBoxedMonPtr(0, 0)), GetCurrentLevelCap());
    EXPECT(Test_PCBoxTrainingMenu()); // No eligible Pokemon.
    gSaveBlock3Ptr->forkLevelCapEnabled = FALSE;
    SetBoxMonAt(0, 0, &mon.box);
    EXPECT(Test_PCBoxTrainingMenu());
    EXPECT_EQ(GetLevelFromBoxMonExp(GetBoxedMonPtr(0, 0)), 5);
}


TEST("Box training retains the master save layout")
{
    // Captured from master before the feature was ported. Do not update these
    // to accommodate a layout change on the save-compatible branch.
    EXPECT_EQ(sizeof(struct SaveBlock1), 15852);
    EXPECT_EQ(sizeof(struct SaveBlock2), 3884);
    EXPECT_EQ(sizeof(struct SaveBlock3), 1212);
    EXPECT_EQ(sizeof(struct PokemonStorage), 34144);
    EXPECT_EQ(sizeof(struct BoxPokemon), 80);
    EXPECT_EQ(sizeof(struct Pokemon), 100);
    EXPECT_EQ(offsetof(struct PokemonStorage, boxes), 4);
    EXPECT_EQ(offsetof(struct PokemonStorage, boxNames), 33604);
    EXPECT_EQ(SECTOR_DATA_SIZE, 3968);
    EXPECT_EQ(SAVE_BLOCK_3_CHUNK_SIZE, 116);
    EXPECT_EQ(NUM_SECTORS_PER_SLOT, 14);
    EXPECT_EQ(SECTOR_SIGNATURE, 0x8012025);
}
