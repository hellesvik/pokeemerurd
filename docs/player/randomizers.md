# Randomizer Guide

Randomizer settings are saved per playthrough. A new game creates the mapping
from its own seed, so results remain stable after saving and loading.

## Encounter randomizer

When enabled, randomize wild encounters. These will randomize encounters based
on biomes and BST. In short: 
- The later in the game, the better pokemon you will encounter.
- If you fish, you are more likely to get a water type. Similar for other biomes.

Some quirks to mention are as follows:
- Cities each have an encounter, and there exist a city biome.
- Rock Smash, surf and fishing has typing guarantees.
- Diving have its own biome, different from surf.
- Fossils are randomized when received, but convert to their correct pokemon.
- Steven house has a gift pokemon after 8th badge.

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
| Dive | Three distinct species from the curated Underwater biome and the underwater map's BST range. Non-Water and non-Flying species are permitted. |
| Fishing | Six distinct Water-type species from the map's biome and BST range: two for the Old Rod, two for the Good Rod, and two for the Super Rod. Each rod repeats its pair across its normal weighted slots. |
| Rock Smash | Three distinct Rock-, Ground-, or Steel-type species from the map's biome and BST range. The normal weighted slots repeat those three species. |
| Hatched Eggs | One first-stage species that can evolve, selected from any biome using BST 100–550. Species with no evolutions are excluded. The result is chosen when the Egg hatches. Ordinary special-species exclusions apply. |
| Ordinary static encounters | One species from the current map's land biome and BST range. Kecleon, Voltorb, and Electrode use this rule. The two Aqua Hideout Electrode use the Underwater biome with a 350–450 BST range. If another map has no land assignment, the original species remains. |
| Special static encounters | Regirock, Regice, Registeel, Groudon, Kyogre, and Rayquaza use BST 550–600. Special species are permitted, but the generation limit still applies. |
| Birch's Route 101 starters | Three distinct base-form species using BST 275–325. The selected generation limit applies. The chosen starter receives at least two perfect IVs. |

Every supported generation setting is tested against every assignment. A build
fails its tests if a pool cannot provide all of that method's distinct species.
Route 103 fishing therefore uses BST 160–270, while its other methods use
160–260. Shoal Cave Surfing and fishing use 290–535, while its land encounters
use 380–480.

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
| 350–450 | Route 123, Lilycove City, Aqua Hideout Electrode |
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
Single-purpose evolution items, Mega Stones, and species-specific form items
appear only when their Pokémon can actually be obtained from the Pokémon
randomizer in the selected generation range. Randomized special static
encounters and obtainable pre-evolutions count; Pokémon excluded by the BST
rules do not. Items requiring multiple Pokémon require all of them. Evolution
items with battle-held effects remain available regardless, and all Mega
Stones are excluded when Mega Evolution is disabled. 
Mauville Game Corner sells evolution stones and the Linking Cord instead of
randomized stock.

## Ability randomizer

Eligible evolutionary families receive one deterministic ability. 
Exceptions exist, including boss battles and form change abilites.
More info is available in the [Ability Randomizer documentation](../fork/randomizers/ability_randomizer.md).

## TM randomizer

TMs are randomized independently from ordinary item placement. TMs are
single-use unless **Reusable TMs** is enabled. HMs are unaffected by TM
consumption rules and do not need to be taught to a Pokémon for field use.

## Gifts, eggs, and trades

Eggs are randomized when they hatch, regardless of their source. NPC trades
are randomized at the start of the game from their configured, generation-
capped pools. The custom trades and gift details are listed in the [fork
randomizer documentation](../fork/randomizers/gift_pokemon_and_fossils.md).
