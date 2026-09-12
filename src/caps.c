#include "global.h"
#include "battle_setup.h"
#include "battle.h"
#include "event_data.h"
#include "caps.h"
#include "field_message_box.h"
#include "pokemon.h"
#include "fork_run.h"
#include "string_util.h"
#include "constants/opponents.h"

struct LevelCapMilestone
{
    u8 levelCap;
    const u16 *trainerIds;
};

static EWRAM_DATA u8 sQueuedLevelCapIncrease = 0;
static const u8 sText_LevelCapRaised[] = _("LEVEL CAP RAISED!\nYour POKéMON can now reach\nLv. {STR_VAR_1}.");

static bool32 HasAnyTrainerBeenFought(const u16 *trainerIds)
{
    for (u32 i = 0; trainerIds[i] != TRAINER_NONE; i++)
    {
        if (HasTrainerBeenFought(trainerIds[i]))
            return TRUE;
    }

    return FALSE;
}

static const u16 sCapRustboroRival[] =
{
    TRAINER_BRENDAN_RUSTBORO_MUDKIP,
    TRAINER_BRENDAN_RUSTBORO_TREECKO,
    TRAINER_BRENDAN_RUSTBORO_TORCHIC,
    TRAINER_MAY_RUSTBORO_MUDKIP,
    TRAINER_MAY_RUSTBORO_TREECKO,
    TRAINER_MAY_RUSTBORO_TORCHIC,
    TRAINER_NONE,
};

static const u16 sCapRoute110Rival[] =
{
    TRAINER_BRENDAN_ROUTE_110_MUDKIP,
    TRAINER_BRENDAN_ROUTE_110_TREECKO,
    TRAINER_BRENDAN_ROUTE_110_TORCHIC,
    TRAINER_MAY_ROUTE_110_MUDKIP,
    TRAINER_MAY_ROUTE_110_TREECKO,
    TRAINER_MAY_ROUTE_110_TORCHIC,
    TRAINER_NONE,
};

static const u16 sCapRoute119Rival[] =
{
    TRAINER_BRENDAN_ROUTE_119_MUDKIP,
    TRAINER_BRENDAN_ROUTE_119_TREECKO,
    TRAINER_BRENDAN_ROUTE_119_TORCHIC,
    TRAINER_MAY_ROUTE_119_MUDKIP,
    TRAINER_MAY_ROUTE_119_TREECKO,
    TRAINER_MAY_ROUTE_119_TORCHIC,
    TRAINER_NONE,
};

static const u16 sCapLilycoveRival[] =
{
    TRAINER_BRENDAN_LILYCOVE_MUDKIP,
    TRAINER_BRENDAN_LILYCOVE_TREECKO,
    TRAINER_BRENDAN_LILYCOVE_TORCHIC,
    TRAINER_MAY_LILYCOVE_MUDKIP,
    TRAINER_MAY_LILYCOVE_TREECKO,
    TRAINER_MAY_LILYCOVE_TORCHIC,
    TRAINER_NONE,
};

static const u16 sCapRoxanne[] = { TRAINER_ROXANNE_1, TRAINER_NONE };
static const u16 sCapBrawly[] = { TRAINER_BRAWLY_1, TRAINER_NONE };
static const u16 sCapWattson[] = { TRAINER_WATTSON_1, TRAINER_NONE };
static const u16 sCapMaxieMtChimney[] = { TRAINER_MAXIE_MT_CHIMNEY, TRAINER_NONE };
static const u16 sCapFlannery[] = { TRAINER_FLANNERY_1, TRAINER_NONE };
static const u16 sCapNorman[] = { TRAINER_NORMAN_1, TRAINER_NONE };
static const u16 sCapShellyWeatherInstitute[] = { TRAINER_SHELLY_WEATHER_INSTITUTE, TRAINER_NONE };
static const u16 sCapWinona[] = { TRAINER_WINONA_1, TRAINER_NONE };
static const u16 sCapMaxieMagmaHideout[] = { TRAINER_MAXIE_MAGMA_HIDEOUT, TRAINER_NONE };
static const u16 sCapTateAndLiza[] = { TRAINER_TATE_AND_LIZA_1, TRAINER_NONE };
static const u16 sCapMaxieTabithaMossdeep[] = { TRAINER_MAXIE_MOSSDEEP, TRAINER_TABITHA_MOSSDEEP, TRAINER_NONE };
static const u16 sCapArchie[] = { TRAINER_ARCHIE, TRAINER_NONE };
static const u16 sCapJuan[] = { TRAINER_JUAN_1, TRAINER_NONE };
static const u16 sCapWallyVictoryRoad[] = { TRAINER_WALLY_VR_1, TRAINER_NONE };
static const u16 sCapSteven[] = { TRAINER_STEVEN, TRAINER_NONE };

static const struct LevelCapMilestone sLevelCapMilestones[] =
{
    { 15, sCapRoxanne },
    { 16, sCapRustboroRival },
    { 19, sCapBrawly },
    { 20, sCapRoute110Rival },
    { 24, sCapWattson },
    { 25, sCapMaxieMtChimney },
    { 29, sCapFlannery },
    { 31, sCapNorman },
    { 32, sCapShellyWeatherInstitute },
    { 32, sCapRoute119Rival },
    { 33, sCapWinona },
    { 34, sCapLilycoveRival },
    { 39, sCapMaxieMagmaHideout },
    { 42, sCapTateAndLiza },
    { 44, sCapMaxieTabithaMossdeep },
    { 45, sCapArchie },
    { 46, sCapJuan },
    { 47, sCapWallyVictoryRoad },
    { 80, sCapSteven },
};


