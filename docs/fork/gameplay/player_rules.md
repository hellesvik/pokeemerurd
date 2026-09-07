# Player rules and quality-of-life items

## Battle items

When the setting is disabled, bag items cannot be used during trainer
battles. Healing items, revives, X-items, escape items, and balls are blocked
through the normal battle-item validation path without consuming the item.

Held items consumed or removed during a battle are restored when the battle
ends. This applies to player and opposing Pokémon in the current battle
configuration.

## Infinite key items

When enabled, new games receive two reusable key items:

- **Infinite Rare Candy** raises a selected Pokémon one level at a time, but
  never above the current level cap.
- **Infinite Repel** toggles an effectively infinite repel state on or off and
  can be registered like other field-use key items.

Existing saves receive missing items when they are continued.

## Running Shoes

New games receive the Running Shoes immediately. Their availability does not
depend on receiving the Mother's gift.

## EVs and IVs

With player EV gain disabled, player-owned Pokémon are normalized to 0 EVs.
This applies to new Pokémon and to party, box, and daycare Pokémon when an
older save is loaded. The summary screen cycles through the Stats, IVs, and
EVs views; the top-right label identifies the view currently displayed.

## Nuzlite and Nuzlocke start

Catch and faint rules become active after the player receives the first
Poké Balls, so the opening fights cannot cause an immediate rules wipe. These
modes also lock the battle style to Set.

See [New-Game Feature Options](../configuration/new_game_options.md) for the
available presets.
