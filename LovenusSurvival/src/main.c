#include "gearLoader/gearLoader_c.h"
#include "baseMod/baseMod_c.h"
#include <windows.h>
#include <stdio.h>

static const BaseMod_Api* _baseModApi;
BYTE *base;

void PatchSafe(void* address, void* data, size_t size) { //copied from patch.h, blame the compiler for being mean
    DWORD oldProtect;
    WINBOOL success = VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProtect);

    memcpy(address, data, size);

    success = VirtualProtect(address, size, oldProtect, &oldProtect);
}

DWORD StageTravel(void) {
	DWORD RandomSeed = (DWORD)base+0x722ae8;
	
	short TimeTravelCnt;
	memcpy(&TimeTravelCnt,base+0x6a7002,2);
	
	DWORD buf; 
	switch(TimeTravelCnt) {
		case 1:
			buf = 0x2b2d3030;
			PatchSafe(base+0x2c7ae6,&buf,4);
			buf = 0x312a2e29;
			PatchSafe(base+0x2c7aed,&buf,4);
			buf = 0x2c32362a;
			PatchSafe(base+0x2c7af4,&buf,4);
			buf = 0x322f2d2a;
			PatchSafe(base+0x2c7afb,&buf,4);
			buf = 0x33363534;
			PatchSafe(base+0x2c7b02,&buf,4);
			buf = 0x2639382e;
			PatchSafe(base+0x2c7b09,&buf,4);
			buf = 0x27;
			PatchSafe(base+0x2c7b11,&buf,1);
			break;
		case 2:
			buf = 0x17191c1c;
			PatchSafe(base+0x2c7ae6,&buf,4);
			buf = 0x1d161a15;
			PatchSafe(base+0x2c7aed,&buf,4);
			buf = 0x181d1a16;
			PatchSafe(base+0x2c7af4,&buf,4);
			buf = 0x1f1b1916;
			PatchSafe(base+0x2c7afb,&buf,4);
			buf = 0x1f222120;
			PatchSafe(base+0x2c7b02,&buf,4);
			buf = 0x2639381a;
			PatchSafe(base+0x2c7b09,&buf,4);
			buf = 0x27;
			PatchSafe(base+0x2c7b11,&buf,1);
			break;
		default:
			break;
	};
	
	return RandomSeed;
}

void DecidePal(void) {
	__asm__ (
		".intel_syntax noprefix\n\t"
		"push edx\n\t"
		"push ecx\n\t"
		"mov ecx,0x15\n\t"
		"div ecx\n\t"
		"mov edi,edx\n\t"
		"cmp edi,0xe\n\t"
		"je subtract\n\t"
		"cmp edi,0x13\n\t"
		"je subtract\n\t"
		"jmp exit\n\t"
		"subtract:\n\t"
		"sub edi,0x1\n\t"
		"exit:\n\t"
		"pop ecx\n\t"
		"pop edx\n\t"
		".att_syntax prefix\n\t"
		:
	);
}

void Patch() {
	char BeforeDarkLimit = 0x1a;
	PatchSafe(base+0x2c7cac,&BeforeDarkLimit,1);
	
	char call[8] = {0xE8};
	*(DWORD*)(call+1) = (DWORD)DecidePal - ((DWORD)base+0x2c7d56 + 5);
	call[5] = 0x90;
	call[6] = 0x90;
	call[7] = 0x90;
	PatchSafe(base+0x2c7d56,call,8);
	
	char call2[5] = {0xE8};
	*(DWORD*)(call2+1) = (DWORD)StageTravel - ((DWORD)base+0x2c82d5 + 5);
	PatchSafe(base+0x2c82d5,call2,5);
	return;
}

GEARLOADER_EXPORT void GEARLOADER_CALL Init(GearLoaderContext* ctx, GearLoaderApi* api) {
    base = (BYTE *)GetModuleHandle(NULL);

    const void* retApi;
    SemanticVersion retVer;
    int32_t result = api->RetrieveModApi(ctx,BASEMOD_NAME,BASEMOD_API_VERSION,&retApi,&retVer);

    if (result != 0) {
        printf("BaseMod loading failed for LovenusSurvival (X) - result = %d", result);
        return;
    }

    _baseModApi = (const BaseMod_Api*)retApi;

	Patch();
}

