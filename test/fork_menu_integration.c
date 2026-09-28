#include "global.h"
#include "fork_new_game_options.h"
#include "fork_run.h"
#include "load_save.h"
#include "new_game.h"
#include "save.h"
#include "test/test.h"

void Test_RunNewGameFeatureMenu(const u16 *keys, u32 count);
void Test_RunOptionMenu(const u16 *keys, u32 count);

TEST("Feature menu presets survive new game and flash save reload")
{
    for (u8 mode = 0; mode < 4; mode++)
    {
        u16 keys[4] = {DPAD_RIGHT, DPAD_RIGHT, DPAD_RIGHT, START_BUTTON};
        keys[mode] = START_BUTTON;
        Test_RunNewGameFeatureMenu(keys, mode + 1);
        struct SaveBlock3 before = *gSaveBlock3Ptr;
        NewGameInitData();
        EXPECT_EQ(gSaveBlock3Ptr->forkBattleStyleLocked, mode <= 1);
        EXPECT_EQ(ForkGetRandomizerMaxGen(), mode <= 1 ? GEN_9 : GEN_3);
        EXPECT_EQ(ForkArePlayerEvsEnabled(), FALSE);
        EXPECT_EQ(ForkAreBattleItemsEnabled(), mode == 2);
        // The contiguous gameplay fields cover every feature-menu setting.
        EXPECT_EQ(memcmp(&before.forkGameplayOptionsConfigured,
                         &gSaveBlock3Ptr->forkGameplayOptionsConfigured,
                         offsetof(struct SaveBlock3, forkLilycoveTmShopPurchases)
                         - offsetof(struct SaveBlock3, forkGameplayOptionsConfigured)), 0);
        CheckForFlashMemory();
        EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
        ClearSav3();
        EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
        EXPECT_EQ(memcmp(&before.forkGameplayOptionsConfigured,
                         &gSaveBlock3Ptr->forkGameplayOptionsConfigured,
                         offsetof(struct SaveBlock3, forkLilycoveTmShopPurchases)
                         - offsetof(struct SaveBlock3, forkGameplayOptionsConfigured)), 0);
    }
}

TEST("Custom feature menu edits every setting without clearing other settings")
{
    static const u16 keys[] = {
        DPAD_LEFT, // Nuzlite to Custom.
        DPAD_DOWN, DPAD_RIGHT, DPAD_DOWN, DPAD_RIGHT, DPAD_DOWN, DPAD_RIGHT,
        R_BUTTON, DPAD_RIGHT, DPAD_DOWN, DPAD_RIGHT, DPAD_DOWN, DPAD_RIGHT, DPAD_DOWN, DPAD_RIGHT,
        R_BUTTON, DPAD_RIGHT, DPAD_DOWN, DPAD_RIGHT, DPAD_DOWN, DPAD_RIGHT, DPAD_DOWN, DPAD_RIGHT,
        R_BUTTON, DPAD_RIGHT, DPAD_DOWN, DPAD_RIGHT,
        L_BUTTON, L_BUTTON, L_BUTTON, DPAD_DOWN, DPAD_DOWN, DPAD_DOWN, DPAD_RIGHT,
        START_BUTTON,
    };
    Test_RunNewGameFeatureMenu(keys, ARRAY_COUNT(keys));
    NewGameInitData();
    EXPECT_EQ(ForkIsCatchLimitEnabled(), TRUE);
    EXPECT_EQ(ForkIsLevelCapEnabled(), TRUE);
    EXPECT_EQ(ForkGetFaintRule(), FORK_FAINT_RULE_ON_FAINT);
    EXPECT_EQ(ForkAreBattleItemsEnabled(), TRUE);
    EXPECT_EQ(ForkHasInfiniteRareCandy(), TRUE);
    EXPECT_EQ(ForkHasInfiniteRepel(), TRUE);
    EXPECT_EQ(ForkArePlayerEvsEnabled(), TRUE);
    EXPECT_EQ(ForkIsItemRandomizerEnabled(), TRUE);
    EXPECT_EQ(ForkAreRandomEncountersEnabled(), TRUE);
    EXPECT_EQ(ForkAreRandomAbilitiesEnabled(), TRUE);
    EXPECT_EQ(ForkGetRandomizerMaxGen(), GEN_4);
    EXPECT_EQ(ForkAreMegaEvolutionsEnabled(), TRUE);
    EXPECT_EQ(ForkAreTMsReusable(), TRUE);
    EXPECT_EQ(ForkIsBattleStyleLocked(), FALSE);
}

