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

The candidate Ability pool is built from ordinary eligible Pokémon available
within the selected maximum generation. It also includes Legendary, Mythical,
Ultra Beast, and Paradox Pokémon that qualify for the special static-encounter
pool (550–600 BST). Those qualifying special Pokémon receive deterministic
randomized Abilities and their normal Abilities contribute to the pool.
Other Legendary, Mythical, Ultra Beast, and Paradox Pokémon keep their normal
Abilities and do not contribute an Ability to the pool.

All ordinary eligible Abilities can be randomized. This deliberately includes
Truant, Arena Trap, Shadow Tag, Huge Power, Pure Power, and Pickup. Wonder
Guard is excluded from the candidate pool.

Shedinja's Nincada family is excluded from ability randomization, so Shedinja
always keeps its normal Wonder Guard.

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
