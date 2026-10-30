#ifndef GUARD_FORK_TM_RANDOMIZER_H
#define GUARD_FORK_TM_RANDOMIZER_H

#include "global.h"

#define FORK_LILYCOVE_TM_SHOP_ITEM_COUNT 8

bool32 IsForkLilycoveTmShopItem(enum Item item);
enum Item ResolveForkLilycoveTmShopItem(enum Item item);
bool32 HasForkLilycoveTmShopItemBeenPurchased(enum Item item);
void MarkForkLilycoveTmShopItemPurchased(enum Item item);

#endif
