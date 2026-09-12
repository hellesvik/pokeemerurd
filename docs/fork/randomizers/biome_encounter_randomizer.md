# Biome Encounter Randomizer

> Status: implemented. The generated CSV files in this folder are the
> reviewable source data for species pools and map/method assignments.

## Goal

Randomize wild encounters by biome rather than giving every map an unrelated
pool. A player should meet Pokémon that make sense for where and how they are
encountering them, while each new save receives a different encounter layout.

The implementation:

- Keep land, surfing, and fishing encounters distinct.
- Allow some intentional overlap between nearby or related biomes.
- Gate the strongest species by each route/place's intended game progression
  without forcing later encounters to be strong.
- Remain stable within a save: revisiting an area must not reshuffle it.

## Scope

The feature applies to ordinary wild encounters:

- Land encounters: grass, caves, deserts, mountains, beaches, and similar
  walkable encounter terrain.
- Surf encounters.
- Old Rod, Good Rod, and Super Rod fishing encounters.
- Rock Smash encounters.

Static encounters and eggs are also randomized when random encounters are
enabled. Gifts, trainer parties, and raid- or overworld-specific encounter
systems remain outside this feature.

Eggs are resolved when they hatch, regardless of whether they came from the
Day Care, a scripted gift, a trade, or another source. Egg results use a
100–550 BST range and exclude Legendary, Mythical, Ultra Beast, and Paradox
species.

Static encounters use the current map's route/place BST range. Kecleon,
Electrode, and Voltorb therefore follow the same range as the route where they
appear. The Regi trio, Groudon, Kyogre, and Rayquaza instead use a 550–600 BST
range and may resolve to any enabled species in that range, including special
species.

## Biomes

Each ordinary encounter table is assigned a biome. The initial biome set is:

- City
- Grassland
- Forest
- Cave
- Mountain
- Desert
- Beach/coast
- Marsh/swamp
- Volcanic
- Snow/ice
- Freshwater
- Ocean

The assignment is explicit map/method data in
[`biome_encounter_assignments.csv`](../reference/biome_encounter_assignments.csv). The
generator uses map-name rules only to create that checked-in assignment file;
the game uses the generated explicit assignments, never a runtime name guess.

Biomes provide a *theme*, not a hard Pokédex type restriction. City includes
Pokémon associated with urban land and waterways, including Normal, Electric,
Water, Fighting, Poison, Flying, Psychic, Bug, Ghost, Dark, Steel, and Fairy
types. Forest can contain Bug-, Grass-, Flying-, Poison-, and Normal-type
Pokémon. A Pokémon may belong to more than one biome.

Every encounter method in a town or city uses the City biome. This includes
land, surfing, fishing, and Rock Smash tables where present. The normal
method-compatibility filter still requires Water types for surfing and fishing,
and Rock, Ground, or Steel types for Rock Smash.

## Encounter methods

Each route/map encounter table is randomized separately. Its biome and encounter
method select the candidate pool used to fill that table:

| Method | Intended flavor |
| --- | --- |
| Land | Three randomized species generated separately for each route/map; every weighted land slot maps to one of them. |
| Surf | Three randomized species generated separately for each route/map with surf encounters. |
| Fishing | Two randomized species per rod, for six across Old, Good, and Super Rod on each route/map. |
| Rock Smash | Three randomized species generated separately for each route/map with Rock Smash encounters. |

Surf and fishing must not use the land pool. A map's biome determines the
appropriate candidates for all of its encounter methods; for example, coastal
and ocean maps can use water-appropriate surf and fishing candidates, while a
cave lake can use freshwater or cave-water candidates. Fishing can overlap with
surfing, but retains its own rod-tier weighting so better rods improve variety
and access to stronger candidates.

The randomizer replaces only species entries. Maps keep their normal encounter
rates and level ranges. Maps that have no encounter data for a method continue
to have no encounters for that method.

## Per-save generation

At new-game initialization, the feature creates a per-save encounter seed. For
each map encounter table and encounter method, it deterministically derives
replacement species from that seed, map identity, method, and slot. This avoids
storing every route table in the save while ensuring contents never depend on
map visit order.

The derived table must be stable for that save. Its results must not depend on
the order in which maps are visited or encounters occur.

Maps in the same biome draw from the same candidate pool, but receive their own
randomized species tables. They may overlap by chance or by intentional
weighting, but are not required to be identical. Maps retain their own encounter
rate and level ranges.

## Power progression by place

Eligibility is based on a species' total base stats (BST). Each route/place has
an explicit maximum BST in its encounter configuration. The cap depends on how
far along that place is intended to be in the game; it does not depend on the
badges currently owned by the player.

Each route/place has both a minimum and maximum BST. The minimum is normally
100 below the maximum, so later areas expand the available power range without
making every encounter equally strong. Neighboring areas intentionally overlap.

