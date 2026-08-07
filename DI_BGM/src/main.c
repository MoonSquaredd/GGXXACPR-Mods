#include "gearLoader/gearLoader_c.h"
#include "baseMod/baseMod_c.h"
#include <windows.h>
#include <stdio.h>

static const BaseMod_Api* _baseModApi;
BYTE *base;
BYTE *patchLocation;
BYTE *StopBGM;
BYTE *RequestBGM;
BYTE *DIBGM_Back;

void DI_BGM() { // made it in assembly because of parameters in register
	__asm__ (
		".intel_syntax noprefix\n\t"
		"push eax\n\t"
		"call [%0]\n\t" // stops the current music
		"mov eax,0x12\n\t"
		"push 0x1\n\t"
		"call [%1]\n\t" // starts ride the fire
		"add esp,4\n\t"
		"pop eax\n\t"
		"push esi\n\t" // original overwritten code
		"mov esi,[ebp+0x8]\n\t"
		"jmp [%2]\n\t" // goes back
		".att_syntax prefix\n\t"
		:
		: "m"(StopBGM), "m"(RequestBGM), "m"(DIBGM_Back)
	);
}

void Patch() {
	DWORD oldProtect;
	char jump[7] = {0xE9};
	*(DWORD*)(jump+1) = (DWORD)DI_BGM - ((DWORD)patchLocation + 5);
	jump[5] = 0x90;
	jump[6] = 0x90;
	VirtualProtect(patchLocation,7,PAGE_EXECUTE_READWRITE,&oldProtect);
	memcpy(patchLocation,jump,7);
	VirtualProtect(patchLocation,7,oldProtect,&oldProtect);
	return;
}

GEARLOADER_EXPORT void GEARLOADER_CALL Init(GearLoaderContext* ctx, GearLoaderApi* api) {
    base = (BYTE *)GetModuleHandle(NULL);

    const void* retApi;
    SemanticVersion retVer;
    int32_t result = api->RetrieveModApi(ctx,BASEMOD_NAME,BASEMOD_API_VERSION,&retApi,&retVer);

    if (result != 0) {
        printf("BaseMod loading failed for DI_BGM (X) - result = %d", result);
        return;
    }

    _baseModApi = (const BaseMod_Api*)retApi;

    patchLocation = base+0x251570;
    StopBGM = base+0x11eb50;
    RequestBGM = base+0x11ec40;
    DIBGM_Back = patchLocation+7;
    Patch();
}

