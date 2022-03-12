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
	void AddEntity(class Character* entity);
	void TraceEntity(class Character* player);
	Vec3 FindRotation(class Character* from, class Character* to);
	void ChangeTarget(EChangeType eType);

private:
	static AimbotMgr* instance;
	class Character* target = nullptr;
	int targetIndex;
	vector<class Character*> entities;
};

