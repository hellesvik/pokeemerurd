# Randomizer Guide

Randomizer settings are saved per playthrough. A new game creates the mapping
from its own seed, so results remain stable after saving and loading.

## Encounter randomizer

When enabled, randomized wild encounters use the fork's biome and area pools.
Each area has its own gradually increasing BST range based on main-story access
order; areas may overlap. Late routes reach 460–560, Ever Grande uses 470–560,
and Victory Road uses 490–560. The minimum configured range is 150.

Towns and cities use a dedicated City biome for land, surfing, fishing, and
Rock Smash encounters. Fortree City instead uses Forest, while Pacifidlog Town
uses Ocean. Surf encounters permit Water or Flying types; fishing remains
Water-only. Rock Smash requires Rock, Ground, or Steel types.

The selected maximum generation limits randomized species. Eggs, gifts,
fossils, and static encounters are randomized too. The table below includes
every BST-gated pool. Fossil choices and Steven's postgame gift use separate
species rules rather than a BST range.

Ordinary static encounters such as Kecleon, Voltorb, and Electrode use the
listed range for their current area. The bounds are inclusive.

### Rules by encounter type

All ordinary encounter methods use the current map's biome, its inclusive BST
range, and the selected maximum generation. Legendary, Mythical, Ultra Beast,
and Paradox Pokémon are excluded from these ordinary pools. Randomization
changes species only: the original encounter rates and level ranges remain.
Each map and method receives its own deterministic selections for that save.

| Encounter type | Species and slot rules |
| --- | --- |
| Land, including grass and cave floors | Three distinct species from the map's biome and BST range. The normal twelve weighted slots repeat those three species. No additional type requirement. |
| Surf | Three distinct Water- or Flying-type species from the map's biome and BST range. The normal five weighted slots repeat those three species. |
| Fishing | Six distinct Water-type species from the map's biome and BST range when enough candidates exist: two for the Old Rod, two for the Good Rod, and two for the Super Rod. Each rod repeats its pair across its normal weighted slots. |
| Rock Smash | Three distinct Rock-, Ground-, or Steel-type species from the map's biome and BST range. The normal weighted slots repeat those three species. |
| Hatched Eggs | One base-form species from any biome, using BST 100–550. The result is chosen when the Egg hatches. Ordinary special-species exclusions apply. |
| Ordinary static encounters | One species from the current map's land biome and BST range. Kecleon, Voltorb, and Electrode use this rule. If the map has no land assignment, the original species remains. |
| Special static encounters | Regirock, Regice, Registeel, Groudon, Kyogre, and Rayquaza use BST 550–600. Special species are permitted, but the generation limit still applies. |
| Birch's Route 101 starters | Three distinct base-form species using BST 275–325. The selected generation limit applies. The chosen starter receives at least two perfect IVs. |

If a pool has too few eligible candidates, the game keeps the original species
for any selection it cannot fill.

| BST range | Encounters, routes, and places |
| ---: | --- |
| 100–550 | Hatched Eggs |
| 150–250 | Route 101, Littleroot Town, Oldale Town |
| 160–260 | Route 103 |
| 170–270 | Route 102 |
| 180–280 | Route 104, Petalburg Woods, Rustboro City |
| 190–290 | Route 116, Rusturf Tunnel |
| 200–300 | Routes 105–106, Dewford Town |
| 210–310 | Granite Cave |
| 220–320 | Routes 107–109 |
| 230–330 | Slateport City |
| 240–340 | Route 110 |
| 250–350 | Route 117, Mauville City, Verdanturf Town |
| 260–360 | Routes 111–112, Fiery Path, Mirage Tower |
| 270–370 | Route 113, Fallarbor Town |
| 275–325 | Birch's three Route 101 starter choices |
| 280–380 | Route 114 |
| 290–390 | Route 115, Meteor Falls |
| 300–400 | Jagged Pass, Lavaridge Town, Petalburg City |
| 310–410 | Route 118, New Mauville, Abandoned Ship |
| 320–420 | Route 119, Safari Zone, Fortree City |
| 330–430 | Route 120 |
| 340–440 | Routes 121–122, Mt. Pyre |
| 350–450 | Route 123, Lilycove City |
| 360–460 | Route 124, Underwater Route 124, Magma Hideout |
| 370–470 | Mossdeep City |
| 380–480 | Route 125, Shoal Cave |
| 390–490 | Route 127 |
| 400–500 | Route 128 |
| 410–510 | Seafloor Cavern |
| 420–520 | Route 126, Underwater Route 126 |
| 430–530 | Cave of Origin, Sootopolis City |
| 440–540 | Route 129 |
| 450–550 | Route 130 |
| 460–560 | Routes 131–134, Sky Pillar, Pacifidlog Town |
| 470–560 | Ever Grande City |
| 490–560 | Victory Road, Artisan Cave, Altering Cave, Desert Underpass |
| 550–600 | Regirock, Regice, Registeel, Groudon, Kyogre, and Rayquaza static encounters; special species are allowed |

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