The implementation includes a complete explicit map/place-to-BST-range
assignment. Starting locations use a low range, midgame routes progressively
increase it, and late-game routes reach the 450–550 range. Victory Road is the
intentional exception, using 490–560. The values are reviewed in
[`biome_encounter_assignments.csv`](../reference/biome_encounter_assignments.csv).

### BST limits

The following table includes every BST-gated random encounter pool. A
route/place limit applies to each configured encounter method there (land,
water, fishing, or Rock Smash), and to ordinary static encounters resolved on
that map. Areas with multiple floors are grouped where they share a limit.

| BST range | Encounters, routes, and places |
| ---: | --- |
| 100–550 | Hatched Eggs |
| 150–250 | Route 101, Littleroot Town, Oldale Town, Petalburg City |
| 160–260 | Route 102 |
| 170–270 | Route 103 |
| 180–280 | Route 104, Petalburg Woods, Rustboro City |
| 190–290 | Route 105 |
| 200–300 | Route 106, Dewford Town |
| 210–310 | Routes 107–109 |
| 220–320 | Granite Cave |
| 230–330 | Route 110 |
| 240–340 | Route 111 |
| 250–350 | Route 112, Fiery Path, Jagged Pass, Lavaridge Town |
| 260–360 | Route 113, Mirage Tower, Desert Underpass |
| 270–370 | Route 114, Rusturf Tunnel, Fallarbor Town |
| 275–325 | Birch's three Route 101 starter choices |
| 280–380 | Route 115 |
| 290–390 | Route 116 |
| 300–400 | Route 117, New Mauville, Mauville City, Verdanturf Town |
| 310–410 | Route 118 |
| 320–420 | Route 119, Safari Zone, Fortree City |
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
| 550–600 | Regirock, Regice, Registeel, Groudon, Kyogre, and Rayquaza static encounters; special species are allowed |

The bounds are inclusive. The exact map/method rows are defined in the CSV
linked above.

The towns and cities listed in the table have natural grass patches, so they
have land encounters in addition to any water or fishing encounters already
present.

The map's ordinary encounter level rules remain in effect. BST gating controls
which species may be selected; it does not raise their level.

## Eligibility and coverage

Legendary, Mythical, Ultra Beast, and Paradox Pokémon are excluded from
ordinary encounters and eggs. The generated species catalog is the source for
ordinary biome pools. The designated high-tier static encounters are the
exception and may select from those categories.
The current runtime implementation does not perform a global per-save coverage
audit or repair pass; it independently derives each table from its assigned
biome pool and BST cap.

Randomized species are limited by the per-save **Randomizer Max Gen** choice
on the new-game screen, from GEN 3 through GEN 9. In the current build,
`FORK_MAX_GEN_MONS` in `include/config/wild_encounter.h` is a GEN 9
compile-time ceiling, so all menu choices are available. Legacy or
unconfigured saves fall back to GEN 3.

## Duplicate and overlap policy

- Repetition within one table is avoided while the compatible pool has enough
  candidates. If it does not, the original species for that slot is retained.
- Related biomes may share species (for example, coast with ocean, or forest
  with grassland).
- The same species may appear through both surf and fishing when appropriate.

## Data and implementation shape

The implementation is data-driven:

- A map encounter-table assignment table containing its biome and maximum BST.
- Species biome tags and allowed encounter methods.
- A species eligibility helper for form validity, Legendary/Mythical/Ultra
  Beast/Paradox exclusion, and BST calculation.
- Per-save seed and generated encounter state.

The existing static encounter tables remain the source of map levels, encounter
rates, and available methods. The randomizer replaces species entries only.

## Acceptance criteria for implementation

- A grass encounter, surf encounter, and fishing encounter on the same or
  related maps draw from their appropriate separate pools.
- Every route/map receives its own per-save table for each encounter method it
  supports.
- Land, surf, and Rock Smash expose three randomized species per map and
  method. Every weighted slot maps onto that method's three-species pool.
- Each fishing rod exposes two randomized species per map. The Old, Good, and
  Super Rod pools are distinct, for six randomized fishing species in total.
- The same biome may overlap across routes, but routes are not required to have
  identical species tables.
- A save reload produces the same encounter tables.
- A new save can produce different tables.
- No Legendary, Mythical, Ultra Beast, or Paradox species is selected for
  ordinary encounters or eggs.
- No species above a place's configured BST cap is selected.
- Weak species remain eligible in later places.

## Generated data

- [`biome_species_catalog.csv`](../reference/biome_species_catalog.csv) is the
  canonical-species biome membership source. It contains 914 eligible species
  and excludes Legendary, Mythical, Ultra Beast, and Paradox species.
- [`biome_encounter_assignments.csv`](../reference/biome_encounter_assignments.csv)
  assigns every ordinary Emerald encounter table a biome, method, slot count,
  and BST ceiling.
- `tools/generate_biome_encounter_data.py` converts both CSV/data sources into
  the compiled tables used by the game.
