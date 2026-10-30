# Player rules and quality-of-life items

## Battle items

Bag items cannot be used during trainer battles in any startup mode. Healing items, revives, X-items, escape items, and balls are blocked
through the normal battle-item validation path without consuming the item.

Held items consumed or removed during a battle are restored when the battle
ends. This applies to player and opposing Pokémon in the current battle
configuration.

Wild Pokémon start encounters without held items, including scripted encounters
and wild double battles. This rule applies regardless of species or lead ability.

Thief and Covet handle stolen items differently depending on the encounter:

- Wild Pokémon start without items to steal.
- Items stolen from Trainer Pokémon are not kept after the battle.

## Replacing HM moves

HM moves can be replaced when teaching a new move. They can also be forgotten
through the game's other move-replacement interfaces.

Field HMs are available after the HM has been received and its normal badge
requirement is met. The player can interact with the relevant object or
terrain directly; no party Pokémon needs to know the move.

After obtaining their HM and badge, **FLASH** and **FLY** also appear in every
non-Egg Pokémon's field menu, even if its species cannot learn the move.
Fainted Pokémon can use these shortcuts too. Normal location restrictions
still apply. The Gen 9 Fly TM alone does not unlock the Fly shortcut.

## Infinite key items

New games in every mode receive two reusable key items:

- **Infinite Rare Candy** raises a selected Pokémon one level at a time, but
  never above the current level cap.
- **Infinite Repel** toggles an effectively infinite repel state on or off and
  can be registered like other field-use key items.

## Heart Charm

Every save receives the reusable **Heart Charm** key item. Its heart icon can
be used from the Key Items pocket or registered for field use. It restores the
player's party exactly as a Pokémon Center does: HP, status conditions, PP,
and fainted Pokémon are restored.

## Portable PC

Every save also receives a reusable **Portable PC** key item. Its compact PC
icon opens the Pokémon Storage System directly to **Move Pokémon**, so the
player can organize their boxes while out in the field.
It cannot be used anywhere in Ever Grande City, including the Pokémon League
and its Pokémon Center, or on any floor of Victory Road.

## Running Shoes

New games receive the Running Shoes immediately. Their availability does not
depend on receiving the Mother's gift. The player runs by default where running
is allowed; hold **B** while moving to walk.

## EVs and IVs

With player EVs disabled, newly obtained Pokémon start with 0 EVs and cannot
gain EVs from battles or EV-raising items. Loading a save no longer scans or
rewrites Pokémon that are already in the party, PC, or daycare.

With player EVs enabled, Pokémon gain EVs normally from battles and EV-raising
items, and their existing EVs are preserved. EV-reducing items still work in
either mode.

The summary screen cycles through the Stats, IVs, and EVs views; the top-right
label identifies the view currently displayed.

## Friendship in battle

Friendship does not grant affection-style battle bonuses. Pokémon cannot
survive at 1 HP, cure their own status, evade attacks, gain extra critical-hit
chance, or receive extra experience because of high friendship.

## Nuzlite and Nuzlocke start

Catch and faint rules become active when Birch gives the Pokédex, so the
opening fights cannot cause an immediate rules wipe. These modes also lock the
battle style to Set.

See [New-Game Feature Options](../configuration/new_game_options.md) for the
available presets.

## Level all PC boxes to cap

Choose **LEVEL BOXES TO CAP** from the Pokémon PC main menu to raise Pokémon
in every stored box to the current level cap. Eggs and Pokémon already at or
above the cap are skipped, and party Pokémon are unaffected. Level caps are enabled in every startup mode. Levels increase directly without evolution or
move-learning prompts; existing moves and species are preserved.

## Day Care

All Day Care services are closed in every gameplay preset, including Route 117,
FRLG Route 5, and Four Island. Pokémon cannot be deposited, trained, or bred,
and Day Care Eggs cannot be collected. Buildings remain accessible and staff
explain that the Day Care is closed.

One-time NPC gift Eggs, including Lavaridge's Wynaut, remain available. Eggs
in the party still hatch normally.

## Bag capacity

The Items pocket holds **320 stacks**, and the TM/HM pocket holds **237**: all
229 TMs plus eight HMs. These limits apply to every gameplay preset. Stack
quantity limits and the other pockets are unchanged. The Items capacity covers
the eligible main randomizer pool plus hidden-item treasures and ordinary
supplies.

This version requires a **new game**. Expanded inventories use a packed save
layout across the existing two rotating save slots; older saves are not
compatible. Legacy external save tools cannot use the old fixed sector layout
or its eight-bit Items-pocket capacity metadata.

## Trainer rematches and optional battle facilities

Trainer rematch systems are disabled in every preset in Emerald, FireRed, and
LeafGreen. Match Call no longer schedules trainer or Gym Leader rematches or
shows rematch indicators. Normal contacts, calls, and required tutorials remain.
The Vs. Seeker is not awarded and cannot be activated.

Gabby and Ty's battles, route rotation, and interviews remain available, but
all six parties award zero prize money, including with money bonuses. Their
victory message displays ₽0. Money-generating moves also award no money.

Battle Frontier, Battle Tents, Trainer Hill, FRLG's Trainer Tower and its grounds,
and Mossdeep's optional e-Reader battle basement are closed. Their entrances
are blocked; the Frontier is absent from ferry and Fly destinations. Gifts and
shops within the closed areas are inaccessible too.

Normal trainer and Gym battles, scripted rival/story battles, Contests, and
repeat Pokémon League runs remain available.

## Renewable money restrictions

Money-generating moves retain their battle effects but produce no money.
Pickup is excluded from randomized abilities. Honey, all Berries, Shoal Salt,
Shoal Shells and Shell Bells cannot be sold; their purchase prices and effects
are unchanged. Ash collecting and workshop exchanges, Selphy requests, and
Lilycove Quiz/Favor Lady activities are disabled. Game Corner games still charge
wagers but award no coins. Coin purchases and prize exchanges remain available.
League, lottery, Secret Base, minigame and Winstrate rules are unchanged.

Pay Day, Make It Rain, and G-Max Gold Rush keep their battle effects without
producing money. Pickup remains a normal species ability when ability
randomization is off; it cannot be selected by the ability randomizer.
Berry growing and gifts, Honey Gather, and Shoal Cave collecting still work,
but their affected rewards cannot be sold. Selphy's rescue is available;
her repeatable requests are disabled. The Lilycove Contest Lady remains active.

See [Repeatable money sources](repeatable_money_audit.md) for remaining income
sources and conditions.
