# Fork Level Limits

This fork uses the built-in hard level cap system.

## Current Behavior

- Battle EXP is blocked once a Pokemon is at or above the current cap.
- Day Care services are closed; Pokémon cannot be trained there.
- Normal one-use Rare Candies are still allowed to push a Pokemon above the cap.
- EV gain is disabled separately through the EV cap configuration.
- After defeating the story battle that raises the cap, a message announces the new cap. It appears after that battle's defeat text and before the map's post-battle event continues.

## Emerald cap ladder

The active cap is chosen from the first uncleared story battle milestone in `src/caps.c`. Rival variants share the same milestone, and boss fights use the highest level present in that battle.

The ladder currently starts at Roxanne, advances through the selected rival, gym, and required villain milestones, then continues through Aqua Admin Matt, Tate & Liza, the remaining villain battles, Juan, Wally's Victory Road battle, and Steven. Wally in Mauville, Tabitha at Mt. Chimney, Shelly at Seafloor Cavern, and the Elite Four do not create cap milestones. The cap never decreases: a later milestone that would be lower than the previous cap is raised to the previous cap plus one.

The final Pokémon League cap is **Lv. 80**. It becomes the active cap after the Victory Road Wally milestone and remains in force through Sidney, Phoebe, Glacia, Drake, and Champion Steven.

Because the rule is tied to story-battle milestones, the cap can be higher than the defeated battle's highest-level Pokémon. Overleveled Pokemon are not reduced in level; they just stop gaining EXP until the active cap rises again or Steven is cleared.

Use **LEVEL BOXES TO CAP** in the Pokémon PC menu to raise all stored, non-Egg
Pokémon below the cap directly to it. Party Pokémon are unaffected; this does
not trigger evolution or move-learning prompts.

FireRed and LeafGreen use [Kanto story milestones](../configuration/firered_leafgreen.md#regional-progression), with caps 63 for the first League and 75 for postgame.
