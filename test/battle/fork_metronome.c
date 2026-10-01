#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Fork Metronome selects a special move on exactly one of four branch rolls")
{
    PASSES_RANDOMLY(1, 4, RNG_METRONOME_SPECIAL_CHANCE);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_METRONOME); }
    } SCENE {
        MESSAGE("Waggling a finger let it use Origin Pulse!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ORIGIN_PULSE, player);
    }
}

SINGLE_BATTLE_TEST("Fork Metronome retains normal move selection on three of four branch rolls")
{
    PASSES_RANDOMLY(3, 4, RNG_METRONOME_SPECIAL_CHANCE);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_METRONOME); }
    } SCENE {
        NOT MESSAGE("Waggling a finger let it use Origin Pulse!");
    }
}
