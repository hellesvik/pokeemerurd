#include "global.h"
#include "battle.h"
#include "battle_interface.h"
#include "battle_setup.h"
#include "caps.h"
#include "egg_hatch.h"
#include "event_data.h"
#include "fork_run.h"
#include "item_use.h"
#include "new_game.h"
#include "party_menu.h"
#include "item.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "safari_zone.h"
#include "test/overworld_script.h"
#include "test/test.h"
#include "constants/characters.h"
#include "constants/daycare.h"
#include "constants/flags.h"
#include "constants/global.h"
#include "constants/items.h"
#include "constants/move_relearner.h"
#include "constants/opponents.h"
#include "wild_encounter.h"

#include "field_player_avatar.h"

TEST("Overworld movement runs by default and walks while B is held")
{
    EXPECT_EQ(PlayerShouldRunWithHeldKeys(0), TRUE);
    EXPECT_EQ(PlayerShouldRunWithHeldKeys(DPAD_UP), TRUE);
    EXPECT_EQ(PlayerShouldRunWithHeldKeys(B_BUTTON), FALSE);
    EXPECT_EQ(PlayerShouldRunWithHeldKeys(DPAD_LEFT | B_BUTTON), FALSE);
}

TEST("HM moves can be replaced when teaching another move")
{
    EXPECT_EQ(IsMoveHM(MOVE_CUT), TRUE);
    EXPECT_EQ(CannotForgetMove(MOVE_CUT), FALSE);
    EXPECT_EQ(CannotForgetMove(MOVE_SURF), FALSE);
    EXPECT_EQ(CannotForgetMove(MOVE_TACKLE), FALSE);
}

static void ClearForkLevelCapTrainerFlags(void)
{
    static const u16 sLevelCapTrainerFlags[] =
    {
        TRAINER_MAY_ROUTE_103_TREECKO,
        TRAINER_ROXANNE_1,
        TRAINER_MAY_RUSTBORO_TREECKO,
        TRAINER_BRAWLY_1,
        TRAINER_WALLY_MAUVILLE,
        TRAINER_MAY_ROUTE_110_TREECKO,
        TRAINER_WATTSON_1,
        TRAINER_TABITHA_MT_CHIMNEY,
        TRAINER_MAXIE_MT_CHIMNEY,
        TRAINER_FLANNERY_1,
        TRAINER_NORMAN_1,
        TRAINER_MAY_ROUTE_119_TREECKO,
        TRAINER_SHELLY_WEATHER_INSTITUTE,
        TRAINER_WINONA_1,
        TRAINER_MAY_LILYCOVE_TREECKO,
        TRAINER_MAXIE_MAGMA_HIDEOUT,
        TRAINER_MATT,
        TRAINER_TATE_AND_LIZA_1,
        TRAINER_MAXIE_MOSSDEEP,
        TRAINER_SHELLY_SEAFLOOR_CAVERN,
        TRAINER_ARCHIE,
        TRAINER_JUAN_1,
        TRAINER_WALLY_VR_1,
        TRAINER_SIDNEY,
        TRAINER_PHOEBE,
        TRAINER_GLACIA,
        TRAINER_DRAKE,
        TRAINER_WALLACE,
        TRAINER_STEVEN,
    };

    for (u32 i = 0; i < ARRAY_COUNT(sLevelCapTrainerFlags); i++)
        ClearTrainerFlag(sLevelCapTrainerFlags[i]);
}

TEST("Nature independent from Hidden Nature")
{
    u32 i, j, nature = 0, hiddenNature = 0;
    struct Pokemon mon;
    for (i = 0; i < NUM_NATURES; i++)
    {
        for (j = 0; j < NUM_NATURES; j++)
        {
            PARAMETRIZE { nature = i; hiddenNature = j; }
        }
    }
    u32 species = SPECIES_WOBBUFFET;
    u32 personality = GetMonPersonality(species, MON_GENDER_RANDOM, nature, RANDOM_UNOWN_LETTER);
    CreateMon(&mon, species, 100, personality, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &hiddenNature);
    EXPECT_EQ(GetNature(&mon), nature);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HIDDEN_NATURE), hiddenNature);
}

TEST("Fork rules enable a hard story-battle level cap while leaving one-use Rare Candy uncapped")
{
    ClearForkLevelCapTrainerFlags();

    EXPECT_EQ(B_EXP_CAP_TYPE, EXP_CAP_HARD);
    EXPECT_EQ(B_LEVEL_CAP_TYPE, LEVEL_CAP_FLAG_LIST);
    EXPECT_EQ(B_RARE_CANDY_CAP, FALSE);
    EXPECT_EQ(GetCurrentLevelCap(), 15);

    SetTrainerFlag(TRAINER_MAY_ROUTE_103_TREECKO);
    EXPECT_EQ(GetCurrentLevelCap(), 15);

    SetTrainerFlag(TRAINER_ROXANNE_1);
    EXPECT_EQ(GetCurrentLevelCap(), 16);

    SetTrainerFlag(TRAINER_MAY_RUSTBORO_TREECKO);
    EXPECT_EQ(GetCurrentLevelCap(), 19);

    SetTrainerFlag(TRAINER_BRAWLY_1);
    EXPECT_EQ(GetCurrentLevelCap(), 20);

    SetTrainerFlag(TRAINER_MAY_ROUTE_110_TREECKO);
    EXPECT_EQ(GetCurrentLevelCap(), 24);

    SetTrainerFlag(TRAINER_WATTSON_1);
    EXPECT_EQ(GetCurrentLevelCap(), 25);

    SetTrainerFlag(TRAINER_MAXIE_MT_CHIMNEY);
    EXPECT_EQ(GetCurrentLevelCap(), 29);

    SetTrainerFlag(TRAINER_FLANNERY_1);
    EXPECT_EQ(GetCurrentLevelCap(), 31);

    SetTrainerFlag(TRAINER_NORMAN_1);
    EXPECT_EQ(GetCurrentLevelCap(), 31);

    SetTrainerFlag(TRAINER_MAY_ROUTE_119_TREECKO);
    EXPECT_EQ(GetCurrentLevelCap(), 33);

    SetTrainerFlag(TRAINER_WINONA_1);
    EXPECT_EQ(GetCurrentLevelCap(), 34);

    SetTrainerFlag(TRAINER_MAY_LILYCOVE_TREECKO);
    EXPECT_EQ(GetCurrentLevelCap(), 39);

    SetTrainerFlag(TRAINER_MAXIE_MAGMA_HIDEOUT);
    EXPECT_EQ(GetCurrentLevelCap(), 42);

    SetTrainerFlag(TRAINER_TATE_AND_LIZA_1);
    EXPECT_EQ(GetCurrentLevelCap(), 44);

    SetTrainerFlag(TRAINER_MAXIE_MOSSDEEP);
    EXPECT_EQ(GetCurrentLevelCap(), 45);

    SetTrainerFlag(TRAINER_ARCHIE);
    EXPECT_EQ(GetCurrentLevelCap(), 46);

    SetTrainerFlag(TRAINER_JUAN_1);
    EXPECT_EQ(GetCurrentLevelCap(), 47);

    SetTrainerFlag(TRAINER_WALLY_VR_1);
    EXPECT_EQ(GetCurrentLevelCap(), 80);

    SetTrainerFlag(TRAINER_WALLACE);
    EXPECT_EQ(GetCurrentLevelCap(), 80);

    SetTrainerFlag(TRAINER_STEVEN);
    EXPECT_EQ(GetCurrentLevelCap(), MAX_LEVEL);
}

