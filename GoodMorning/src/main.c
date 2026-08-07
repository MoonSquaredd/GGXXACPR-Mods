#include "gearLoader/gearLoader_c.h"
#include "baseMod/baseMod_c.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct UserHookContext {
    const BaseMod_Api* baseModApi;
} UserHookContext;

static const BaseMod_Api* _baseModApi;
static UserHookContext _userCtx;
static BaseMod_HookId _bootHookId;

bool greeted = false;
BYTE *base;

void __stdcall booty(void* userData, const BaseMod_HookContext* ctx, const BaseMod_GameUpdateInfo* info) {
    UserHookContext* userCtx = (UserHookContext*)userData;
    const BaseMod_Api* api = userCtx->baseModApi;

    if (greeted == false) {
        PlaySound("Resource/se/boot.wav",NULL,SND_FILENAME|SND_ASYNC);
        greeted = true;
    }
    api->Hooks->RemoveHook(_bootHookId);
}

GEARLOADER_EXPORT void GEARLOADER_CALL Init(GearLoaderContext* ctx, GearLoaderApi* api) {
    base = (BYTE *)GetModuleHandle(NULL);

    const void* retApi;
    SemanticVersion retVer;
    int32_t result = api->RetrieveModApi(ctx,BASEMOD_NAME,BASEMOD_API_VERSION,&retApi,&retVer);

    if (result != 0) {
        printf("BaseMod loading failed for GoodMorning (X) - result = %d", result);
        return;
    }

    _baseModApi = (const BaseMod_Api*)retApi;
    _userCtx = (UserHookContext){_baseModApi};

	_bootHookId = _baseModApi->Hooks->BeforeGameUpdate(booty, &_userCtx);
}


