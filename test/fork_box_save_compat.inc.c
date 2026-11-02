// Standalone legacy fixture writer/reader, excluded from the ordinary suite.
// Link this against the pre-port storage implementation to write a flash
// fixture. Inject that fixture when running fork_box_save_train.inc.c against
// the updated implementation, then inject the resulting flash into the legacy
// reader. Compare gBoxCompatLayout in both ELF files as an additional ABI check.
#include "global.h"
#include "battle_setup.h"
#include "caps.h"
#include "event_data.h"
#include "item.h"
#include "money.h"
#include "new_game.h"
#include "overworld.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "save.h"
#include "load_save.h"
#include "string_util.h"
#include "test/test.h"
#include "constants/game_stat.h"
#include "constants/opponents.h"

// Compare this ROM-resident signature between the old and updated builds.
const u32 gBoxCompatLayout[] __attribute__((used)) = {
    sizeof(struct SaveBlock1), sizeof(struct SaveBlock2), sizeof(struct SaveBlock3),
    sizeof(struct PokemonStorage), sizeof(struct BoxPokemon), sizeof(struct Pokemon),
    offsetof(struct PokemonStorage, boxes), offsetof(struct PokemonStorage, boxNames),
    SECTOR_DATA_SIZE, SAVE_BLOCK_3_CHUNK_SIZE, SECTOR_FOOTER_SIZE,
    NUM_SECTORS_PER_SLOT, SECTOR_SIGNATURE,
};

TEST("Box compatibility write legacy fixture")
{
    struct Pokemon mon;
    u32 value = ITEM_ORAN_BERRY;
    NewGameInitData();
    gSaveBlock3Ptr->forkGameplayOptionsConfigured = TRUE;
    gSaveBlock3Ptr->forkLevelCapEnabled = TRUE;
    ClearTrainerFlag(TRAINER_ROXANNE_1);
    ClearTrainerFlag(TRAINER_STEVEN);
    StringCopy(gSaveBlock2Ptr->playerName, COMPOUND_STRING("BOXTEST"));
    StringCopy(GetBoxNamePtr(TOTAL_BOXES_COUNT - 1), COMPOUND_STRING("KEEPBOX"));
    SetMoney(&gSaveBlock1Ptr->money, 123456);
    AddBagItem(ITEM_POTION, 7);
    SetGameStat(GAME_STAT_STEPS, 1234);
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_PIKACHU, 7, 0, OTID_STRUCT_PRESET(0));
    gPartiesCount[B_TRAINER_PLAYER] = 1;
    CreateMon(&mon, SPECIES_BULBASAUR, 5, 0, OTID_STRUCT_PRESET(0));
    SetMonMoveSlot(&mon, MOVE_TACKLE, 0);
    SetMonData(&mon, MON_DATA_HELD_ITEM, &value);
    value = 32; SetMonData(&mon, MON_DATA_HP_EV, &value);
    SetBoxMonAt(TOTAL_BOXES_COUNT - 1, IN_BOX_COUNT - 1, &mon.box);
    CheckForFlashMemory();
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
}

TEST("Box compatibility read trained fixture")
{
    CheckForFlashMemory();
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
    CopyPartyAndObjectsFromSave();
    EXPECT_EQ(StringCompare(gSaveBlock2Ptr->playerName, COMPOUND_STRING("BOXTEST")), 0);
    EXPECT_EQ(StringCompare(GetBoxNamePtr(TOTAL_BOXES_COUNT - 1), COMPOUND_STRING("KEEPBOX")), 0);
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 123456);
    EXPECT(CheckBagHasItem(ITEM_POTION, 7));
    EXPECT_EQ(GetGameStat(GAME_STAT_STEPS), 1234);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_PIKACHU);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 7);
    EXPECT_EQ(GetBoxMonDataAt(TOTAL_BOXES_COUNT - 1, IN_BOX_COUNT - 1, MON_DATA_SPECIES), SPECIES_BULBASAUR);
    EXPECT_EQ(GetBoxMonDataAt(TOTAL_BOXES_COUNT - 1, IN_BOX_COUNT - 1, MON_DATA_HELD_ITEM), ITEM_ORAN_BERRY);
    EXPECT_EQ(GetBoxMonDataAt(TOTAL_BOXES_COUNT - 1, IN_BOX_COUNT - 1, MON_DATA_HP_EV), 32);
    EXPECT_EQ(GetLevelFromBoxMonExp(GetBoxedMonPtr(TOTAL_BOXES_COUNT - 1, IN_BOX_COUNT - 1)), 15);
}
