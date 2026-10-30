#include "global.h"
#include "battle.h"
#include "event_data.h"
#include "fork_run.h"
#include "item.h"
#include "item_ball.h"
#include "item_menu.h"
#include "load_save.h"
#include "malloc.h"
#include "menu_helpers.h"
#include "new_game.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "save.h"
#include "agb_flash.h"
#include "gba/flash_internal.h"
#include "test/test.h"

extern bool32 Test_ExpandedBagList(enum Pocket pocketId);

static enum Item FillItemsPocket(void)
{
    u32 count = 0;
    SetBagItemsPointers();
    ClearBag();
    for (enum Item item = 1; item < ITEMS_COUNT; item++)
    {
        if (GetItemPocket(item) != POCKET_ITEMS) continue;
        if (count == BAG_ITEMS_COUNT) return item;
        AddBagItem(item, 1);
        count++;
    }
    return ITEM_NONE;
}

static void FillMachines(void)
{
    for (u32 i = 1; i <= NUM_ALL_MACHINES; i++)
        AddBagItem(GetTMHMItemId(i), 1);
}

TEST("Expanded bag holds all machines and handles full Items pockets beyond index 255")
{
    enum Item overflow = FillItemsPocket();
    EXPECT_NE(overflow, ITEM_NONE);
    for (u32 i = 0; i < BAG_ITEMS_COUNT; i++)
        EXPECT_NE(GetBagItemId(POCKET_ITEMS, i), ITEM_NONE);
    EXPECT(!AddBagItem(overflow, 1));
    enum Item last = GetBagItemId(POCKET_ITEMS, BAG_ITEMS_COUNT - 1);
    EXPECT(AddBagItem(last, 1));
    EXPECT_EQ(CountTotalItemQuantityInBag(last), 2);
    MoveItemSlotInPocket(POCKET_ITEMS, BAG_ITEMS_COUNT - 1, 255);
    EXPECT_EQ(GetBagItemId(POCKET_ITEMS, 255), last);
    EXPECT(RemoveBagItem(last, 1));
    EXPECT_EQ(CountTotalItemQuantityInBag(last), 1);
    EXPECT(Test_ExpandedBagList(POCKET_ITEMS));
    SortItemsInBag(&gBagPockets[POCKET_ITEMS], SORT_ALPHABETICALLY);
    EXPECT_EQ(CountTotalItemQuantityInBag(last), 1);
    SortItemsInBag(&gBagPockets[POCKET_ITEMS], SORT_BY_TYPE);
    SortItemsInBag(&gBagPockets[POCKET_ITEMS], SORT_BY_AMOUNT);
    FillMachines();
    EXPECT_EQ(BAG_TMHM_COUNT, NUM_ALL_MACHINES);
    for (u32 i = 1; i <= NUM_ALL_MACHINES; i++)
        EXPECT(CheckBagHasItem(GetTMHMItemId(i), 1));
    EXPECT(Test_ExpandedBagList(POCKET_TM_HM));
    // A lone final-slot item must not wrap an 8-bit scan back to zero.
    ClearBag();
    BagPocket_SetSlotItemIdAndCount(&gBagPockets[POCKET_ITEMS], BAG_ITEMS_COUNT - 1, last, 1);
    EXPECT(IsBagPocketNonEmpty(POCKET_ITEMS));
}

TEST("Expanded bag retains every randomized reward and all HMs together")
{
    SetBagItemsPointers(); ClearBag();
    ForkConfigureGameplayOptions(FALSE, FALSE, FORK_FAINT_RULE_OFF,
        FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, GEN_9, TRUE, FALSE);
    gSaveBlock3Ptr->forkItemRandomizerSeed = 12345;
    ResetForkItemRandomizerState();
    for (u32 source = 0; source < FORK_ITEM_RANDOMIZER_POOL_COUNT; source++)
    {
        enum Item item = ResolveForkRandomizedItem(ITEM_POTION, source);
        EXPECT(AddBagItem(item, 1));
    }
    for (u32 i = NUM_TECHNICAL_MACHINES + 1; i <= NUM_ALL_MACHINES; i++)
        EXPECT(AddBagItem(GetTMHMItemId(i), 1));
    const enum Item treasures[] = {ITEM_TINY_MUSHROOM, ITEM_BIG_MUSHROOM, ITEM_NUGGET,
        ITEM_BIG_NUGGET, ITEM_BALM_MUSHROOM, ITEM_PEARL, ITEM_BIG_PEARL, ITEM_PEARL_STRING,
        ITEM_STARDUST, ITEM_STAR_PIECE, ITEM_COMET_SHARD, ITEM_RARE_BONE};
    for (u32 i = 0; i < ARRAY_COUNT(treasures); i++) EXPECT(AddBagItem(treasures[i], 1));
}

