#include "gearLoader/gearLoader_c.h"
#include "baseMod/baseMod_c.h"
#include <windows.h>
#include <stdio.h>

static const BaseMod_Api* _baseModApi;
BYTE *base;

void Patch() {
	int enable = 1;
	char *path = "bg/bg_ko.cmp";
	memcpy(base+0x5ed168,&enable,4); // Enable Korea Selection
	memcpy(base+0x576f38,&path,4); // AC Korea
	memcpy(base+0x576f88,&path,4); // Reload Korea
	return;
}

GEARLOADER_EXPORT void GEARLOADER_CALL Init(GearLoaderContext* ctx, GearLoaderApi* api) {
    base = (BYTE *)GetModuleHandle(NULL);

    const void* retApi;
    SemanticVersion retVer;
    int32_t result = api->RetrieveModApi(ctx,BASEMOD_NAME,BASEMOD_API_VERSION,&retApi,&retVer);

    if (result != 0) {
        printf("BaseMod loading failed for KoreaStage (X) - result = %d", result);
        return;
    }

    _baseModApi = (const BaseMod_Api*)retApi;

	Patch();
}

