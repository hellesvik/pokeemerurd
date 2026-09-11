# TM Randomizer

## Behavior

The game supports TM01 through TM130. TM01–TM50 retain their existing fixed
contents. TM51–TM130 are assigned random moves when a new save is created.
The assignment is stored in the save and therefore does not change when the
game is reloaded.

## Lilycove Department Store

The two TM clerks on Lilycove Department Store 4F sell TM51–TM58. Their moves
come from the save's seeded TM randomizer. Each of these eight shop TMs can be
purchased once per save. After purchase, its listing shows **SOLD OUT** and
selecting it displays the standard sold-out message.

## Move pool and teaching

Each randomized TM receives a non-HM move found in at least one enabled
species' teachable learnset, so every generated TM can be used by at least one
Pokémon. The randomizer selects without replacement while eligible moves remain;
it would restart that selection only if the eligible move pool were exhausted.

Pokémon retain their normal move compatibility. A Pokémon can learn a
randomized TM only when it can normally learn that TM's assigned move.

The **Reusable TMs** new-game option controls consumption. When it is OFF,
teaching a TM consumes one copy; when it is ON, the TM remains available for
later use. HMs are unaffected.

## Item randomizer

TM51–TM130 are included in the item-randomizer pool alongside TM01–TM50.
Each randomized TM item can therefore appear once in the randomized world-item
layout for a save.

The current TM pocket retains its existing 64-slot capacity. Adding enough
simultaneous TM inventory for all 200 machines would require a separate save
layout redesign.

The randomized-TM save data format is currently version 2. If that version is
updated, the TM assignments are regenerated from the save's item-randomizer
seed.
