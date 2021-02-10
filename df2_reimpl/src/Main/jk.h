#ifndef _MAINJK_H
#define _MAINJK_H

#define jk_MenuReturn_ADDR (0x00403410)
#define jk_MissionReload_ADDR (0x00403440)
#define jk_sub_403470_ADDR (0x00403470)
#define jk_sub_4034D0_ADDR (0x004034D0)
#define jk_loadFile_ADDR (0x00403570)
#define jk_loadFile2_ADDR (0x004035F0)
#define jk_SwitchTo5_2_ADDR (0x004036B0)
#define jk_SwitchTo5_ADDR (0x004036F0)
#define jk_SwitchTo12_ADDR (0x00403740)
#define jk_SwitchTo4_ADDR (0x00403770)
#define jk_SwitchTo13_ADDR (0x004037B0)
#define jk_TitleShow_ADDR (0x00403A20)
#define jk_TitleTick_ADDR (0x00403A40)
#define jk_TitleLeave_ADDR (0x00403A70)
#define jk_MainShow_ADDR (0x00403A80)
#define jk_MainTick_ADDR (0x00403A90)
#define jk_MainLeave_ADDR (0x00403AA0)
#define jk_GameplayShow_ADDR (0x00403AB0)
#define jk_GameplayTick_ADDR (0x00403D40)
#define jk_GameplayLeave_ADDR (0x00403E60)
#define jk_EscapeMenuShow_ADDR (0x00403F40)
#define jk_EscapeMenuTick_ADDR (0x00403F70)
#define jk_EscapeMenuLeave_ADDR (0x004040A0)
#define jk_EndLevelScreenShow_ADDR (0x004041A0)
#define jk_EndLevelScreenTick_ADDR (0x00404240)
#define jk_EndLevelScreenLeave_ADDR (0x00404250)
#define jk_CdSwitchShow_ADDR (0x00404260)
#define jk_VideoShow_ADDR (0x00404270)
#define jk_VideoTick_ADDR (0x00404350)
#define jk_VideoLeave_ADDR (0x00404430)
#define jk_CutsceneShow_ADDR (0x00404450)
#define jk_CutsceneTick_ADDR (0x00404460)
#define jk_CutsceneLeave_ADDR (0x00404470)
#define jk_CreditsShow_ADDR (0x00404480)
#define jk_CreditsTick_ADDR (0x004044B0)
#define jk_CreditsLeave_ADDR (0x004044E0)
#define jk_ChoiceShow_ADDR (0x004044F0)
#define jk_ChoiceTick_ADDR (0x00404550)
#define jk_ChoiceLeave_ADDR (0x00404560)
#define jk_UnkShow_ADDR (0x00404570)
#define jk_UnkTick_ADDR (0x00404580)
#define jk_UnkLeave_ADDR (0x004045F0)

static int (*jk_MissionReload)() = (void*)jk_MissionReload_ADDR;
static int (*jk_MenuReturn)() = (void*)jk_MenuReturn_ADDR;
static int (*jk_loadFile)(char *a1) = (void*)jk_loadFile_ADDR;
static int (*jk_sub_403470)(char *a1) = (void*)jk_sub_403470_ADDR;

#endif // _MAINJK_H