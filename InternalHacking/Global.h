#pragma once
#include "Defines.h"
#include <assert.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include <windows.h>
#include <TlHelp32.h>
#include <unordered_map>
#include <thread>
#include <functional>
#include <algorithm>
#include <mutex>

#include <D3DX10math.h>

typedef D3DXMATRIX Matrix;
typedef D3DXVECTOR2 Vector2;
typedef D3DXVECTOR3 Vector3;
typedef D3DXVECTOR4 Vector4;
typedef class Character* (__cdecl* tGetCrossHair)();