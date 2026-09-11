#include "global.h"
#include "fork_tm_randomizer.h"
#include "pokemon.h"
#include "random.h"
#include "constants/tms_hms.h"

#define FORK_TM_RANDOMIZER_VERSION 2
#define FIRST_FORK_RANDOM_TM 51
#define LAST_FORK_RANDOM_TM 130
#define FORK_RANDOM_TM_COUNT (LAST_FORK_RANDOM_TM - FIRST_FORK_RANDOM_TM + 1)

static bool32 GetBit(const u8 *bits, u16 index)
{
    return (bits[index / 8] >> (index % 8)) & 1;
}

static void SetBit(u8 *bits, u16 index)
{
    bits[index / 8] |= 1 << (index % 8);
}

bool32 IsForkLilycoveTmShopItem(enum Item item)
{
    return item >= ITEM_TM51 && item < ITEM_TM51 + FORK_LILYCOVE_TM_SHOP_ITEM_COUNT;
}

bool32 HasForkLilycoveTmShopItemBeenPurchased(enum Item item)
{
    if (!IsForkLilycoveTmShopItem(item))
        return FALSE;

    return gSaveBlock3Ptr->forkLilycoveTmShopPurchases & (1 << (item - ITEM_TM51));
}

void MarkForkLilycoveTmShopItemPurchased(enum Item item)
{
    if (IsForkLilycoveTmShopItem(item))
        gSaveBlock3Ptr->forkLilycoveTmShopPurchases |= 1 << (item - ITEM_TM51);
}

static bool32 IsHmMove(enum Move move)
{
    switch (move)
    {
#define CHECK_HM_MOVE(moveName) case MOVE_##moveName: return TRUE;
    FOREACH_HM(CHECK_HM_MOVE)
#undef CHECK_HM_MOVE
    default:
        return FALSE;
    }
}

void InitForkRandomizedTMMoves(void)
{
    u8 eligible[(MOVES_COUNT + 7) / 8] = {0};
    u8 selected[(MOVES_COUNT + 7) / 8] = {0};
    u16 eligibleCount = 0;
    u16 selectedCount = 0;
    rng_value_t rng = LocalRandomSeed(gSaveBlock3Ptr->forkItemRandomizerSeed ^ 0x544D5241);

    for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        const u16 *learnset;

        if (!IsSpeciesEnabled(species))
            continue;
        learnset = GetSpeciesTeachableLearnset(species);
        for (u32 i = 0; learnset[i] != MOVE_UNAVAILABLE; i++)
        {
            if (learnset[i] > MOVE_NONE
             && learnset[i] < MOVES_COUNT
             && !IsHmMove(learnset[i])
             && CanLearnTeachableMove(species, learnset[i])
             && !GetBit(eligible, learnset[i]))
            {
                SetBit(eligible, learnset[i]);
                eligibleCount++;
            }
        }
    }

    for (u16 tm = 0; tm < FORK_RANDOM_TM_COUNT; tm++)
    {
        if (selectedCount == eligibleCount)
        {
            for (u16 i = 0; i < ARRAY_COUNT(selected); i++)
                selected[i] = 0;
            selectedCount = 0;
        }

        u16 candidate = LocalRandom(&rng) % MOVES_COUNT;

        for (u16 attempt = 0; attempt < MOVES_COUNT; attempt++)
        {
            if (GetBit(eligible, candidate) && !GetBit(selected, candidate))
                break;
            candidate = (candidate + 1) % MOVES_COUNT;
        }
        gSaveBlock3Ptr->forkRandomizedTMMoves[tm] = candidate;
        SetBit(selected, candidate);
        selectedCount++;
    }
    gSaveBlock3Ptr->forkTMRandomizerVersion = FORK_TM_RANDOMIZER_VERSION;
}

enum Move GetForkRandomizedTMMove(enum Item item)
{
    u16 tmNumber;

    switch (item)
    {
    case ITEM_TM51 ... ITEM_TM100:
        tmNumber = item - ITEM_TM51 + FIRST_FORK_RANDOM_TM;
        break;
#define GET_FORK_TM_NUMBER(number) case ITEM_TM##number: tmNumber = number; break;
    FOREACH_FORK_ADDITIONAL_TM(GET_FORK_TM_NUMBER)
#undef GET_FORK_TM_NUMBER
    default:
        return MOVE_NONE;
    }
    if (gSaveBlock3Ptr->forkTMRandomizerVersion != FORK_TM_RANDOMIZER_VERSION)
        InitForkRandomizedTMMoves();
    return gSaveBlock3Ptr->forkRandomizedTMMoves[tmNumber - FIRST_FORK_RANDOM_TM];
}

enum Item GetForkRandomizedTMItem(u16 tmNumber)
{
    if (tmNumber >= 51 && tmNumber <= 100)
        return ITEM_TM51 + (tmNumber - 51);

    switch (tmNumber)
    {
#define GET_FORK_TM_ITEM(number) case number: return ITEM_TM##number;
    FOREACH_FORK_ADDITIONAL_TM(GET_FORK_TM_ITEM)
#undef GET_FORK_TM_ITEM
    default:
        return ITEM_NONE;
    }
}

enum Item GetForkRandomizedTMItemFromMoveId(enum Move move)
{
    if (gSaveBlock3Ptr->forkTMRandomizerVersion != FORK_TM_RANDOMIZER_VERSION)
        InitForkRandomizedTMMoves();

    for (u16 tm = FIRST_FORK_RANDOM_TM; tm <= LAST_FORK_RANDOM_TM; tm++)
    {
        if (gSaveBlock3Ptr->forkRandomizedTMMoves[tm - FIRST_FORK_RANDOM_TM] == move)
            return GetForkRandomizedTMItem(tm);
    }
    return ITEM_NONE;
}
