class ESPManager{
public:
	ESPManager();
	~ESPManager();

	void ClearEntities() { entities.clear(); }
	void AddEntity(class Character* entity, int numofEntities);
	void SetPlayer(class Character* player) { this->player = player; }
	void ChangeTarget();
	void Run();
	void RunEspThread();

public:
	static void Create();
	static ESPManager* Get();
	static void Destroy();

private:
	bool WorldToScreen(Vector3 pos, Vector2& screen, float matrix[16], int windowWidth, int windowHeight);
	bool IsTeamGame();
	void DrawBorderBox(int x, int y, int w, int h, int thickness);
	void DrawFilledRect(int x, int y, int w, int h, HBRUSH color = nullptr);

private:
	static ESPManager* instance;
	class Character* player;
	std::vector<class Character*> entities;
	class Character* target;
	int targetIndex = 0;
	std::vector<std::thread> espThreads;
	HWND hWnd;
	HDC hdc;
	std::mutex m;
	int numofentity = 0;
	float mat[16];
	int* gameMode = (int*)(0x50F49C);
};