TEST("Level cap notifications are queued only after a cap increase")
{
    u32 previousCap;

    ClearForkLevelCapTrainerFlags();
    ForkConfigureGameplayOptions(TRUE, TRUE, FORK_FAINT_RULE_WHITEOUT, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, GEN_3, TRUE, FALSE);

    previousCap = GetCurrentLevelCap();
    EXPECT_EQ(previousCap, 15);
    EXPECT_EQ(QueueLevelCapIncreaseMessage(previousCap), FALSE);

    SetTrainerFlag(TRAINER_ROXANNE_1);
    EXPECT_EQ(QueueLevelCapIncreaseMessage(previousCap), TRUE);
    EXPECT_EQ(ConsumeQueuedLevelCapIncrease(), 16);
    EXPECT_EQ(ConsumeQueuedLevelCapIncrease(), 0);

    previousCap = GetCurrentLevelCap();
    EXPECT_EQ(QueueLevelCapIncreaseMessage(previousCap), FALSE);
}

TEST("Fork gameplay options gate the catch limit and level cap")
{
    ForkConfigureGameplayOptions(FALSE, FALSE, FORK_FAINT_RULE_WHITEOUT, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, GEN_3, TRUE, FALSE);
    EXPECT_EQ(ForkIsCatchLimitEnabled(), FALSE);
    EXPECT_EQ(ForkIsLevelCapEnabled(), FALSE);
    EXPECT_EQ(GetCurrentLevelCap(), MAX_LEVEL);

    ForkConfigureGameplayOptions(TRUE, TRUE, FORK_FAINT_RULE_WHITEOUT, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, GEN_3, TRUE, FALSE);
    EXPECT_EQ(ForkIsCatchLimitEnabled(), TRUE);
    EXPECT_EQ(ForkIsLevelCapEnabled(), TRUE);
}

TEST("Fork rules disable EV gain and expose IV and EV values on the summary screen")
{
    EXPECT_EQ(B_EV_CAP_TYPE, EV_CAP_NO_GAIN);
    EXPECT_EQ(P_SUMMARY_SCREEN_IV_ONLY, FALSE);
    EXPECT_EQ(P_SUMMARY_SCREEN_IV_EV_VALUES, TRUE);
}

TEST("Fork rules only allow catching the first legal wild encounter")
{
    ZeroEnemyPartyMons();
    ZeroPlayerPartyMons();
    ResetPokemonStorageSystem();
    ResetPokedex();
    ForkResetAreaEncounterState();
    ForkInvalidateOwnedFamilyCache();
    FlagSet(FLAG_ADVENTURE_STARTED);
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROUTE101);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROUTE101);
    gBattleTypeFlags = 0;
    FlagClear(WE_FLAG_NO_CATCHING);

    ForkPrepareWildEncounter();
    CreateMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_POOCHYENA, 3, 0, OTID_STRUCT_PLAYER_ID);
    ForkResolveWildEncounter();
    EXPECT_EQ(CanThrowBall(), TRUE);
    EXPECT_EQ(CanThrowLastUsedBall(), TRUE);
    ForkFinalizeWildEncounter();

    ZeroEnemyPartyMons();
    ForkPrepareWildEncounter();
    CreateMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_ZIGZAGOON, 3, 0, OTID_STRUCT_PLAYER_ID);
    ForkResolveWildEncounter();
    EXPECT_EQ(CanThrowBall(), FALSE);
    EXPECT_EQ(CanThrowLastUsedBall(), FALSE);

    ForkResetAreaEncounterState();
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_POOCHYENA), FLAG_SET_CAUGHT);
    ZeroEnemyPartyMons();
    ForkPrepareWildEncounter();
    CreateMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_MIGHTYENA, 3, 0, OTID_STRUCT_PLAYER_ID);
    ForkResolveWildEncounter();
    EXPECT_EQ(CanThrowBall(), FALSE);
    EXPECT_EQ(CanThrowLastUsedBall(), FALSE);
}

TEST("Safari Zone spends its shared limit on the first encounter even when it is a dupe")
{
    ZeroEnemyPartyMons();
    ZeroPlayerPartyMons();
    ResetPokemonStorageSystem();
    ResetPokedex();
    ForkResetAreaEncounterState();
    ForkInvalidateOwnedFamilyCache();
    ForkConfigureGameplayOptions(TRUE, FALSE, FORK_FAINT_RULE_OFF, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, GEN_3, TRUE, FALSE);
    FlagSet(FLAG_ADVENTURE_STARTED);
    SetSafariZoneFlag();
    gMapHeader.regionMapSectionId = MAPSEC_SAFARI_ZONE;

    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_POOCHYENA, 3, 0, OTID_STRUCT_PLAYER_ID);
    ForkInvalidateOwnedFamilyCache();
    EXPECT_EQ(ForkPlayerOwnsSpeciesFamily(SPECIES_MIGHTYENA), TRUE);
    ForkPrepareWildEncounter();
    CreateMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_MIGHTYENA, 3, 0x12345678, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_IS_SHINY), FALSE);
    ForkResolveWildEncounter();
    EXPECT_EQ(ForkCanCatchCurrentEncounter(), TRUE);
    ForkFinalizeWildEncounter();

    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_SAFARI_ZONE_NORTH);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_SAFARI_ZONE_NORTH);
    ZeroEnemyPartyMons();
    ForkPrepareWildEncounter();
    CreateMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_PIKACHU, 3, 0, OTID_STRUCT_PLAYER_ID);
    ForkResolveWildEncounter();
    EXPECT_EQ(ForkCanCatchCurrentEncounter(), FALSE);

    ForkFinalizeWildEncounter();
    ResetSafariZoneFlag();
}

TEST("Fork rules do not spend an area encounter before Professor Birch gives Poké Balls")
{
    ZeroEnemyPartyMons();
    ZeroPlayerPartyMons();
    ResetPokemonStorageSystem();
    ResetPokedex();
    ForkResetAreaEncounterState();
    ForkInvalidateOwnedFamilyCache();
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROUTE101);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROUTE101);
    gMapHeader.regionMapSectionId = MAPSEC_ROUTE_101;
    FlagClear(FLAG_ADVENTURE_STARTED);
    EXPECT_EQ(ForkPlayerOwnsSpeciesFamily(SPECIES_ABRA), FALSE);

    ForkPrepareWildEncounter();
    CreateMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_ABRA, 3, 0, OTID_STRUCT_PLAYER_ID);
    ForkResolveWildEncounter();
    ForkFinalizeWildEncounter();

    EXPECT_EQ(ForkIsAreaEncounterSpent(MAPSEC_ROUTE_101), FALSE);
}

TEST("Fork area encounter rule starts after Professor Birch gives Poké Balls")
{
    FlagClear(FLAG_ADVENTURE_STARTED);
    EXPECT_EQ(ForkIsAreaEncounterRuleActive(), FALSE);
    FlagSet(FLAG_ADVENTURE_STARTED);
    EXPECT_EQ(ForkIsAreaEncounterRuleActive(), TRUE);
}

