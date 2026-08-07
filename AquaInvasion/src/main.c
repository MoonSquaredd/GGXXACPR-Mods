#include "gearLoader/gearLoader_c.h"
#include "baseMod/baseMod_c.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

typedef struct UserHookContext {
    const BaseMod_Api* baseModApi;
} UserHookContext;

typedef struct aquaObj {
    uint8_t animation;
    uint8_t frame;
    uint8_t duration;
    uint8_t state;
    int16_t position[2];
    float direction[2];
} aquaObj;
aquaObj aquaPool[256];
int aquaCnt = 0;

uint8_t animDurById[2] = {5,8};
uint8_t frameCntById[2] = {4,7};

static const BaseMod_Api* _baseModApi;
static UserHookContext _userCtx;
static BaseMod_HookId _aquaHookId;

uint16_t xmin = 0;
uint16_t ymin = 70;
uint16_t xmax = 578;
uint16_t ymax = 480;

uint32_t draw = 0;
uint32_t rest = 0;
uint32_t frequency = 10000;
uint32_t freqId = 3;
uint32_t speed = 1;
uint32_t amount = 256;

typedef struct {
    char* name;
    uint32_t* value;
} setting;

setting settings[] = {
    {"draw",&draw},
    {"amount",&amount},
    {"frequency",&freqId},
    {"speed",&speed},
    {"rest",&rest},
};

BYTE *base;
FILE *fptr;
int *spritesAddr;
GGXXACPR_DrawSpriteParams arg;
GGXXACPR_DrawSpriteParams *argp;
unsigned char *buf;
uint32_t cnt = 0;

int cornerCheck(aquaObj* obj) {
    if (obj->position[0] <= xmin && obj->position[1] <= ymin) {
        return 1;
    };
    if (obj->position[0] <= xmin && obj->position[1] >= ymax) {
        return 1;
    };
    if (obj->position[0] >= xmax && obj->position[1] <= ymin) {
        return 1;
    }; 
    if (obj->position[0] >= xmax && obj->position[1] >= ymax) {
        return 1;
    }
    return 0;
}

void __stdcall aquaHandle(void* userData, const BaseMod_HookContext* ctx, const BaseMod_GameUpdateInfo* info) {
    UserHookContext* userCtx = (UserHookContext*)userData;
    const BaseMod_Api* api = userCtx->baseModApi;

    int32_t isInGame = api->GameData->IsInGame();
    
    if (isInGame == 0) {
        return;    
    }

    int32_t *isPaused = api->GameData->GetPauseState();

    int rng = rand();
    if ((cnt % 60) == 0 && frequency != 0) {
		if ((rng % frequency) == 0 && aquaCnt < amount) {
            aquaObj newAqua = {
                .animation = 0,
                .frame = 0,
                .duration = 0,
                .state = 0,
                .position = {
                    (rand() % 16)+768,240
                },
                .direction = {
                    -1.0,
                    (((float)rand()/(float)(RAND_MAX)) * 2.0)-1.0
                }
            };
            if (newAqua.direction[1] < 0.5 && newAqua.direction[1] > -0.5) {
                newAqua.direction[1] = 1.0;
            }
            aquaPool[aquaCnt] = newAqua;
            aquaCnt += 1;    
        }
    }
    api->NativeFunctions->RegisterSprites(0,spritesAddr,0);

    for (int i = 0; i < aquaCnt; i++) {
        if (i >= amount) {
            break;
        }
        aquaObj *aqua =& aquaPool[i];

        if (*isPaused != 0 && rest == 0) {
            aqua->state = 1;
            aqua->animation = 1;
        } else {
            aqua->state = 0;
            aqua->animation = 0;
        }

        uint8_t frameId = aqua->frame;
        switch (aqua->animation) {
            case 1:
                frameId += 5;
                break;
        }

        arg.attribute = 525;
        if (aqua->state == 1) {
            if (aqua->position[0] < 320) {
                arg.attribute |= 0x10;
            }
        }

        int cornerHit = cornerCheck(aqua);
        if (cornerHit == 1) {
            printf("[AquaInvasion] Aqua %d has hit the corner >:)\n", i);
        }

        arg.spriteId = frameId;
		arg.x = aqua->position[0];
		arg.y = aqua->position[1];
		arg.z = 4.0;
		arg.alpha = 1.0;
		arg.zm_x = 1.0;
		arg.zm_y = 1.0;
		arg.listType = 1;
		arg.angle = 0;
		arg.u1 = 1.0;
		arg.v1 = 1.0;
		arg.u0 = 0.0;
		arg.v0 = 0.0;
		argp = &arg;
        if (draw == 0) {
            api->NativeFunctions->DrawSprite(argp,0);
        }
        
        if (aqua->duration >= animDurById[aqua->animation]) {
            aqua->duration = 0;
            if (aqua->frame >= frameCntById[aqua->animation]) {
                aqua->frame = 0;
            } else {
                aqua->frame += 1;
            }
        } else {
            aqua->duration += 1;
        }

        if (aqua->position[0] <= xmin) {
            aqua->direction[0] = 1.0;
        } else {
            if (aqua->position[0] >= xmax) {
                aqua->direction[0] = -1.0;
            }
        }

        if (aqua->position[1] <= ymin) {
            aqua->direction[1] = ((float)rand()/(float)(RAND_MAX)) * 1.0;
        } else {
            if (aqua->position[1] >= ymax) {
                aqua->direction[1] = ((float)rand()/(float)(RAND_MAX)) * 1.0;
                aqua->direction[1] *= -1;
            }
        }

        if (aqua->state == 0) {
            aqua->position[0] += (int)ceil(aqua->direction[0]*4*speed);
            aqua->position[1] += (int)ceil(aqua->direction[1]*3*speed);
        }
    }
    
}

