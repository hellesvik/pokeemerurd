#include "global.h"
#include "event_data.h"
#include "field_move.h"
#include "pokemon.h"
#include "test/test.h"
#include "test/overworld_script.h"
#include "script.h"
#include "constants/decorations.h"
#include "constants/flags.h"
#include "constants/field_move.h"
#include "constants/moves.h"

TEST("Received field HMs do not require a Pokémon that knows the move")
{
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_WOBBUFFET, 10, 0, OTID_STRUCT_PLAYER_ID);

    FlagClear(FLAG_BADGE01_GET);
    RUN_OVERWORLD_SCRIPT(checkfieldmove FIELD_MOVE_CUT, TRUE;);
    EXPECT_EQ(gSpecialVar_Result, PARTY_SIZE);

    FlagSet(FLAG_BADGE01_GET);
    FlagClear(FLAG_RECEIVED_HM_CUT);
    RUN_OVERWORLD_SCRIPT(checkfieldmove FIELD_MOVE_CUT, TRUE;);
    EXPECT_EQ(gSpecialVar_Result, PARTY_SIZE);

    FlagSet(FLAG_RECEIVED_HM_CUT);
    RUN_OVERWORLD_SCRIPT(checkfieldmove FIELD_MOVE_CUT, TRUE;);
    EXPECT_EQ(gSpecialVar_Result, 0);

    FlagClear(FLAG_BADGE01_GET);
    FlagClear(FLAG_RECEIVED_HM_CUT);
}

TEST("Every Emerald HM field move requires both its badge and received flag")
{
    static const struct
    {
        enum FieldMove fieldMove;
        u16 badgeFlag;
        u16 receivedFlag;
    } sFieldHms[] =
    {
        { FIELD_MOVE_CUT,        FLAG_BADGE01_GET, FLAG_RECEIVED_HM_CUT },
        { FIELD_MOVE_FLASH,      FLAG_BADGE02_GET, FLAG_RECEIVED_HM_FLASH },
        { FIELD_MOVE_ROCK_SMASH, FLAG_BADGE03_GET, FLAG_RECEIVED_HM_ROCK_SMASH },
        { FIELD_MOVE_STRENGTH,   FLAG_BADGE04_GET, FLAG_RECEIVED_HM_STRENGTH },
        { FIELD_MOVE_SURF,       FLAG_BADGE05_GET, FLAG_RECEIVED_HM_SURF },
        { FIELD_MOVE_FLY,        FLAG_BADGE06_GET, FLAG_RECEIVED_HM_FLY },
        { FIELD_MOVE_DIVE,       FLAG_BADGE07_GET, FLAG_RECEIVED_HM_DIVE },
        { FIELD_MOVE_WATERFALL,  FLAG_BADGE08_GET, FLAG_RECEIVED_HM_WATERFALL },
    };

    for (u32 i = 0; i < ARRAY_COUNT(sFieldHms); i++)
    {
        FlagClear(sFieldHms[i].badgeFlag);
        FlagClear(sFieldHms[i].receivedFlag);
        EXPECT(!IsFieldMoveUnlocked(sFieldHms[i].fieldMove));

        FlagSet(sFieldHms[i].badgeFlag);
        EXPECT(!IsFieldMoveUnlocked(sFieldHms[i].fieldMove));

        FlagSet(sFieldHms[i].receivedFlag);
        EXPECT(IsFieldMoveUnlocked(sFieldHms[i].fieldMove));

        FlagClear(sFieldHms[i].badgeFlag);
        EXPECT(!IsFieldMoveUnlocked(sFieldHms[i].fieldMove));

        FlagClear(sFieldHms[i].receivedFlag);
    }
}

TEST("Script_HasNoEffect control flow")
{
    const u8 *script = OVERWORLD_SCRIPT(
        nop;
        nop1;
        checkflag FLAG_TEMP_1;
        checktrainerflag 0;
        compare VAR_TEMP_0, 0;
        goto_if_eq GoneTo;
      GoneTo:
        call Sub;
        call_if_eq Sub;
        end;

      Sub:
        goto SubRet;
      SubRet:
        return;
    );
    EXPECT(Script_HasNoEffect(script));
}

TEST("Script_HasNoEffect variables")
{
    // Writes to special variables are not considered player-visible
    // because their values are indeterminate if the player has control.
    const u8 *writeSpecial = OVERWORLD_SCRIPT(
        setvar VAR_0x8000, 1;
        addvar VAR_0x8000, 1;
        subvar VAR_0x8000, VAR_TEMP_0;
        copyvar VAR_0x8000, VAR_TEMP_0;
        setorcopyvar VAR_0x8000, VAR_TEMP_0;
        specialvar VAR_RESULT, GetPlayerFacingDirection;
        getplayerxy VAR_0x8000, VAR_0x8001;
        getpartysize;
        checkitemspace ITEM_POTION, 1;
        checkitem ITEM_POTION, 1;
        checkitemtype ITEM_POTION;
        checkpcitem ITEM_POTION, 1;
        checkdecorspace DECOR_SNORLAX_DOLL;
        checkdecor DECOR_SNORLAX_DOLL;
        checkfieldmove 0, TRUE; // doesn't work with enums currently
        random 2;
        checkmoney 5000;
        getpokenewsactive POKENEWS_LILYCOVE;
        checkplayergender;
        checkcoins VAR_RESULT;
        checkmodernfatefulencounter 0;
        end;
    );

    // Writes to other variables are considered player-visible because
    // their values are preserved even while the player has control.
    const u8 *setVariable = OVERWORLD_SCRIPT(
        setvar VAR_TEMP_0, 1;
        end;
    );

    const u8 *addVariable = OVERWORLD_SCRIPT(
        addvar VAR_TEMP_0, 1;
        end;
    );

    const u8 *subVariable = OVERWORLD_SCRIPT(
        subvar VAR_TEMP_0, 1;
        end;
    );

    const u8 *copyVariable = OVERWORLD_SCRIPT(
        copyvar VAR_TEMP_0, VAR_RESULT;
        end;
    );

    const u8 *setorcopyVariable = OVERWORLD_SCRIPT(
        setorcopyvar VAR_TEMP_0, VAR_RESULT;
        end;
    );

    const u8 *specialvarVariable = OVERWORLD_SCRIPT(
        specialvar VAR_TEMP_0, GetPlayerFacingDirection;
        end;
    );

    const u8 *getPlayerXYVariable1 = OVERWORLD_SCRIPT(
        getplayerxy VAR_TEMP_0, VAR_RESULT;
        end;
    );

    const u8 *getPlayerXYVariable2 = OVERWORLD_SCRIPT(
        getplayerxy VAR_RESULT, VAR_TEMP_0;
        end;
    );

    const u8 *checkCoinsVariable = OVERWORLD_SCRIPT(
        checkcoins VAR_TEMP_0;
        end;
    );

    EXPECT(Script_HasNoEffect(writeSpecial));
    EXPECT(!Script_HasNoEffect(setVariable));
    EXPECT(!Script_HasNoEffect(addVariable));
    EXPECT(!Script_HasNoEffect(subVariable));
    EXPECT(!Script_HasNoEffect(copyVariable));
    EXPECT(!Script_HasNoEffect(setorcopyVariable));
    EXPECT(!Script_HasNoEffect(specialvarVariable));
    EXPECT(!Script_HasNoEffect(getPlayerXYVariable1));
    EXPECT(!Script_HasNoEffect(getPlayerXYVariable2));
    EXPECT(!Script_HasNoEffect(checkCoinsVariable));
}
