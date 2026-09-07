#include "global.h"
#include "fork_new_game_options.h"

u8 ForkNewGameOptionsNextPage(u8 page, u16 newKeys)
{
    if (newKeys & R_BUTTON)
        return page == 3 ? 0 : page + 1;
    if (newKeys & L_BUTTON)
        return page == 0 ? 3 : page - 1;
    return page;
}

u8 ForkNewGameOptionsMoveSelection(u8 selection, u16 newKeys)
{
    if (newKeys & DPAD_UP)
        return selection == 0 ? 3 : selection - 1;
    if (newKeys & DPAD_DOWN)
        return selection == 3 ? 0 : selection + 1;
    return selection;
}

u8 ForkNewGameOptionsToggleValue(u8 value, u16 newKeys)
{
    if (newKeys & (DPAD_LEFT | DPAD_RIGHT))
        return value ^ 1;
    return value;
}

u8 ForkNewGameOptionsNextSetting(u8 settings, u16 newKeys)
{
    if (newKeys & DPAD_RIGHT)
        return settings == 3 ? 0 : settings + 1;
    if (newKeys & DPAD_LEFT)
        return settings == 0 ? 3 : settings - 1;
    return settings;
}

u8 ForkNewGameOptionsNextFaintRule(u8 rule, u16 newKeys)
{
    if (newKeys & DPAD_RIGHT)
        return rule == 2 ? 0 : rule + 1;
    if (newKeys & DPAD_LEFT)
        return rule == 0 ? 2 : rule - 1;
    return rule;
}

u8 ForkNewGameOptionsNextMaxGen(u8 generation, u16 newKeys)
{
    if (newKeys & DPAD_RIGHT)
        return generation >= GEN_9 ? GEN_3 : generation + 1;
    if (newKeys & DPAD_LEFT)
        return generation <= GEN_3 ? GEN_9 : generation - 1;
    return generation;
}

u8 ForkNewGameOptionsBitForIndex(u8 index)
{
    if (index == 11)
        return 14;
    if (index == 12)
        return 15;
    return index >= 4 ? index : index - 1;
}

bool8 ForkNewGameOptionsCanEdit(u8 settings)
{
    return settings == 3;
}

bool8 ForkNewGameOptionsCanEditIndex(u8 settings, u8 index)
{
    return ForkNewGameOptionsCanEdit(settings) || index == 10;
}

u16 ForkNewGameOptionsPresetValues(u8 settings)
{
    if (settings == 0)
        return 0xCBE7; // Catch limit + level cap + conveniences + all randomizers + Mega Evolution, Gen 3.
    if (settings == 1)
        return 0xCBEB; // Catch limit + level cap + ON FAINT + conveniences + all randomizers + Mega Evolution, Gen 3.
    return settings == 2 ? 0x810 : 0x800; // NORMAL keeps battle items enabled; generation defaults to Gen 3.
}

bool8 ForkNewGameOptionsPresetReusableTMs(u8 settings)
{
    (void)settings;
    return FALSE;
}

bool8 ForkNewGameOptionsLocksBattleStyle(u8 settings)
{
    return settings == 0 || settings == 1;
}

bool8 ForkNewGameOptionsShouldContinue(u16 newKeys)
{
    return (newKeys & START_BUTTON) != 0;
}
