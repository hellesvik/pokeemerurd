# Reference data

This directory contains generated or reviewable data used by the fork's biome
encounter randomizer:

- [Biome species catalog](./biome_species_catalog.csv) lists eligible species
  by biome and generation constraints.
- [Biome encounter assignments](./biome_encounter_assignments.csv) lists the
  map, encounter method, and biome assignments.
- [Pokémon evolution methods](./pokemon_evolution_methods.md) lists every
  represented species and form with its configured evolution method.

These CSVs are derived artifacts for review and documentation. Update the
generator/source data that produces them rather than editing them manually.
The [biome encounter randomizer](../randomizers/biome_encounter_randomizer.md)
explains how they are consumed.