TEST("Fork faint penalties do not start before Professor Birch gives the Pokédex")
{
    ForkConfigureGameplayOptions(TRUE, FALSE, FORK_FAINT_RULE_ON_FAINT, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, GEN_3, TRUE, FALSE);
    FlagClear(FLAG_ADVENTURE_STARTED);
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_MUDKIP, 5, 0, OTID_STRUCT_PLAYER_ID);

    ForkApplySoftNuzlockeFaintPenalty(0);

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SOFT_NUZLOCKE), FALSE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 5);

    ForkConfigureGameplayOptions(FALSE, FALSE, FORK_FAINT_RULE_WHITEOUT, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, GEN_3, TRUE, FALSE);
    ForkApplySoftNuzlockeWhiteOutPenalty();

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SOFT_NUZLOCKE), FALSE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 5);
}

TEST("Fork faint penalty permanently faints without changing level")
{
    ForkConfigureGameplayOptions(TRUE, FALSE, FORK_FAINT_RULE_ON_FAINT, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, GEN_3, TRUE, FALSE);
    FlagSet(FLAG_ADVENTURE_STARTED);
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_MUDKIP, 20, 0, OTID_STRUCT_PLAYER_ID);

    ForkApplySoftNuzlockeFaintPenalty(0);

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SOFT_NUZLOCKE), TRUE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 20);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 0);

    HealPokemon(&gParties[B_TRAINER_PLAYER][0]);

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 0);
}

TEST("Fork faint penalty works when the catch limit is disabled")
{
    ForkConfigureGameplayOptions(FALSE, FALSE, FORK_FAINT_RULE_ON_FAINT, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, GEN_3, TRUE, FALSE);
    FlagSet(FLAG_ADVENTURE_STARTED);
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_EEVEE, 15, 0, OTID_STRUCT_PLAYER_ID);

    ForkApplySoftNuzlockeFaintPenalty(0);

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SOFT_NUZLOCKE), TRUE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 0);
}

TEST("Fork rules track one encounter per named area")
{
    ForkResetAreaEncounterState();
    EXPECT_EQ(ForkIsAreaEncounterSpent(MAPSEC_ROUTE_101), FALSE);
    ForkSetAreaEncounterSpent(MAPSEC_ROUTE_101);
    EXPECT_EQ(ForkIsAreaEncounterSpent(MAPSEC_ROUTE_101), TRUE);
    EXPECT_EQ(ForkIsAreaEncounterSpent(MAPSEC_ROUTE_102), FALSE);
}

TEST("Fork rules can resolve a wild encounter after battle setup creates the opponent mon")
{
    ZeroEnemyPartyMons();
    ZeroPlayerPartyMons();
    ResetPokemonStorageSystem();
    ResetPokedex();
    ForkResetAreaEncounterState();
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROUTE101);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROUTE101);

    ForkPrepareWildEncounter();
    CreateMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_POOCHYENA, 3, 0, OTID_STRUCT_PLAYER_ID);
    ForkResolveWildEncounter();

    EXPECT_EQ(ForkCanCatchCurrentEncounter(), TRUE);
    EXPECT_EQ(ForkShouldShowFirstEncounterIndicator(), TRUE);
    ForkFinalizeWildEncounter();
    EXPECT_EQ(ForkIsAreaEncounterSpent(MAPSEC_ROUTE_101), TRUE);

    ZeroEnemyPartyMons();
    ForkPrepareWildEncounter();
    CreateMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_ZIGZAGOON, 3, 0, OTID_STRUCT_PLAYER_ID);
    ForkResolveWildEncounter();
    EXPECT_EQ(ForkCanCatchCurrentEncounter(), FALSE);
    EXPECT_EQ(ForkShouldShowFirstEncounterIndicator(), FALSE);
    EXPECT_EQ(ForkShouldRejectSelectedBall(ITEM_POKE_BALL), TRUE);
    EXPECT_EQ(ForkShouldRejectSelectedBall(ITEM_POTION), FALSE);
}

TEST("Fork rules prepare an encounter before creating its wild Pokémon")
{
    ZeroEnemyPartyMons();
    ZeroPlayerPartyMons();
    ResetPokemonStorageSystem();
    ResetPokedex();
    ForkResetAreaEncounterState();
    ForkInvalidateOwnedFamilyCache();
    FlagSet(FLAG_ADVENTURE_STARTED);
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROUTE101);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROUTE101);

    CreateWildMon(SPECIES_ABRA, 3);
    ForkFinalizeWildEncounter();

    EXPECT_EQ(ForkIsAreaEncounterSpent(MAPSEC_ROUTE_101), TRUE);
}

TEST("Fork rules prefer the first-encounter indicator over the caught icon tile")
{
    EXPECT_EQ(BattleInterface_GetCaughtIndicatorType(TRUE, TRUE, TRUE), BATTLE_CAUGHT_INDICATOR_FIRST_ENCOUNTER);
    EXPECT_EQ(BattleInterface_GetCaughtIndicatorType(FALSE, TRUE, TRUE), BATTLE_CAUGHT_INDICATOR_CAUGHT);
    EXPECT_EQ(BattleInterface_GetCaughtIndicatorType(FALSE, TRUE, FALSE), BATTLE_CAUGHT_INDICATOR_NONE);
    EXPECT_EQ(BattleInterface_GetCaughtIndicatorType(FALSE, FALSE, TRUE), BATTLE_CAUGHT_INDICATOR_NONE);
}

TEST("Fork rules treat owned evolutionary families as dupes from party, storage, daycare, and pokedex")
{
    struct Pokemon mon;
    ZeroPlayerPartyMons();
    ResetPokemonStorageSystem();
    ZeroBoxMonData(&gSaveBlock1Ptr->daycare.mons[0].mon);
    ZeroBoxMonData(&gSaveBlock1Ptr->daycare.mons[1].mon);
    ResetPokedex();

    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_BULBASAUR, 5, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(ForkPlayerOwnsSpeciesFamily(SPECIES_VENUSAUR), TRUE);

    ZeroPlayerPartyMons();
    CreateMon(&mon, SPECIES_CHARMANDER, 5, 0, OTID_STRUCT_PLAYER_ID);
    CopyMon(GetBoxedMonPtr(0, 0), &mon.box, sizeof(mon.box));
    EXPECT_EQ(ForkPlayerOwnsSpeciesFamily(SPECIES_CHARIZARD), TRUE);

    ZeroBoxMonData(GetBoxedMonPtr(0, 0));
    CreateMon(&mon, SPECIES_SQUIRTLE, 5, 0, OTID_STRUCT_PLAYER_ID);
    CopyMon(&gSaveBlock1Ptr->daycare.mons[0].mon, &mon.box, sizeof(mon.box));
    EXPECT_EQ(ForkPlayerOwnsSpeciesFamily(SPECIES_BLASTOISE), TRUE);

    ZeroBoxMonData(&gSaveBlock1Ptr->daycare.mons[0].mon);
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_CATERPIE), FLAG_SET_CAUGHT);
    EXPECT_EQ(ForkPlayerOwnsSpeciesFamily(SPECIES_BUTTERFREE), TRUE);
}

TEST("Fork rules mark soft nuzlocke mons and stop further level gains")
{
    struct Pokemon mon;
    u32 softNuzlocke = TRUE;
    u32 exp;

    CreateMon(&mon, SPECIES_WOBBUFFET, 10, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_SOFT_NUZLOCKE, &softNuzlocke);
    exp = gExperienceTables[gSpeciesInfo[SPECIES_WOBBUFFET].growthRate][11];
    SetMonData(&mon, MON_DATA_EXP, &exp);

    EXPECT_EQ(ForkIsSoftNuzlockeMon(&mon), TRUE);
    EXPECT_EQ(TryIncrementMonLevel(&mon), FALSE);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_LEVEL), 10);
}

