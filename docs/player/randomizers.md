# Randomizer Guide

Randomizer settings are saved per playthrough. A new game creates the mapping
from its own seed, so results remain stable after saving and loading.

## Encounter randomizer

When enabled, randomized wild encounters use the fork's biome and area pools.
Each area has its own gradually increasing BST range; areas may overlap. The
highest ordinary route range tops out at 550, while Victory Road uses
490–560. The minimum configured range is 150.

The selected maximum generation limits randomized species. Eggs, gifts,
fossils, and static encounters are randomized too, with eggs using BST
100–550. Legendary static encounters such as the Regis, Groudon, Kyogre, and
Rayquaza use BST 550–600 and may include any enabled species in that range,
including special species.

Kecleon, Voltorb, and Electrode use the range for the area where they are
encountered.

## Item randomizer

Item Balls, hidden items, and gym leader item gifts can be randomized.
Mauville Game Corner sells evolution stones and the Linking Cord instead of
randomized stock.

## Ability randomizer

Eligible evolutionary families receive one deterministic ability. Special
Pokémon in the 550–600 BST legendary-static pool also receive randomized
Abilities and contribute their normal Abilities to that pool; other Legendary,
Mythical, Ultra Beast, Paradox, and form-mechanic families remain restricted
as described in the technical [Ability Randomizer documentation](../fork/randomizers/ability_randomizer.md).

Shedinja always keeps Wonder Guard; the Nincada family is protected from
ability randomization. Wonder Guard may still appear on other eligible
families.

## TM randomizer

TMs are randomized independently from ordinary item placement. TMs are
single-use unless **Reusable TMs** is enabled. HMs are unaffected by TM
consumption rules and do not need to be taught to a Pokémon for field use.

## Gifts, eggs, and trades

Eggs are randomized when they hatch, regardless of their source. NPC trades
are randomized at the start of the game from their configured, generation-
capped pools. The custom trades and gift details are listed in the [fork
randomizer documentation](../fork/randomizers/gift_pokemon_and_fossils.md).
