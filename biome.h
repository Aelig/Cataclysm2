#ifndef _BIOME_H_
#define _BIOME_H_

#include "world_terrain.h"
#include <istream>

struct World_terrain_chance
{
  World_terrain_chance(int C = 10, World_terrain* WT = NULL) :
    chance (C), terrain (WT) {}
  int chance;
  World_terrain* terrain;
};

struct Variable_world_terrain
{
public:
  Variable_world_terrain();
  ~Variable_world_terrain(){}

  void add_terrain(int chance, World_terrain* terrain);
  void add_terrain(World_terrain_chance terrain);
  void load_data(std::istream &data, std::string name = "unknown");

  World_terrain* pick();

private:
  std::vector<World_terrain_chance> ter;
  int total_chance;
};

enum Biome_flag
{
  BIOME_FLAG_NULL = 0,
  BIOME_FLAG_LAKE,    // "lake" - turn to ocean if ocean-adjacent
  BIOME_FLAG_CITY,    // "city" - turn into city buildings
  BIOME_FLAG_NO_OCEAN,// "no_ocean" - don't turn into ocean even if altitude<=0
  BIOME_FLAG_MAX
};

Biome_flag lookup_biome_flag(std::string name);
std::string biome_flag_name(Biome_flag flag);

struct Biome
{
  Biome();
  ~Biome();

  std::string name;
  int uid;

  Variable_world_terrain terrain;

  void assign_uid(int id);
  std::string get_name();
  bool load_data(std::istream &data);

  World_terrain* pick_terrain();
  bool has_flag(Biome_flag flag);
private:
  std::vector<bool> flags;
};

enum Lake_status
{
  LAKE_UNCHECKED,
  LAKE_CHECKED,
  LAKE_NOTLAKE
};

enum City_status
{
  CITY_NOTCITY,
  CITY_HUB,
  CITY_RAW, // Not generated yet
  CITY_BUILDING,
  CITY_BUILDING_CLOSED,
  CITY_ROAD,
  CITY_ROAD_CLOSED
};

#endif