TEST("Fork rules zero EVs on player-owned mons added to party and storage")
{
    struct Pokemon mon;
    u8 ev = 42;

    ZeroPlayerPartyMons();
    ResetPokemonStorageSystem();
    CreateMon(&mon, SPECIES_WOBBUFFET, 10, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_ATK_EV, &ev);
    SetMonData(&mon, MON_DATA_SPEED_EV, &ev);

    EXPECT_EQ(GiveScriptedMonToPlayer(&mon, PARTY_SIZE), MON_GIVEN_TO_PARTY);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ATK_EV), 0);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPEED_EV), 0);

    ZeroPlayerPartyMons();
    for (u32 i = 0; i < PARTY_SIZE; i++)
        CreateMon(&gParties[B_TRAINER_PLAYER][i], SPECIES_POOCHYENA + i, 5, 0, OTID_STRUCT_PLAYER_ID);

    CreateMon(&mon, SPECIES_WOBBUFFET, 10, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_ATK_EV, &ev);
    EXPECT_EQ(CopyMonToPC(&mon), MON_GIVEN_TO_PC);
    EXPECT_EQ(GetBoxMonData(GetBoxedMonPtr(0, 0), MON_DATA_ATK_EV), 0);
}

TEST("Test mode starts with Emerald key items and fork test items")
{
    NewGameInitData();
    ForkEnsureKeyItemsPresent();

    EXPECT_EQ(CheckBagHasItem(ITEM_INFINITE_RARE_CANDY, 1), TRUE);
    EXPECT_EQ(CheckBagHasItem(ITEM_INFINITE_REPEL, 1), TRUE);
    EXPECT_EQ(CheckBagHasItem(ITEM_MACH_BIKE, 1), TRUE);
    EXPECT_EQ(CheckBagHasItem(ITEM_ACRO_BIKE, 1), TRUE);
    EXPECT_EQ(CheckBagHasItem(ITEM_OLD_ROD, 1), TRUE);
    EXPECT_EQ(CheckBagHasItem(ITEM_GOOD_ROD, 1), TRUE);
    EXPECT_EQ(CheckBagHasItem(ITEM_SUPER_ROD, 1), TRUE);
    EXPECT_EQ(CheckBagHasItem(ITEM_GO_GOGGLES, 1), TRUE);
    EXPECT_EQ(CheckBagHasItem(ITEM_DEVON_SCOPE, 1), TRUE);
    EXPECT_EQ(CheckBagHasItem(ITEM_MAGMA_EMBLEM, 1), TRUE);
    EXPECT_EQ(CheckBagHasItem(ITEM_Z_POWER_RING, 1), FALSE);
    EXPECT_EQ(CheckBagHasItem(ITEM_DYNAMAX_BAND, 1), FALSE);
    EXPECT_EQ(CheckBagHasItem(ITEM_SILPH_SCOPE, 1), FALSE);
}

TEST("Heart Charm is a reusable key item that heals the party")
{
    NewGameInitData();

    EXPECT_EQ(CheckBagHasItem(ITEM_HEART_CHARM, 1), TRUE);
    EXPECT_EQ(GetItemPocket(ITEM_HEART_CHARM), POCKET_KEY_ITEMS);
    EXPECT_EQ(GetItemImportance(ITEM_HEART_CHARM), TRUE);
    EXPECT_EQ(GetItemConsumability(ITEM_HEART_CHARM), FALSE);
    EXPECT_EQ(GetItemFieldFunc(ITEM_HEART_CHARM), ItemUseOutOfBattle_HealParty);
}

TEST("Portable PC is a reusable key item that opens Pokemon storage")
{
    NewGameInitData();

    EXPECT_EQ(CheckBagHasItem(ITEM_POKEMON_BOX_LINK, 1), TRUE);
    EXPECT_EQ(GetItemPocket(ITEM_POKEMON_BOX_LINK), POCKET_KEY_ITEMS);
    EXPECT_EQ(GetItemImportance(ITEM_POKEMON_BOX_LINK), TRUE);
    EXPECT_EQ(GetItemConsumability(ITEM_POKEMON_BOX_LINK), FALSE);
    EXPECT_EQ(GetItemFieldFunc(ITEM_POKEMON_BOX_LINK), ItemUseOutOfBattle_PokemonBoxLink);
}

TEST("Temporary startup Rayquaza is stored in every box")
{
    NewGameInitData();

    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
    {
        struct BoxPokemon *mon = GetBoxedMonPtr(box, 0);

        EXPECT_EQ(GetBoxMonData(mon, MON_DATA_SPECIES), SPECIES_RAYQUAZA);
        EXPECT_EQ(GetLevelFromBoxMonExp(mon), 100);
        EXPECT_EQ(GetBoxMonData(mon, MON_DATA_HELD_ITEM), ITEM_NONE);
        EXPECT_EQ(GetBoxMonData(mon, MON_DATA_MOVE1), MOVE_ACROBATICS);
    }
}

TEST("Running Shoes are available at the start of a new game")
{
    NewGameInitData();

    EXPECT_EQ(FlagGet(FLAG_SYS_B_DASH), TRUE);
}

TEST("Fork rules force battle style to Set by default")
{
    u32 battleStyle;

    NewGameInitData();
    battleStyle = gSaveBlock2Ptr->optionsBattleStyle;
    EXPECT_EQ(battleStyle, OPTIONS_BATTLE_STYLE_SET);
}

TEST("Fork randomizer generation is sanitized from save data")
{
    u8 oldConfigured = gSaveBlock3Ptr->forkGameplayOptionsConfigured;
    u8 oldMaxGen = gSaveBlock3Ptr->forkRandomizerMaxGen;

    gSaveBlock3Ptr->forkGameplayOptionsConfigured = TRUE;
    gSaveBlock3Ptr->forkRandomizerMaxGen = GEN_1;
    EXPECT_EQ(ForkGetRandomizerMaxGen(), GEN_3);

    gSaveBlock3Ptr->forkRandomizerMaxGen = GEN_9 + 1;
    EXPECT_EQ(ForkGetRandomizerMaxGen(), GEN_9);

    gSaveBlock3Ptr->forkGameplayOptionsConfigured = oldConfigured;
    gSaveBlock3Ptr->forkRandomizerMaxGen = oldMaxGen;
}

TEST("Fork rules ban battle items in trainer battles")
{
    struct Pokemon mon;
    u32 hp;

    CreateMon(&mon, SPECIES_WOBBUFFET, 10, 0, OTID_STRUCT_PLAYER_ID);
    hp = GetMonData(&mon, MON_DATA_MAX_HP) - 1;
    SetMonData(&mon, MON_DATA_HP, &hp);
    gPartyMenu.slotId = 0;

    gBattleTypeFlags = 0;
    EXPECT_EQ(CannotUseItemsInBattle(ITEM_POTION, &mon), FALSE);

    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    EXPECT_EQ(CannotUseItemsInBattle(ITEM_POTION, &mon), TRUE);
}

