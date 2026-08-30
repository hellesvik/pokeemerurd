#ifndef GUARD_FORK_ENCOUNTER_RANDOMIZER_H
#define GUARD_FORK_ENCOUNTER_RANDOMIZER_H

#include "wild_encounter.h"

void InitForkEncounterRandomizerSeed(void);
u16 GetForkMaxNationalDex(void);
enum Species ResolveForkRandomizedEncounterSpecies(u8 mapGroup, u8 mapNum, enum WildPokemonArea area, u8 slot, enum Species fallback);
u16 GetForkEncounterRandomizerBstCap(u8 mapGroup, u8 mapNum, enum WildPokemonArea area);
u16 GetForkEncounterRandomizerBstMin(u8 mapGroup, u8 mapNum, enum WildPokemonArea area);

#endif // GUARD_FORK_ENCOUNTER_RANDOMIZER_H
