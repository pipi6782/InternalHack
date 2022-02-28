#include "Global.h"
#include "Character.h"

void Character::SetCharacterInfo(char* name, float pitch, float yaw, int health, int ammo)
{
	this->name = name;
	rotation.Pitch = pitch;
	rotation.Yaw = yaw;
	this->health = health;
	this->ammo = ammo;
}

void Character::PrintCharacterInfo()
{
	cout << "Name : " << name << endl;
	cout << "Pitch : " << rotation.Pitch << endl;
	cout << "Yaw  : " << rotation.Yaw << endl;
	cout << "Health : " << health << endl;
	cout << "Ammo : " : << ammo << endl;
}