TEST("Fork rules let Infinite Repel toggle off when used again")
{
    VarSet(VAR_REPEL_STEP_COUNT, 0);
    EXPECT_EQ(ToggleInfiniteRepelState(), TRUE);
    EXPECT_EQ(VarGet(VAR_REPEL_STEP_COUNT), 0x7FFF);

    EXPECT_EQ(ToggleInfiniteRepelState(), FALSE);
    EXPECT_EQ(VarGet(VAR_REPEL_STEP_COUNT), 0);
}

TEST("Terastallization type defaults to primary or secondary type")
{
    u32 i;
    enum Type teraType;
    struct Pokemon mon;
    for (i = 0; i < 128; i++) PARAMETRIZE {}
    CreateRandomMonWithIVs(&mon, SPECIES_PIDGEY, 100, 0);
    teraType = GetMonData(&mon, MON_DATA_TERA_TYPE);
    EXPECT(teraType == GetSpeciesType(SPECIES_PIDGEY, 0)
        || teraType == GetSpeciesType(SPECIES_PIDGEY, 1));
}

TEST("Terastallization type can be set to any type except TYPE_NONE")
{
    u32 i;
    enum Type teraType;
    struct Pokemon mon;
    for (i = 1; i < NUMBER_OF_MON_TYPES; i++)
    {
        PARAMETRIZE { teraType = i; }
    }
    CreateRandomMonWithIVs(&mon, SPECIES_WOBBUFFET, 100, 0);
    SetMonData(&mon, MON_DATA_TERA_TYPE, &teraType);
    EXPECT_EQ(teraType, GetMonData(&mon, MON_DATA_TERA_TYPE));
}

TEST("Terastallization type is reset to the default types when setting Tera Type back to TYPE_NONE")
{
    u32 i;
    enum Type teraType, typeNone;
    struct Pokemon mon;
    for (i = 1; i < NUMBER_OF_MON_TYPES; i++)
    {
        PARAMETRIZE { teraType = i; typeNone = TYPE_NONE; }
    }
    CreateRandomMonWithIVs(&mon, SPECIES_PIDGEY, 100, 0);
    SetMonData(&mon, MON_DATA_TERA_TYPE, &teraType);
    EXPECT_EQ(teraType, GetMonData(&mon, MON_DATA_TERA_TYPE));
    if (typeNone == TYPE_NONE)
        typeNone = GetTeraTypeFromPersonality(&mon);
    SetMonData(&mon, MON_DATA_TERA_TYPE, &typeNone);
    typeNone = GetMonData(&mon, MON_DATA_TERA_TYPE);
    EXPECT(typeNone == GetSpeciesType(SPECIES_PIDGEY, 0)
        || typeNone == GetSpeciesType(SPECIES_PIDGEY, 1));
}

TEST("Shininess independent from PID and OTID")
{
    u32 pid, otId, data;
    bool32 isShiny;
    struct Pokemon mon;
    PARAMETRIZE { pid = 0; otId = 0; }
    CreateMon(&mon, SPECIES_WOBBUFFET, 100, pid, OTID_STRUCT_PRESET(otId));
    isShiny = IsMonShiny(&mon);
    data = !isShiny;
    SetMonData(&mon, MON_DATA_IS_SHINY, &data);
    EXPECT_EQ(pid, GetMonData(&mon, MON_DATA_PERSONALITY));
    EXPECT_EQ(otId, GetMonData(&mon, MON_DATA_OT_ID));
    EXPECT_EQ(!isShiny, GetMonData(&mon, MON_DATA_IS_SHINY));
}

TEST("Shininess set on an Egg persists after hatching")
{
    u32 personality = SHINY_ODDS;
    u32 trainerId = 0;
    bool32 isShiny = TRUE;
    bool8 isEgg = TRUE;

    SetTrainerId(trainerId, gSaveBlock2Ptr->playerTrainerId);
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_TOGEPI, EGG_HATCH_LEVEL, personality, OTID_STRUCT_PLAYER_ID);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG, &isEgg);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_SHINY, &isShiny);

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_SHINY), TRUE);

    gSpecialVar_0x8004 = 0;
    ScriptHatchMon();

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG), FALSE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_SHINY), TRUE);
}

TEST("Hyper Training increases stats without affecting IVs")
{
    u32 data, hp, atk, def, speed, spatk, spdef, friendship = 0;
    struct Pokemon mon;
    CreateMonWithIVs(&mon, SPECIES_WOBBUFFET, 100, 0, OTID_STRUCT_PRESET(0), 3);
    // Consider B_FRIENDSHIP_BOOST.
    SetMonData(&mon, MON_DATA_FRIENDSHIP, &friendship);
    CalculateMonStats(&mon);

    hp = GetMonData(&mon, MON_DATA_HP);
    atk = GetMonData(&mon, MON_DATA_ATK);
    def = GetMonData(&mon, MON_DATA_DEF);
    speed = GetMonData(&mon, MON_DATA_SPEED);
    spatk = GetMonData(&mon, MON_DATA_SPATK);
    spdef = GetMonData(&mon, MON_DATA_SPDEF);

    data = TRUE;
    SetMonData(&mon, MON_DATA_HYPER_TRAINED_HP, &data);
    SetMonData(&mon, MON_DATA_HYPER_TRAINED_ATK, &data);
    SetMonData(&mon, MON_DATA_HYPER_TRAINED_DEF, &data);
    SetMonData(&mon, MON_DATA_HYPER_TRAINED_SPEED, &data);
    SetMonData(&mon, MON_DATA_HYPER_TRAINED_SPATK, &data);
    SetMonData(&mon, MON_DATA_HYPER_TRAINED_SPDEF, &data);
    CalculateMonStats(&mon);

    EXPECT_EQ(GetMonData(&mon, MON_DATA_HP_IV), 3);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_ATK_IV), 3);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_DEF_IV), 3);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPEED_IV), 3);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPATK_IV), 3);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPDEF_IV), 3);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPEED_IV), 3);

    EXPECT_EQ(hp - 3 + MAX_PER_STAT_IVS, GetMonData(&mon, MON_DATA_HP));
    EXPECT_EQ(atk - 3 + MAX_PER_STAT_IVS, GetMonData(&mon, MON_DATA_ATK));
    EXPECT_EQ(def - 3 + MAX_PER_STAT_IVS, GetMonData(&mon, MON_DATA_DEF));
    EXPECT_EQ(speed - 3 + MAX_PER_STAT_IVS, GetMonData(&mon, MON_DATA_SPEED));
    EXPECT_EQ(spatk - 3 + MAX_PER_STAT_IVS, GetMonData(&mon, MON_DATA_SPATK));
    EXPECT_EQ(spdef - 3 + MAX_PER_STAT_IVS, GetMonData(&mon, MON_DATA_SPDEF));
}

TEST("Status1 round-trips through BoxPokemon")
{
    u32 status1;
    struct Pokemon mon1, mon2;
    PARAMETRIZE { status1 = STATUS1_NONE; }
    PARAMETRIZE { status1 = STATUS1_SLEEP_TURN(1); }
    PARAMETRIZE { status1 = STATUS1_SLEEP_TURN(2); }
    PARAMETRIZE { status1 = STATUS1_SLEEP_TURN(3); }
    PARAMETRIZE { status1 = STATUS1_SLEEP_TURN(4); }
    PARAMETRIZE { status1 = STATUS1_SLEEP_TURN(5); }
    PARAMETRIZE { status1 = STATUS1_POISON; }
    PARAMETRIZE { status1 = STATUS1_BURN; }
    PARAMETRIZE { status1 = STATUS1_FREEZE; }
    PARAMETRIZE { status1 = STATUS1_PARALYSIS; }
    PARAMETRIZE { status1 = STATUS1_TOXIC_POISON; }
    PARAMETRIZE { status1 = STATUS1_FROSTBITE; }
    CreateRandomMonWithIVs(&mon1, SPECIES_WOBBUFFET, 100, 0);
    SetMonData(&mon1, MON_DATA_STATUS, &status1);
    BoxMonToMon(&mon1.box, &mon2);
    EXPECT_EQ(GetMonData(&mon2, MON_DATA_STATUS), status1);
}

