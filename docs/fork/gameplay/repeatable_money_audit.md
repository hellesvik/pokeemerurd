# Repeatable money audit

Audited the current working tree, 2026-10-30. E = Emerald; FRLG = both
FireRed and LeafGreen. This describes this fork, not unmodified retail games.
No gameplay changes were made. Findings below are from static source inspection;
multiplayer paths and profit exploits have not been exercised in an emulator.

Money is capped at 999,999. “Repeatable” means income can be earned again after
spending it. Items sell for one quarter of their configured price, rounded down,
under the current Gen 9 settings, not the retail Gen 3 half-price rule.
See [sale calculation](../../../src/item.c),
[item configuration](../../../include/config/item.h), and
[item prices](../../../src/data/items.h).

## Active sources

| Source | Games | Repeatable loop and requirements | Reward / costs | Evidence |
| --- | --- | --- | --- | --- |
| Pokémon League | E, FRLG | Beat the League again after completion; Elite Four state resets. | Normal trainer prize money; recovery costs depend on play. | `data/scripts/hall_of_fame.inc`, `data/scripts/hall_of_fame_frlg.inc`, `src/battle_script_commands.c:GetTrainerMoneyToGive` |
| Coin-generating moves | E, FRLG | Repeated wild or trainer battles with Pay Day or Make It Rain; G-Max Gold Rush additionally requires access to its battle mechanic and a trainer battle. | Pay Day effect adds 5 × user level per successful effect; Gold Rush adds 100 × level. Money bonuses apply. Move access depends on the available Pokémon/moves. | `src/battle_script_commands.c:MOVE_EFFECT_PAYDAY`, `MOVE_EFFECT_CONFUSE_PAY_DAY_SIDE`, `Cmd_givepaydaymoney`; `src/data/moves_info.h` |
| Pickup | E, FRLG | Empty-handed Pickup party Pokémon; remove and sell found items, then battle again. | 10% chance per eligible party member; level-dependent table includes saleable medicines, Mushrooms, Nuggets and other items. No consumable input required. | `src/battle_script_commands.c:Cmd_pickup`, `sPickupTable` |
| Honey Gather | E, FRLG | Empty-handed Honey Gather Pokémon; remove Honey and repeat battles. | Honey chance rises with level; configured Honey price 900, sale 225. | `src/battle_script_commands.c:Cmd_pickup`; `src/data/items.h:ITEM_HONEY` |
| Berry cultivation | E | Plant a Berry, grow and harvest multiple Berries, reserve seed stock and sell surplus. Needs clock advancement and growth care. | Positive sale value for ordinary Berries; yield and time vary. | `data/scripts/berry_tree.inc`; `src/berry.c:ObjectEventInteractionPickBerryTree` |
| Daily Berry gifts | E | Collect again after daily reset. Flower Shop; Route 111, 114, 120; Berry Master and wife; Lilycove; Sootopolis; Contest Lobby blender NPCs. | Free saleable Berries. Special wife phrases are individually limited, but normal Berry gifts recur. | `include/constants/flags.h:DAILY_FLAGS_START`; `src/clock.c:UpdatePerDay`; corresponding map scripts; `data/scripts/berry_blender.inc` |
| Shoal Cave materials / Shell Bells | E | Daily reset marks materials renewable; talk to Shell Bell expert to clear collection flags. Collect Salt and Shells across tides. | Salt/Shell sell for 5 each; exchange 4 of each for a Shell Bell worth 5,000 at current prices. No money input. | `src/clock.c:UpdatePerDay`; `data/maps/ShoalCave_LowTideEntranceRoom/scripts.inc:ResetShoalItems`; other low-tide room scripts |
| Ash → glass flutes | E | Soot Sack; collect ash repeatedly and exchange at Route 113 workshop; sell flutes. | Ash is the input. Black/White Flutes sell for 5 each under current prices; use each flute's configured price for the others. Decorations are not ordinary Bag sale items. | `src/field_tasks.c` (ash step callback); `data/maps/Route113_GlassWorkshop/scripts.inc` |
| Lilycove lottery | E | One draw per day; party/PC OT IDs must match winning digits. | PP Up, Exp. Share, Max Revive or Master Ball. Sell prizes with positive prices; a zero-price prize does not generate money. | `src/lottery_corner.c:sLotteryPrizes`; `data/maps/LilycoveCity_DepartmentStore_1F/scripts.inc`; `src/clock.c` |
| Master Rank Contest repeat wins | E | Win again with a Pokémon that already won that contest ribbon. | Repeat Master Rank wins award a Luxury Ball; sell it. No entry money charge. | `data/scripts/contest_hall.inc:ContestHall_EventScript_GiveContestPrizes` |
| Game Corner winnings → items | E, FRLG | Win renewable coins at slots (also roulette in E), exchange for saleable TMs/items, and sell. | Gambling can lose coins. This is renewable income through winnings, not guaranteed profit from buying coins. | `src/slot_machine.c`, `src/roulette.c`; Mauville Game Corner and Celadon Game Corner Prize Room scripts |
| Selphy requests | FRLG | Show requested species at Resort Gorgeous within the step limit; leave/re-enter to reset temporary completion flag and request again. Pokémon is shown, not surrendered. | Luxury Ball (70%) or Big Pearl/Pearl/Stardust/Star Piece/Nugget/Rare Candy; sell rewards. No entry fee. Species availability can make an individual request impractical. | `data/maps/FiveIsland_ResortGorgeous_House_Frlg/scripts.inc`; `src/field_specials.c:SampleResortGorgeousMonAndReward` |

## Multiplayer-dependent sources

