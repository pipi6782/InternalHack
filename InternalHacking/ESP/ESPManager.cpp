#include "Global.h"
#include "Aimbot\Character.h"
#include "ESPManager.h"

ESPManager* ESPManager::instance = nullptr;

ESPManager::ESPManager()
{
	hWnd = FindWindow(NULL, L"AssaultCube");
	hdc = GetDC(hWnd);
}

ESPManager::~ESPManager()
{
	for (int i = 0; i < 4; i++)
	{
		espThreads[i].detach();
	}
}

void ESPManager::AddEntity(Character* entity, int numofEntities)
{
	entities.push_back(entity);
	target = entity;
	targetIndex = (targetIndex + 1) % numofEntities;
}

void ESPManager::ChangeTarget()
{
	//제일 가까이 있는 타겟으로 변경
	float length = FLT_MAX;
	for (int i = 0; i < entities.size(); i++)
	{
		Vector3 direction = player->position - entities[i]->position;
		float temp = D3DXVec3Length(&direction);
		if (length > temp)
		{
			targetIndex = i;
			target = entities[i];
			length = temp;
		}
	}
}

void ESPManager::Run()
{
	for (int i = 0; i < 4; i++)
	{
		espThreads.push_back(std::thread(std::bind(&ESPManager::RunEspThread,this)));
	}
}

void ESPManager::Create()
{
	if (instance == nullptr)
		instance = new ESPManager();
}

ESPManager* ESPManager::Get()
{
	return instance;
}

void ESPManager::Destroy()
{
	SafeDelete(instance);
}

bool ESPManager::WorldToScreen(Vector3 pos, Vector2& screen, float matrix[16], int windowWidth, int windowHeight)
{
	//WVP 변환을 CPU레벨에서 실행
	Vector4 clipCoord;
	clipCoord.x = pos.x * matrix[0] + pos.y * matrix[4] + pos.z * matrix[8]  + matrix[12];
	clipCoord.y = pos.x * matrix[1] + pos.y * matrix[5] + pos.z * matrix[9]  + matrix[13];
	clipCoord.z = pos.x * matrix[2] + pos.y * matrix[6] + pos.z * matrix[10] + matrix[14];
	clipCoord.w = pos.x * matrix[3] + pos.y * matrix[7] + pos.z * matrix[11] + matrix[15];

	if (clipCoord.w < 0.1f)
		return false;

	//perspective division, dividing by clip.W = Normalized Device Coordinates
	Vector3 NDC;
	NDC.x = clipCoord.x / clipCoord.w;
	NDC.y = clipCoord.y / clipCoord.w;
	NDC.z = clipCoord.z / clipCoord.w;

	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	return true;
}

bool ESPManager::IsTeamGame()
{
	if ((*gameMode == 0 || *gameMode == 4 || *gameMode == 5 || *gameMode == 7 ||
		*gameMode == 11 || *gameMode == 13 || *gameMode == 14 || *gameMode == 16 ||
		*gameMode == 17 || *gameMode == 20 || *gameMode == 21))
		return true;
	else return false;
}

void ESPManager::DrawBorderBox(int x, int y, int w, int h, int thickness)
{
	HBRUSH color = CreateSolidBrush(RGB(255, 255, 255));
	DrawFilledRect(x + 2, y, w - 2, thickness, color);
	DrawFilledRect(x, y, thickness, h, color);
	DrawFilledRect((x + w), y, thickness, h, color);
	DrawFilledRect(x, y + h, w + thickness, thickness, color);
	DeleteObject(color);
}

void ESPManager::DrawFilledRect(int x, int y, int w, int h, HBRUSH color)
{
	if (color == nullptr)
	{
		color = CreateSolidBrush(RGB(255, 0, 0));
		RECT rect = { x, y, x + w, y + h };
		FillRect(hdc, &rect, color);
		DeleteObject(color);
	}
	else
	{
		RECT rect = { x, y, x + w, y + h };
		FillRect(hdc, &rect, color);
	}
}


void ESPManager::RunEspThread()
{
	float* temp = (float*)(0x501AE8);
	float mat[16] = { 0 };
	RECT rect;
	GetClientRect(hWnd, &rect);
	Vector2 screenPos, screenPosHead;
	while (true)
	{
		bool bEnableWindow = IsWindow(hWnd) == false;
		if (bEnableWindow)
			break;
		m.lock();
		targetIndex += 1;
		targetIndex %= entities.size();
		int index = targetIndex;
		m.unlock();
		for (int i = 0; i < 16; i++)
			mat[i] = temp[i];
		bool b = true;
		b &= WorldToScreen(entities[index]->position, screenPos, mat, rect.right - rect.left, rect.bottom - rect.top);
		b &= WorldToScreen(entities[index]->headPos, screenPosHead, mat, rect.right - rect.left, rect.bottom - rect.top);
		b &= (IsTeamGame() == false || entities[index]->team != player->team);
		b &= (entities[index]->health > 0.0f);
		if (b == true)
		{
			float height = screenPosHead.y - screenPos.y;
			float width = height / 2.4f;
			DrawBorderBox(screenPos.x - (width / 2), screenPos.y, width, height, 2);
			std::string str = std::to_string(entities[index]->health) + "/100";
			DrawFilledRect(screenPos.x + width + 1, screenPos.y , 5 , height * entities[index]->health / 100);
		}
	}
	/*for (Character* entity : entities)
	{
		bool b = true;
		b &= WorldToScreen(entity->position, screenPos, mat, rect.right - rect.left, rect.bottom-rect.top);
		b &= WorldToScreen(entity->headPos, screenPosHead, mat, rect.right - rect.left, rect.bottom-rect.top);
		b &= (IsTeamGame() == false || entity->team != player->team);
		b &= (entity->health > 0.0f);
		if (b == true)
		{
			float height = screenPosHead.y - screenPos.y;
			float width = height / 2.4f;
			RECT rect;
			rect.left = static_cast<LONG>(screenPos.x - width / 2);
			rect.right = static_cast<LONG>(screenPos.x + width / 2);
			rect.top = static_cast<LONG>(screenPosHead.y);
			rect.bottom = static_cast<LONG>(screenPos.y);
			HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
			FillRect(hdc, &rect, hBrush);
			DeleteObject(hBrush);
		}
	}*/
}