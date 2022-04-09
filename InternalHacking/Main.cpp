#include "Global.h"
#include "Aimbot/Proc.h"
#include "Aimbot/Mem.h"
#include "Aimbot/Character.h"
#include "Aimbot/Weapon.h"
#include "Aimbot/AimbotMgr.h"
#include "ESP/ESPManager.h"

#define IDC_INFAMMO 100
#define IDC_HEALTH 101
#define IDC_RECOIL 102
#define IDC_AUTOSHOT 103
#define IDC_AIMBOT 104

#define DLL_EXPORT extern "C" __declspec(dllexport)

//Global Variable
HINSTANCE hInstance;
HWND g_hWnd;
DWORD procID;
uintptr_t moduleBase;
HANDLE hProcess;
uintptr_t engineAddress;
std::vector<uintptr_t> ammoAddress = { 0x374,0x14,0 };
int* numOfPlayers;
Character* player;
uintptr_t* entityList;
static bool bRunAimbot = false;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void RegisterWndClass();

void GetEntities();

tGetCrossHair GetCrossHair = nullptr;

bool bTrigger = false;

bool IsTeamGame()
{
	if ((*(int*)(0x50F49C) == 0 || *(int*)(0x50F49C) == 4 || *(int*)(0x50F49C) == 5 || *(int*)(0x50F49C) == 7 ||
		*(int*)(0x50F49C) == 11 || *(int*)(0x50F49C) == 13 || *(int*)(0x50F49C) == 14 || *(int*)(0x50F49C) == 16 ||
		*(int*)(0x50F49C) == 17 || *(int*)(0x50F49C) == 20 || *(int*)(0x50F49C) == 21))
		return true;
	else return false;
}

void Init()
{
	Proc::Create();
	Mem::Create();
	AimbotMgr::Create();
	ESPManager::Create();
}

void Destroy()
{
	AimbotMgr::Destroy();
	Proc::Destroy();
	Mem::Destroy();
	ESPManager::Destroy();
}

DWORD WINAPI HackThread(HMODULE hModule)
{
	hInstance = (HINSTANCE)hModule;
	MSG msg;
	RegisterWndClass();	

	g_hWnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		L"HACKING",
		L"HACKING",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		800,
		600,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	Init();

	if (g_hWnd == nullptr)
	{
		FreeLibraryAndExitThread(hModule, 0);
		return 0;
	}

	HWND hWnd = FindWindow(NULL, L"AssaultCube");

	ShowWindow(g_hWnd, SW_SHOW);

	UpdateWindow(g_hWnd);

	//모듈 아이디 얻어오기
	moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");
	numOfPlayers = (int*)(moduleBase + 0x10F500);
	player = *(Character**)(moduleBase + 0x10f4f4);
	entityList = (uintptr_t*)(moduleBase + 0x10f4f8);
	GetCrossHair = (tGetCrossHair)(moduleBase + 0x607C0);

	GetEntities();

	AimbotMgr::Get()->SetPlayer(player);
	ESPManager::Get()->SetPlayer(player);
	
	ESPManager::Get()->Run();

	while (true)
	{
		if (PeekMessage(&msg, g_hWnd, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (IsWindow(hWnd) == false)
				break;
			if (bTrigger == true)
			{
				Character* crossHair = GetCrossHair();

				if (crossHair != nullptr)
				{
					if (player->team != crossHair->team || IsTeamGame() == false)
						player->bAttack = 1;
				}
				else
					player->bAttack = 0;
			}

			if (GetAsyncKeyState(VK_LBUTTON) && bTrigger == false && bRunAimbot == true)
				AimbotMgr::Get()->TraceEntity();
			else
				AimbotMgr::Get()->ResetTarget();
		}
	}
	
	Destroy();

	FreeLibraryAndExitThread(hModule, 0);
	return (DWORD)msg.wParam;
}

void GetEntities()
{
	for (int i = 1; i <= *numOfPlayers -1; i++)
	{
		Character* entity = *(Character**)(*entityList + 0x4 * i);
		if (entity != nullptr)
		{
			AimbotMgr::Get()->AddEntity(entity, *numOfPlayers - 1);
			ESPManager::Get()->AddEntity(entity, *numOfPlayers - 1);
		}
	}
}

void RegisterWndClass()
{
	WNDCLASS wndClass;

	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = CreateSolidBrush(COLORREF(0xf0f0f0));
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = NULL;
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = L"HACKING";
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wndClass);
}

int APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
	default:
		break;
	}
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hInfiniteAmmo;
	static HWND hRecoil;
	static HWND hHealth;
	static HWND hTrigger;
	static HWND hRunAimbot;
	static bool bInfAmmo = false;
	static bool bRecoil = false;
	static bool bHealth = false;

	switch (message)
	{
	case WM_CREATE:
		hInfiniteAmmo = CreateWindow(L"BUTTON", L"InfAmmo", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 20, 20, 100, 25, hWnd, (HMENU)IDC_INFAMMO, hInstance, NULL);
		hRecoil = CreateWindow(L"BUTTON", L"No Recoil", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 20, 50, 100, 25, hWnd, (HMENU)IDC_RECOIL, hInstance, NULL);
		hHealth = CreateWindow(L"BUTTON", L"Inf Health", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 20, 80, 100, 25, hWnd, (HMENU)IDC_HEALTH, hInstance, NULL);
		hTrigger = CreateWindow(L"BUTTON", L"Auto Shoot", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 20, 110, 100, 25, hWnd, (HMENU)IDC_AUTOSHOT, hInstance, NULL);
		hRunAimbot = CreateWindow(L"BUTTON", L"Run Aimbot", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 20, 140, 100, 25, hWnd, (HMENU)IDC_AIMBOT, hInstance, NULL);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_INFAMMO:
			bInfAmmo = !bInfAmmo;
			if (bInfAmmo == true)
			{
				SendMessage(hInfiniteAmmo, BM_SETCHECK, BST_CHECKED, 0);
				Mem::Get()->Nop((BYTE*)(moduleBase + 0x637E9), 2);
			}
			else
			{
				SendMessage(hInfiniteAmmo, BM_SETCHECK, BST_UNCHECKED, 0);
				Mem::Get()->Patch((BYTE*)(moduleBase + 0x637E9), (BYTE*)("\xFF\x0E"), 2);
			}
			break;

		case IDC_RECOIL: {
			bRecoil = !bRecoil;
			if (bRecoil == true)
			{
				Mem::Get()->Nop((BYTE*)(moduleBase+0x63786),10);
				SendMessage(hRecoil, BM_SETCHECK, BST_CHECKED, 0);
			}
			else
			{
				Mem::Get()->Patch((BYTE*)(moduleBase + 0x63786), (BYTE*)("\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2"), 10);
				SendMessage(hRecoil, BM_SETCHECK, BST_UNCHECKED, 0);
			}
			}
			break;

		case IDC_HEALTH: 
			bHealth = !bHealth;
			if (bHealth == true)
			{
				Mem::Get()->Nop((BYTE*)(moduleBase + 0x2CA5E), 6);
				SendMessage(hHealth, BM_SETCHECK, BST_CHECKED, 0);
			}
			else
			{
				Mem::Get()->Patch((BYTE*)(moduleBase + 0x2CA5E), (BYTE*)("\x89\x82\xF8\x00\x00\x00"), 6);
				SendMessage(hHealth, BM_SETCHECK, BST_UNCHECKED, 0);
			}
			break;
			
		case IDC_AUTOSHOT :
			bTrigger = !bTrigger;
			if (bTrigger == true)
			{
				SendMessage(hTrigger, BM_SETCHECK, BST_CHECKED, 0);
			}
			else
			{
				SendMessage(hTrigger, BM_SETCHECK, BST_UNCHECKED, 0);
			}
			break;

		case IDC_AIMBOT:
			bRunAimbot = !bRunAimbot;
			if (bRunAimbot == true)
			{
				SendMessage(hRunAimbot, BM_SETCHECK, BST_CHECKED, 0);
			}
			else
			{
				SendMessage(hRunAimbot, BM_SETCHECK, BST_UNCHECKED, 0);
			}
			break;

		default :
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
