#include "global.h"
#include "event_data.h"
#include "fork_fossil_randomizer.h"
#include "fork_run.h"
#include "item.h"
#include "random.h"

static const enum Item sRevivableFossils[] =
{
    ITEM_OLD_AMBER,
    ITEM_HELIX_FOSSIL,
    ITEM_DOME_FOSSIL,
    ITEM_ROOT_FOSSIL,
    ITEM_CLAW_FOSSIL,
    ITEM_ARMOR_FOSSIL,
    ITEM_SKULL_FOSSIL,
    ITEM_COVER_FOSSIL,
    ITEM_PLUME_FOSSIL,
    ITEM_JAW_FOSSIL,
    ITEM_SAIL_FOSSIL,
};

enum Item GetForkMirageTowerFossil(u8 choice)
{
    enum Item selected[2];
    rng_value_t rng = LocalRandomSeed(gSaveBlock3Ptr->forkItemRandomizerSeed ^ 0x464F5353);

    if (choice >= ARRAY_COUNT(selected))
        return ITEM_NONE;
    if (!ForkAreRandomEncountersEnabled())
        return choice == 0 ? ITEM_ROOT_FOSSIL : ITEM_CLAW_FOSSIL;

    for (u8 selectedCount = 0; selectedCount <= choice; selectedCount++)
    {
        u16 index = LocalRandom(&rng) % ARRAY_COUNT(sRevivableFossils);

        while (selectedCount != 0 && sRevivableFossils[index] == selected[0])
            index = (index + 1) % ARRAY_COUNT(sRevivableFossils);
        selected[selectedCount] = sRevivableFossils[index];
    }

    return selected[choice];
}

enum Species GetForkFossilRevivalSpecies(enum Item item)
{
    switch (item)
    {
    case ITEM_OLD_AMBER: return SPECIES_AERODACTYL;
    case ITEM_HELIX_FOSSIL: return SPECIES_OMANYTE;
    case ITEM_DOME_FOSSIL: return SPECIES_KABUTO;
    case ITEM_ROOT_FOSSIL: return SPECIES_LILEEP;
    case ITEM_CLAW_FOSSIL: return SPECIES_ANORITH;
    case ITEM_ARMOR_FOSSIL: return SPECIES_SHIELDON;
    case ITEM_SKULL_FOSSIL: return SPECIES_CRANIDOS;
    case ITEM_COVER_FOSSIL: return SPECIES_TIRTOUGA;
    case ITEM_PLUME_FOSSIL: return SPECIES_ARCHEN;
    case ITEM_JAW_FOSSIL: return SPECIES_TYRUNT;
    case ITEM_SAIL_FOSSIL: return SPECIES_AMAURA;
    default: return SPECIES_NONE;
    }
}

enum Item GetForkFirstRevivableFossil(void)
{
    for (u16 i = 0; i < ARRAY_COUNT(sRevivableFossils); i++)
        if (CheckBagHasItem(sRevivableFossils[i], 1))
            return sRevivableFossils[i];
    return ITEM_NONE;
}

u16 GetForkMirageTowerFossilSpecial(void)
{
    return GetForkMirageTowerFossil(gSpecialVar_0x8004);
}

u16 GetForkFossilRevivalSpeciesSpecial(void)
{
    return GetForkFossilRevivalSpecies(gSpecialVar_0x8004);
}

u16 GetForkFirstRevivableFossilSpecial(void)
{
    return GetForkFirstRevivableFossil();
}
