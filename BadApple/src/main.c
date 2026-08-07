#include "gearLoader/gearLoader_c.h"
#include "baseMod/baseMod_c.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

typedef struct UserHookContext {
    const BaseMod_Api* baseModApi;
} UserHookContext;

DWORD LFF_ADDR = 0x00113820;

static const BaseMod_Api* _baseModApi;
static UserHookContext _userCtx;
static BaseMod_HookId _jumpscareHookId;

bool jumpin = false;
BYTE *base;
unsigned int LFF;
FILE *fptr;
int *spritesAddr;
GGXXACPR_DrawSpriteParams arg;
GGXXACPR_DrawSpriteParams *argp;
unsigned char *buffers[7];
unsigned char *buf;
int cnt = 0;
int jframes = 0;

void lff_call() {
	__asm__ (
		".intel_syntax noprefix\n\t"
		"push eax\n\t"
		"mov eax,0x0\n\t"
		"push 0x0\n\t"
		"push [%0]\n\t"
		"call [%1]\n\t"
		"add esp,8\n\t"
		"pop eax\n\t"
		".att_syntax prefix\n\t"
		:
		: "m"(spritesAddr), "m"(LFF)
	);
}

void __stdcall mimde(void* userData, const BaseMod_HookContext* ctx, const BaseMod_GameUpdateInfo* info) {
    UserHookContext* userCtx = (UserHookContext*)userData;
    const BaseMod_Api* api = userCtx->baseModApi;

    int isInGame = api->GameData->IsInGame();
    
    if (isInGame == 0) {
        jframes = 0;
        jumpin = false;
        return;    
    }

	if (jumpin == false) {	
	    jumpin = true;
	} else {
        spritesAddr = (int*)buffers[jframes/500];
        int sprId = jframes - ((jframes/500) * 500);
		lff_call(sprId);
		arg.spriteId = sprId;
		arg.x = 0.0;
		arg.y = 480.0;
		arg.z = 0.0;
		arg.alpha = 1.0;
		arg.attribute = 525;
		arg.zm_x = 1.334;
		arg.zm_y = 1.334;
		arg.listType = 1;
		arg.angle = 0;
		arg.u1 = 1.0;
		arg.v1 = 1.0;
		arg.u0 = 0.0;
		arg.v0 = 0.0;
		argp = &arg;
        api->NativeFunctions->DrawSprite(argp,0);
		if ((cnt % 4) == 0) {
			jframes += 1;
		}
		if (jframes >= 3288) {
			jframes = 0;
			jumpin = false;
		}
	}
	cnt += 1;
}

void Patch() {
    LFF = (unsigned int)base+LFF_ADDR;

    for (int i = 0; i < 7; i++) {
        char fname[MAX_PATH];
        sprintf(fname,"Resource/demo/badapple%d.bin",i);
        fptr = fopen(fname,"rb");
        if (fptr != NULL) {
		    fseek(fptr,0,SEEK_END);
		    int size = ftell(fptr);
            buffers[i] = malloc(size);
		    fseek(fptr,0,SEEK_SET);
		    fread(buffers[i],size,1,fptr);

            spritesAddr = (int*)buffers[i];

            int j = 0;
		    while (true) {
			    int ptr = spritesAddr[j];
			    if (ptr == 0xFFFFFFFF) {
				    spritesAddr[j] = 0;
				    break;
			    } else {
				    spritesAddr[j] = ptr + (int)spritesAddr;
			    }
			    j++;
		    }
		    fclose(fptr);
        } else {
		    return;
	    }
	}
    spritesAddr = (int*)buffers[0];
    _jumpscareHookId = _baseModApi->Hooks->AfterGameUpdate(mimde, &_userCtx);
	return;
}

GEARLOADER_EXPORT void GEARLOADER_CALL Init(GearLoaderContext* ctx, GearLoaderApi* api) {
    base = (BYTE *)GetModuleHandle(NULL);

    const void* retApi;
    SemanticVersion retVer;
    int32_t result = api->RetrieveModApi(ctx,BASEMOD_NAME,BASEMOD_API_VERSION,&retApi,&retVer);

    if (result != 0) {
        printf("BaseMod loading failed for BadApple (X) - result = %d", result);
        return;
    }

    _baseModApi = (const BaseMod_Api*)retApi;
    _userCtx = (UserHookContext){_baseModApi};

	Patch();
}

