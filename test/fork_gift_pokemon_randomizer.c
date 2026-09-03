#include "global.h"
#include "fork_gift_pokemon_randomizer.h"
#include "fork_encounter_randomizer.h"
#include "pokemon.h"
#include "test/test.h"
#include "constants/items.h"
#include "constants/moves.h"

static bool8 IsPseudoLegendaryFirstStage(enum Species species)
{
    switch (species)
    {
    case SPECIES_DRATINI:
    case SPECIES_LARVITAR:
    case SPECIES_BAGON:
    case SPECIES_GIBLE:
    case SPECIES_DEINO:
    case SPECIES_GOOMY:
    case SPECIES_JANGMO_O:
    case SPECIES_DREEPY:
    case SPECIES_FRIGIBAX:
        return TRUE;
    default:
        return FALSE;
    }
}

static bool8 IsOfficialStarterSpecies(enum Species species)
{
    switch (species)
    {
    case SPECIES_BULBASAUR: case SPECIES_CHARMANDER: case SPECIES_SQUIRTLE:
    case SPECIES_CHIKORITA: case SPECIES_CYNDAQUIL: case SPECIES_TOTODILE:
    case SPECIES_TREECKO: case SPECIES_TORCHIC: case SPECIES_MUDKIP:
    case SPECIES_TURTWIG: case SPECIES_CHIMCHAR: case SPECIES_PIPLUP:
    case SPECIES_SNIVY: case SPECIES_TEPIG: case SPECIES_OSHAWOTT:
    case SPECIES_CHESPIN: case SPECIES_FENNEKIN: case SPECIES_FROAKIE:
    case SPECIES_ROWLET: case SPECIES_LITTEN: case SPECIES_POPPLIO:
    case SPECIES_GROOKEY: case SPECIES_SCORBUNNY: case SPECIES_SOBBLE:
    case SPECIES_SPRIGATITO: case SPECIES_FUECOCO: case SPECIES_QUAXLY:
        return TRUE;
    default:
        return FALSE;
    }
}

TEST("Fork starter choices use distinct Generation 3 Pokémon at or below 325 BST")
{
    enum Species starters[3];

    gSaveBlock3Ptr->forkItemRandomizerSeed = 0x12345678;
    for (u8 i = 0; i < ARRAY_COUNT(starters); i++)
    {
        starters[i] = GetForkRandomizedStarterSpecies(i);
        EXPECT_LE((u16)gSpeciesInfo[starters[i]].natDexNum, NATIONAL_DEX_DEOXYS);
        EXPECT_LE(GetSpeciesBaseStatTotal(starters[i]), 325);
    }
    EXPECT_NE(starters[0], starters[1]);
    EXPECT_NE(starters[0], starters[2]);
    EXPECT_NE(starters[1], starters[2]);
}

TEST("Fork starter randomizer can choose non-starter Pokémon")
{
    bool8 foundNonStarter = FALSE;

    for (u32 seed = 1; seed <= 32; seed++)
    {
        gSaveBlock3Ptr->forkItemRandomizerSeed = seed;
        for (u8 slot = 0; slot < 3; slot++)
            if (!IsOfficialStarterSpecies(GetForkRandomizedStarterSpecies(slot)))
                foundNonStarter = TRUE;
    }

    EXPECT(foundNonStarter);
}

TEST("Steven's gift is a Generation-compatible pseudo-legendary first stage")
{
    enum Species species;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 0x87654321;
    species = GetForkRandomizedStevenGiftSpecies();

    EXPECT_LE((u16)gSpeciesInfo[species].natDexNum, NATIONAL_DEX_DEOXYS);
    EXPECT(IsPseudoLegendaryFirstStage(species));
}

TEST("Fork starter receives at least two perfect IVs")
{
    struct Pokemon mon;
    u8 perfectIvs = 0;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 0x13572468;
    CreateMon(&mon, SPECIES_TREECKO, 5, 0, OTID_STRUCT_PRESET(0));
    ApplyForkStarterPerfectIvs(&mon, 0);

    for (u8 stat = 0; stat < NUM_STATS; stat++)
        if (GetMonData(&mon, MON_DATA_HP_IV + stat) == MAX_PER_STAT_IVS)
            perfectIvs++;
    EXPECT_GE(perfectIvs, 2);
}

TEST("Fortree trade uses its generation-filtered random pool")
{
    enum Species species;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 0x10293847;
    species = GetForkRandomizedFortreeTradeSpecies();

    EXPECT(species == SPECIES_BLASTOISE || species == SPECIES_SCIZOR || species == SPECIES_TORKOAL
        || species == SPECIES_MAGMORTAR || species == SPECIES_GOLURK || species == SPECIES_DRAGALGE
        || species == SPECIES_SHIINOTIC || species == SPECIES_MORPEKO || species == SPECIES_DACHSBUN);
    EXPECT_LE((u16)gSpeciesInfo[species].natDexNum, GetForkMaxNationalDex());
}