static void PrepareExpandedSave(void)
{
    NewGameInitData();
    FillItemsPocket(); FillMachines();
    struct Pokemon mon;
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
    {
        CreateMon(&mon, SPECIES_PIKACHU, box + 10, box + 123, OTID_STRUCT_PRESET(123));
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++) SetBoxMonAt(box, slot, &mon.box);
    }
    ForkConfigureGameplayOptions(FALSE, FALSE, FORK_FAINT_RULE_OFF,
        FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, GEN_9, TRUE, FALSE);
    gSaveBlock3Ptr->forkItemRandomizerSeed = 98765;
    ResetForkItemRandomizerState();
    ResolveForkRandomizedItem(ITEM_POTION, 10);
    ForkEnsureKeyItemsPresent();
    CheckForFlashMemory();
    Save_ResetSaveCounters();
}

TEST("Expanded bag packed flash save round trips full inventory PC and randomizer data")
{
    PrepareExpandedSave();
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    struct SaveBlock1 *save1 = Alloc(sizeof(*save1)); *save1 = *gSaveBlock1Ptr;
    struct SaveBlock2 *save2 = Alloc(sizeof(*save2)); *save2 = *gSaveBlock2Ptr;
    struct PokemonStorage *storage = Alloc(sizeof(*storage)); *storage = *gPokemonStoragePtr;
    struct SaveBlock3 *fork = Alloc(sizeof(*fork)); *fork = *gSaveBlock3Ptr;
    for (u32 i = 0; i < 3; i++)
    {
        EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
        ClearSav1(); ClearSav2();
        memset(gPokemonStoragePtr, 0, sizeof(*storage));
        ClearSav3();
        EXPECT_EQ(GetSaveBlocksPointersBaseOffset(), save2->playerTrainerId[0] + save2->playerTrainerId[1]
            + save2->playerTrainerId[2] + save2->playerTrainerId[3]);
        EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
        EXPECT_EQ(memcmp(save1, gSaveBlock1Ptr, sizeof(*save1)), 0);
        EXPECT_EQ(memcmp(save2, gSaveBlock2Ptr, sizeof(*save2)), 0);
        EXPECT_EQ(memcmp(storage, gPokemonStoragePtr, sizeof(*storage)), 0);
        EXPECT_EQ(memcmp(fork, gSaveBlock3Ptr, sizeof(*fork)), 0);
    }
    Free(fork); Free(storage); Free(save1); Free(save2);
}

TEST("Expanded bag packed partial and incremental saves preserve all data")
{
    PrepareExpandedSave();
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    struct PokemonStorage *storage = Alloc(sizeof(*storage)); *storage = *gPokemonStoragePtr;
    gSaveBlock3Ptr->forkItemRandomizerSeed = 11111;
    enum Item last = GetBagItemId(POCKET_ITEMS, BAG_ITEMS_COUNT - 1);
    EXPECT(AddBagItem(last, 1));
    EXPECT_EQ(TrySavingData(SAVE_LINK), SAVE_STATUS_OK);
    ClearBag(); ClearSav3();
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
    EXPECT_EQ(CountTotalItemQuantityInBag(last), 2);
    EXPECT_EQ(gSaveBlock3Ptr->forkItemRandomizerSeed, 11111);
    EXPECT_EQ(memcmp(storage, gPokemonStoragePtr, sizeof(*storage)), 0);
    gSaveBlock3Ptr->forkItemRandomizerSeed = 22222;
    EXPECT(!WriteSaveBlock2());
    u32 steps = 0;
    while (!WriteSaveBlock1Sector() && steps++ < NUM_SECTORS_PER_SLOT) {}
    EXPECT_LT(steps, NUM_SECTORS_PER_SLOT);
    ClearSav3();
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
    EXPECT_EQ(gSaveBlock3Ptr->forkItemRandomizerSeed, 22222);
    EXPECT(!LinkFullSave_Init());
    steps = 0;
    while (!LinkFullSave_WriteSector() && steps++ < NUM_SECTORS_PER_SLOT) {}
    EXPECT_LT(steps, NUM_SECTORS_PER_SLOT);
    LinkFullSave_ReplaceLastSector(); LinkFullSave_SetLastSectorSignature();
    ClearBag();
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
    EXPECT_EQ(CountTotalItemQuantityInBag(last), 2);
    EXPECT_EQ(memcmp(storage, gPokemonStoragePtr, sizeof(*storage)), 0);
    Free(storage);
}

