# Fork Level Limits

This fork uses the built-in hard level cap system.

## Current Behavior

- Battle EXP is blocked once a Pokemon is at or above the current cap.
- Daycare EXP is capped by the same limit.
- Normal one-use Rare Candies are still allowed to push a Pokemon above the cap.
- EV gain is disabled separately through the EV cap configuration.

## Current Cap Ladder

The active cap is chosen from the first uncleared story battle milestone in `src/caps.c`. Rival variants share the same milestone, and boss fights use the highest level present in that battle.

The ladder currently starts at Roxanne, advances through the selected rival, gym, and required villain milestones, then continues through Tate & Liza, the remaining villain battles, Juan, Wally's Victory Road battle, and Steven. Wally in Mauville, Tabitha at Mt. Chimney, Shelly at the Weather Institute and Seafloor Cavern, Matt, and the Elite Four do not create cap milestones. The cap never decreases: a later milestone that would be lower than the previous cap is raised to the previous cap plus one.

Because the rule is tied to story-battle milestones, the cap can be higher than the defeated battle's highest-level Pokémon. Overleveled Pokemon are not reduced in level; they just stop gaining EXP until the active cap rises again or Steven is cleared.