| Source | Games | Repeatability and limitations | Evidence |
| --- | --- | --- | --- |
| Imported Secret Base trainer | E | Record-mixed base owner can be battled once per day. Separate daily state bypasses ordinary Match Call scheduling. Reward is 20 × first opponent level × money multiplier. Requires a second player's base record. | `src/secret_base.c:GetSecretBaseOwnerAndState`, `PrepSecretBaseBattleFlags`; `src/battle_script_commands.c:GetTrainerMoneyToGive`; `data/scripts/shared_secret_base.inc` |
| Pokémon Jump / Dodrio Berry Picking | E, FRLG | Wireless minigame sessions give saleable Berry prizes for qualifying scores; repeat sessions. Requires compatible players and eligible Pokémon. | `src/pokemon_jump.c:TryGivePrize`, `sPrizeItems`; `src/dodrio_berry_picking.c:TryGivePrize`; `data/scripts/cable_club_frlg.inc` and Emerald cable club scripts |
| Berry Crush → powder vendor items | E, FRLG | Repeated multiplayer sessions consume Berries for powder; exchange powder for saleable medicines. Renewable Berry stock makes this repeatable; net gain versus selling inputs depends on powder yield. | `src/berry_crush.c`; `src/berry_powder.c`; `data/maps/CeruleanCity_House5_Frlg/scripts.inc`; Emerald vendor scripts |
| Lilycove Quiz/Favor Lady | E | Record mixing resets completion state. Quiz gives a prize; Favor Lady consumes requested items before rewarding. Requires compatible records; Favor net profit depends on inputs. Not an automatic daily reset. | `src/lilycove_lady.c:ResetLilycoveLadyForRecordMix`; `src/record_mixing.c`; `data/scripts/lilycove_lady.inc` |

## Repeatable exploit and conversion checks

- **Winstrate partial completion (E):** Route 111 clears Victor, Victoria and
  Vivi's defeated flags whenever Vicky remains undefeated. Their normal payouts
  total 640 + 680 + 600 = 1,920 before bonuses. Losing to Vicky and returning
  allows earlier payouts again. Net profit depends on whiteout loss, badges,
  party level, recovery, and faint rules; this is a confirmed reset path, not a
  confirmed sustainable run under every preset. Evidence: Route 111 transition
  script, `src/data/trainers.party`, Winstrate money factor 10 in
  `src/battle_main.c`, and whiteout formula in `Cmd_getmoneyreward`.
- Regular shop buy/sell loops lose money: purchase uses configured price (or
  half-price sale), while resale uses one quarter. Premier Ball purchase bonuses
  do not cover that gap. TM randomization changes which move is taught, not this
  price relationship.
- Shard exchanges and Berry Crush can convert renewable inputs into more useful
  sale items; they do not independently create unlimited inputs. Shard sources
  must be renewable first. Imported held items are excluded from this audit.

## Disabled, finite, or inactive candidates

- Match Call/Gym rematches and Vs. Seeker: disabled by fork gates.
- Gabby and Ty: repeat battles remain, but all six parties award zero trainer
  prize money. Coin-generating moves still work during those battles.
- Frontier, Battle Tents, Trainer Hill, Trainer Tower and optional e-Reader
  basement: inaccessible. Their rewards cannot supply normal-play income.
- Wild held-item theft/farming: wild held items removed. Trainer theft is not a
  renewable stock source under current held-item restoration / NPC return rules
  (`src/battle_util.c:TryRestoreHeldItems`).
- Day Care breeding / held-item duplication through breeding: Day Cares closed.
- Infinite Candy, Infinite Repel, Heart Charm and Portable PC restoration: these
  are zero-price key items, not saleable renewable stock.
- Shuckle Oran Berry → Berry Juice conversion: compiled condition requires Gen 2;
  current setting is GEN_LATEST. Inactive.
- FRLG renewable hidden items: `VAR_RENEWABLE_ITEM_STEP_COUNTER` exists only as a
  definition/comment; no implementation references found. Do not assume vanilla
  Berry Forest / beach / cave respawns work here.
- Apricorn trees: generic daily-reset/grant code exists, but no Apricorn tree map
  placements found; not an accessible income source.
- Nugget Bridge, Dunsparce Tunnel Nugget, Lostelle gift, fossil pickups, ordinary
  item balls/hidden items, gym rewards and story gifts use progress/collection
  flags. Finite rewards are not recurring income merely because their scripts
  contain `giveitem`.
- Larger-Magikarp/Heracross records have a bounded size domain; improving records
  is finite, not an indefinitely repeating resource.
- Money multipliers, healing, reusable TMs and free PC leveling reduce costs or
  multiply an existing source; they do not create money independently.

## Coverage and limits

Inspected all `AddMoney` / `SetMoney` callers and the `AddBagItem` caller inventory,
scripted item grants, trainer-flag clears, daily resets, sale pricing, rematch
gates, and facility access. Production money credits are trainer payouts,
move payouts, script `addmoney`, and Bag sales. Script `addmoney` has no gameplay
call sites in the inspected scripts. New-game money is finite; debug money is
excluded.

The table is a source audit of repeatable mechanisms, not a claim that every
seed can obtain every required Pokémon. Randomized encounters/abilities affect
access to Pickup, Honey Gather and money moves; randomized rewards may change
sale value, including to zero. A reward must retain positive sale value to count
as income. No claim is made about arbitrary memory glitches or external save
imports.

Outstanding validation: exercise multiplayer connectivity for each build;
measure Berry Crush/Favor Lady input-output profit; reproduce Winstrate farming
under individual faint presets; inspect unusual battle-item recovery combinations
for duplication beyond the normal theft/restoration paths. These are explicitly
unresolved; this audit is not an exhaustive proof against every logic exploit.
