#include "gearLoader/gearLoader_c.h"
#include "baseMod/baseMod_c.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

static const BaseMod_Api* _baseModApi;

BYTE *base;
uint32_t *DebugFlag;

uint32_t flag1 = 0;
uint32_t flag2 = 0;
uint32_t flag3 = 0;
uint32_t flag4 = 0;
uint32_t flag5 = 0;
uint32_t flag6 = 0;
uint32_t flag7 = 0;

typedef struct {
    char* name;
    uint32_t* value;
} setting;

setting settings[1];

void PatchSafe(void* address, void* data, size_t size) { //copied from patch.h, blame the compiler for being mean
    DWORD oldProtect;
    WINBOOL success = VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProtect);

    memcpy(address, data, size);

    success = VirtualProtect(address, size, oldProtect, &oldProtect);
}

__attribute__ ((stdcall)) void apply() {
    uint32_t newFlag = 0;
    newFlag |= flag1;
    newFlag |= 0x4 * flag2;
    newFlag |= 0x10 * flag3;
    newFlag |= 0x80 * flag4;
    newFlag |= 0x200 * flag5;
    newFlag |= 0x400 * flag6;
    newFlag |= 0x800 * flag7;
    PatchSafe(DebugFlag,&newFlag,4);
}

GEARLOADER_EXPORT void GEARLOADER_CALL Init(GearLoaderContext* ctx, GearLoaderApi* api) {
    base = (BYTE *)GetModuleHandle(NULL);
    DebugFlag = (uint32_t*)(base+0x7109e0);

    const void* retApi;
    SemanticVersion retVer;
    int32_t result = api->RetrieveModApi(ctx,BASEMOD_NAME,BASEMOD_API_VERSION,&retApi,&retVer);

    if (result != 0) {
        printf("BaseMod loading failed for GuiltyDebug (X) - result = %d\n", result);
        return;
    }

    _baseModApi = (const BaseMod_Api*)retApi;
 
    static const char *toggle[2] = {"NO","YES"};
    static BaseMod_ModMenuEntry menuEntry[8] = {
        {"Slowdown",&flag1,0,1,toggle,NULL,NULL},
        {"Flag 2",&flag2,0,1,toggle,NULL,NULL},
        {"Flag 3",&flag3,0,1,toggle,NULL,NULL},
        {"Flag 4",&flag4,0,1,toggle,NULL,NULL},
        {"Immortality",&flag5,0,1,toggle,NULL,NULL},
        {"Inf Tension",&flag6,0,1,toggle,NULL,NULL},
        {"Flag 7",&flag7,0,1,toggle,NULL,NULL},
        {"Apply",NULL,0,0,NULL,apply,NULL}
    };
    _baseModApi->ModMenu->RegisterMenuTab("DEBUG",menuEntry,8);
    printf("[GuiltyDebug] Hello World!\n");
}


