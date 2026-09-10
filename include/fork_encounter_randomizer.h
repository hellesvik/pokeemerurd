#ifndef GUARD_FORK_ENCOUNTER_RANDOMIZER_H
#define GUARD_FORK_ENCOUNTER_RANDOMIZER_H

#include "wild_encounter.h"

#define FORK_LAND_RANDOMIZED_SLOT_COUNT 3
#define FORK_WATER_RANDOMIZED_SLOT_COUNT 3
#define FORK_ROCK_RANDOMIZED_SLOT_COUNT 3
#define FORK_FISHING_RANDOMIZED_SLOT_COUNT 2

void InitForkEncounterRandomizerSeed(void);
u16 GetForkMaxNationalDex(void);
enum Species ResolveForkRandomizedEncounterSpecies(u8 mapGroup, u8 mapNum, enum WildPokemonArea area, u8 slot, enum Species fallback);
enum Species ResolveForkRandomizedEggSpecies(enum Species fallback);
enum Species ResolveForkRandomizedStaticEncounterSpecies(enum Species fallback);
u16 GetForkEncounterRandomizerBstCap(u8 mapGroup, u8 mapNum, enum WildPokemonArea area);
u16 GetForkEncounterRandomizerBstMin(u8 mapGroup, u8 mapNum, enum WildPokemonArea area);

#endif // GUARD_FORK_ENCOUNTER_RANDOMIZER_H