TEST("canhypertrain/hypertrain affect MON_DATA_HYPER_TRAINED_* and recalculate stats")
{
    u32 atk, friendship = 0;
    CreateRandomMonWithIVs(&gParties[B_TRAINER_PLAYER][0], SPECIES_WOBBUFFET, 100, 0);

    // Consider B_FRIENDSHIP_BOOST.
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_FRIENDSHIP, &friendship);
    CalculateMonStats(&gParties[B_TRAINER_PLAYER][0]);

    atk = GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ATK);

    RUN_OVERWORLD_SCRIPT(
        canhypertrain STAT_ATK, 0;
    );
    EXPECT(VarGet(VAR_RESULT));

    RUN_OVERWORLD_SCRIPT(
        hypertrain STAT_ATK, 0;
        canhypertrain STAT_ATK, 0;
    );
    EXPECT(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HYPER_TRAINED_ATK));
    EXPECT_EQ(atk + 31, GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ATK));
    EXPECT(!VarGet(VAR_RESULT));
}

TEST("hasgigantamaxfactor/togglegigantamaxfactor affect MON_DATA_GIGANTAMAX_FACTOR")
{
    CreateRandomMonWithIVs(&gParties[B_TRAINER_PLAYER][0], SPECIES_WOBBUFFET, 100, 0);

    RUN_OVERWORLD_SCRIPT(
        hasgigantamaxfactor 0;
    );
    EXPECT(!VarGet(VAR_RESULT));

    RUN_OVERWORLD_SCRIPT(
        togglegigantamaxfactor 0;
        hasgigantamaxfactor 0;
    );
    EXPECT(VarGet(VAR_RESULT));
    EXPECT(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_GIGANTAMAX_FACTOR));

    RUN_OVERWORLD_SCRIPT(
        togglegigantamaxfactor 0;
        hasgigantamaxfactor 0;
    );
    EXPECT(!VarGet(VAR_RESULT));
    EXPECT(!GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_GIGANTAMAX_FACTOR));
}

TEST("togglegigantamaxfactor fails for Melmetal")
{
    CreateRandomMonWithIVs(&gParties[B_TRAINER_PLAYER][0], SPECIES_MELMETAL, 100, 0);

    RUN_OVERWORLD_SCRIPT(
        hasgigantamaxfactor 0;
    );
    EXPECT(!VarGet(VAR_RESULT));

    RUN_OVERWORLD_SCRIPT(
        togglegigantamaxfactor 0;
    );
    EXPECT(!VarGet(VAR_RESULT));
    EXPECT(!GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_GIGANTAMAX_FACTOR));
}

TEST("givemon [simple]")
{
    ZeroPlayerPartyMons();

    RUN_OVERWORLD_SCRIPT(
        givemon SPECIES_WOBBUFFET, 100;
    );

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_WOBBUFFET);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 100);
}

TEST("givemon respects perfectIVCount")
{
    ZeroPlayerPartyMons();
    u32 perfectIVs[6] = {0};

    ASSUME(gSpeciesInfo[SPECIES_MEW].perfectIVCount == 3);
    ASSUME(gSpeciesInfo[SPECIES_CELEBI].perfectIVCount == 3);
    ASSUME(gSpeciesInfo[SPECIES_JIRACHI].perfectIVCount == 3);
    ASSUME(gSpeciesInfo[SPECIES_MANAPHY].perfectIVCount == 3);
    ASSUME(gSpeciesInfo[SPECIES_VICTINI].perfectIVCount == 3);
    ASSUME(gSpeciesInfo[SPECIES_DIANCIE].perfectIVCount == 3);

    RUN_OVERWORLD_SCRIPT(
        givemon SPECIES_MEW, 100;
        givemon SPECIES_CELEBI, 100;
        givemon SPECIES_JIRACHI, 100;
        givemon SPECIES_MANAPHY, 100;
        givemon SPECIES_VICTINI, 100;
        givemon SPECIES_DIANCIE, 100;
    );

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_MEW);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SPECIES), SPECIES_CELEBI);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][2], MON_DATA_SPECIES), SPECIES_JIRACHI);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][3], MON_DATA_SPECIES), SPECIES_MANAPHY);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][4], MON_DATA_SPECIES), SPECIES_VICTINI);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][5], MON_DATA_SPECIES), SPECIES_DIANCIE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 100);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_LEVEL), 100);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][2], MON_DATA_LEVEL), 100);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][3], MON_DATA_LEVEL), 100);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][4], MON_DATA_LEVEL), 100);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][5], MON_DATA_LEVEL), 100);
    for (u32 j = 0; j < 6; j++)
    {
        for (u32 k = 0; k < NUM_STATS; k++)
        {
            if (GetMonData(&gParties[B_TRAINER_PLAYER][j], MON_DATA_HP_IV + k) == MAX_PER_STAT_IVS)
                perfectIVs[j]++;
        }
        EXPECT_GE(perfectIVs[j], 3);
    }
}

TEST("givemon respects perfectIVCount but does overwrite fixed IVs (1)")
{
    ZeroPlayerPartyMons();

    ASSUME(gSpeciesInfo[SPECIES_MEW].perfectIVCount == 3);
    RUN_OVERWORLD_SCRIPT(
        givemon SPECIES_MEW, 100, hpIv=7, atkIv=8, defIv=9, speedIv=10, spAtkIv=11, spDefIv=12
    );

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP_IV), 7);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ATK_IV), 8);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_DEF_IV), 9);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPEED_IV), 10);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPATK_IV), 11);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPDEF_IV), 12);
}

TEST("givemon respects perfectIVCount but does overwrite fixed IVs (2)")
{
    ZeroPlayerPartyMons();

    ASSUME(gSpeciesInfo[SPECIES_MEW].perfectIVCount == 3);
    RUN_OVERWORLD_SCRIPT(
        givemon SPECIES_MEW, 100, hpIv=7, atkIv=8, defIv=9
    );

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP_IV), 7);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ATK_IV), 8);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_DEF_IV), 9);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPEED_IV), MAX_PER_STAT_IVS);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPATK_IV), MAX_PER_STAT_IVS);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPDEF_IV), MAX_PER_STAT_IVS);
}

TEST("givemon respects FORM_CHANGE_ITEM_HOLD")
{
    ZeroPlayerPartyMons();

    RUN_OVERWORLD_SCRIPT(
        givemon SPECIES_ARCEUS_NORMAL, 100, item=ITEM_ZAP_PLATE;
        givemon SPECIES_ARCEUS_GRASS, 100, item=ITEM_ZAP_PLATE;
        givemon SPECIES_ARCEUS_ELECTRIC, 100, item=ITEM_ZAP_PLATE;
        givemon SPECIES_GIRATINA_ORIGIN, 100, item=ITEM_POTION;
    );

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_ARCEUS_ELECTRIC);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SPECIES), SPECIES_ARCEUS_ELECTRIC);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][2], MON_DATA_SPECIES), SPECIES_ARCEUS_ELECTRIC);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][3], MON_DATA_SPECIES), SPECIES_GIRATINA_ALTERED);
}

