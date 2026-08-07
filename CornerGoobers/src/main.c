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
static BaseMod_HookId _gooberHookId;

BYTE *base;
GGXXACPR_DrawSpriteParams arg;
GGXXACPR_DrawSpriteParams *argp;
bool once = false;
int cnt = 0;

unsigned int ISTL;
int *texadr;
int sprid;
int palid;

void IST_Load() {
	__asm__ (
		".intel_syntax noprefix\n\t"
		"push edi\n\t"
        "push eax\n\t"
		"lea edi,[%0]\n\t"
		"push [%1]\n\t"
        "mov eax,[%2]\n\t"
		"call [%3]\n\t"
		"add esp,4\n\t"
		"pop eax\n\t"
        "pop edi\n\t"
		".att_syntax prefix\n\t"
		:
		: "m"(texadr), "m"(sprid), "m"(palid), "m"(ISTL)
	);
}

void __stdcall goobing(void* userData, const BaseMod_HookContext* ctx, const BaseMod_GameUpdateInfo* info) {
    UserHookContext* userCtx = (UserHookContext*)userData;
    const BaseMod_Api* api = userCtx->baseModApi;

    int isInGame = api->GameData->IsInGame();
    
    if (isInGame == 0) {
        once = false;
        return;    
    }

    GGXXACPR_Entity *p1;
    GGXXACPR_Entity *p2;
    p1 = api->GameData->GetPlayer(0);
    p2 = api->GameData->GetPlayer(1);

    if (p1 == NULL) {
        return;
    }

    texadr = (int *)(p1->cellTop + p1->imageNo);
    sprid = (int)(p1->spriteId + p1->spriteIdTop);
    palid = -1;

    IST_Load();

    arg.spriteId = p1->spriteId + p1->spriteIdTop;
	arg.x = 320.0;
	arg.y = 240.0;
	arg.z = 1.0;
	arg.alpha = 1.0;
	arg.attribute = 0xd;
	arg.zm_x = 1.0;
	arg.zm_y = 1.0;
	arg.listType = 1;
	arg.angle = 0;
	arg.u1 = 1.0;
	arg.v1 = 1.0;
	arg.u0 = 0.0;
	arg.v0 = 0.0;
	argp = &arg;
    api->NativeFunctions->DrawSprite(argp,0);

    if (cnt % 600 == 0) {
        if (p1 != NULL) {
            printf("P1:\n");
            printf("imageNo:       %d\n",p1->imageNo);
            printf("spriteId:      %d\n",p1->spriteId);
            printf("spriteIdTop:   %d\n",p1->spriteIdTop);
            printf("fbSpriteIdTop: %d\n",p1->fbSpriteIdTop);
        }
        if (p2 != NULL) {
            printf("P2:\n");
            printf("imageNo:       %d\n",p2->imageNo);
            printf("spriteId:      %d\n",p2->spriteId);
            printf("spriteIdTop:   %d\n",p2->spriteIdTop);
            printf("fbSpriteIdTop: %d\n",p2->fbSpriteIdTop);
        }
        once = true;
    }
    cnt += 1;
}

GEARLOADER_EXPORT void GEARLOADER_CALL Init(GearLoaderContext* ctx, GearLoaderApi* api) {
    base = (BYTE *)GetModuleHandle(NULL);

    const void* retApi;
    SemanticVersion retVer;
    int32_t result = api->RetrieveModApi(ctx,BASEMOD_NAME,BASEMOD_API_VERSION,&retApi,&retVer);

    if (result != 0) {
        printf("BaseMod loading failed for CornerGoobers (X) - result = %d", result);
        return;
    }
    ISTL = (unsigned int)base+0x109000;
    _baseModApi = (const BaseMod_Api*)retApi;
    _userCtx = (UserHookContext){_baseModApi};
	_gooberHookId = _baseModApi->Hooks->AfterGameUpdate(goobing, &_userCtx);
}


