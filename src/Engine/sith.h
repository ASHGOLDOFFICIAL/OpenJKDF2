#ifndef _SITH__H
#define _SITH__H

#include "types.h"
#include "globals.h"

#define sith_Startup_ADDR (0x004C4630)
#define sith_Shutdown_ADDR (0x004C4700)
#define sith_Load_ADDR (0x004C4780)
#define sith_Free_ADDR (0x004C47B0)
#define sith_Mode1Init_ADDR (0x004C47D0)
#define sith_AutoSave_ADDR (0x004C4880)
#define sithOpenNormal_ADDR (0x004C49D0)
#define sith_Mode1Init_3_ADDR (0x004C4A70)
#define sith_Open_ADDR (0x004C4B10)
#define sith_Close_ADDR (0x004C4B80)
#define sith_SetEndLevel_ADDR (0x004C4BF0)
#define sith_Tick_ADDR (0x004C4C00)
#define sith_UpdateCamera_ADDR (0x004C4D30)
#define sith_sub_4C4D80_ADDR (0x004C4D80)
#define sith_set_sithmode_5_ADDR (0x004C4DB0)
#define sith_SetEpisodeName_ADDR (0x004C4DC0)

extern float sith_lastAspect;

int sith_Startup(struct common_functions *commonFuncs);
void sith_Shutdown();
int sith_Load(char *path);
void sith_Free();
int sith_Mode1Init(char *a1);
int sithOpenNormal(char *path);
int sith_Mode1Init_3(char *fpath);
int sith_Open();
void sith_Close();
void sith_SetEndLevel();
int sith_Tick();
void sith_UpdateCamera();
void sith_sub_4C4D80();
void sith_set_sithmode_5();
void sith_SetEpisodeName(char *text);
void sith_AutoSave();
void sith_sub_4C4D80();

//static int (*sith_Startup)() = (void*)sith_Startup_ADDR;
static int (*_sith_Tick)() = (void*)sith_Tick_ADDR;
//static void (*sith_AutoSave)() = (void*)sith_AutoSave_ADDR;
//static int (*sith_Mode1Init)(char*) = (void*)sith_Mode1Init_ADDR;
//static int (*sith_Mode1Init_3)(char*) = (void*)sith_Mode1Init_3_ADDR;
//static void (*sith_Close)() = (void*)sith_Close_ADDR;

//static void (*sith_SetEpisodeName)(char *text) = (void*)sith_SetEpisodeName_ADDR;

#endif // _SITH__H
