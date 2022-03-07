#pragma once
class Weapon
{
public:
	char padding1[4];
	int32_t weaponID;
	class Character* owner;
	char padding2[4];
	class ammoPtr* ammoReserve;
	class ammoPtr* ammoClip;
	char padding3[44];
};
static_assert(sizeof(Weapon) == 0x44);

class ammoPtr
{
public:
	int32_t ammo;
};
static_assert(sizeof(ammoPtr) == 0x4);