u32 GetCurrentLevelCap(void)
{
    u32 i;

    if (!ForkIsLevelCapEnabled())
        return MAX_LEVEL;

    if (B_LEVEL_CAP_TYPE == LEVEL_CAP_FLAG_LIST)
    {
        if (HasTrainerBeenFought(TRAINER_STEVEN))
            return MAX_LEVEL;

        for (i = 0; i < ARRAY_COUNT(sLevelCapMilestones); i++)
        {
            if (!HasAnyTrainerBeenFought(sLevelCapMilestones[i].trainerIds))
                return sLevelCapMilestones[i].levelCap;
        }

        return sLevelCapMilestones[ARRAY_COUNT(sLevelCapMilestones) - 1].levelCap;
    }
    else if (B_LEVEL_CAP_TYPE == LEVEL_CAP_VARIABLE)
    {
        return VarGet(B_LEVEL_CAP_VARIABLE);
    }

    return MAX_LEVEL;
}

bool8 QueueLevelCapIncreaseMessage(u32 previousCap)
{
    u32 newCap = GetCurrentLevelCap();

    if (!ForkIsLevelCapEnabled() || newCap <= previousCap)
        return FALSE;

    sQueuedLevelCapIncrease = newCap;
    return TRUE;
}

u32 ConsumeQueuedLevelCapIncrease(void)
{
    u32 cap = sQueuedLevelCapIncrease;
    sQueuedLevelCapIncrease = 0;
    return cap;
}

void ShowQueuedLevelCapIncreaseMessage(void)
{
    u32 cap = ConsumeQueuedLevelCapIncrease();

    gSpecialVar_Result = FALSE;
    if (cap == 0)
        return;

    ConvertIntToDecimalStringN(gStringVar1, cap, STR_CONV_MODE_LEFT_ALIGN, 3);
    if (ShowFieldMessage(sText_LevelCapRaised))
        gSpecialVar_Result = TRUE;
}

u32 GetSoftLevelCapExpValue(u32 level, u32 expValue)
{
    static const u32 sExpScalingDown[5] = { 4, 8, 16, 32, 64 };
    static const u32 sExpScalingUp[5]   = { 16, 8, 4, 2, 1 };

    u32 levelDifference;
    u32 currentLevelCap = GetCurrentLevelCap();

    if (B_EXP_CAP_TYPE == EXP_CAP_NONE)
        return expValue;

    if (level < currentLevelCap)
    {
        if (B_LEVEL_CAP_EXP_UP)
        {
            levelDifference = currentLevelCap - level;
            if (levelDifference > ARRAY_COUNT(sExpScalingUp) - 1)
                return expValue + (expValue / sExpScalingUp[ARRAY_COUNT(sExpScalingUp) - 1]);
            else
                return expValue + (expValue / sExpScalingUp[levelDifference]);
        }
        else
        {
            return expValue;
        }
    }
    else if (B_EXP_CAP_TYPE == EXP_CAP_HARD)
    {
        return 0;
    }
    else if (B_EXP_CAP_TYPE == EXP_CAP_SOFT)
    {
        levelDifference = level - currentLevelCap;
        if (levelDifference > ARRAY_COUNT(sExpScalingDown) - 1)
            return expValue / sExpScalingDown[ARRAY_COUNT(sExpScalingDown) - 1];
        else
            return expValue / sExpScalingDown[levelDifference];
    }
    else
    {
       return expValue;
    }
}

u32 GetCurrentEVCap(void)
{
    static const u16 sEvCapFlagMap[][2] = {
        // Define EV caps for each milestone
        {FLAG_BADGE01_GET, MAX_TOTAL_EVS *  1 / 17},
        {FLAG_BADGE02_GET, MAX_TOTAL_EVS *  3 / 17},
        {FLAG_BADGE03_GET, MAX_TOTAL_EVS *  5 / 17},
        {FLAG_BADGE04_GET, MAX_TOTAL_EVS *  7 / 17},
        {FLAG_BADGE05_GET, MAX_TOTAL_EVS *  9 / 17},
        {FLAG_BADGE06_GET, MAX_TOTAL_EVS * 11 / 17},
        {FLAG_BADGE07_GET, MAX_TOTAL_EVS * 13 / 17},
        {FLAG_BADGE08_GET, MAX_TOTAL_EVS * 15 / 17},
        {FLAG_IS_CHAMPION, MAX_TOTAL_EVS},
    };

    if (B_EV_CAP_TYPE == EV_CAP_FLAG_LIST)
    {
        for (u32 evCap = 0; evCap < ARRAY_COUNT(sEvCapFlagMap); evCap++)
        {
            if (!FlagGet(sEvCapFlagMap[evCap][0]))
                return sEvCapFlagMap[evCap][1];
        }
    }
    else if (B_EV_CAP_TYPE == EV_CAP_VARIABLE)
    {
        return VarGet(B_EV_CAP_VARIABLE);
    }
    else if (B_EV_CAP_TYPE == EV_CAP_NO_GAIN)
    {
        return 0;
    }

    return MAX_TOTAL_EVS;
}
