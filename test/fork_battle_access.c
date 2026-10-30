#include "global.h"
#include "battle.h"
#include "battle_setup.h"
#include "debug.h"
#include "event_data.h"
#include "fieldmap.h"
#include "fork_run.h"
#include "gym_leader_rematch.h"
#include "item.h"
#include "match_call.h"
#include "new_game.h"
#include "random.h"
#include "pokenav.h"
#include "script.h"
#include "string_util.h"
#include "vs_seeker.h"
#include "test/test.h"
#include "constants/maps.h"
#include "constants/characters.h"
#include "constants/opponents.h"

extern bool32 Test_BlockBattleFacilityWarp(s32 warpEventId);
extern bool32 Test_CanFlyToBattleFrontier(void);

TEST("Disabled rematches never become ready through walking battles or calls")
{
    NewGameInitData();
    SeedRng(1);
    StringCopy(gSaveBlock2Ptr->playerName, COMPOUND_STRING("TEST"));
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_PIKACHU, 20, 0, OTID_STRUCT_PLAYER_ID);
    gPartiesCount[B_TRAINER_PLAYER] = 1;
    memset(gSaveBlock1Ptr->trainerRematches, 0, sizeof(gSaveBlock1Ptr->trainerRematches));
    gSaveBlock1Ptr->trainerRematchStepCounter = 254;
    FlagSet(FLAG_SYS_GAME_CLEAR);
    for (u32 i = FLAG_BADGE01_GET; i <= FLAG_BADGE08_GET; i++) FlagSet(i);
    for (u32 i = 0; i < 512; i++)
    {
        IncrementRematchStepCounter();
        UpdateVsSeekerStepCounter();
        UpdateGymLeaderRematch();
    }
    EXPECT_EQ(gSaveBlock1Ptr->trainerRematchStepCounter, 254);
    EXPECT_EQ(GetCurrentGymLeaderRematchLevel(), 0);
    UpdateRematchIfDefeated(REMATCH_ROSE);
    EXPECT_EQ(gSaveBlock1Ptr->trainerRematches[REMATCH_ROSE], 0);
    gSaveBlock1Ptr->trainerRematches[REMATCH_ROSE] = 1;
    EXPECT(!ShouldTryRematchBattleForTrainerId(TRAINER_ROSE_1));
    EXPECT(!DoesSomeoneWantRematchIn(MAP_GROUP(MAP_ROUTE118), MAP_NUM(MAP_ROUTE118)));
    EXPECT(!ShouldDrawRematchPokeballIcon(0)); // Also safe without an allocated PokeNav.
    gStringVar1[0] = gStringVar2[0] = gStringVar3[0] = EOS;
    bool8 wasDebugBattle = gIsDebugBattle;
    gIsDebugBattle = FALSE;
    // The battle-test trainer table omits normal NPC names. Use a contact whose
    // display name is supplied by Match Call itself and select its personal topic.
    for (u32 seed = 0; seed < 100; seed++)
    {
        SeedRng(seed);
        rng_value_t before = gRngValue;
        if (Random() % 3 == 0)
        {
            gRngValue = before;
            break;
        }
    }
    EXPECT(!SelectMatchCallMessage(TRAINER_LILA_AND_ROY_1, gStringVar4));
    gIsDebugBattle = wasDebugBattle;
    // Defeat tracking for ordinary first-time trainers is unchanged.
    ClearTrainerFlag(TRAINER_ROSE_1);
    EXPECT(!HasTrainerBeenFought(TRAINER_ROSE_1));
    SetTrainerFlag(TRAINER_ROSE_1);
    EXPECT(HasTrainerBeenFought(TRAINER_ROSE_1));
}

TEST("Disabled rematches keep an injected Vs Seeker inactive and uncharged")
{
    SetBagItemsPointers(); ClearBag();
    EXPECT(AddBagItem(ITEM_VS_SEEKER, 1));
    gSaveBlock1Ptr->trainerRematchStepCounter = 0;
    EXPECT(!IsVsSeekerEnabled());
    for (u32 i = 0; i < 200; i++) EXPECT(!UpdateVsSeekerStepCounter());
    EXPECT_EQ(gSaveBlock1Ptr->trainerRematchStepCounter, 0);
    EXPECT_EQ(GetRematchTrainerIdVSSeeker(TRAINER_ROSE_1), 0);
}

TEST("Disabled battle facilities block entrances and retain normal maps")
{
    const u16 closed[] = {MAP_BATTLE_FRONTIER_OUTSIDE_WEST, MAP_BATTLE_FRONTIER_OUTSIDE_EAST,
        MAP_BATTLE_FRONTIER_BATTLE_TOWER_LOBBY, MAP_SLATEPORT_CITY_BATTLE_TENT_LOBBY,
        MAP_FALLARBOR_TOWN_BATTLE_TENT_LOBBY, MAP_VERDANTURF_TOWN_BATTLE_TENT_LOBBY,
        MAP_TRAINER_HILL_ENTRANCE, MAP_TRAINER_TOWER_LOBBY, MAP_SEVEN_ISLAND_TRAINER_TOWER,
        MAP_MOSSDEEP_CITY_GAME_CORNER_B1F};
    const struct MapEvents *savedEvents = gMapHeader.events;
    for (u32 i = 0; i < ARRAY_COUNT(closed); i++)
    {
        EXPECT(ForkIsBattleFacilityMap(MAP_GROUP(closed[i]), MAP_NUM(closed[i])));
        const struct WarpEvent warp = {.mapGroup = MAP_GROUP(closed[i]), .mapNum = MAP_NUM(closed[i])};
        const struct MapEvents events = {.warpCount = 1, .warps = &warp};
        gMapHeader.events = &events;
        ScriptContext_Init();
        EXPECT(Test_BlockBattleFacilityWarp(0));
        EXPECT(ScriptContext_IsEnabled());
        EXPECT(!Test_BlockBattleFacilityWarp(-1));
        EXPECT(!Test_BlockBattleFacilityWarp(1));
    }
    const u16 open[] = {MAP_ROUTE111, MAP_LILYCOVE_CITY, MAP_ROUTE117,
        MAP_MOSSDEEP_CITY_GAME_CORNER_1F, MAP_SOOTOPOLIS_CITY_GYM_1F, MAP_EVER_GRANDE_CITY,
        MAP_EVER_GRANDE_CITY_CHAMPIONS_ROOM, MAP_POKEMON_LEAGUE_CHAMPIONS_ROOM, MAP_LILYCOVE_CITY_CONTEST_LOBBY};
    for (u32 i = 0; i < ARRAY_COUNT(open); i++)
    {
        EXPECT(!ForkIsBattleFacilityMap(MAP_GROUP(open[i]), MAP_NUM(open[i])));
        const struct WarpEvent warp = {.mapGroup = MAP_GROUP(open[i]), .mapNum = MAP_NUM(open[i])};
        const struct MapEvents events = {.warpCount = 1, .warps = &warp};
        gMapHeader.events = &events;
        ScriptContext_Init();
        EXPECT(!Test_BlockBattleFacilityWarp(0));
        EXPECT(!ScriptContext_IsEnabled());
    }
    gMapHeader.events = savedEvents;
    ScriptContext_Init();
    EXPECT(!ForkIsBattleFacilityMap(65535, 65535));
    FlagSet(FLAG_LANDMARK_BATTLE_FRONTIER);
    EXPECT(!Test_CanFlyToBattleFrontier());
}
