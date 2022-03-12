#pragma once
#include "Defines.h"
#include <iostream>
#include <vector>
#include <windows.h>
#include <TlHelp32.h>
#include <unordered_map>
#include <thread>

//Direct3D
#include <dxgi1_2.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

using namespace std;

typedef D3DXVECTOR3 Vec3;
struct Matrix : public D3DXMATRIX
{
public:
	Matrix(): D3DXMATRIX(){}

	Matrix(Vec3 v1, Vec3 v2, Vec3 v3)
	{
		_11 = v1.x; _12 = v1.y; _13 = v1.z; _14 = 0.0f;
		_21 = v2.x;	_22 = v2.y; _32 = v2.z; _24 = 0.0f;
		_31 = v3.x; _32 = v3.y; _33 = v3.z; _34 = 0.0f;
		_41 = 0.0f; _42 = 0.0f; _43 = 0.0f; _44 = 1.0f;
	}
};