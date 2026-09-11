# One Catch Per Area

This fork enforces one normal encounter per named area. Named areas use the region map section, not the raw map file, so shared places like the Safari Zone count as one area.

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