TEST("Option menu saves battle style changes and honors the Set lock")
{
    static const u16 keys[] = {DPAD_DOWN, DPAD_DOWN, DPAD_RIGHT, B_BUTTON};
    gSaveBlock3Ptr->forkGameplayOptionsConfigured = TRUE;
    ForkSetBattleStyleLocked(FALSE);
    gSaveBlock2Ptr->optionsBattleStyle = OPTIONS_BATTLE_STYLE_SET;
    Test_RunOptionMenu(keys, ARRAY_COUNT(keys));
    EXPECT_EQ((u32)gSaveBlock2Ptr->optionsBattleStyle, OPTIONS_BATTLE_STYLE_SHIFT);
    ForkSetBattleStyleLocked(TRUE);
    Test_RunOptionMenu(keys, ARRAY_COUNT(keys));
    EXPECT_EQ((u32)gSaveBlock2Ptr->optionsBattleStyle, OPTIONS_BATTLE_STYLE_SET);
}

TEST("Feature menu locks preset rules but allows generation changes")
{
    static const u16 keys[] = {
        DPAD_DOWN, DPAD_RIGHT, // Attempt to disable the locked catch limit.
        R_BUTTON, DPAD_RIGHT, // Attempt to enable locked battle items.
        R_BUTTON, DPAD_DOWN, DPAD_DOWN, DPAD_LEFT, // Gen 9 to Gen 8.
        START_BUTTON,
    };
    Test_RunNewGameFeatureMenu(keys, ARRAY_COUNT(keys));
    NewGameInitData();
    EXPECT_EQ(ForkIsCatchLimitEnabled(), TRUE);
    EXPECT_EQ(ForkAreBattleItemsEnabled(), FALSE);
    EXPECT_EQ(ForkGetRandomizerMaxGen(), GEN_8);
    EXPECT_EQ(ForkIsBattleStyleLocked(), TRUE);
}

TEST("Option menu edits all standard settings and persists them through flash")
{
    static const u16 keys[] = {
        DPAD_RIGHT, DPAD_DOWN, DPAD_RIGHT, DPAD_DOWN, DPAD_RIGHT,
        DPAD_DOWN, DPAD_RIGHT, DPAD_DOWN, DPAD_RIGHT, DPAD_DOWN, DPAD_RIGHT,
        B_BUTTON,
    };
    gSaveBlock3Ptr->forkGameplayOptionsConfigured = TRUE;
    ForkSetBattleStyleLocked(FALSE);
    gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_MID;
    gSaveBlock2Ptr->optionsBattleSceneOff = FALSE;
    gSaveBlock2Ptr->optionsBattleStyle = OPTIONS_BATTLE_STYLE_SET;
    gSaveBlock2Ptr->optionsSound = OPTIONS_SOUND_MONO;
    gSaveBlock2Ptr->optionsButtonMode = OPTIONS_BUTTON_MODE_NORMAL;
    gSaveBlock2Ptr->optionsWindowFrameType = 0;
    Test_RunOptionMenu(keys, ARRAY_COUNT(keys));
    CheckForFlashMemory();
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    ClearSav2();
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
    EXPECT_EQ((u32)gSaveBlock2Ptr->optionsTextSpeed, OPTIONS_TEXT_SPEED_FAST);
    EXPECT_EQ((u32)gSaveBlock2Ptr->optionsBattleSceneOff, TRUE);
    EXPECT_EQ((u32)gSaveBlock2Ptr->optionsBattleStyle, OPTIONS_BATTLE_STYLE_SHIFT);
    EXPECT_EQ((u32)gSaveBlock2Ptr->optionsSound, OPTIONS_SOUND_STEREO);
    EXPECT_EQ((u32)gSaveBlock2Ptr->optionsButtonMode, OPTIONS_BUTTON_MODE_LR);
    EXPECT_EQ((u32)gSaveBlock2Ptr->optionsWindowFrameType, 1);
}
