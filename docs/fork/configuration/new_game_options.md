# New-Game Feature Options

Before Birch's introduction, a new game presents the fork feature menu. The
settings are stored in the save when START is pressed and apply for the rest
of that playthrough.

## Mode

The first row controls the preset for the remaining rules:

- **NUZLITE**: Enables the current fork rules. This is the default and may
  gain additional Nuzlite-specific rules later.
- **NUZLOCKE**: Enables the catch limit, level cap, and ON FAINT loss rule.
- **NORMAL**: Disables the listed fork rules, including the faint penalty.
- **CUSTOM**: Allows each remaining rule to be changed independently.

NUZLITE and NUZLOCKE lock the battle style to Set. Their catch and faint
rules begin when Birch gives the Pokédex.

When Mode is NUZLITE, NORMAL, or NUZLOCKE, the other rows remain visible but
are locked. Choose CUSTOM before changing them.

### Complete preset matrix

The following is the default state when each mode is selected. In `CUSTOM`,
the values in the last column are the initial values before the player edits
them. `ON` for Player EVs means normal EV gain is enabled.

| Setting | NUZLITE | NUZLOCKE | NORMAL | CUSTOM initial |
| --- | --- | --- | --- | --- |
| Catch Limit | ON | ON | OFF | OFF |
| Faint Rule | 1/WHITEOUT | ON FAINT | OFF | OFF |
| Level Cap | ON | ON | OFF | OFF |
| Items in Battle | OFF | OFF | ON | ON |
| Infinite Rare Candy | ON | ON | OFF | OFF |
| Infinite Repel | ON | ON | OFF | OFF |
| Player EVs | NORMAL | NORMAL | OFF | OFF |
| Item Randomizer | ON | ON | OFF | OFF |
| Random Encounters | ON | ON | OFF | OFF |
| Randomizer Max Gen | GEN 3 | GEN 3 | GEN 3 | GEN 3 |
| Random Abilities | ON | ON | OFF | OFF |
| Mega Evolution | ON | ON | OFF | OFF |
| Reusable TMs | OFF | OFF | OFF | OFF |

In NUZLITE, NUZLOCKE, and NORMAL, the rule rows are visible but locked. The
maximum generation remains editable in every mode. Choose CUSTOM to edit the
other rows independently.

## Catch Limit

- **ON**: Allows one normal wild encounter per named area, with the existing
  evolutionary-family dupes clause and shiny exception.
- **OFF**: Disables the fork's area encounter limit. Wild Pokémon can be
  caught without spending or checking an area encounter slot.

The default is **ON**, preserving the fork's current behavior.

## Faint Rule

- **1/WHITEOUT**: A whiteout marks one random party Pokémon as lost. Lost
  Pokémon remain fainted and cannot be healed.
- **ON FAINT**: Each player Pokémon that faints is marked as lost immediately.
  Lost Pokémon remain fainted and cannot be healed.
- **OFF**: Fainting and whiteouts do not mark Pokémon as lost.

NUZLITE defaults to **1/WHITEOUT**. NORMAL defaults to **OFF**. The rule can
be changed in CUSTOM.

## Level Cap

- **ON**: Uses the fork's hard story-progression level cap.
- **OFF**: Removes the fork's story level cap; Pokémon can continue gaining
  experience normally.

The default is **ON**, preserving the fork's current behavior.

## Items in Battle

- **ON**: The player can open the Bag and use battle items during battles.
- **OFF**: The battle-item action is disabled for the player.

NUZLITE and NUZLOCKE default to **OFF**. NORMAL defaults to **ON**. The rule
can be changed in CUSTOM.

## Reusable TMs

- **ON**: TMs remain in the Bag after teaching a move and can be taught again.
- **OFF**: Teaching a TM consumes one copy. HMs are unaffected.

All presets default to **OFF**. This option can be changed in CUSTOM.

## Infinite Rare Candy

- **ON**: Adds an Infinite Rare Candy to the Key Items pocket. It can be used
  repeatedly without being consumed.
- **OFF**: The Infinite Rare Candy is not added.

## Infinite Repel

- **ON**: Adds an Infinite Repel to the Key Items pocket. It can be toggled on
  and off without being consumed.
- **OFF**: The Infinite Repel is not added.

NUZLITE enables both convenience items. NORMAL disables them; NUZLOCKE keeps
 them enabled. CUSTOM can change them independently.

## Player EVs

- **NORMAL**: Player Pokémon gain EVs normally from battles.
- **OFF**: Player Pokémon do not gain EVs from battles.

NUZLITE and NUZLOCKE use **NORMAL** EV gains. NORMAL disables them. CUSTOM can
change this option independently.

## Item Randomizer

- **ON**: Randomized item-ball and hidden-item results are used.
- **OFF**: The original item results are preserved.

NUZLITE and NUZLOCKE enable the item randomizer. NORMAL disables it. CUSTOM
can change it independently.

When Mega Evolution is OFF, the Mega Ring is not granted and Mega Stones are
excluded from the randomized item pool.

## Mega Evolution

- **ON**: The player receives the Mega Ring and Mega Stones may appear in the
  item-randomizer pool.
- **OFF**: The player does not receive the Mega Ring, and Mega Stones are
  excluded from the item-randomizer pool.

## Random Encounters

- **ON**: Wild encounter species use the fork's randomized encounter tables.
- **OFF**: Wild encounters use the original species tables.

This setting also controls randomized starters, Steven's gift, and Mirage
Tower fossils. When OFF, those gifts use their vanilla species/items.

NUZLITE and NUZLOCKE enable randomized encounters. NORMAL disables them.
CUSTOM can change this option independently.

## Randomizer Max Gen

Selects the highest generation that randomized encounters may use. The menu
offers GEN 3 through GEN 9. In the current build the compile-time
`FORK_MAX_GEN_MONS` ceiling is GEN 9, so every menu choice is available. The
selected value is saved per playthrough and limits randomized encounter, egg,
gift, and static-encounter pools; it does not change vanilla encounter
tables. The default is GEN 3. Unlike the other custom rules, this setting
remains editable in every mode, including NUZLITE, NORMAL, and NUZLOCKE.

## Random Abilities

- **ON**: Eligible Pokémon families use the fork's randomized abilities.
- **OFF**: Pokémon retain their normal abilities.

NUZLITE and NUZLOCKE enable randomized abilities. NORMAL disables them.
CUSTOM can change this option independently.

Additional entries will be documented here as they are added to the menu.
