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

- Catch limits, evolutionary-family dupes, shiny exceptions, and egg-hatch
  handling.
- Story-based level caps and configurable whiteout/faint penalties.
- Battle-item restrictions, infinite key items, and zero-EV player Pokémon.
- Randomized items, TMs, encounters, abilities, gifts, and fossils.
- Generation limits, Mega Evolution controls, and starter IV improvements.
- A startup menu for choosing presets or customizing these rules.

The Sealed Chamber unlock no longer requires Wailord and Relicanth in the
player's party.

## Developer reference

Implementation files and tests are linked from the individual feature pages.
