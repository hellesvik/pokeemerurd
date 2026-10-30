#include "global.h"
#include "pokemon_storage_system.h"
#include "save.h"
#include "test/test.h"

// Freeze the fresh-game packed layout so future changes require an explicit format review.
#define T_SAVEBLOCK1_SIZE 17704
#define T_SAVEBLOCK2_SIZE 3884
#define T_SAVEBLOCK3_SIZE 1064
#define T_POKEMONSTORAGE_SIZE 34144

TEST("Packed save layout: SaveBlock1")
{
    EXPECT_EQ(sizeof(struct SaveBlock1), T_SAVEBLOCK1_SIZE);
}

TEST("Packed save layout: SaveBlock2")
{
    EXPECT_EQ(sizeof(struct SaveBlock2), T_SAVEBLOCK2_SIZE);
}

TEST("Packed save layout: SaveBlock3")
{
    EXPECT_EQ(sizeof(struct SaveBlock3), T_SAVEBLOCK3_SIZE);
}

TEST("Packed save layout: PokemonStorage")
{
    EXPECT_EQ(sizeof(struct PokemonStorage), T_POKEMONSTORAGE_SIZE);
}

#undef T_SAVEBLOCK1_SIZE
#undef T_SAVEBLOCK2_SIZE
#undef T_SAVEBLOCK3_SIZE
#undef T_POKEMONSTORAGE_SIZE

TEST("Packed save blocks fit both rotating save slots")
{
    EXPECT_LE(SAVE_STREAM_TOTAL_SIZE, SAVE_STREAM_SECTOR_SIZE * NUM_SECTORS_PER_SLOT);
    EXPECT_EQ(sizeof(struct SaveSector), SECTOR_SIZE);
    EXPECT_EQ(offsetof(struct SaveSector, id), SAVE_STREAM_SECTOR_SIZE);
}
