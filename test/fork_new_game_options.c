#include "global.h"
#include "fork_new_game_options.h"
#include "test/test.h"

TEST("New game options page changes with L and R")
{
    EXPECT_EQ(ForkNewGameOptionsNextPage(0, L_BUTTON), 1);
    EXPECT_EQ(ForkNewGameOptionsNextPage(1, R_BUTTON), 2);
    EXPECT_EQ(ForkNewGameOptionsNextPage(0, A_BUTTON), 0);
    EXPECT_EQ(ForkNewGameOptionsNextPage(2, R_BUTTON), 3);
}

TEST("New game options continues with START")
{
    EXPECT_EQ(ForkNewGameOptionsShouldContinue(START_BUTTON), TRUE);
    EXPECT_EQ(ForkNewGameOptionsShouldContinue(A_BUTTON), FALSE);
}

TEST("New game options selection moves vertically")
{
    EXPECT_EQ(ForkNewGameOptionsMoveSelection(0, DPAD_UP), 3);
    EXPECT_EQ(ForkNewGameOptionsMoveSelection(3, DPAD_DOWN), 0);
    EXPECT_EQ(ForkNewGameOptionsMoveSelection(1, DPAD_LEFT), 1);
}

TEST("New game options value changes sideways")
{
    EXPECT_EQ(ForkNewGameOptionsToggleValue(0, DPAD_RIGHT), 1);
    EXPECT_EQ(ForkNewGameOptionsToggleValue(1, DPAD_LEFT), 0);
    EXPECT_EQ(ForkNewGameOptionsToggleValue(1, DPAD_UP), 1);
}

TEST("New game options presets lock custom rules")
{
    EXPECT_EQ(ForkNewGameOptionsCanEdit(0), FALSE);
    EXPECT_EQ(ForkNewGameOptionsCanEdit(1), FALSE);
    EXPECT_EQ(ForkNewGameOptionsCanEdit(2), FALSE);
    EXPECT_EQ(ForkNewGameOptionsCanEdit(3), TRUE);
    EXPECT_EQ(ForkNewGameOptionsPresetValues(0), 52199);
    EXPECT_EQ(ForkNewGameOptionsPresetValues(1), 52203);
    EXPECT_EQ(ForkNewGameOptionsPresetValues(2), 2064);
}

TEST("Randomizer max generation remains editable in every mode")
{
    EXPECT_EQ(ForkNewGameOptionsCanEditIndex(0, 10), TRUE);
    EXPECT_EQ(ForkNewGameOptionsCanEditIndex(1, 10), TRUE);
    EXPECT_EQ(ForkNewGameOptionsCanEditIndex(2, 10), TRUE);
    EXPECT_EQ(ForkNewGameOptionsCanEditIndex(3, 10), TRUE);
    EXPECT_EQ(ForkNewGameOptionsCanEditIndex(0, 9), FALSE);
    EXPECT_EQ(ForkNewGameOptionsCanEditIndex(3, 13), TRUE);
    EXPECT_EQ(ForkNewGameOptionsCanEditIndex(0, 13), FALSE);
}

TEST("New game presets keep TMs consumable")
{
    EXPECT_EQ(ForkNewGameOptionsPresetReusableTMs(0), FALSE);
    EXPECT_EQ(ForkNewGameOptionsPresetReusableTMs(1), FALSE);
    EXPECT_EQ(ForkNewGameOptionsPresetReusableTMs(2), FALSE);
    EXPECT_EQ(ForkNewGameOptionsPresetReusableTMs(3), FALSE);
}

TEST("New game faint rule cycles through three values")
{
    EXPECT_EQ(ForkNewGameOptionsNextFaintRule(0, DPAD_RIGHT), 1);
    EXPECT_EQ(ForkNewGameOptionsNextFaintRule(1, DPAD_RIGHT), 2);
    EXPECT_EQ(ForkNewGameOptionsNextFaintRule(2, DPAD_RIGHT), 0);
    EXPECT_EQ(ForkNewGameOptionsNextFaintRule(0, DPAD_LEFT), 2);
}

TEST("Randomizer max generation cycles from Gen 3 through Gen 9")
{
    EXPECT_EQ(ForkNewGameOptionsNextMaxGen(GEN_3, DPAD_RIGHT), GEN_4);
    EXPECT_EQ(ForkNewGameOptionsNextMaxGen(GEN_9, DPAD_RIGHT), GEN_3);
    EXPECT_EQ(ForkNewGameOptionsNextMaxGen(GEN_3, DPAD_LEFT), GEN_9);
}

TEST("New game settings cycles through four modes")
{
    EXPECT_EQ(ForkNewGameOptionsNextSetting(0, DPAD_RIGHT), 1);
    EXPECT_EQ(ForkNewGameOptionsNextSetting(1, DPAD_RIGHT), 2);
    EXPECT_EQ(ForkNewGameOptionsNextSetting(2, DPAD_RIGHT), 3);
    EXPECT_EQ(ForkNewGameOptionsNextSetting(3, DPAD_RIGHT), 0);
    EXPECT_EQ(ForkNewGameOptionsNextSetting(0, DPAD_LEFT), 3);
}

TEST("Nuzlite and Nuzlocke lock battle style to Set")
{
    EXPECT_EQ(ForkNewGameOptionsLocksBattleStyle(0), TRUE);
    EXPECT_EQ(ForkNewGameOptionsLocksBattleStyle(1), TRUE);
    EXPECT_EQ(ForkNewGameOptionsLocksBattleStyle(2), FALSE);
    EXPECT_EQ(ForkNewGameOptionsLocksBattleStyle(3), FALSE);
}
