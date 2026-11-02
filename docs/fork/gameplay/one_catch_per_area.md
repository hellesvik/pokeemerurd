# One Catch Per Area

This fork enforces one normal encounter per named area. Named areas use the
region map section, not the raw map file, so shared places like the Safari Zone
count as one area. A numbered route and its corresponding underwater Dive map
also share one encounter: an encounter from Surfing, fishing, grass, or diving
spends that route's single limit. Separately named underwater locations remain
their own areas.

The PokéNav's **Catch Log** replaces the Ribbons viewer. It lists Hoenn routes
and named places accessible before the League in story progression order,
including unvisited areas and optional destinations. Post-League destinations
and Trainer Hill are omitted. Red **USED** means the area's encounter allowance has been
spent; green **OPEN** means it has not. A failed catch or fleeing encounter can
therefore still show USED. Shared areas such as the Safari Zone appear once.

The log opens at your current area. Use Up/Down to scroll, Left/Right to page,
and B to return. The side panel shows the total number of used areas. If catch
limits are disabled, the log says so and continues to show previously recorded allowances. It is available as soon as
you have the PokéNav, without earning a Ribbon. Pokémon retain their Ribbons.

The first non-shiny wild encounter that is not covered by dupes clause spends the area's encounter when the battle ends, whether or not it is caught. Dupes are checked by evolutionary family across party, boxes, daycare, and caught Pokedex data, and they do not spend the area. Shiny encounters are always catchable and never blocked by a spent area.

The Safari Zone is one shared strict encounter area. Its first wild encounter
is the only catchable encounter for the entire zone and spends the limit when
the battle ends, including when it escapes or the player runs. The duplicate
and shiny exceptions do not grant another Safari Zone encounter.

Egg hatching also respects the area rule. If an egg is ready to hatch in an
area that has already spent its encounter, the hatch is blocked and its hatch
counter resets. If it becomes ready again in another spent area, the warning
is shown again and the counter resets again.

Eggs hatch at approximately one-tenth of the normal step interval. Magma
Armor, Flame Body, and Steam Engine still provide their additional hatch-speed
bonus.

Daycare breeding checks for a new Egg every 25 steps instead of every 256
steps, making checks slightly more than ten times as frequent. Parent
compatibility and the chance of producing an Egg per check are unchanged.

**Collecturd**, the FR/LG scientist beside the right-hand table in the Pokémon League lobby, offers a one-time trade for
4,000 Game Corner coins. He supplies remaining visible item-ball pickups from
pre-League areas, using their normal randomized rewards when enabled. Hidden
items, post-League pickups, Pokémon disguised as balls, and both Mirage Tower
fossils are excluded. The entire collection must fit in every affected bag
pocket, including Items and TMs. If it cannot fit, nothing is charged or removed.
A successful trade removes the corresponding item balls from the world.