TEST("givemon [moves]")
{
    ZeroPlayerPartyMons();

    RUN_OVERWORLD_SCRIPT(
        givemon SPECIES_WOBBUFFET, 100, move1=MOVE_SCRATCH, move2=MOVE_SPLASH, move3=MOVE_NONE, move4=MOVE_NONE;
    );

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_WOBBUFFET);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 100);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE1), MOVE_SCRATCH);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE2), MOVE_SPLASH);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE3), MOVE_NONE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE4), MOVE_NONE);
}

TEST("givemon [moves (default)]")
{
    ZeroPlayerPartyMons();

    RUN_OVERWORLD_SCRIPT(
        givemon SPECIES_PYUKUMUKU, 100, move1=MOVE_DEFAULT, move2=MOVE_DEFAULT, move3=MOVE_DEFAULT;
    );

    const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(SPECIES_PYUKUMUKU);
    u32 learnsetLength;
    for (learnsetLength = 0; learnset[learnsetLength].move != LEVEL_UP_MOVE_END; learnsetLength++)
    {
        ; // we just want to get length of the learnset array
    }
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_PYUKUMUKU);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 100);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE1), learnset[learnsetLength - 4].move);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE2), learnset[learnsetLength - 3].move);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE3), learnset[learnsetLength - 2].move);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE4), learnset[learnsetLength - 1].move);
}

TEST("givemon [all]")
{
    ZeroPlayerPartyMons();

    RUN_OVERWORLD_SCRIPT(
        givemon SPECIES_WOBBUFFET, 100, item=ITEM_LEFTOVERS, ball=BALL_MASTER, nature=NATURE_BOLD, abilityNum=2, gender=MON_MALE, hpEv=1, atkEv=2, defEv=3, speedEv=4, spAtkEv=5, spDefEv=6, hpIv=7, atkIv=8, defIv=9, speedIv=10, spAtkIv=11, spDefIv=12, move1=MOVE_SCRATCH, move2=MOVE_SPLASH, move3=MOVE_CELEBRATE, move4=MOVE_EXPLOSION, shinyMode=SHINY_MODE_ALWAYS, gmaxFactor=TRUE, teraType=TYPE_FIRE, dmaxLevel=7;
    );

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_WOBBUFFET);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 100);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_LEFTOVERS);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_POKEBALL), BALL_MASTER);
    EXPECT_EQ(GetNature(&gParties[B_TRAINER_PLAYER][0]), NATURE_BOLD);
    EXPECT_EQ(GetMonAbility(&gParties[B_TRAINER_PLAYER][0]), GetSpeciesAbility(SPECIES_WOBBUFFET, 2));
    EXPECT_EQ(GetMonGender(&gParties[B_TRAINER_PLAYER][0]), MON_MALE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP_EV), 1);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ATK_EV), 2);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_DEF_EV), 3);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPEED_EV), 4);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPATK_EV), 5);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPDEF_EV), 6);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP_IV), 7);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ATK_IV), 8);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_DEF_IV), 9);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPEED_IV), 10);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPATK_IV), 11);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPDEF_IV), 12);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE1), MOVE_SCRATCH);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE2), MOVE_SPLASH);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE3), MOVE_CELEBRATE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE4), MOVE_EXPLOSION);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_SHINY), TRUE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_GIGANTAMAX_FACTOR), TRUE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_TERA_TYPE), TYPE_FIRE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_DYNAMAX_LEVEL), 7);
}

TEST("givemon [vars]")
{
    ZeroPlayerPartyMons();

    VarSet(VAR_TEMP_C, SPECIES_WOBBUFFET);
    VarSet(VAR_TEMP_D, 100);
    VarSet(VAR_0x8000, ITEM_LEFTOVERS);
    VarSet(VAR_0x8001, BALL_MASTER);
    VarSet(VAR_0x8002, NATURE_BOLD);
    VarSet(VAR_0x8003, 2);
    VarSet(VAR_0x8004, MON_MALE);
    VarSet(VAR_0x8005, 1);
    VarSet(VAR_0x8006, 2);
    VarSet(VAR_0x8007, 3);
    VarSet(VAR_0x8008, 4);
    VarSet(VAR_0x8009, 5);
    VarSet(VAR_0x800A, 6);
    VarSet(VAR_0x800B, 7);
    VarSet(VAR_TEMP_0, 8);
    VarSet(VAR_TEMP_1, 9);
    VarSet(VAR_TEMP_2, 10);
    VarSet(VAR_TEMP_3, 11);
    VarSet(VAR_TEMP_4, 12);
    VarSet(VAR_TEMP_5, MOVE_SCRATCH);
    VarSet(VAR_TEMP_6, MOVE_SPLASH);
    VarSet(VAR_TEMP_7, MOVE_CELEBRATE);
    VarSet(VAR_TEMP_8, MOVE_EXPLOSION);
    VarSet(VAR_TEMP_9, SHINY_MODE_ALWAYS);
    VarSet(VAR_TEMP_A, TRUE);
    VarSet(VAR_TEMP_B, TYPE_FIRE);
    VarSet(VAR_TEMP_E, 7);

    RUN_OVERWORLD_SCRIPT(
        givemon VAR_TEMP_C, VAR_TEMP_D, item=VAR_0x8000, ball=VAR_0x8001, nature=VAR_0x8002, abilityNum=VAR_0x8003, gender=VAR_0x8004, hpEv=VAR_0x8005, atkEv=VAR_0x8006, defEv=VAR_0x8007, speedEv=VAR_0x8008, spAtkEv=VAR_0x8009, spDefEv=VAR_0x800A, hpIv=VAR_0x800B, atkIv=VAR_TEMP_0, defIv=VAR_TEMP_1, speedIv=VAR_TEMP_2, spAtkIv=VAR_TEMP_3, spDefIv=VAR_TEMP_4, move1=VAR_TEMP_5, move2=VAR_TEMP_6, move3=VAR_TEMP_7, move4=VAR_TEMP_8, shinyMode=VAR_TEMP_9, gmaxFactor=VAR_TEMP_A, teraType=VAR_TEMP_B, dmaxLevel=VAR_TEMP_E;
    );

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_WOBBUFFET);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 100);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_LEFTOVERS);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_POKEBALL), BALL_MASTER);
    EXPECT_EQ(GetNature(&gParties[B_TRAINER_PLAYER][0]), NATURE_BOLD);
    EXPECT_EQ(GetMonAbility(&gParties[B_TRAINER_PLAYER][0]), GetSpeciesAbility(SPECIES_WOBBUFFET, 2));
    EXPECT_EQ(GetMonGender(&gParties[B_TRAINER_PLAYER][0]), MON_MALE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP_EV), 1);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ATK_EV), 2);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_DEF_EV), 3);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPEED_EV), 4);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPATK_EV), 5);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPDEF_EV), 6);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP_IV), 7);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ATK_IV), 8);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_DEF_IV), 9);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPEED_IV), 10);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPATK_IV), 11);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPDEF_IV), 12);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE1), MOVE_SCRATCH);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE2), MOVE_SPLASH);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE3), MOVE_CELEBRATE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE4), MOVE_EXPLOSION);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_SHINY), TRUE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_GIGANTAMAX_FACTOR), TRUE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_TERA_TYPE), TYPE_FIRE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_DYNAMAX_LEVEL), 7);
}

