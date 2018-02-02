#include<Windows.h>
#include<ctime>
#include<iostream>

// Set Console screen buffer information
CONSOLE_SCREEN_BUFFER_INFO csbinfo;

/*
Displaying text on game screen
Function Address = 004090F0
Parameters		 = (char*, char*)
return type		 = void or not used
Calling Convention = __cdecl
*/
typedef void(__cdecl * GameDisp)(const char*, const char*);

// Health
bool  bHealthStatus = 0;
DWORD dwHealthAddr = 0x00429D1F;
BYTE  HealthDefOpcode[] = { 0x29, 0x7B, 0x04 };
BYTE  HealthNewOpcode[] = { 0x90, 0x90, 0x90 };

// Ammo
bool  bAmmoStatus = 0;
DWORD dwAmmoAddr = 0x004637E9;
BYTE  AmmoDefOpcode[] = { 0xFF, 0x0E};
BYTE  AmmoNewOpcode[] = { 0x90, 0x90 };

// Rapid Fire
bool  bRapidFireStatus = 0;
DWORD dwRapidFireAddr = 0x0046367F;
BYTE  RapidFireDefOpcode[] = { 0xE8, 0x1C, 0xBF, 0xFF, 0xFF };
BYTE  RapidFireNewOpcode[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };

// Automatic Fire
bool  bAutomaticFireStatus = 0;
DWORD dwAutomaticFireAddr = 0x00463716;
BYTE  AutomaticFireDefOpcode[] = { 0x75, 0x09 };
BYTE  AutomaticFireNewOpcode[] = { 0xEB, 0x09 };

// No Push Back while shooting
DWORD dwNoPushBackAddr = 0x00463781;
BYTE  NoPushBackNewOpcode[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

// Weapons values
DWORD dwPlayerEntAddr = 0x00509B74;
DWORD dwMachineGunAmmoOffset = 0x150;
DWORD dwPistolAmmoOffset = 0x13C;
DWORD dwGrenadesAmmoOffset = 0x158;
DWORD dwBodyArmorOffset = 0xFC;
DWORD dwPlayerHealthOffset = 0xF8;

HMODULE hgDllHandle = NULL;

void  PrepareConsole(const char*);
void  WriteToMemory(DWORD, BYTE*, int);
DWORD WINAPI OnInject(void*);

__declspec(dllexport) BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		hgDllHandle = hModule;
		DisableThreadLibraryCalls(hModule);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)OnInject, (LPVOID)NULL, NULL, NULL);
	}

	return true;
}

void WriteToMemory(DWORD memAddr, BYTE* val, int valSize)
{
	DWORD oldProtection;

	VirtualProtect((LPVOID)memAddr, valSize, PAGE_READWRITE, &oldProtection);
	memcpy((LPVOID)memAddr, val, valSize);
	VirtualProtect((LPVOID)memAddr, valSize, oldProtection, NULL);
}

DWORD WINAPI OnInject(void* params)
{
	DWORD dwTemp = NULL;
	DWORD dwTempAddr = NULL;
	DWORD dwPlayerBase = NULL;
	DWORD dwButtonTMR = clock();
	DWORD dwConsoleTMR = clock();
	DWORD dwModuleBase = (DWORD)GetModuleHandle(NULL);
	BYTE  HealthVal[] = { 0x27, 0x10 };
	DWORD   currHealth = 0;
	std::string sConsoleBuffer = "";
	std::string sAmmoStatus = "OFF";
	std::string sHealthStatus = "OFF";
	std::string sRapidFireStatus = "OFF";
	std::string sAutomaticFireStatus = "OFF";

	// Set up ingame text display
	GameDisp Display = (GameDisp)(dwModuleBase + 0x90F0);

	// Start a new console
	//PrepareConsole("Debug Console");

	// Disable Push Back's while shooting
	WriteToMemory(dwNoPushBackAddr, NoPushBackNewOpcode, sizeof(NoPushBackNewOpcode));

	while (!GetAsyncKeyState(VK_INSERT))
	{
		// Read current health value continuously
		memcpy(&dwTemp, (const void*)dwPlayerEntAddr, sizeof(dwPlayerEntAddr));
		dwTemp = dwTemp + dwPlayerHealthOffset;
		memcpy(&currHealth, (const void*)dwTemp, sizeof(dwTemp));

		// Patch health if user enabled cheat hotkey
		if (currHealth < 5000 & bHealthStatus)
		{
			WriteToMemory(dwTemp, HealthVal, sizeof(HealthVal));
		}
		if (clock() - dwButtonTMR > 400)
		{
			dwButtonTMR = clock();

			// Health = F1
			if (GetAsyncKeyState(VK_F1))
			{
				bHealthStatus = !bHealthStatus;
				if (bHealthStatus)
					Display("%s [ON]", "Infinite Health:");
				else
					Display("%s [OFF]", "Infinite Health:");
			}
			// Ammo = F2
			else if (GetAsyncKeyState(VK_F2))
			{
				bAmmoStatus = !bAmmoStatus;
				if (bAmmoStatus)
				{
					Display("%s [ON]", "Infinite Ammo:");
					WriteToMemory(dwAmmoAddr, AmmoNewOpcode, sizeof(AmmoNewOpcode));

				}
				else
				{
					Display("%s [OFF]", "Infinite Ammo:");
					WriteToMemory(dwAmmoAddr, AmmoDefOpcode, sizeof(AmmoDefOpcode));
				}
			}
			// Rapid Fire = F3
			else if (GetAsyncKeyState(VK_F3))
			{
				bRapidFireStatus = !bRapidFireStatus;
				if (bRapidFireStatus)
				{
					WriteToMemory(dwRapidFireAddr, RapidFireNewOpcode, sizeof(RapidFireNewOpcode));
					Display("%s [ON]", "Rapid Fire:");
				}
				else
				{
					WriteToMemory(dwRapidFireAddr, RapidFireDefOpcode, sizeof(RapidFireDefOpcode));
					Display("%s [OFF]", "Rapid Fire:");
				}
			}
			// Automatic Fire = F4
			else if (GetAsyncKeyState(VK_F4))
			{
				bAutomaticFireStatus = !bAutomaticFireStatus;
				if (bAutomaticFireStatus)
				{
					WriteToMemory(dwAutomaticFireAddr, AutomaticFireNewOpcode, sizeof(AutomaticFireNewOpcode));
					Display("%s [ON]", "Automatic Weapons:");
				}
				else
				{
					WriteToMemory(dwAutomaticFireAddr, AutomaticFireDefOpcode, sizeof(AutomaticFireDefOpcode));
					Display("%s [OFF]", "Automatic Weapons:");
				}	
			}
		}
	}
	return 0;
}

void PrepareConsole(const char* title)
{
	FILE *fp = 0;
	HANDLE hStdout = NULL;

	// Start a new console
	AllocConsole();

	// Set console title
	SetConsoleTitle(title);

	// Redirect unbuffered stdout to the console
	freopen_s(&fp, "CONOUT$", "w", stdout);

	// Save the current console text buffer information
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdout == INVALID_HANDLE_VALUE)
	{
		std::cout << "[!] Invalid 'stdout' handle value!" << std::endl;
		exit(-1);
	}

	if (!GetConsoleScreenBufferInfo(hStdout, &csbinfo))
	{
		std::cout << "[!] Couldn't get console screen buffer information!" << std::endl;
		exit(-1);
	}

}

