#include "Global.h"
#include "Proc.h"
#include "Mem.h"
#include "Character.h"
#include "Weapon.h"
#include "AimbotMgr.h"

#define IDC_INFAMMO 100
#define IDC_HEALTH 101
#define IDC_RECOIL 102
#define IDC_AUTOSHOT 103

#define DLL_EXPORT extern "C" __declspec(dllexport)

//Global Variable
HINSTANCE hInstance;
HWND g_hWnd;
DWORD procID;
uintptr_t moduleBase;
HANDLE hProcess;
uintptr_t engineAddress;
vector<uintptr_t> ammoAddress = { 0x374,0x14,0 };


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void RegisterWndClass();

typedef Character* (__cdecl* tGetCrossHair)();

tGetCrossHair GetCrossHair = nullptr;

bool bTrigger = false;

DWORD WINAPI HackThread(HMODULE hModule)
{
	Proc::Create();
	Mem::Create();
	AimbotMgr::Create();
	hInstance = hModule;
	MSG msg;
	RegisterWndClass();	
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	g_hWnd = CreateWindow(
		L"Hacking",
		L"Hacking",
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

	if (g_hWnd == nullptr)
	{
		FreeLibraryAndExitThread(hModule, 0);
		return 0;
	}

	ShowWindow(g_hWnd, SW_SHOW);

	UpdateWindow(g_hWnd);

	//모듈 아이디 얻어오기
	moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");
	int* numOfPlayers = (int*)(moduleBase + 0x10F500);
	Character* player = *(Character**)(moduleBase + 0x10f4f4);
	uintptr_t* entityList = (uintptr_t*)(moduleBase + 0x10f4f8);
	GetCrossHair = (tGetCrossHair)(moduleBase + 0x607C0);

	for (int i = 1; i < *numOfPlayers; i++)
	{
		Character* entity = *(Character**)(*entityList + 0x4 * i);
		if (entity != nullptr)
			if (player->team != entity->team)
				AimbotMgr::Get()->AddEntity(entity);
	}
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
			if (bTrigger == true)
			{
				Character* crossHair = GetCrossHair();

				if (crossHair != nullptr)
				{
					if (player->team != crossHair->team)
						player->bAttack = 1;
				}
				else
					player->bAttack = 0;
			}
			if (GetAsyncKeyState(VK_LBUTTON) && bTrigger == false)
				AimbotMgr::Get()->TraceEntity(player);

			system("cls");
		}
	}
	
	AimbotMgr::Destroy();
	Proc::Destroy();
	Mem::Destroy();
	
	fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
	return (DWORD)msg.wParam;
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
	wndClass.lpszClassName = L"Hacking";
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
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT :
			AimbotMgr::Get()->ChangeTarget(EChangeType::Prev);
			break;

		case VK_RIGHT :
			AimbotMgr::Get()->ChangeTarget(EChangeType::Next);
			break;
		}
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
