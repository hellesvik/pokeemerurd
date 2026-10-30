# Repeatable money sources

These rules apply to Emerald and FireRed/LeafGreen. Money is capped at
999,999, but income can be earned again after spending it. Ordinary items sell
for one quarter of their configured purchase price, rounded down. Honey, all
Berries, Shoal Salt, Shoal Shells, and Shell Bells cannot be sold.

## Available income

| Source | Games | Requirements and repeatability |
| --- | --- | --- |
| Pokémon League | All | Complete the League and challenge it again. Normal trainer prize money applies. |
| Lilycove lottery | Emerald | One draw per day. Matching party or PC Pokémon OT IDs can win prizes. Sell prizes with positive sale values; zero-price prizes do not generate money. |
| Master Rank Contests | Emerald | Repeat a win with a Pokémon that already won the ribbon for that contest. The Luxury Ball reward can be sold. |
| Pickup with ability randomization off | All | An empty-handed Pokémon with its normal Pickup ability can find items after battles. Remove and sell the item, then repeat. Pickup is excluded from randomized ability assignments. |
| Imported Secret Base battles | Emerald | Record mixing supplies another player's base. Its trainer can be battled once per day. Prize money is 20 × the first opponent's level × money multiplier. |

The lottery, Secret Base battles, and Winstrate challenge retain their usual
rules. The lottery draws new numbers daily; collecting more distinct OT IDs
improves matching opportunities but does not guarantee a prize.

## Renewable activities without direct cash rewards

Berry cultivation and daily gifts, Honey Gather, Shoal Cave collecting, Pokémon
Jump, and Dodrio Berry Picking remain playable. Their Berries, Honey, Shoal
materials, and Shell Bells cannot be sold. Minigame rewards therefore do not
provide income by selling those items directly.

Berry Crush consumes Berries and gives Berry Powder. Powder vendors exchange
it for items, some of which can be sold. This remains a possible indirect
income route when supplied with renewable Berries; profitability depends on
powder yield and input costs. Multiplayer connectivity and net returns have
not been verified at runtime.

Shard exchanges convert Shards into evolution stones. They are exchanges,
not independent sources of renewable Shards. Imported resources from another
save are outside this reference's income model.

## Money restrictions

- Pay Day, Make It Rain, and G-Max Gold Rush retain their battle effects but
  do not generate money.
- Pickup is excluded from the randomized ability pool. Honey Gather still
  works, but Honey has no sale value.
- Berry gifts and growing remain active, but all Berries have no sale value.
- Shoal Cave collecting and Shell Bell exchanges remain active, but the
  materials and Bells have no sale value.
- The ash workshop does not award the Soot Sack or offer exchanges. Ash
  collection is disabled.
- Slots and Emerald roulette charge wagers but award no coins. Coin purchases
  and prize exchanges remain available; winning games cannot fund exchanges.
- Selphy's rescue remains available, but her repeatable requests are disabled.
- Lilycove Quiz and Favor Lady activities are disabled.
- Gabby and Ty can still be battled and interviewed, but all six parties
  award zero prize money, even with money bonuses.
- Match Call and Vs. Seeker rematches are disabled. Closed battle facilities
  cannot provide prizes or access to their shops.

Ordinary shop purchases and resale lose money: even half-price purchases cost
more than the quarter-price resale value. Premier Ball bonuses do not cover
that gap. Money bonuses multiply an existing payout rather than creating one.

## Winstrate partial completion

In Emerald, returning to Route 111 while Vicky remains undefeated resets
Victor, Victoria, and Vivi. Their payouts total 1,920 before bonuses. Losing
to Vicky can therefore allow earlier victories and payouts to be repeated.
Net profit depends on whiteout penalties, party level, badges, recovery costs,
and faint rules. The reset path is confirmed in source; sustainable farming
under individual faint presets has not been verified at runtime.

## Finite or unavailable candidates

Ordinary collected items, story gifts, Nugget Bridge, and the Dunsparce Tunnel
Nugget are finite rewards. Size-record improvements have a bounded domain and
do not provide an indefinite loop. Wild held-item farming is unavailable:
wild Pokémon start without items. Trainer-stolen items are not kept after
battle. Day Care services are closed. Infinite key items have zero prices
and cannot be sold.

Shuckle's Berry Juice conversion requires the Gen 2 configuration and is
inactive. Apricorn tree code has no map placements. The FRLG renewable
hidden-item step counter exists as a definition/comment, but no implementation
references were found; retail-game hidden-item respawns should not be assumed.

## Implementation references and verification limits

- [Money awards and post-battle item abilities](../../../src/battle_script_commands.c)
- [Sale calculation](../../../src/item.c) and [item prices](../../../src/data/items.h)
- [Ability pool exclusions](../../../src/fork_randomizer_catalog.c)
- [Daily resets](../../../src/clock.c), [lottery](../../../src/lottery_corner.c),
  and [Secret Base battles](../../../src/secret_base.c)
- [Contest rewards](../../../data/scripts/contest_hall.inc)
- [Winstrate reset](../../../data/maps/Route111/scripts.inc)
- [Berry Crush](../../../src/berry_crush.c) and [powder exchange](../../../src/berry_powder.c)

This inventory comes from static inspection of money credits, item grants,
trainer resets, daily events, sale prices, and feature gates. It is not an
exhaustive proof against memory glitches or item-duplication exploits.
Multiplayer paths, Berry Crush profitability, and Winstrate farming need
runtime verification. Species and move availability depend on encounter and
ability settings and the selected generation limit.
