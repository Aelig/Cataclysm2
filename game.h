#ifndef _GAME_H_
#define _GAME_H_

#include "map.h"
#include "cuss.h"
#include "worldmap.h"
#include "monster.h"
#include "player.h"

class Game
{
public:
  Game();
  ~Game();

  bool setup();
  bool main_loop();
  //void draw();
  void update_hud();

  void move_monsters();

  void player_move(int xdif, int ydif); // Handles all aspects of moving player

  Map*      map;
  Worldmap* worldmap;
  Player*   player;
  Monster_pool monsters;

private:
  Window *w_map;
  Window *w_hud;
  cuss::interface i_hud;
};

#endif
