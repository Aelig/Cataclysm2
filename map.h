#ifndef _MAP_H_
#define _MAP_H_

#define SUBMAP_SIZE 25
#define MAP_SIZE 13
#define VERTICAL_MAP_SIZE 3

#include "window.h"
#include "terrain.h"
#include "world_terrain.h"
#include "mapgen.h"
#include "worldmap.h"
#include "item.h"
#include "enum.h"
#include "geometry.h"
#include "attack.h"
#include "pathfind.h"

class Entity_pool;

struct Tile
{
  Terrain *terrain;
  std::vector<Item> items;
  int hp;

   Tile() { hp = 0; }
  ~Tile() { }

  void set_terrain(Terrain* ter);

  glyph top_glyph();
  int move_cost();
  bool blocks_sense(Sense_type sense = SENSE_SIGHT);
  bool has_flag(Terrain_flag flag);

  std::string smash(Damage_set damage); // Returns the sound
  void open();
  void close();
};

struct Submap
{
  Tile tiles[SUBMAP_SIZE][SUBMAP_SIZE];

/* Subname is the specific flavor of the mapgen_spec used here.  This is used
 * when building second stories; so only use house_wood to build the 2nd floor
 * of a house_wood map.
 * rotation and level are used to further match to the floor below.
 */
  std::string subname;
  Direction rotation;
  int level;

  Submap();
  ~Submap();

  void generate_empty();
  void generate_open();

  void generate(Worldmap* map, int posx, int posy, int posz = 0);
  void generate(World_terrain* terrain[5], int posz = 0);
  void generate(std::string terrain_name);
  void generate(Mapgen_spec* spec);
  void generate_adjacent(Mapgen_spec* spec);
  void generate_above(World_terrain* type, Submap* below);

  bool add_item(Item item, int x, int y);
  int item_count(int x, int y);
  std::vector<Item>* items_at(int x, int y);

};

struct Submap_pool
{
public:
  Submap_pool();
  ~Submap_pool();
  Submap* at_location(int x, int y, int z = 0);
  Submap* at_location(Point p);
  Submap* at_location(Tripoint p);

  std::list<Submap*> instances;
private:
  std::map<Tripoint,Submap*,Tripointcomp> point_map;
};

class Map
{
public:
  Map();
  ~Map();

// Generation
  void generate_empty();
  void test_generate(std::string terrain_name);
  void generate(Worldmap *world, int wposx = -999, int wposy = -999,
                                 int wposz = -999);
  void shift(Worldmap *world, int shiftx, int shifty, int shiftz = 0);

// Game engine access
  Generic_map get_movement_map(Intel_level intel);
  int  move_cost(int x, int y, int z = 999);
  bool is_smashable(int x, int y, int z = 999);
  bool has_flag(Terrain_flag flag, int x, int y, int z = 999);
  int  item_count(int x, int y, int z = 999);
  std::vector<Item>* items_at(int x, int y, int z = 999);
  Tile* get_tile(int x, int y, int z = 999);
  std::string get_name(int x, int y, int z = 999);
  bool add_item(Item item, int x, int y, int z = 999);
  std::string smash(int x, int y, Damage_set damage); // Returns the sound
  std::string smash(int x, int y, int z, Damage_set damage);
  bool open (int x, int y, int z = 999);
  bool close(int x, int y, int z = 999);

  bool senses(int x0, int y0, int x1, int y1, Sense_type sense = SENSE_SIGHT);
  bool senses(int x0, int y0, int z0, int x1, int y1, int z1,
              Sense_type sense = SENSE_SIGHT);
  bool senses(Point origin, Point target, Sense_type sense = SENSE_SIGHT);
  bool senses(Tripoint origin, Tripoint target, Sense_type sense = SENSE_SIGHT);
  std::vector<Point> line_of_sight(int x0, int y0, int x1, int y1);
  std::vector<Point> line_of_sight(int x0, int y0, int z0,
                                   int x1, int y1, int z1);
  std::vector<Point> line_of_sight(Point origin, Point target);
  std::vector<Point> line_of_sight(Tripoint origin, Tripoint target);

// Output
  void draw(Window *w, Entity_pool *entities, int refx, int refy, int refz,
            Sense_type sense = SENSE_SIGHT);

  void draw_tile(Window* w, Entity_pool *entities, int tilex, int tiley,
                 int refx, int refy, bool invert);
  void draw_tile(Window* w, Entity_pool *entities,
                 int tilex, int tiley, int tilez,
                 int refx, int refy, bool invert);

  Point get_center_point();
  int posx, posy, posz;

private:
  Submap* submaps[MAP_SIZE][MAP_SIZE][VERTICAL_MAP_SIZE * 2 + 1];
  Tile tile_oob;
};
#endif
