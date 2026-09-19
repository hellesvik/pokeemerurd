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

## Fork changelog

### 2026-09-19

- Rebuilt Brendan and May's Rustboro, Route 110, Route 119, and Lilycove
  rival teams for every player-starter choice, with the configured species,
  levels, held items, moves, 31 IVs, and randomized Abilities. The player boss
  reference now shows every variant with Pokémon and type sprites.
- Replaced U-Turn with Tailwind on Winona's Swellow and updated the boss
  reference to match.
- Standardized the boss reference on the game's Hardy-nature behavior. A
  regression now checks both the runtime boss override and the trainer-party
  compiler's Hardy default so rivals and admins cannot silently diverge.
- Nuzlite and Nuzlocke now default the randomizer generation limit to Gen 9
  in the new-game menu. Normal and Custom continue to default to Gen 3.

### 2026-09-16

- Randomized TM51–TM130 entries now display their assigned move descriptions
  in the Bag instead of placeholder text.
- Level-cap increase notices now appear after the complete post-battle event,
  including badge, reward, and other trainer dialogue.
- Gym Leader reward dialogue and acquisition displays now name the item
  actually awarded when the item randomizer replaces the original TM.

### 2026-09-15

- Fixed Infinite Repel sometimes requiring multiple uses to turn off after
  walking. Its active state is no longer decremented by overworld steps.

### 2026-09-14

- Reworked the shared Pokémon randomizer catalog so ability pools, evolution
  items, species-specific items, and Mega Stones use the same generation- and
  availability-aware family data. Unavailable regional forms and Mega forms
  no longer contribute Abilities, and Mega Pokémon retain their native
  Abilities.
- Fixed the Mauville Wally battle failing to complete when the level-cap
  notification overwrote the battle script result. Post-battle notifications
  now preserve `VAR_RESULT`.
- Changed New Mauville's land and encounter-method assignments to use the City
  biome.
- Fixed the Lilycove Department Store TM clerks so their menus display the
  seeded TM assignments, award those exact TMs, limit each listing to one
  purchase, and show sold-out listings correctly.
- Updated every Gym Leader's reward explanation to name the item actually
  awarded, including randomized rewards.
- Changed Metronome to use its normal selection behavior 75% of the time and
  a curated signature/chaos move pool 25% of the time.
- Disabled the remaining playable-ROM test helpers. New games no longer
  receive a test Rayquaza, ignore obedience, or unlock every Fly destination;
  startup badges and progression key items also remain disabled.

### 2026-09-13

- Regional branch evolutions that normally depend on being in Alola, Galar,
  or Hisui now use the Trick House as their location requirement while
  retaining their normal level, item, time, move, and weather requirements.
- Added evolution items, Kubfu's Scrolls, species-specific form items, and
  Mega Stones to the item-randomizer master pool. Single-purpose evolution
  items and Mega Stones are eligible only when their associated evolutionary
  family is available under the selected generation and randomizer BST rules.
  Mega Stones are also excluded when Mega Evolution is disabled.

### 2026-09-12

- Added Shelly's Weather Institute battle as a Lv. 32 cap milestone. The
  following Route 119 rival battle also uses Lv. 32.
- Fixed the Mauville Game Corner prize menu repeatedly allocating prize-name
  buffers by moving its names to static storage.
- Blocked egg hatches now reset the egg's hatch counter, preventing the hatch
  warning from triggering after every step.

### 2026-09-11

- Removed battle-item inventories from Gym Leaders, rivals, Wally, Aqua and
  Magma leaders/admins, the Elite Four, and the Champion. Pokémon held items
  are unchanged.
- Egg hatching is now approximately ten times faster while retaining the
  existing helper-ability bonus.
- NPC trade Pokémon now receive randomized IVs when traded instead of fixed
  low IV templates.
- Raised the Pokémon League level cap to 80 and updated every Elite Four and
  Champion team to the new Lv. 74–80 progression.
- The entire Safari Zone now shares one strict catch limit. Its first wild
  encounter spends the limit even if it escapes or the player runs.
- Nuzlite and Nuzlocke rules now start when Birch gives the Pokédex. A
  Pokémon lost under the ON FAINT rule remains fainted and cannot be healed.
- Fixed ordinary battle knockouts bypassing the ON FAINT loss rule.
- Special-static-pool Pokémon (550–600 BST) now both contribute their normal
  Abilities to the ability pool and receive seeded randomized Abilities.
- Lilycove Department Store 4F now sells eight distinct, seeded TMs selected
  from TM01–TM130. Each listing can be purchased once per save and then shows
  as sold out.
- Overworld movement now runs by default. Hold B to walk where running is
  normally permitted.
- Hardened fork randomizers and item-ball handling against invalid save or
  script state that could cause a game crash.
- Reworked the player EV option. When disabled, battle and item EV gains are
  blocked at their source; when enabled, EVs work normally. Loading a save no
  longer scans and rewrites every party, PC, and daycare Pokémon.

### 2026-09-10

- Added a human-readable evolution reference for all configured generations,
  ordered by Pokédex number, without duplicate entries; regional forms are
  listed separately under their debut generation.
- Added small grass encounter patches to cities that previously had no
  possible encounters, including their biome and BST-range assignments in the
  encounter reference data.
- Expanded the boss battle reference with the complete level-cap battle order,
  rival team possibilities, trainer sprites, Pokémon types, and type markers
  for moves.
- Added a post-battle message when the player reaches a new level cap. The
  message appears after the defeated trainer's final battle text.
- Test mode now starts with Emerald's progression and utility key items only;
  FireRed and later-generation key items are no longer granted automatically.

### 2026-09-09

- Updated Phoebe, Glacia, Drake, and Steven's Champion teams with their new
  species, abilities, held items, and moves.
- Phoebe's Oricorio now receives a deterministic random form based on the
  save's seeded randomizer.
- Test mode starts with Emerald key items and all badges, unlocking every HM;
  the key-item pocket was expanded for the test setup.
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

- Changed the Mauville Game Corner evolution-item stock to stones and the
  Linking Cord instead of randomized items.
