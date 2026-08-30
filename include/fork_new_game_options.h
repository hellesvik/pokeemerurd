#ifndef GUARD_FORK_NEW_GAME_OPTIONS_H
#define GUARD_FORK_NEW_GAME_OPTIONS_H

u8 ForkNewGameOptionsNextPage(u8 page, u16 newKeys);
u8 ForkNewGameOptionsMoveSelection(u8 selection, u16 newKeys);
u8 ForkNewGameOptionsToggleValue(u8 value, u16 newKeys);
u8 ForkNewGameOptionsNextSetting(u8 settings, u16 newKeys);
u8 ForkNewGameOptionsNextFaintRule(u8 rule, u16 newKeys);
u8 ForkNewGameOptionsNextMaxGen(u8 generation, u16 newKeys);
bool8 ForkNewGameOptionsCanEdit(u8 settings);
bool8 ForkNewGameOptionsCanEditIndex(u8 settings, u8 index);
u16 ForkNewGameOptionsPresetValues(u8 settings);
bool8 ForkNewGameOptionsLocksBattleStyle(u8 settings);
bool8 ForkNewGameOptionsShouldContinue(u16 newKeys);

#endif // GUARD_FORK_NEW_GAME_OPTIONS_H
