# Fork Ability Randomizer

## Behavior

Every save receives a deterministic randomized Ability mapping. The mapping is
derived from that save's randomizer seed, so it is stable after saving and
reloading, while a new save can produce a different mapping.

The randomized Ability applies everywhere the species is used: wild Pokémon,
gift Pokémon, the player's Pokémon, and trainer Pokémon.

## Families and ability pool

One Ability is assigned to each evolutionary family. Every member of that
family, including ordinary forms, uses the same randomized Ability. For
example, if Charmander receives Levitate, Charmeleon and Charizard also have
Levitate.

The candidate Ability pool is built from the non-Legendary, non-Mythical
Pokémon available within `FORK_MAX_GEN_MONS`. An Ability cannot be assigned
unless a qualifying Pokémon can supply it. Legendary, Mythical, Ultra Beast,
and Paradox Pokémon keep their normal Abilities and do not contribute an
Ability to the pool.

All ordinary eligible Abilities can be randomized. This deliberately includes
Wonder Guard, Truant, Arena Trap, Shadow Tag, Huge Power, Pure Power, and
Pickup.

Shedinja is the exception for Wonder Guard: the Nincada family is excluded
from ability randomization so Shedinja always keeps Wonder Guard. Wonder Guard
remains available for other eligible families.

## Fixed form-mechanic Pokémon

Forecast is exclusive to Castform. Castform is not ability-randomized and
keeps Forecast.

The following form-mechanic Abilities are excluded from the pool:

- Multitype and RKS System
- Schooling, Disguise, Shields Down, and Power Construct
- Stance Change, Battle Bond, Zen Mode, and Flower Gift
- Gulp Missile, Ice Face, Hunger Switch, and Zero to Hero
- Commander, both As One variants, Tera Shift, and Teraform Zero

A Pokémon family that uses one of those Abilities is not ability-randomized;
it keeps its normal Ability data. It may still appear normally in encounters
when its generation is enabled.
