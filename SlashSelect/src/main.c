#include "gearLoader/gearLoader_c.h"
#include "baseMod/baseMod_c.h"
#include <windows.h>
#include <stdio.h>

static const BaseMod_Api* _baseModApi;
BYTE *base;
int slash_selectmain;

void Patch(void* address, void* data, size_t size, void* overWrittenBytes) { //copied from patch.h, blame the compiler for being mean
    DWORD oldProtect;
    WINBOOL success = VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProtect);

    if (!success) {
        // err handling?
    }

    if (overWrittenBytes) {
        memcpy(overWrittenBytes, address, size);
    }
    memcpy(address, data, size);

    success = VirtualProtect(address, size, oldProtect, &oldProtect);

    if (!success) {
        // err handling?
    }
}

GEARLOADER_EXPORT void GEARLOADER_CALL Init(GearLoaderContext* ctx, GearLoaderApi* api) {
    base = (BYTE *)GetModuleHandle(NULL);

    const void* retApi;
    SemanticVersion retVer;
    int32_t result = api->RetrieveModApi(ctx,BASEMOD_NAME,BASEMOD_API_VERSION,&retApi,&retVer);

    if (result != 0) {
        printf("BaseMod loading failed for SlashSelect (X) - result = %d", result);
        return;
    }

    _baseModApi = (const BaseMod_Api*)retApi;

    slash_selectmain = base+0x001b9cc0;
	Patch(base+0x1bda55,&slash_selectmain,4,NULL);
}
