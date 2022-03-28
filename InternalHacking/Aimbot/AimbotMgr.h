#pragma once

enum class EChangeType
{
	Next=1,Prev=-1
};

class AimbotMgr
{
private:
	AimbotMgr() = default;
	~AimbotMgr() = default;

public:
	static void Create();
	static AimbotMgr* Get();
	static void Destroy();

public:
	void AddEntity(class Character* entity, int numofEntities);
	void ResetTarget() { target = nullptr; }
	void TraceEntity();
	void SetPlayer(class Character* player) { this->player = player; }

private:
	bool IsTeamGame();
	void ChangeTarget();
	Vector3 FindRotation(class Character* from, class Character* to);

private:
	static AimbotMgr* instance;
	class Character* player = nullptr;
	class Character* target = nullptr;
	int targetIndex;
	std::vector<class Character*> entities;
	int* gameMode = (int*)(0x50F49C);
};

