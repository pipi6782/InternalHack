#pragma once
class Character
{
public:
	char padding1[4];
	Vec3 headPos;
	char padding2[36];
	Vec3 position;
	Vec3 roation;
	char padding3[172];
	int32_t health;
	char padding4[296];
	int8_t bAttack;
	char padding5[263];
	int32_t team;
	char padding6[68];
	class weapon* currentWeapon;
	char padding7[1248];
};
static_assert(sizeof(Character) == 0x858);