TEST("Fortree trade keeps the selected ability and shiny treatment")
{
    enum Species species;
    enum Ability expectedAbility;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 0x56473829;
    species = GetForkRandomizedFortreeTradeSpecies();

    switch (species)
    {
    case SPECIES_BLASTOISE: expectedAbility = ABILITY_MEGA_LAUNCHER; break;
    case SPECIES_SCIZOR: expectedAbility = ABILITY_TECHNICIAN; break;
    case SPECIES_TORKOAL: expectedAbility = ABILITY_DROUGHT; break;
    case SPECIES_MAGMORTAR: expectedAbility = ABILITY_FLASH_FIRE; break;
    case SPECIES_GOLURK: expectedAbility = ABILITY_UNSEEN_FIST; break;
    case SPECIES_DRAGALGE: expectedAbility = ABILITY_REGENERATOR; break;
    case SPECIES_SHIINOTIC: expectedAbility = ABILITY_POISON_HEAL; break;
    case SPECIES_MORPEKO: expectedAbility = ABILITY_HUNGER_SWITCH; break;
    case SPECIES_DACHSBUN: expectedAbility = ABILITY_WELL_BAKED_BODY; break;
    default: expectedAbility = ABILITY_NONE; break;
    }
    EXPECT_EQ(GetForkRandomizedFortreeTradeAbility(), expectedAbility);
    EXPECT_EQ(GetForkRandomizedFortreeTradeIsShiny(), species == SPECIES_GOLURK);
}

TEST("Rustboro trade uses its generation-filtered random pool")
{
    enum Species species;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 0x91827364;
    species = GetForkRandomizedRustboroTradeSpecies();

    EXPECT(species == SPECIES_PSYDUCK || species == SPECIES_CHIKORITA || species == SPECIES_MAWILE
        || species == SPECIES_SPIRITOMB || species == SPECIES_EELEKTROSS || species == SPECIES_MALAMAR
        || species == SPECIES_WISHIWASHI || species == SPECIES_CURSOLA || species == SPECIES_PALAFIN);
    EXPECT_LE((u16)gSpeciesInfo[species].natDexNum, GetForkMaxNationalDex());
}

TEST("Rustboro trade assigns its special held items and moves")
{
    enum Species species;
    enum Item heldItem;
    enum Move move;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 0x19283746;
    species = GetForkRandomizedRustboroTradeSpecies();
    heldItem = GetForkRandomizedRustboroTradeHeldItem();
    move = GetForkRandomizedRustboroTradeMove(0);

    if (species == SPECIES_PSYDUCK)
        EXPECT_EQ(heldItem, ITEM_EVIOLITE);
    else if (species == SPECIES_CHIKORITA)
        EXPECT_EQ(heldItem, ITEM_MEGANIUMITE);
    else if (species == SPECIES_MAWILE)
        EXPECT_EQ(heldItem, ITEM_MAWILITE);
    else
        EXPECT_EQ(heldItem, ITEM_NONE);

    if (species == SPECIES_WISHIWASHI)
        EXPECT_EQ(move, MOVE_SHADOW_SNEAK);
    else if (species == SPECIES_EELEKTROSS)
        EXPECT_EQ(move, MOVE_THUNDERBOLT);
    else
        EXPECT_EQ(move, MOVE_NONE);
}

TEST("Rustboro Psyduck receives its special moveset")
{
    bool8 foundPsyduck = FALSE;

    for (u32 seed = 1; seed <= 64; seed++)
    {
        gSaveBlock3Ptr->forkItemRandomizerSeed = seed;
        if (GetForkRandomizedRustboroTradeSpecies() == SPECIES_PSYDUCK)
        {
            foundPsyduck = TRUE;
            EXPECT_EQ(GetForkRandomizedRustboroTradeMove(0), MOVE_FOLLOW_ME);
            EXPECT_EQ(GetForkRandomizedRustboroTradeMove(1), MOVE_CONFUSION);
            EXPECT_EQ(GetForkRandomizedRustboroTradeMove(2), MOVE_CONFUSE_RAY);
            EXPECT_EQ(GetForkRandomizedRustboroTradeMove(3), MOVE_EXPLOSION);
            break;
        }
    }
    EXPECT(foundPsyduck);
}

TEST("Fork starter's other IVs can also roll perfect")
{
    struct Pokemon mon;
    bool8 foundExtraPerfectIv = FALSE;

    for (u32 seed = 1; seed <= 128; seed++)
    {
        u8 perfectIvs = 0;

        gSaveBlock3Ptr->forkItemRandomizerSeed = seed;
        CreateMon(&mon, SPECIES_TREECKO, 5, 0, OTID_STRUCT_PRESET(0));
        ApplyForkStarterPerfectIvs(&mon, 0);
        for (u8 stat = 0; stat < NUM_STATS; stat++)
            if (GetMonData(&mon, MON_DATA_HP_IV + stat) == MAX_PER_STAT_IVS)
                perfectIvs++;
        if (perfectIvs > 2)
            foundExtraPerfectIv = TRUE;
    }

    EXPECT(foundExtraPerfectIv);
}

TEST("Fork starter perfect IV stats vary between saves")
{
    struct Pokemon mon;
    u8 firstPair = 0;
    bool8 foundDifferentPair = FALSE;

    for (u32 seed = 1; seed <= 32; seed++)
    {
        u8 pair = 0;

        gSaveBlock3Ptr->forkItemRandomizerSeed = seed;
        CreateMon(&mon, SPECIES_TREECKO, 5, 0, OTID_STRUCT_PRESET(0));
        ApplyForkStarterPerfectIvs(&mon, 0);
        for (u8 stat = 0; stat < NUM_STATS; stat++)
            if (GetMonData(&mon, MON_DATA_HP_IV + stat) == MAX_PER_STAT_IVS)
                pair |= 1 << stat;
        if (seed == 1)
            firstPair = pair;
        else if (pair != firstPair)
            foundDifferentPair = TRUE;
    }

    EXPECT(foundDifferentPair);
}
