#include "attack.h"
#include "rng.h"
#include "stringfunc.h"
#include "window.h"
#include "entity.h" // For Stats
#include <sstream>

Damage_set::Damage_set()
{
  for (int i = 0; i < DAMAGE_MAX; i++) {
    damage[i] = 0;
  }
}

Damage_set::~Damage_set()
{
}

void Damage_set::set_damage(Damage_type type, int amount)
{
  damage[type] = amount;
}

void Damage_set::set_damage(int index, int amount)
{
  if (index < 0 || index >= DAMAGE_MAX) {
    return;
  }
  damage[index] = amount;
}

int Damage_set::get_damage(Damage_type type) const
{
  return damage[type];
}

int Damage_set::get_damage(int index) const
{
  if (index < 0 || index >= DAMAGE_MAX) {
    return 0;
  }
  return damage[index];
}

int Damage_set::total_damage()
{
  int ret = 0;
  for (int i = 0; i < DAMAGE_MAX; i++) {
    ret += get_damage(i);
  }
  return ret;
}

Damage_set& Damage_set::operator+=(const Damage_set& rhs)
{
  for (int i = 0; i < DAMAGE_MAX; i++) {
    damage[i] += rhs.get_damage(i);
  }
  return *this;
}

Damage_set& Damage_set::operator-=(const Damage_set& rhs)
{
  for (int i = 0; i < DAMAGE_MAX; i++) {
    damage[i] -= rhs.get_damage(i);
    if (damage[i] < 0) {
      damage[i] = 0;
    }
  }
  return *this;
}

Attack::Attack()
{
  verb_first = "hit";
  verb_third = "hits";
  weight =  10;
  speed  = 100;
  to_hit =   0;
  for (int i = 0; i < DAMAGE_MAX; i++) {
    damage[i] = 0;
  }
}

Attack::~Attack()
{
}

bool Attack::load_data(std::istream &data, std::string owner_name)
{
  std::string ident, junk;
  while (ident != "done" && !data.eof()) {

    if ( ! (data >> ident) ) {
      return false;
    }

    ident = no_caps(ident);

    if (!ident.empty() && ident[0] == '#') { // I'ts a comment
      std::getline(data, junk);

    } else if (ident == "verb_first:") {
      std::getline(data, verb_first);
      verb_first = trim(verb_first);
    } else if (ident == "verb:" || ident == "verb_third:") {
      std::getline(data, verb_third);
      verb_third = trim(verb_third);
    } else if (ident == "weight:") {
      data >> weight;
      std::getline(data, junk);
    } else if (ident == "speed:") {
      data >> speed;
      std::getline(data, junk);
    } else if (ident == "to_hit:") {
      data >> to_hit;
      std::getline(data, junk);
    } else if (ident != "done") {
      std::string damage_name = ident;
      size_t colon = ident.find(':');
      if (colon != std::string::npos) {
        damage_name = ident.substr(0, colon);
      }
      Damage_type type = lookup_damage_type(damage_name);
      if (type == DAMAGE_NULL) {
        debugmsg("Unknown Attack property '%s' (%s)",
                 ident.c_str(), owner_name.c_str());
        return false;
      } else {
        data >> damage[type];
      }
    }
  }
  return true;
}

void Attack::use_weapon(Item weapon, Stats stats)
{
  speed  += weapon.get_base_attack_speed(stats);
  to_hit += weapon.get_to_hit();
  for (int i = 0; i < DAMAGE_MAX; i++) {
    damage[i] += weapon.get_damage( Damage_type(i) );
  }
}

Damage_set Attack::roll_damage()
{
  Damage_set ret;
  for (int i = 0; i < DAMAGE_MAX; i++) {
    ret.set_damage( Damage_type(i), rng(0, damage[i]) );
  }
  return ret;
}

Ranged_attack::Ranged_attack()
{
  verb_first = "shoot";
  verb_third = "shoots";
  weight = 0;
  speed = 100;
  charge_time = 0;
  range = 0;
  for (int i = 0; i < DAMAGE_MAX; i++) {
    damage[i] = 0;
    armor_divisor[i] = 1;
  }
}

Ranged_attack::~Ranged_attack()
{
}

bool Ranged_attack::load_data(std::istream &data, std::string owner_name)
{
  std::string ident, junk;
  while (ident != "done" && !data.eof()) {

    if ( ! (data >> ident) ) {
      return false;
    }

    ident = no_caps(ident);

    if (!ident.empty() && ident[0] == '#') { // I'ts a comment
      std::getline(data, junk);

    } else if (ident == "verb_first:") {
      std::getline(data, verb_first);
      verb_first = trim(verb_first);
    } else if (ident == "verb:" || ident == "verb_third:") {
      std::getline(data, verb_third);
      verb_third = trim(verb_third);
    } else if (ident == "weight:") {
      data >> weight;
      std::getline(data, junk);
    } else if (ident == "speed:") {
      data >> speed;
      std::getline(data, junk);
    } else if (ident == "charge:" || ident == "charge_time:") {
      data >> charge_time;
      std::getline(data, junk);
    } else if (ident == "range:") {
      data >> range;
      std::getline(data, junk);
    } else if (ident == "variance:") {
      std::string variance_line;
      std::getline(data, variance_line);
      std::istringstream variance_data(variance_line);
      int tmpnum;
      while (data >> tmpnum) {
        variance.push_back(tmpnum);
      }
    } else if (ident == "armor_pierce:") {
      std::string damage_name;
      data >> damage_name;
      Damage_type damtype = lookup_damage_type(damage_name);
      if (damtype == DAMAGE_NULL) {
        debugmsg("Unknown damage type for Ranged_attack pierce: '%s' (%s)",
                 damage_name.c_str(), owner_name.c_str());
        return false;
      }
      data >> armor_divisor[damtype];
      if (armor_divisor[damtype] == 0) {
        armor_divisor[damtype] = 1;
      }
    } else if (ident != "done") {
      std::string damage_name = ident;
      size_t colon = ident.find(':');
      if (colon != std::string::npos) {
        damage_name = ident.substr(0, colon);
      }
      Damage_type type = lookup_damage_type(damage_name);
      if (type == DAMAGE_NULL) {
        debugmsg("Unknown Attack property '%s' (%s)",
                 ident.c_str(), owner_name.c_str());
        return false;
      } else {
        data >> damage[type];
      }
    }
  }
  return true;
}

int Ranged_attack::roll_variance()
{
  int ret = 0;
  for (int i = 0; i < variance.size(); i++) {
    ret += rng(0, variance[i]);
    debugmsg("[%d/%d]: %d, total %d", i, variance.size(), variance[i], ret);
  }
  return ret;
}

Damage_set Ranged_attack::roll_damage()
{
  Damage_set ret;
  for (int i = 0; i < DAMAGE_MAX; i++) {
    ret.set_damage( Damage_type(i), rng(0, damage[i]) );
  }
  return ret;
}


Body_part random_body_part_to_hit()
{
  int pick = rng(1, 13);
  switch (pick) {
    case  1:  return BODYPART_HEAD;
    case  2:
    case  3:  return BODYPART_LEFT_ARM;
    case  4:
    case  5:  return BODYPART_RIGHT_ARM;
    case  6:
    case  7:  return BODYPART_LEFT_LEG;
    case  8:
    case  9:  return BODYPART_RIGHT_LEG;
    case 10:
    case 11:
    case 12:
    case 13:  return BODYPART_TORSO;
  }

  return BODYPART_TORSO;
}

