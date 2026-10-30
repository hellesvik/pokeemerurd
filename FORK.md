# Emerurd fork rules

Emerurd is based on `pokeemerald-expansion` and adds configurable Nuzlite,
Nuzlocke, randomizer, and quality-of-life features.

The detailed, player-facing documentation is organized in
[`docs/fork/`](docs/fork/index.md). The most important entry points are:

- [New-game feature options](docs/fork/configuration/new_game_options.md)
- [Gameplay rules](docs/fork/gameplay/index.md)
- [Randomizers](docs/fork/randomizers/index.md)
- [Reference data](docs/fork/reference/index.md)

## Feature overview

- One encounter per named area, evolutionary-family dupes, and egg-hatch rules.
- Story-based level caps, faint penalties, and trainer-battle item restrictions.
- Infinite key items, Portable PC, all-box leveling, and Flash/Fly menu shortcuts.
- Seeded items, encounters, abilities, gifts, and fossils; fixed Gen 9 TMs.
- Expanded Bag capacity and a new-game save layout.
- Three startup modes: Nuzlite, Nuzlocke, and Custom, with seven visible settings.
- Wild Pokémon without held items and guaranteed permitted Safari Ball catches.
- Closed Day Cares and optional battle facilities, disabled rematches, and renewable money restrictions.

The Sealed Chamber unlock no longer requires Wailord and Relicanth in the
player's party.

## Developer reference

Implementation files and tests are linked from the individual feature pages.
