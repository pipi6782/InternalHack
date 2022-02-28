#pragma once
class Character
{
public:
	Character() = default;
	~Character() = default;

public:
	void SetCharacterInfo(char* name, float pitch, float yaw, int health, int ammo);
	void PrintCharacterInfo();

private:
	struct Rotation
	{
		float Pitch;
		float Yaw;
	};

private:
	char* name;
	Rotation rotation;
	int health;
	int ammo;
};

