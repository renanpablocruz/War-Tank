#ifndef TANK_H
#define TANK_H

#include "Bullet.h"
#include "Utils.h"

enum tankState {WAITING, MOVING, SELECTING_TARGET, SHOOTING, INVALID_TANK};
enum tankName {PANZER, TIGER, PANTHER};

class Tank
{
protected:
	int ammo;
	Bullet* bullet;
	int life;
	int max_ammo;
	int max_life;
	int max_mov;
	int mov;
	dir moveTo;
	tankName name;
	dir orientation;
	int range;
	tankState state;
	elem type;
	float x, y, z;
	float xf, yf, zf;
public:
	Tank();
	Tank(int ml, int r, int a, int ms, elem t);
	bool canMove();
	bool canShoot(float x2, float y2, float z2);
	void computeDamage(int damage);
	virtual void draw() = 0;
	Bullet* getBullet();
	void getFinalPos(float &_x, float &_y, float &_z);
	int getLife();
	void getPos(float &_x, float &_y, float &_z);
	tankState getState();
	elem getType();
	void modLife(int d);
	void passTurn();
	void setMoveTo(dir dr);
	void setPos(float x, float y, float z);
	void setSelectTargetMode();
	void setWaitingMode();
	virtual void shoot(float _x, float _y, float _z) = 0;
	void spendAmmo();
	void spendAMove();
	void update(int value);
};

#endif
