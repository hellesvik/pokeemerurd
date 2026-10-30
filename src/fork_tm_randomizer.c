#include "global.h"
#include "fork_tm_randomizer.h"
#include "item.h"
#include "pokemon.h"
#include "random.h"
#include "constants/tms_hms.h"

enum Item ResolveForkLilycoveTmShopItem(enum Item item)
{
    bool8 selected[NUM_TECHNICAL_MACHINES + 1] = {0};
    u16 targetSlot;
    u16 tmNumber = 0;
    rng_value_t rng;

    if (item < ITEM_TM51 || item >= ITEM_TM51 + FORK_LILYCOVE_TM_SHOP_ITEM_COUNT)
        return item;

    targetSlot = item - ITEM_TM51;
    rng = LocalRandomSeed(gSaveBlock3Ptr->forkItemRandomizerSeed ^ 0x4C544D53);
    for (u16 slot = 0; slot <= targetSlot; slot++)
    {
        do
            tmNumber = (LocalRandom(&rng) % NUM_TECHNICAL_MACHINES) + 1;
        while (selected[tmNumber]);
        selected[tmNumber] = TRUE;
    }

    return GetTMHMItemId(tmNumber);
}

static s16 GetForkLilycoveTmShopSlot(enum Item item)
{
    for (u16 slot = 0; slot < FORK_LILYCOVE_TM_SHOP_ITEM_COUNT; slot++)
    {
        if (ResolveForkLilycoveTmShopItem(ITEM_TM51 + slot) == item)
            return slot;
    }

    return -1;
}

bool32 IsForkLilycoveTmShopItem(enum Item item)
{
    return GetForkLilycoveTmShopSlot(item) >= 0;
}

bool32 HasForkLilycoveTmShopItemBeenPurchased(enum Item item)
{
    s16 slot = GetForkLilycoveTmShopSlot(item);

    if (slot < 0)
        return FALSE;

    return gSaveBlock3Ptr->forkLilycoveTmShopPurchases & (1 << slot);
}

void MarkForkLilycoveTmShopItemPurchased(enum Item item)
{
    s16 slot = GetForkLilycoveTmShopSlot(item);

    if (slot >= 0)
        gSaveBlock3Ptr->forkLilycoveTmShopPurchases |= 1 << slot;
}