TEST("Expanded bag packed save recovers from interrupted writes and rejects invalid sector IDs")
{
    PrepareExpandedSave();
    gSaveBlock3Ptr->forkItemRandomizerSeed = 33333;
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    gSaveBlock3Ptr->forkItemRandomizerSeed = 44444;
    LinkFullSave_Init();
    for (u32 i = 0; i < 3; i++) LinkFullSave_WriteSector();
    u32 damagedSector = gLastWrittenSector + NUM_SECTORS_PER_SLOT * (gSaveCounter % NUM_SAVE_SLOTS);
    ClearSav3();
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_ERROR);
    EXPECT_EQ(gSaveBlock3Ptr->forkItemRandomizerSeed, 33333);
    struct SaveSector *sector = Alloc(sizeof(*sector));
    ReadFlash(damagedSector, 0, (u8 *)sector, sizeof(*sector));
    sector->id = 0xFFFF;
    EXPECT_EQ(ProgramFlashSectorAndVerify(damagedSector, (u8 *)sector), 0);
    ClearSav3();
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_ERROR);
    EXPECT_EQ(gSaveBlock3Ptr->forkItemRandomizerSeed, 33333);
    Free(sector);
}

TEST("Expanded bag packed checksum protects payload tails core data and PC data")
{
    u32 logicalSector;
    PARAMETRIZE { logicalSector = 1; }
    PARAMETRIZE { logicalSector = SAVE_STREAM_CORE_LAST_SECTOR; }
    PARAMETRIZE { logicalSector = NUM_SECTORS_PER_SLOT - 1; }
    PrepareExpandedSave();
    gSaveBlock3Ptr->forkItemRandomizerSeed = 11111;
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    gSaveBlock3Ptr->forkItemRandomizerSeed = 55555;
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    u32 physical = (gLastWrittenSector + logicalSector) % NUM_SECTORS_PER_SLOT
        + NUM_SECTORS_PER_SLOT * (gSaveCounter % NUM_SAVE_SLOTS);
    struct SaveSector *sector = Alloc(sizeof(*sector));
    ReadFlash(physical, 0, (u8 *)sector, sizeof(*sector));
    // Sector 1 tests the 116 bytes previously excluded from the checksum.
    ((u8 *)sector)[logicalSector == 1 ? SECTOR_DATA_SIZE + 16 : 0] ^= 1;
    EXPECT_EQ(ProgramFlashSectorAndVerify(physical, (u8 *)sector), 0);
    ClearSav3();
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_ERROR);
    EXPECT_EQ(gSaveBlock3Ptr->forkItemRandomizerSeed, 11111);
    Free(sector);
}

TEST("Expanded bag interrupted partial saves fall back to the previous complete slot")
{
    u32 steps;
    PARAMETRIZE { steps = 0; }
    PARAMETRIZE { steps = SAVE_STREAM_CORE_LAST_SECTOR; }
    PrepareExpandedSave();
    gSaveBlock3Ptr->forkItemRandomizerSeed = 11111;
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    gSaveBlock3Ptr->forkItemRandomizerSeed = 22222;
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    gSaveBlock3Ptr->forkItemRandomizerSeed = 33333;
    WriteSaveBlock2();
    for (u32 i = 0; i < steps; i++) EXPECT(!WriteSaveBlock1Sector());
    ClearSav3();
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_ERROR);
    EXPECT_EQ(gSaveBlock3Ptr->forkItemRandomizerSeed, 11111);
}
