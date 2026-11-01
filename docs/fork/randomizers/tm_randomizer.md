# Gen 9 TMs

## Fixed move assignments

TM001–TM229 use the official Pokémon Scarlet/Violet assignments, including
The Teal Mask and The Indigo Disk DLC. TM001 teaches Take Down and TM229
teaches Upper Hand. These assignments are the same in every new game and do
not depend on the randomizer seed or enabled species generations.

Reference: [Scarlet/Violet TM list](https://www.serebii.net/scarletviolet/tm.shtml).
The canonical numbered table is in `include/constants/tms_hms.h`.

The eight Emerald HMs remain separate items with their normal field behavior.
Moves such as Surf, Fly, and Waterfall can be taught by either their TM or HM.
Pokémon retain this project's existing teachable learnsets. The catalog change
does not add TM crafting or change which Pokémon can learn a move.

The **Reusable TMs** option controls consumption: OFF consumes one copy when
teaching; ON keeps the TM. HMs remain reusable.

## Department Store TM shops

The two TM clerks on Lilycove Department Store 4F sell eight distinct TMs
selected from TM001–TM229 by the save's seed. The shop displays and awards the
resolved TM rather than the internal TM51–TM58 stock-slot placeholders.
Each listing sells exactly one copy per save, then shows **SOLD OUT**.

Celadon Department Store 2F provides the same eight seeded, single-copy TM
listings in FireRed and LeafGreen.

## Item randomizer and inventory

All 229 TMs participate in the item-randomizer pool. Their locations and shop
stock can vary by seed, but the move taught by each TM is fixed.

The TM/HM pocket holds 237 stacks, enough for all 229 TMs and eight HMs at
once. The Items pocket holds 320 stacks. These capacities and the expanded
save layout require a new game; old saves are not supported.

Move-named rewards use the corresponding Gen 9 TM. Rewards whose moves are
absent from the Gen 9 TM catalog retain their original numbered reward slot
and therefore teach that number's new move.
