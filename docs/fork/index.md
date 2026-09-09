# Fork Docs

This folder contains the player-facing documentation for Emerurd's fork
features. Start with the [fork overview](../../FORK.md), then choose a
category below.

## Gameplay rules

- [Gameplay rules index](./gameplay/index.md)
- [Story battles](./gameplay/story_battles.md)
- [One Catch Per Area](./gameplay/one_catch_per_area.md)
- [Level Limits](./gameplay/level_limits.md)
- [Whiteout and faint penalties](./gameplay/whiteout_penalty.md)
- [Battle items, key items, and EV/IV rules](./gameplay/player_rules.md)

## Randomizers

- [Randomizer index](./randomizers/index.md)
- [Item Randomizer](./randomizers/item_randomizer.md)
- [TM Randomizer](./randomizers/tm_randomizer.md)
- [Biome Encounter Randomizer](./randomizers/biome_encounter_randomizer.md)
- [Ability Randomizer](./randomizers/ability_randomizer.md)
- [Gift Pokémon and Fossils](./randomizers/gift_pokemon_and_fossils.md)

## Configuration and reference data

- [New-Game Feature Options](./configuration/new_game_options.md)
- [Reference data index](./reference/index.md)
- [Biome species catalog](./reference/biome_species_catalog.csv)
- [Biome encounter assignments](./reference/biome_encounter_assignments.csv)

The CSV files are generated/reviewable reference data, not hand-written
player configuration. Their generation and source-of-truth notes are
documented in the [reference index](./reference/index.md).

## Errata changelog

### 2026-09-09

- Updated Phoebe, Glacia, Drake, and Steven's Champion teams with their new
  species, abilities, held items, and moves.
- Phoebe's Oricorio now receives a deterministic random form based on the
  save's seeded randomizer.
- Test mode now starts with all key items and all badges, unlocking every HM;
  the key-item pocket was expanded to hold the complete set.
- Story boss Pokémon now receive 31 IVs in every stat and Hardy (neutral)
  natures.
- Fixed Rustboro fossil-researcher exchanges producing script errors before
  returning the randomized Pokémon.

### 2026-09-07

- Fixed the saved Randomizer Max Gen selector so GEN 4–9 choices are no
  longer forced back to GEN 3. The current build ceiling is GEN 9; legacy
  unconfigured saves still use GEN 3.
- Fixed new-game menu state handling when changing generation, preserving the
  Random Abilities and Mega Evolution settings.
- Made Running Shoes available from the start of a new game instead of
  waiting for the Mother's gift.
- Added EVs as a summary-screen view alongside the existing IV view, with the
  top-right label showing the view currently displayed.
- Corrected the post-Roxanne level-cap milestone so the next rival cap is 16.

### 2026-09-04

- Added the custom gym, rival, Elite Four, and champion battle teams, with
  Steven as champion.
- Trainer Pokémon may use configured custom abilities even when an ability is
  not normally legal for that species.

### 2026-09-03

- Added the reusable-TM new-game option, custom NPC trade pools, and gym
  leader item gifts to item randomization.

### 2026-08-30

- Added randomized egg hatching and static encounters, including the special
  550–600 BST legendary/static pool.
- Replaced the old encounter bands with gradually increasing per-route and
  per-area BST ranges, including Victory Road at 490–560.

### 2026-08-22

- Removed the Wailord and Relicanth party requirement from the Sealed Chamber
  unlock.
- Changed the Mauville Game Corner evolution-item stock to stones and the
  Linking Cord instead of randomized items.
