#ifndef GUARD_FORK_GIFT_POKEMON_RANDOMIZER_H
#define GUARD_FORK_GIFT_POKEMON_RANDOMIZER_H

#include "global.h"

enum Species GetForkRandomizedStarterSpecies(u8 slot);
enum Species GetForkRandomizedStevenGiftSpecies(void);
enum Species GetForkRandomizedFortreeTradeSpecies(void);
enum Ability GetForkRandomizedFortreeTradeAbility(void);
bool8 GetForkRandomizedFortreeTradeIsShiny(void);
enum Species GetForkRandomizedRustboroTradeSpecies(void);
enum Ability GetForkRandomizedRustboroTradeAbility(void);
enum Item GetForkRandomizedRustboroTradeHeldItem(void);
enum Move GetForkRandomizedRustboroTradeMove(u8 slot);
void ApplyForkStarterPerfectIvs(struct Pokemon *mon, u8 starterSlot);

#endif // GUARD_FORK_GIFT_POKEMON_RANDOMIZER_H