__attribute__ ((stdcall)) void clearAquas() {
    memset(aquaPool,0,sizeof(aquaPool));
    aquaCnt = 0;
}

__attribute__ ((stdcall)) void freqChange() {
    switch (freqId) {
        case 0:
            frequency = 0;
            break;
        case 1:
            frequency = 1000000;
            break;
        case 2:
            frequency = 100000;
            break;
        case 3:
            frequency = 10000;
            break;
        case 4:
            frequency = 1000;
            break;
        case 5:
            frequency = 100;
            break;
    };
}

__attribute__ ((stdcall)) void saveSettings() {
    fptr = fopen("mods/AquaInvasion/settings.txt","w");
	fprintf(
        fptr,
        "draw:%d\namount:%d\nfrequency:%d\nspeed:%d\nrest:%d\n",
        draw,amount,freqId,speed,rest
    );
    fclose(fptr);
}

void loadSettings() {
    fptr = fopen("mods/AquaInvasion/settings.txt","r");
    if (fptr != NULL) {
        char line[256];

        while (fgets(line, sizeof(line), fptr)) {
            char name[64];
            char value[8];

            if (line[0] == '/' || line[0] == '\n') {
                continue;
            }

            if (sscanf(line," %63[^:] : %8s", name, value) == 2) {
                for (int i = 0; i < 5; i++) {
                    if (strcmp(name,settings[i].name) == 0) {
                        *settings[i].value = strtoul(value,NULL,10);
                    }
                }    
            }
        }
        fclose(fptr);
        freqChange();
    }
}

void Patch() {
    fptr = fopen("Resource/demo/aqua.bin","rb");
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
    _aquaHookId = _baseModApi->Hooks->AfterGameUpdate(aquaHandle, &_userCtx);
    static const char *toggle[2] = {"YES", "NO"};
    static BaseMod_ModMenuEntry menuEntry[7] = {
        {"Draw Aquas",&draw,0,1,toggle,NULL,NULL},
        {"Max Amount",&amount,0,256,NULL,NULL,NULL},
        {"Spawn Frequency",&freqId,0,5,NULL,NULL,freqChange},
        {"Speed",&speed,1,10,NULL,NULL,NULL},
        {"Rest",&rest,0,1,toggle,NULL,NULL},
        {"Clear Aquas",NULL,0,0,NULL,clearAquas,NULL},
        {"Save Settings",NULL,0,0,NULL,saveSettings,NULL}
    };
    _baseModApi->ModMenu->RegisterMenuTab("AQUA INVASION",menuEntry,7);
}

GEARLOADER_EXPORT void GEARLOADER_CALL Init(GearLoaderContext* ctx, GearLoaderApi* api) {
    base = (BYTE *)GetModuleHandle(NULL);

    const void* retApi;
    SemanticVersion retVer;
    int32_t result = api->RetrieveModApi(ctx,BASEMOD_NAME,BASEMOD_API_VERSION,&retApi,&retVer);

    if (result != 0) {
        printf("BaseMod loading failed for AquaInvasion (X) - result = %d\n", result);
        return;
    }

    _baseModApi = (const BaseMod_Api*)retApi;
    _userCtx = (UserHookContext){_baseModApi};

    srand(time(NULL));
	Patch();
    loadSettings();
    printf("[AquaInvasion] A mysterious festival begins.\n");
}


