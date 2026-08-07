#include "gearLoader/gearLoader_c.h"
#include "baseMod/baseMod_c.h"

#include <stdio.h>
#include <math.h>

typedef struct UserHookContext {
    const BaseMod_Api* baseModApi;
} UserHookContext;

static BaseMod_HookId _DITHookId;
static const BaseMod_Api* _baseModApi;
static UserHookContext _DITCtx;

void __stdcall DisplayIt(void* userData, const BaseMod_HookContext* ctx, const BaseMod_GameUpdateInfo* info) {
    UserHookContext* userCtx = (UserHookContext*)userData;
    const BaseMod_Api* api = userCtx->baseModApi;

    GGXXACPR_Entity* p1 = api->GameData->getPlayer(0);
    GGXXACPR_Entity* p2 = api->GameData->getPlayer(1);
    int isInGame = api->GameData->isInGame();

    if (isInGame > 0) {
        if (p1 && p1->id == ENTITY_ID_SOL) {
            int16_t frames = p1->playerEntityDataPtr->charSpecificVar;
            char str[16];
            snprintf(str, 16, "DI: %d%s", (int)ceil((frames)/60.0f), "s");
            api->NativeFunctions->renderText(str, 20, 420, 2.0f, 0xFF, 1.0f);
        }
        if (p2 && p2->id == ENTITY_ID_SOL) {
            int16_t frames = p2->playerEntityDataPtr->charSpecificVar;
            char str[16];
            snprintf(str, 16, "DI: %d%s", (int)ceil((frames)/60.0f), "s");
            api->NativeFunctions->renderText(str, 520, 420, 2.0f, 0xFF, 1.0f);
        }
    } 
}

GEAR_LOADER_EXPORT void GEAR_LOADER_CALL Init(GearLoaderContext* ctx, GearLoaderApi* api) {
    const void* retApi;
    SemanticVersion retVer;
    int32_t result = api->RetrieveModApi(ctx,BASEMOD_NAME,">=0.1.0",&retApi,&retVer);

    if (result > 0) {
        return;    
    }

    _baseModApi = (const BaseMod_Api*)retApi;
    _DITCtx = (UserHookContext){_baseModApi};
    _DITHookId = _baseModApi->Hooks->afterGameUpdate(DisplayIt, &_DITCtx);
}