TEST("checkteratype/setteratype work")
{
    CreateRandomMonWithIVs(&gParties[B_TRAINER_PLAYER][0], SPECIES_WOBBUFFET, 100, 0);

    RUN_OVERWORLD_SCRIPT(
        checkteratype 0;
    );
    EXPECT(VarGet(VAR_RESULT) == TYPE_PSYCHIC);

    RUN_OVERWORLD_SCRIPT(
        setteratype TYPE_FIRE, 0;
        checkteratype 0;
    );
    EXPECT(VarGet(VAR_RESULT) == TYPE_FIRE);
}

TEST("createmon [simple]")
{
    ZeroPlayerPartyMons();

    RUN_OVERWORLD_SCRIPT(
        createmon 1, 0, SPECIES_WOBBUFFET, 100;
        createmon 1, 1, SPECIES_WYNAUT, 10;
    );

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_SPECIES), SPECIES_WOBBUFFET);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_LEVEL), 100);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_SPECIES), SPECIES_WYNAUT);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_LEVEL), 10);
}

TEST("Pokémon level up learnsets fit within MAX_LEVEL_UP_MOVES and MAX_RELEARNER_MOVES")
{
    KNOWN_FAILING;

    u32 j, count, species = 0;
    const struct LevelUpMove *learnset;

    for(j = 0; j < SPECIES_EGG; j++)
    {
        PARAMETRIZE { species = j; }
    }

    learnset = GetSpeciesLevelUpLearnset(species);
    count = 0;
    for (j = 0; learnset[j].move != LEVEL_UP_MOVE_END; j++)
        count++;
    EXPECT_LT(count, MAX_LEVEL_UP_MOVES);
    EXPECT_LT(count, MAX_RELEARNER_MOVES - 1); // - 1 because at least one move is already known
}

TEST("Optimised GetMonData")
{
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_WOBBUFFET, 5, Random32(), OTID_STRUCT_PRESET(0x12345678));
    u32 exp = 0x123456;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_EXP, &exp);
    struct Benchmark optimised,
        vanilla = (struct Benchmark) { .ticks = 137 }; // From prior testing
    u32 expGet = 0;
    BENCHMARK(&optimised) { expGet = GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_EXP); }
    EXPECT_EQ(exp, expGet);
    EXPECT_FASTER(optimised, vanilla);
}

TEST("Optimised SetMonData")
{
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_WOBBUFFET, 5, Random32(), OTID_STRUCT_PRESET(0x12345678));
    u32 exp = 0x123456;
    struct Benchmark optimised,
        vanilla = (struct Benchmark) { .ticks = 205 }; // From prior testing
    BENCHMARK(&optimised) { SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_EXP, &exp); }
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SANITY_IS_BAD_EGG), FALSE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_EXP), exp);
    EXPECT_FASTER(optimised, vanilla);
}

//Sanity check for a CalculateMonStats refactor (could be deleted or improved)
TEST("CalculateMonStats")
{
    ZeroPlayerPartyMons();

    RUN_OVERWORLD_SCRIPT(
        givemon SPECIES_WOBBUFFET, 100, item=ITEM_LEFTOVERS, ball=BALL_MASTER, nature=NATURE_BOLD, abilityNum=2, gender=MON_MALE, hpEv=1, atkEv=2, defEv=3, speedEv=4, spAtkEv=5, spDefEv=6, hpIv=7, atkIv=8, defIv=9, speedIv=10, spAtkIv=11, spDefIv=12, move1=MOVE_SCRATCH, move2=MOVE_SPLASH, move3=MOVE_CELEBRATE, move4=MOVE_EXPLOSION, shinyMode=SHINY_MODE_ALWAYS, gmaxFactor=TRUE, teraType=TYPE_FIRE, dmaxLevel=7;
    );

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MAX_HP), 497);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ATK), 71);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_DEF), 143);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPEED), 82);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPATK), 83);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPDEF), 134);

}

TEST("BoxPokemon encryption works")
{
    // This test exists to ensure that expansion has not broken anything with regards to how BoxPokemon encryption works.
    // If users make changes to the definitions of BoxPokemon, Pokemon, or any of their members, it is expected that this test will fail. To avoid the failing test from blocking CI, users can uncomment the KNOWN_FAILING declaration.
    // KNOWN_FAILING;
    u32 raw[20] =
    {
        990384375,
        2948624514,
        3907508686,
        14410461,
        35316705,
        3907508686,
        64742109,
        718729,
        3102307966,
        2160206402,
        49956971,
        2495766612,
        1424318580,
        273408756,
        2371630199,
        2708871082,
        3059937332,
        2529190026,
        2290634828,
        2870614922
    };

    struct Pokemon mon;
    BoxMonToMon((struct BoxPokemon *)&raw, &mon);

    EXPECT_EQ(GetMonData(&mon, MON_DATA_SANITY_IS_BAD_EGG), 0);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_TORCHIC);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MARKINGS), 3);
    const u8 *actualNickname = COMPOUND_STRING("Testing mon");
    u8 nickname[12];
    GetMonData(&mon, MON_DATA_NICKNAME, nickname);
    u32 charIndex = 0;
    while (actualNickname[charIndex] != EOS)
    {
        EXPECT_EQ(actualNickname[charIndex], nickname[charIndex]);
        charIndex++;
    }
    EXPECT_EQ(GetNature(&mon), NATURE_HARDY);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HIDDEN_NATURE), NATURE_ADAMANT);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HP_LOST), 10);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), ITEM_ORAN_BERRY);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE1), MOVE_TACKLE);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE2), MOVE_SCRATCH);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE3), MOVE_POUND);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE4), MOVE_GROWL);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_PP1), 1);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_PP2), 2);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_PP3), 3);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_PP4), 4);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_PP_BONUSES), 255);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_COOL), 10);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_BEAUTY), 20);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_CUTE), 30);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SMART), 40);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_TOUGH), 50);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SHEEN), 150);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_EXP), 12345);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MET_LEVEL), 20);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HP_EV), 11);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_ATK_EV), 22);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_DEF_EV), 33);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPEED_EV), 44);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPATK_EV), 55);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPDEF_EV), 66);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_FRIENDSHIP), 123);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_POKERUS), 2);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_POKEBALL), BALL_FRIEND);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HP_IV), 31);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_ATK_IV), 30);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_DEF_IV), 29);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPEED_IV), 28);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPATK_IV), 27);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPDEF_IV), 26);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_CUTE_RIBBON), 1);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_BEAUTY_RIBBON), 0);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_TOUGH_RIBBON), 1);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SMART_RIBBON), 0);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_CHAMPION_RIBBON), 1);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_VICTORY_RIBBON), 1);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_EFFORT_RIBBON), 1);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_LAND_RIBBON), 1);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_COUNTRY_RIBBON), 1);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_EARTH_RIBBON), 1);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HYPER_TRAINED_HP), 1);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HYPER_TRAINED_ATK), 1);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HYPER_TRAINED_DEF), 1);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HYPER_TRAINED_SPEED), 1);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HYPER_TRAINED_SPATK), 1);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HYPER_TRAINED_SPDEF), 1);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_DYNAMAX_LEVEL), 3);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_OT_GENDER), 0);
}
