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
DWORD GSD_ADDR = 0x0010d1a0;

static const BaseMod_Api* _baseModApi;
static UserHookContext _userCtx;
static BaseMod_HookId _jumpscareHookId;

bool jumpin = false;
BYTE *base;
unsigned int LFF;
unsigned int gSpriteDraw;
FILE *fptr;
int *spritesAddr;
GGXXACPR_DrawSpriteParams arg;
GGXXACPR_DrawSpriteParams *argp;
unsigned char *buf;
unsigned char *wav;
int cnt = 0;
int jframes = 0;
int rng;

void drawsprite() {
	__asm__ (
		".intel_syntax noprefix\n\t"
		"push ecx\n\t"
		"mov ecx,[%0]\n\t"
		"push 0x0\n\t"
		"call [%1]\n\t"
		"add esp,4\n\t"
		"pop ecx\n\t"
		".att_syntax prefix\n\t"
		:
		: "m"(argp), "m"(gSpriteDraw)
	);
}

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
        return;    
    }

	srand(time(NULL));
	rng = rand();
	if (jumpin == false) {
		if ((cnt % 60) == 0) {
			if ((rng % 10000) == 0) {
				jumpin = true;
				PlaySound(wav,NULL,SND_MEMORY|SND_ASYNC);
			}
		}
	} else {
		lff_call(jframes);
		arg.spriteId = jframes;
		arg.x = 0.0;
		arg.y = 480.0;
		arg.z = 0.0;
		arg.alpha = 1.0;
		arg.attribute = 525;
		arg.zm_x = 3.2;
		arg.zm_y = 3.2;
		arg.listType = 1;
		arg.angle = 0;
		arg.u1 = 1.0;
		arg.v1 = 1.0;
		arg.u0 = 0.0;
		arg.v0 = 0.0;
		argp = &arg;
        api->NativeFunctions->DrawSprite(argp,0);
		//drawsprite();
		if ((cnt % 3) == 0) {
			jframes += 1;
		}
		if (jframes >= 14) {
			jframes = 0;
			jumpin = false;
		}
	}
	cnt += 1;
}

void Patch() {
	LFF = (unsigned int)base+LFF_ADDR;
	gSpriteDraw = (unsigned int)base+GSD_ADDR;
    fptr = fopen("Resource/se/jumpscare.wav","rb");
    if (fptr != NULL) {
		fseek(fptr,0,SEEK_END);
		int size = ftell(fptr);
        wav = malloc(size);
		fseek(fptr,0,SEEK_SET);
		fread(wav,size,1,fptr);
        fclose(fptr);
    }
	fptr = fopen("Resource/demo/foxy.bin","rb");
	if (fptr != NULL) {
		fseek(fptr,0,SEEK_END);
		int size = ftell(fptr);
        buf = malloc(size);
		fseek(fptr,0,SEEK_SET);
		fread(buf,size,1,fptr);
		
		spritesAddr = (int*)buf;
		
		int i = 0;
		while (true) {
			int ptr = spritesAddr[i];
			if (ptr == 0xFFFFFFFF) {
				spritesAddr[i] = 0;
				break;
			} else {
				spritesAddr[i] = ptr + (int)spritesAddr;
			}
			i++;
		}
		fclose(fptr);
	} else {
		return;
	}
    _jumpscareHookId = _baseModApi->Hooks->AfterGameUpdate(mimde, &_userCtx);
	//AssignToGameLoop(mimde);
	return;
}

GEARLOADER_EXPORT void GEARLOADER_CALL Init(GearLoaderContext* ctx, GearLoaderApi* api) {
    base = (BYTE *)GetModuleHandle(NULL);

    const void* retApi;
    SemanticVersion retVer;
    int32_t result = api->RetrieveModApi(ctx,BASEMOD_NAME,BASEMOD_API_VERSION,&retApi,&retVer);

    if (result != 0) {
        printf("BaseMod loading failed for FoxyJumpscare (X) - result = %d", result);
        return;
    }

    _baseModApi = (const BaseMod_Api*)retApi;
    _userCtx = (UserHookContext){_baseModApi};

	Patch();
}

