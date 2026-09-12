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

| BST range | Routes and places |
| ---: | --- |
| 150–250 | Route 101, Petalburg City |
| 160–260 | Route 102 |
| 170–270 | Route 103 |
| 180–280 | Route 104, Petalburg Woods |
| 190–290 | Route 105 |
| 200–300 | Route 106, Dewford Town |
| 210–310 | Routes 107–109 |
| 220–320 | Granite Cave |
| 230–330 | Route 110 |
| 240–340 | Route 111 |
| 250–350 | Route 112, Fiery Path, Jagged Pass |
| 260–360 | Route 113, Mirage Tower, Desert Underpass |
| 270–370 | Route 114, Rusturf Tunnel |
| 280–380 | Route 115 |
| 290–390 | Route 116 |
| 300–400 | Route 117, New Mauville |
| 310–410 | Route 118 |
| 320–420 | Route 119, Safari Zone |
| 330–430 | Route 120, Mt. Pyre |
| 340–440 | Routes 121–122 |
| 350–450 | Route 123, Lilycove City |
| 360–460 | Abandoned Ship, Slateport City, Pacifidlog Town |
| 370–470 | Routes 124–125, Underwater Route 124 |
| 380–480 | Shoal Cave |
| 390–490 | Route 126, Underwater Route 126, Mossdeep City |
| 400–500 | Meteor Falls, Magma Hideout |
| 410–510 | Route 127 |
| 420–520 | Route 128, Seafloor Cavern |
| 430–530 | Route 129 |
| 440–540 | Route 130 |
| 450–550 | Routes 131–134, Cave of Origin, Ever Grande City, Sky Pillar, Sootopolis City, Artisan Cave, Altering Cave |
| 490–560 | Victory Road |

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
