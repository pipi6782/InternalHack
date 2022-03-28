#include "Global.h"
#include "Aimbot\Character.h"
#include "ESPManager.h"

ESPManager* ESPManager::instance = nullptr;

ESPManager::ESPManager()
{
	hWnd = FindWindow(NULL, L"AssaultCube");
	hdc = GetDC(hWnd);
	/*for (int i = 0; i < 4; i++)
	{
		espThreads.push_back(std::thread(&ESPManager::RunEspThread));
	}*/

}

ESPManager::~ESPManager()
{
	/*for (int i = 0; i < 4; i++)
	{
		espThreads[i].join();
	}*/
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

void ESPManager::RunEspThread()
{
	float* mat = (float*)(0x501AE8);
	RECT rect;
	GetClientRect(hWnd, &rect);
	Vector2 screenPos, screenPosHead;
	for (Character* entity : entities)
	{
		bool b = true;
		b &= WorldToScreen(entity->position, screenPos, mat, rect.right - rect.left, rect.bottom-rect.top);
		b &= WorldToScreen(entity->headPos, screenPosHead, mat, rect.right - rect.left, rect.bottom-rect.top);
		b &= (IsTeamGame() == false || entity->team != player->team);
		if (b == true)
		{
			float height = screenPosHead.y - screenPos.y;
			float width = height / 2.4f;
			RECT rect;
			rect.left = static_cast<LONG>(screenPos.x - width / 2);
			rect.right = static_cast<LONG>(screenPos.x + width / 2);
			rect.top = static_cast<LONG>(screenPosHead.y);
			rect.bottom = static_cast<LONG>(screenPos.y);
			FillRect(hdc, &rect, CreateSolidBrush(COLORREF(RGB(255,0,0))));
		}
	}
}