#pragma once
class Mem
{
private:
	Mem() = default;
	~Mem() = default;

public:
	static void Create();
	static Mem* Get();
	static void Destroy();

public:
	void PatchEx(BYTE* dst, BYTE* src, UINT size, HANDLE hProcess);
	void NopEx(BYTE* dst, UINT size, HANDLE hProcess);

	void Patch(BYTE* dst, BYTE* src, UINT size);
	void Nop(BYTE* dst, UINT size);

private:
	static Mem* instance;
};

