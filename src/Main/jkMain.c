#include "jkMain.h"

#include "../jk.h"
#include "Engine/rdroid.h"
#include "Engine/sithNet.h"
#include "Engine/sith.h"
#include "Engine/sithControl.h"
#include "Engine/sithSoundSys.h"
#include "Dss/sithGamesave.h"
#include "Engine/sithCamera.h"
#include "Engine/sithMulti.h"
#include "Engine/sithRender.h"
#include "Engine/sithCamera.h"
#include "Engine/sithTime.h"
#include "Main/jkSmack.h"
#include "Main/jkGame.h"
#include "Main/jkCredits.h"
#include "Main/jkCutscene.h"
#include "Main/jkHudInv.h"
#include "Main/jkHud.h"
#include "Main/jkDev.h"
#include "Main/jkEpisode.h"
#include "Main/jkRes.h"
#include "Main/jkStrings.h"
#include "Gui/jkGUIRend.h"
#include "Gui/jkGUI.h"
#include "Gui/jkGUINet.h"
#include "Gui/jkGUIMultiTally.h"
#include "Gui/jkGUIForce.h"
#include "Gui/jkGUIMain.h"
#include "Gui/jkGUITitle.h"
#include "Gui/jkGUIDialog.h"
#include "Gui/jkGUIEsc.h"
#include "Gui/jkGUISingleTally.h"
#include "World/jkPlayer.h"
#include "World/jkSaber.h"
#include "World/sithWorld.h"
#include "Platform/stdControl.h"
#include "Win95/Windows.h"
#include "Win95/Video.h"
#include "Win95/sithDplay.h"
#include "Win95/stdDisplay.h"
#include "Win95/Window.h"
#include "General/util.h"
#include "General/stdBitmap.h"
#include "General/stdPalEffects.h"
#include "World/jkPlayer.h"
#include "stdPlatform.h"

#ifdef QOL_IMPROVEMENTS
#define TICKRATE_MS (jkPlayer_fpslimit ? 1000 / jkPlayer_fpslimit : 0) // no cap
#else
#define TICKRATE_MS (20) // 50fps
#endif

#define jkMain_CutsceneShow ((void*)(0x00404450))
#define jkMain_CutsceneTick ((void*)(0x00404460))
#define jkMain_CutsceneLeave ((void*)(0x00404470))

jkEpisodeEntry* jkMain_pEpisodeEnt = NULL;
jkEpisodeEntry* jkMain_pEpisodeEnt2 = NULL;

static jkGuiStateFuncs jkMain_aGuiStateFuncs[15] = {
    {0,  0,  0},
    {jkMain_VideoShow_ADDR, jkMain_VideoTick_ADDR, jkMain_VideoLeave_ADDR},
    {jkMain_TitleShow_ADDR, jkMain_TitleTick_ADDR, jkMain_TitleLeave_ADDR},
    {jkMain_MainShow_ADDR, jkMain_MainTick_ADDR, jkMain_MainLeave_ADDR},
    {jkMain_VideoShow_ADDR, jkMain_VideoTick_ADDR, jkMain_VideoLeave_ADDR},
    {jkMain_GameplayShow_ADDR, jkMain_GameplayTick_ADDR, jkMain_GameplayLeave_ADDR},
    {jkMain_EscapeMenuShow_ADDR, jkMain_EscapeMenuTick_ADDR, jkMain_EscapeMenuLeave_ADDR},
    {jkMain_CdSwitchShow_ADDR,  0,  0},
    {jkMain_VideoShow_ADDR, jkMain_VideoTick_ADDR, jkMain_VideoLeave_ADDR},
    {jkMain_EndLevelScreenShow_ADDR, jkMain_EndLevelScreenTick_ADDR, jkMain_EndLevelScreenLeave_ADDR},
    {jkMain_VideoShow_ADDR, jkMain_VideoTick_ADDR, jkMain_VideoLeave_ADDR},
    {jkMain_ChoiceShow_ADDR, jkMain_ChoiceTick_ADDR, jkMain_ChoiceLeave_ADDR},
    {jkMain_CutsceneShow_ADDR, jkMain_CutsceneTick_ADDR, jkMain_CutsceneLeave_ADDR},
    {jkMain_CreditsShow_ADDR, jkMain_CreditsTick_ADDR, jkMain_CreditsLeave_ADDR},
    {jkMain_UnkShow_ADDR, jkMain_UnkTick_ADDR, jkMain_UnkLeave_ADDR},
};

void jkMain_GuiAdvance()
{
    unsigned int v1; // esi
    int v3; // esi
    int v4; // esi
    void (__cdecl *v5)(int, int); // ecx
    int v6; // eax
    void (__cdecl *v7)(int, int); // ecx
    void (__cdecl *v8)(int); // ecx

    if ( !g_app_suspended )
    {
        if ( thing_nine )
            stdControl_ToggleCursor(0);
        if ( thing_eight )
        {
            if ( sithNet_isMulti && !thing_six)
            {
                v1 = stdPlatform_GetTimeMsec();
                
                if (v1 > jkMain_lastTickMs + TICKRATE_MS)
                {
                    jkMain_lastTickMs = v1;
                    if (!sith_Tick()) return;
                }
                
                if ( g_sithMode == 5 )
                {
                    if ( jkGuiRend_thing_five )
                        jkGuiRend_thing_four = 1;
                    jkSmack_stopTick = 1;
                    jkSmack_nextGuiState = JK_GAMEMODE_MAIN;
                    thing_nine = 0;
                    return;
                }
                if ( sithNet_dword_832638 )
                {
                    sithNet_dword_832638 = 0;
                    if ( sithNet_isServer )
                        jkSaber_cogMsg_SendEndLevel();
                }
                if ( sith_bEndLevel )
                {
                    sith_bEndLevel = 0;
                    jkMain_EndLevel(1);
                }
                jkPlayer_nullsub_1(&playerThings[playerThingIdx]);
                jkGame_dword_552B5C += stdPlatform_GetTimeMsec() - v1;
                v3 = stdPlatform_GetTimeMsec();
                if ( g_app_suspended && jkSmack_currentGuiState != 6 ) {
#ifdef QOL_IMPROVEMENTS
                    if (jkMain_lastTickMs == v1)
#endif
                    _jkGame_Update();
                }
                game_updateMsecsTotal += stdPlatform_GetTimeMsec() - v3;
            }
        }
        thing_nine = 0;
        return;
    }

    if ( !thing_nine )
    {
        switch ( jkSmack_currentGuiState )
        {
            case JK_GAMEMODE_VIDEO:
            case JK_GAMEMODE_VIDEO2:
            case JK_GAMEMODE_VIDEO3:
            case JK_GAMEMODE_VIDEO4:
                _jkCutscene_PauseShow(0);
                break;
            case JK_GAMEMODE_GAMEPLAY:
                stdControl_ToggleCursor(1);
                _jkGame_ddraw_idk_palettes(0);
                break;
            default:
                break;
        }
        stdControl_Flush();
        thing_nine = 1;
    }
    if ( jkSmack_stopTick && !jkGuiRend_thing_five )
    {
        jkGuiRend_thing_four = 0;
        v4 = jkSmack_currentGuiState;
        v5 = jkMain_aGuiStateFuncs[jkSmack_currentGuiState].leaveFunc;
        if ( v5 )
            v5(jkSmack_currentGuiState, jkSmack_nextGuiState);
        //jk_printf("leave %u\n", jkSmack_currentGuiState);

        v6 = jkSmack_nextGuiState;
        jkSmack_stopTick = 0;
        jkSmack_currentGuiState = jkSmack_nextGuiState;
        v7 = jkMain_aGuiStateFuncs[jkSmack_nextGuiState].showFunc;
        if ( !v7 )
            goto LABEL_35;
        //jk_printf("show %u\n", jkSmack_currentGuiState);
        v7(jkSmack_nextGuiState, v4);
        //jk_printf("showed %u\n", jkSmack_currentGuiState);
    }
    v6 = jkSmack_currentGuiState;
LABEL_35:
    if ( !jkSmack_stopTick )
    {
        //jk_printf("tick %u %x\n", jkSmack_currentGuiState, jkMain_aGuiStateFuncs[v6].tickFunc);
        v8 = jkMain_aGuiStateFuncs[v6].tickFunc;
        if ( v8 )
            v8(v6);
    }
}

void jkMain_EscapeMenuShow(int a1, int a2)
{
    if ( !sithNet_isMulti )
        sithTime_Pause();
    jkGui_SetModeMenu(jkGui_stdBitmaps[4]->palette);
    jkGuiEsc_Show();
}

void jkMain_EscapeMenuTick(int a2)
{
    unsigned int v1; // esi
    int v3; // esi

    if ( sithNet_isMulti )
    {
        if ( !thing_six )
        {
            if ( thing_eight )
            {
                v1 = stdPlatform_GetTimeMsec();
                
                if (v1 > jkMain_lastTickMs + TICKRATE_MS)
                {
                    jkMain_lastTickMs = v1;
                    if (sith_Tick()) return;
                }
                
                if ( g_sithMode == 5 )
                {
                    if ( jkGuiRend_thing_five )
                        jkGuiRend_thing_four = 1;
                    jkSmack_stopTick = 1;
                    jkSmack_nextGuiState = JK_GAMEMODE_MAIN;
                }
                else
                {
                    if ( sithNet_dword_832638 )
                    {
                        sithNet_dword_832638 = 0;
                        if ( sithNet_isServer )
                            jkSaber_cogMsg_SendEndLevel();
                    }
                    if ( sith_bEndLevel )
                    {
                        sith_bEndLevel = 0;
                        jkMain_EndLevel(1);
                    }
                    jkPlayer_nullsub_1(&playerThings[playerThingIdx]);
                    jkGame_dword_552B5C += stdPlatform_GetTimeMsec() - v1;
                    v3 = stdPlatform_GetTimeMsec();
                    if ( g_app_suspended && a2 != 6 ) {
#ifdef QOL_IMPROVEMENTS
                    if (jkMain_lastTickMs == v1)
#endif
                        _jkGame_Update();
                    }
                    game_updateMsecsTotal += stdPlatform_GetTimeMsec() - v3;
                }
            }
        }
    }
}

void jkMain_EscapeMenuLeave(int a2, int a3)
{
    int v3; // eax

    if ( !sithNet_isMulti )
        sithTime_Resume();
    if ( a3 != JK_GAMEMODE_GAMEPLAY )
    {
        if ( a3 == JK_GAMEMODE_ESCAPE )
        {
            stdControl_ToggleCursor(0);
            sithSoundSys_StopAll();
        }
        if ( jkGame_isDDraw )
        {
            Windows_ShutdownGdi();
            Video_SwitchToGDI();
            jkPlayer_nullsub_30();
            jkGame_isDDraw = 0;
        }
        if ( a3 != JK_GAMEMODE_ESCAPE && jkMain_bInit )
        {
            jkPlayer_Shutdown();
            sith_Close();
            jkMain_bInit = 0;
            thing_eight = 0;
        }
        if ( sithNet_isMulti && a3 != JK_GAMEMODE_ESCAPE )
        {
            thing_eight = 0;
            if ( a3 == 3 )
                sithMulti_Shutdown();
            else
                sithMulti_LobbyMessage();
            if ( sithNet_isServer )
                DirectPlay_SetSessionFlagidk(0);
            thing_six = 1;
            v3 = jkGuiMultiTally_Show(sithNet_isMulti);
            thing_six = 0;
            if ( v3 == -1 )
            {
                sithMulti_Shutdown();
                if ( jkGuiRend_thing_five )
                    jkGuiRend_thing_four = 1;
                jkSmack_stopTick = 1;
                jkSmack_nextGuiState = JK_GAMEMODE_MAIN;
            }
        }
    }
    jkGui_SetModeGame();
}

void jkMain_EndLevelScreenShow(int a1, int a2)
{
    stdControl_ToggleCursor(0); // Added
    if ( jkEpisode_mLoad.field_0 != 1 && jkSmack_gameMode == 2
      || jkGuiSingleTally_Show() != -1
      && (sithPlayer_GetBinAmt(SITHBIN_NEW_STARS) <= 0.0 && sithPlayer_GetBinAmt(SITHBIN_SPEND_STARS) <= 0.0
       || jkGuiForce_Show(1, 0.0, jkMain_dword_552B98, 0, 0, 1) != -1) )
    {
        jkMain_CdSwitch(0, 1);
        return;
    }

    if ( jkGuiRend_thing_five )
        jkGuiRend_thing_four = 1;
    jkSmack_stopTick = 1;
    jkSmack_nextGuiState = 3;
    return;
}

void jkMain_EndLevelScreenTick(int a1)
{
    ;
}

void jkMain_EndLevelScreenLeave(int a1, int a2)
{
    ;
}

void jkMain_GameplayShow(int a1, int a2)
{
    signed int level_loaded; // esi
    signed int v3; // eax
    wchar_t *v4; // eax
    DWORD v5; // eax
    wchar_t *v6; // [esp-4h] [ebp-Ch]

    level_loaded = 0;
    if ( a2 == JK_GAMEMODE_ESCAPE )
    {
        sithSoundSys_ResumeAll();
        sithSoundSys_ResumeMusic(1);
#ifdef SDL2_RENDER
        jkGame_isDDraw = 0;
#endif
        goto LABEL_39;
    }

    if ( jkSmack_gameMode == JK_GAMEMODE_VIDEO2 )
    {
        jkPlayer_Startup();
        jkPlayer_InitForceBins();
        jkMain_bInit = 1;
        jkPlayer_InitSaber();
        sith_AutoSave();
LABEL_39:
        if ( jkMain_SetVideoMode() )
        {
            stdControl_ToggleCursor(1);
            stdControl_Flush();
            _jkGame_Update();
            thing_eight = 1;
        }
        else
        {
            if ( jkGuiRend_thing_five )
                jkGuiRend_thing_four = 1;
            jkSmack_stopTick = 1;
            thing_eight = 1;
            jkSmack_nextGuiState = JK_GAMEMODE_MAIN;
        }
        return;
    }

    if ( jkSmack_gameMode == 1 )
    {
        jkGui_copies_string(gamemode_1_str);
        jkGuiTitle_ShowLoading(gamemode_1_str, 0);
    }
    else
    {
        jkGui_copies_string(gamemode_0_2_str);
        jkGuiTitle_ShowLoading(gamemode_0_2_str, 0);
    }

    if ( jkSmack_gameMode )
    {
        if ( jkSmack_gameMode == 1 )
        {
            v3 = sithGamesave_Load(gamemode_0_2_str, 0, 1);
        }
        else
        {
            if ( jkSmack_gameMode != 2 )
                goto LABEL_15;
            v3 = sith_Mode1Init_3(gamemode_0_2_str);
        }
    }
    else
    {
        v3 = sith_Mode1Init(gamemode_0_2_str);
    }

    level_loaded = v3;
LABEL_15:
    jkGuiTitle_LoadingFinalize();
    if ( !level_loaded )
    {
        if ( jkGame_isDDraw )
        {
            Windows_ShutdownGdi();
            Video_SwitchToGDI(1);
            jkPlayer_nullsub_30();
            jkGame_isDDraw = 0;
        }
        if ( jkGuiRend_thing_five )
            jkGuiRend_thing_four = 1;
        jkSmack_stopTick = 1;
        jkSmack_nextGuiState = JK_GAMEMODE_MAIN;
        v6 = jkStrings_GetText("ERR_CANNOT_LOAD_LEVEL");
        v4 = jkStrings_GetText("ERROR");
        jkGuiDialog_ErrorDialog(v4, v6);
        return;
    }

    if ( !sithNet_isMulti )
    {
        jkPlayer_Startup();
        jkPlayer_InitForceBins();
        jkMain_bInit = 1;
        if ( jkSmack_gameMode == 2 || !jkSmack_gameMode )
        {
            sithCamera_SetsFocus();
            jkPlayer_InitSaber();
            sith_AutoSave();
        }

        goto LABEL_39;
    }
    if ( sithNet_isServer )
    {
LABEL_28:
        sithInventory_ClearInventory(g_localPlayerThing);
        jkPlayer_MpcInitBins(g_selfPlayerInfo);
        
        jkPlayer_Startup();
        jkPlayer_InitForceBins();
        jkMain_bInit = 1;
        if ( jkSmack_gameMode == 2 || !jkSmack_gameMode )
        {
            sithCamera_SetsFocus();
            jkPlayer_InitSaber();
            sith_AutoSave();
        }
        if ( sithNet_isMulti )
        {
//#ifdef TARGET_HAS_DPLAY
            if ( sithNet_isServer )
            {
                DirectPlay_SetSessionFlagidk(1);
                v5 = idx_13b4_related;
                if ( idx_13b4_related >= (unsigned int)jkPlayer_maxPlayers )
                    v5 = jkPlayer_maxPlayers;
                DirectPlay_SetSessionDesc(gamemode_0_2_str, v5);
            }
            if ( sithNet_isMulti )
                jkSaber_cogMsg_wrap_SendSaberInfo_alt();
//#endif
        }
        goto LABEL_39;
    }
    thing_six = 1;
    stdControl_ToggleCursor(0);
    if ( jkGuiNet_ShowSynchronizing() == 1 )
    {
        thing_six = 0;
        stdControl_ToggleCursor(1);
        goto LABEL_28;
    }
    sith_Close();
    sithMulti_Shutdown();
    if ( jkGuiRend_thing_five )
        jkGuiRend_thing_four = 1;
    jkSmack_stopTick = 1;
    jkSmack_nextGuiState = JK_GAMEMODE_MAIN;
    thing_six = 0;
}

void jkMain_GameplayTick(int a2)
{
    unsigned int v1; // esi
    int v3; // esi

    if ( !thing_six )
    {
        if ( thing_eight )
        {
            v1 = stdPlatform_GetTimeMsec();
            
            if (v1 > jkMain_lastTickMs + TICKRATE_MS)
            {
                jkMain_lastTickMs = v1;
                if (sith_Tick()) return;
            }
            
            if ( g_sithMode == 5 )
            {
                if ( jkGuiRend_thing_five )
                    jkGuiRend_thing_four = 1;
                jkSmack_stopTick = 1;
                jkSmack_nextGuiState = JK_GAMEMODE_MAIN;
            }
            else
            {
                if ( sithNet_dword_832638 )
                {
                    sithNet_dword_832638 = 0;
                    if ( sithNet_isServer )
                        jkSaber_cogMsg_SendEndLevel();
                }
                if ( sith_bEndLevel )
                {
                    sith_bEndLevel = 0;
                    jkMain_EndLevel(1);
                }
                jkPlayer_nullsub_1(&playerThings[playerThingIdx]);
                jkGame_dword_552B5C += stdPlatform_GetTimeMsec() - v1;
                v3 = stdPlatform_GetTimeMsec();
                if ( g_app_suspended && a2 != 6 ) {
#ifdef QOL_IMPROVEMENTS
                    if (jkMain_lastTickMs == v1)
#endif
                    _jkGame_Update();
                }
                game_updateMsecsTotal += stdPlatform_GetTimeMsec() - v3;
            }
        }
    }
}

void jkMain_GameplayLeave(int a2, int a3)
{
    int v3; // eax

    if ( a3 == JK_GAMEMODE_ESCAPE )
    {
        stdControl_ToggleCursor(0);
        sithSoundSys_StopAll();
    }
    if ( jkGame_isDDraw )
    {
        Windows_ShutdownGdi();
        Video_SwitchToGDI();
        jkPlayer_nullsub_30();
        jkGame_isDDraw = 0;
    }
    if ( a3 != 6 && jkMain_bInit )
    {
        jkPlayer_Shutdown();
        sith_Close();
        jkMain_bInit = 0;
        thing_eight = 0;
    }
    if ( sithNet_isMulti && a3 != 6 )
    {
        thing_eight = 0;
        if ( a3 == 3 )
            sithMulti_Shutdown();
        else
            sithMulti_LobbyMessage();
        if ( sithNet_isServer )
            DirectPlay_SetSessionFlagidk(0);
        thing_six = 1;
        v3 = jkGuiMultiTally_Show(sithNet_isMulti);
        thing_six = 0;
        if ( v3 == -1 )
        {
            sithMulti_Shutdown();
            if ( jkGuiRend_thing_five )
                jkGuiRend_thing_four = 1;
            jkSmack_stopTick = 1;
            jkSmack_nextGuiState = JK_GAMEMODE_MAIN;
        }
    }
}

void jkMain_TitleShow(int a1, int a2)
{
    jkGuiTitle_ShowLoadingStatic();
    sith_Load("static.jkl");
    jkHudInv_InitItems();
}

void jkMain_TitleTick(int a1)
{
    jkGuiTitle_LoadingFinalize();
    if ( jkGuiRend_thing_five )
        jkGuiRend_thing_four = 1;
    jkSmack_stopTick = 1;
    jkSmack_nextGuiState = JK_GAMEMODE_MAIN;
}

void jkMain_TitleLeave(int a1, int a2)
{
    ;
}

void jkMain_MainShow(int a1, int a2)
{
    stdControl_ShowCursor(1);
    stdControl_ToggleCursor(0); // Added
    jkGuiMain_Show();
}

void jkMain_MainTick(int a1)
{
    ;
}

void jkMain_MainLeave(int a1, int a2)
{
    ;
}

void jkMain_ChoiceShow(int a1, int a2)
{
    int v1; // [esp+0h] [ebp-4h] BYREF

    if ( jkGuiForce_Show(0, 0.0, 1, 0, &v1, 1) == -1 )
    {
        if ( jkGuiRend_thing_five )
            jkGuiRend_thing_four = 1;
        jkSmack_stopTick = 1;
        jkSmack_nextGuiState = JK_GAMEMODE_MAIN;
    }
    else
    {
        jkMain_CdSwitch(0, v1);
    }
}

void jkMain_ChoiceTick(int a1)
{
    ;
}

void jkMain_ChoiceLeave(int a1, int a2)
{
    ;
}

void jkMain_UnkShow(int a1, int a2)
{
    ;
}

void jkMain_UnkTick(int a1)
{
    jkRes_LoadGob(jkMain_strIdk);
    if ( jkEpisode_mLoad.paEntries )
    {
        pHS->free(jkEpisode_mLoad.paEntries);
        jkEpisode_mLoad.paEntries = 0;
    }
    jkEpisode_Load(&jkEpisode_mLoad);

    jkSmack_gameMode = 1;
    if ( jkGuiRend_thing_five )
        jkGuiRend_thing_four = 1;
    jkSmack_stopTick = 1;
    jkSmack_nextGuiState = JK_GAMEMODE_GAMEPLAY;
}

void jkMain_UnkLeave(int a1, int a2)
{
    ;
}

int jkMain_sub_403470(char *a1)
{
    int result; // eax

    sithInventory_549FA0 = 1;
    _strncpy(gamemode_0_2_str, a1, 0x7Fu);
    result = 0;
    gamemode_0_2_str[127] = 0;
    jkSmack_gameMode = 0;
    if ( jkGuiRend_thing_five )
        jkGuiRend_thing_four = 1;
    jkSmack_stopTick = 1;
    jkSmack_nextGuiState = JK_GAMEMODE_GAMEPLAY;
    return result;
}

int jkMain_LoadFile(char *a1)
{
    if (jkRes_LoadCD(1))
    {
        sithInventory_549FA0 = 1;
        jkRes_LoadGob(a1);
        if ( jkEpisode_mLoad.paEntries )
        {
            pHS->free(jkEpisode_mLoad.paEntries);
            jkEpisode_mLoad.paEntries = 0;
        }
        if ( jkEpisode_Load(&jkEpisode_mLoad) )
        {
            return jkMain_CdSwitch(1, 1);
        }
        else
        {
            Windows_ErrorMsgboxWide("ERR_CANNOT_LOAD_FILE %s", a1);
            return 0;
        }
    }
    return 0;
}

int jkMain_CdSwitch(int a1, int bIsAPath)
{
    jkEpisodeEntry *v2; // eax
    jkEpisodeEntry *v3; // ecx
    int v4; // eax
    signed int result; // eax

    if ( !jkEpisode_mLoad.numSeq )
    {
        if ( jkGuiRend_thing_five )
        {
            jkGuiRend_thing_four = 1;
        }
        jkSmack_stopTick = 1;
        jkSmack_nextGuiState = JK_GAMEMODE_MAIN;
        return 0;
    }
    if ( a1 )
    {
        v2 = jkEpisode_idk1(&jkEpisode_mLoad);
        v3 = v2;
        jkMain_pEpisodeEnt = v2;
        jkMain_pEpisodeEnt2 = v2;
        jkPlayer_dword_525470 = 0;
    }
    else
    {
        v3 = jkMain_pEpisodeEnt;
        v2 = jkMain_pEpisodeEnt2;
    }
    if ( jkPlayer_dword_525470 )
    {
        jkMain_pEpisodeEnt = jkEpisode_idk1(&jkEpisode_mLoad);
        v2 = jkEpisode_idk2(&jkEpisode_mLoad, bIsAPath);
        v3 = jkMain_pEpisodeEnt;
        jkMain_pEpisodeEnt2 = v2;
        jkPlayer_dword_525470 = 0;
    }
    if ( !v2 )
    {
        v4 = jkGuiRend_thing_five;
        if ( v3->gotoA == -1 )
        {
            if ( jkGuiRend_thing_five )
                jkGuiRend_thing_four = 1;
            jkSmack_stopTick = 1;
            jkSmack_nextGuiState = JK_GAMEMODE_CREDITS;
            return 1;
        }
        if ( v4 )
            jkGuiRend_thing_four = 1;

        jkSmack_stopTick = 1;
        jkSmack_nextGuiState = JK_GAMEMODE_MAIN;
        return 0;
    }
    if ( sithNet_isMulti && (sithNet_MultiModeFlags & 0x80u) != 0 )
    {
        v4 = jkGuiRend_thing_five;
        
        if ( v4 )
            jkGuiRend_thing_four = 1;
        jkSmack_stopTick = 1;
        jkSmack_nextGuiState = JK_GAMEMODE_MAIN;
        return 0;
    }
    if ( v3->level == v2->level || jkSmack_currentGuiState == JK_GAMEMODE_ENDLEVEL )
    {
        if ( v2->type == 1 && jkSmack_currentGuiState == JK_GAMEMODE_GAMEPLAY )
        {
            if ( jkGuiRend_thing_five )
                jkGuiRend_thing_four = 1;
            jkSmack_stopTick = 1;
            jkSmack_nextGuiState = JK_GAMEMODE_CD_SWITCH;
            result = 1;
        }
        else
        {
            jkPlayer_dword_525470 = 1;
            jkMain_cd_swap_reverify(v2);
            result = 1;
        }
    }
    else
    {
        if ( jkGuiRend_thing_five )
            jkGuiRend_thing_four = 1;
        jkSmack_stopTick = 1;
        jkSmack_nextGuiState = JK_GAMEMODE_ENDLEVEL;
        result = 1;
    }
    return result;
}

int jkMain_cd_swap_reverify(jkEpisodeEntry *ent)
{
    int v1; // eax
    int v2; // eax
    signed int result; // eax
    signed int v4; // edi
    int v5; // edi
    signed int v6; // esi
    wchar_t *v7; // eax
    wchar_t *v8; // [esp-4h] [ebp-94h]
    char v9[128]; // [esp+10h] [ebp-80h] BYREF

    v1 = ent->type;
    if ( !v1 )
    {
        v5 = 0;
        v6 = 0;
        while ( !v6 )
        {
            if ( Windows_installType < 9 )
                v6 = jkRes_LoadCD(ent->cdNum);
            else
                v6 = 1;
            if ( !v6 )
            {
                v8 = jkStrings_GetText("GUI_CONFIRM_ABORTCD");
                v7 = jkStrings_GetText("GUI_ABORTCDREQUEST");
                if ( jkGuiDialog_YesNoDialog(v7, v8) )
                    v5 = 1;
            }
            if ( v5 )
            {
                if ( !v6 )
                {
                    if ( jkGuiRend_thing_five )
                        jkGuiRend_thing_four = 1;
                    jkSmack_stopTick = 1;
                    jkSmack_nextGuiState = JK_GAMEMODE_MAIN;
                    return 1;
                }
                break;
            }
        }
        _strncpy(gamemode_0_2_str, ent->fileName, 0x7Fu);
        gamemode_0_2_str[127] = 0;
        jkSmack_gameMode = sithNet_isMulti != 0 ? 2 : 0;
        if ( jkGuiRend_thing_five )
            jkGuiRend_thing_four = 1;
        jkSmack_stopTick = 1;
        jkSmack_nextGuiState = JK_GAMEMODE_GAMEPLAY;
        return 1;
    }
    v2 = v1 - 1;
    if ( v2 )
    {
        if ( v2 == 1 )
        {
            if ( jkGuiRend_thing_five )
                jkGuiRend_thing_four = 1;
            jkSmack_stopTick = 1;
            jkSmack_nextGuiState = 11; // force select/choice?
            return 1;
        }
        return 1;
    }
    if ( jkPlayer_setDisableCutscenes )
        v4 = 0;
    else
        v4 = jkRes_LoadCD(ent->cdNum);

    jkPlayer_WriteConfSwap(&playerThings[playerThingIdx], ent->cdNum, ent->fileName);
    if ( !v4 )
        return jkMain_CdSwitch(0, 1);
    _sprintf(v9, "video%c%s", 92, ent->fileName);
    if ( !util_FileExists(v9) )
        return jkMain_CdSwitch(0, 1);
    jkRes_FileExists(v9, gamemode_0_2_str, 128);
    switch ( jkSmack_currentGuiState )
    {
        case 3:
        case 9:
            if ( jkGuiRend_thing_five )
                jkGuiRend_thing_four = 1;
            jkSmack_stopTick = 1;
            jkSmack_nextGuiState = 10;
            result = 1;
            break;
        case 5:
        case 7:
        case 8:
            if ( jkGuiRend_thing_five )
                jkGuiRend_thing_four = 1;
            jkSmack_stopTick = 1;
            jkSmack_nextGuiState = 8;
            result = 1;
            break;
        default:
            return 1;
    }
    return result;
}

int jkMain_SetMap(int levelNum)
{
    jkEpisode_EndLevel(&jkEpisode_mLoad, levelNum);
    return jkMain_cd_swap_reverify(jkEpisode_idk1(&jkEpisode_mLoad));
}

void jkMain_do_guistate6()
{
    if ( !jkSmack_stopTick )
    {
        if ( jkGuiRend_thing_five )
            jkGuiRend_thing_four = 1;
        jkSmack_stopTick = 1;
        jkSmack_nextGuiState = JK_GAMEMODE_ESCAPE;
    }
}

int jkMain_sub_4034D0(char *a1, char *a2, char *a3, wchar_t *a4)
{
    sithInventory_549FA0 = 0;
    _strncpy(gamemode_0_2_str, a2, 0x7Fu);
    gamemode_0_2_str[127] = 0;
    _strncpy(jkMain_strIdk, a1, 0x7Fu);
    jkMain_strIdk[127] = 0;
    _strncpy(gamemode_1_str, a3, 0x7Fu);
    gamemode_1_str[127] = 0;
    _wcsncpy(jkMain_wstrIdk, a4, 0x7Fu);

    jkMain_wstrIdk[127] = 0;
    jkPlayer_dword_525470 = 1;
    if ( jkGuiRend_thing_five )
        jkGuiRend_thing_four = 1;
    jkSmack_stopTick = 1;
    jkSmack_nextGuiState = JK_GAMEMODE_UNK;
    return 1;
}

int jkMain_MissionReload()
{
    signed int result; // eax

    result = 1;
    if ( jkGuiRend_thing_five )
        jkGuiRend_thing_four = 1;
    jkSmack_stopTick = 1;
    jkSmack_nextGuiState = JK_GAMEMODE_GAMEPLAY;
    return result;
}

int jkMain_MenuReturn()
{
    signed int result; // eax

    result = 1;
    if ( jkGuiRend_thing_five )
        jkGuiRend_thing_four = 1;
    jkSmack_stopTick = 1;
    jkSmack_nextGuiState = JK_GAMEMODE_MAIN;
    return result;
}

int jkMain_EndLevel(int a1)
{
    jkEpisodeEntry *v1; // esi
    int v2; // eax
    int v4; // eax

    if ( jkEpisode_mLoad.numSeq )
    {
        v1 = jkEpisode_idk1(&jkEpisode_mLoad);
        if ( v1->darkpow || v1->lightpow )
        {
            v2 = v1->lightpow;
            if ( v2 )
            {
                if ( v2 >= 20 && v2 <= 34 && jkPlayer_GetChoice() != 2 )
                    sithInventory_SetCarries(playerThings[playerThingIdx].actorThing, v1->lightpow, 1);
            }
            v4 = v1->darkpow;
            if ( v4 )
            {
                if ( v4 >= 20 && v4 <= 34 && jkPlayer_GetChoice() != 1 )
                    sithInventory_SetCarries(playerThings[playerThingIdx].actorThing, v1->darkpow, 1);
            }
        }
    }
    return jkMain_CdSwitch(0, a1);
}

int jkMain_CdSwitchShow(int a1, int a2)
{
    return jkMain_CdSwitch(0, 1);
}

void jkMain_VideoShow(int a1, int a2)
{
    signed int result; // eax

    result = jkCutscene_sub_421310(gamemode_0_2_str);
    if ( !result )
    {
        Windows_ErrorMsgboxWide("ERR_CANNOT_LOAD_FILE %s", gamemode_0_2_str);
        switch ( a1 )
        {
            case JK_GAMEMODE_VIDEO:
                result = 1;
                if ( jkGuiRend_thing_five )
                    jkGuiRend_thing_four = 1;
                jkSmack_stopTick = 1;
                jkSmack_nextGuiState = JK_GAMEMODE_TITLE;
                break;
            case JK_GAMEMODE_VIDEO2:
                result = 1;
                if ( jkGuiRend_thing_five )
                    jkGuiRend_thing_four = 1;
                jkSmack_stopTick = 1;
                jkSmack_nextGuiState = JK_GAMEMODE_CUTSCENE;
                break;
            case JK_GAMEMODE_VIDEO3:
            case JK_GAMEMODE_VIDEO4:
                result = jkMain_CdSwitch(0, 1);
                break;
            default:
                result = 1;
                if ( jkGuiRend_thing_five )
                    jkGuiRend_thing_four = 1;
                jkSmack_stopTick = 1;
                jkSmack_nextGuiState = JK_GAMEMODE_MAIN;
                break;
        }
    }
    return;
}

void jkMain_VideoTick(int a2)
{
    signed int result; // eax

    result = jkCutscene_smack_related_loops();
    if ( result )
    {
        result = a2 - 1;
        switch ( a2 )
        {
            case JK_GAMEMODE_VIDEO:
                result = 1;
                if ( jkGuiRend_thing_five )
                    jkGuiRend_thing_four = 1;
                jkSmack_stopTick = 1;
                jkSmack_nextGuiState = JK_GAMEMODE_TITLE;
                break;
            case JK_GAMEMODE_VIDEO2:
                result = 1;
                if ( jkGuiRend_thing_five )
                    jkGuiRend_thing_four = 1;
                jkSmack_stopTick = 1;
                jkSmack_nextGuiState = JK_GAMEMODE_CUTSCENE;
                break;
            case JK_GAMEMODE_VIDEO3:
                result = 1;
                if ( jkGuiRend_thing_five )
                    jkGuiRend_thing_four = 1;
                jkSmack_stopTick = 1;
                jkSmack_nextGuiState = JK_GAMEMODE_ENDLEVEL;
                break;
            case JK_GAMEMODE_VIDEO4:
                result = 1;
                if ( jkGuiRend_thing_five )
                    jkGuiRend_thing_four = 1;
                jkSmack_stopTick = 1;
                jkSmack_nextGuiState = JK_GAMEMODE_GAMEPLAY;
                break;
            default:
                return;
        }
    }
    return;
}

void jkMain_VideoLeave(int a1, int a2)
{
    jkCutscene_sub_421410();
    if ( a1 == JK_GAMEMODE_VIDEO3 || a1 == JK_GAMEMODE_VIDEO4 )
        jkMain_CdSwitch(0, 1);
}

void jkMain_CreditsShow()
{
    if ( !jkCredits_Show() )
    {
        if ( jkGuiRend_thing_five )
            jkGuiRend_thing_four = 1;
        jkSmack_stopTick = 1;
        jkSmack_nextGuiState = 3;
    }
}

void jkMain_CreditsTick()
{
    if ( jkCredits_Tick() )
    {
        if ( jkGuiRend_thing_five )
            jkGuiRend_thing_four = 1;
        jkSmack_stopTick = 1;
        jkSmack_nextGuiState = 3;
    }
}

void jkMain_CreditsLeave()
{
    jkCredits_Skip();
}

int jkMain_SwitchTo13()
{
    signed int result; // eax

    result = 1;
    if ( jkGuiRend_thing_five )
        jkGuiRend_thing_four = 1;
    jkSmack_stopTick = 1;
    jkSmack_nextGuiState = 13;
    return result;
}

#ifdef SDL2_RENDER
void jkMain_FixRes()
{
    if (!jkGame_isDDraw)
        return;
    
    uint32_t newW = Window_xSize;
    uint32_t newH = Window_ySize;

    //if (jkGame_isDDraw)
    {
        newW = (uint32_t)((float)Window_xSize * ((480.0*2.0)/Window_ySize));
        newH = 480*2;
    }

    if (newW > Window_xSize)
    {
        newW = Window_xSize;
        newH = Window_ySize;
    }

    if (newW < 640)
        newW = 640;
    if (newH < 480)
        newH = 480;

    Video_modeStruct.viewSizeIdx = 0;
    Video_modeStruct.aViewSizes[Video_modeStruct.viewSizeIdx].xMin = 0;
    Video_modeStruct.aViewSizes[Video_modeStruct.viewSizeIdx].yMin = 0;
    Video_modeStruct.aViewSizes[Video_modeStruct.viewSizeIdx].xMax = newW / 2;
    Video_modeStruct.aViewSizes[Video_modeStruct.viewSizeIdx].yMax = newH / 2;
    
    stdDisplay_pCurVideoMode->format.width = newW;
    stdDisplay_pCurVideoMode->format.height = newH;
    stdDisplay_pCurVideoMode->widthMaybe = newW;
    stdDisplay_pCurVideoMode->format.width_in_pixels = newW;
    stdDisplay_pCurVideoMode->format.width_in_bytes = newW;
    
    Video_menuBuffer.format.width_in_pixels = newW;
    Video_otherBuf.format.width_in_pixels = newW;
    Video_menuBuffer.format.width_in_bytes = newW;
    Video_otherBuf.format.width_in_bytes = newW;
    Video_menuBuffer.format.width = newW;
    Video_otherBuf.format.width = newW;
    Video_menuBuffer.format.height = newH;
    Video_otherBuf.format.height = newH;
    
    _memcpy(&Video_format, &stdDisplay_pCurVideoMode->format, sizeof(stdVBufferTexFmt));
    _memcpy(&Video_format2, &stdDisplay_pCurVideoMode->format, sizeof(stdVBufferTexFmt));
    
    Video_format.width = newW;
    Video_format.height = newH;
    
    jkDev_Close();
    jkHud_Close();
    jkHudInv_Close();
    sithCamera_Close();
    rdCanvas_Free(Video_pCanvas);

    jkHudInv_LoadItemRes();
    jkHud_Open();
    jkDev_Open();
    
    Video_pCanvas = rdCanvas_New(2, Video_pMenuBuffer, Video_pVbufIdk, 0, 0, newW, newH, 6);
    sithCamera_Open(Video_pCanvas, stdDisplay_pCurVideoMode->widthMaybe);
}

int jkMain_SetVideoMode()
{
    signed int result; // eax
    wchar_t *v1; // eax
    wchar_t *v2; // eax
    wchar_t *v3; // [esp-4h] [ebp-10h]
    wchar_t *v4; // [esp-4h] [ebp-10h]

    if ( jkGame_isDDraw )
        return 0;
    
    /*if ( !sithNet_isMulti )
    {
        thing_six = 1;
        //sithControl_Close();
        v3 = jkStrings_GetText("ERR_CHANGING_VIDEO_DESC");
        v1 = jkStrings_GetText("ERR_CHANGING_VIDEO_MODE");
        jkGuiDialog_ErrorDialog(v1, v3);
        //sithControl_Open();
        thing_six = 0;
    }*/
    
    sithControl_Open();
    sithRender_SetRenderWeaponHandle(jkPlayer_renderSaberWeaponMesh);

    uint32_t newW = Window_xSize;
    uint32_t newH = Window_ySize;

    //if (jkGame_isDDraw)
    {
        newW = (uint32_t)((float)Window_xSize * ((480.0*2.0)/Window_ySize));
        newH = 480*2;
    }

    if (newW > Window_xSize)
    {
        newW = Window_xSize;
        newH = Window_ySize;
    }

    if (newW < 640)
        newW = 640;
    if (newH < 480)
        newH = 480;

    Video_modeStruct.viewSizeIdx = 0;
    Video_modeStruct.aViewSizes[Video_modeStruct.viewSizeIdx].xMin = 0;
    Video_modeStruct.aViewSizes[Video_modeStruct.viewSizeIdx].yMax = 0;
    Video_modeStruct.aViewSizes[Video_modeStruct.viewSizeIdx].xMax = newW / 2;
    Video_modeStruct.aViewSizes[Video_modeStruct.viewSizeIdx].yMax = newH / 2;

    stdDisplay_pCurVideoMode->format.width = newW;
    stdDisplay_pCurVideoMode->format.height = newH;
    stdDisplay_pCurVideoMode->widthMaybe = newW;
    stdDisplay_pCurVideoMode->format.width_in_pixels = newW;
    stdDisplay_pCurVideoMode->format.width_in_bytes = newW;
    
    Video_menuBuffer.format.width_in_pixels = newW;
    Video_otherBuf.format.width_in_pixels = newW;
    Video_menuBuffer.format.width_in_bytes = newW;
    Video_otherBuf.format.width_in_bytes = newW;
    Video_menuBuffer.format.width = newW;
    Video_otherBuf.format.width = newW;
    Video_menuBuffer.format.height = newH;
    Video_otherBuf.format.height = newH;
    
    _memcpy(&Video_format, &stdDisplay_pCurVideoMode->format, sizeof(stdVBufferTexFmt));
    _memcpy(&Video_format2, &stdDisplay_pCurVideoMode->format, sizeof(stdVBufferTexFmt));
    
    Video_format.width = newW;
    Video_format.height = newH;
    
    Window_AddMsgHandler(Windows_GdiHandler);
    
    stdPalEffects_RefreshPalette();
    sithRender_SetPalette(stdDisplay_GetPalette());

    jkHudInv_LoadItemRes();
    // Added close
    jkHud_Close();
    jkHud_Open();
    jkDev_Open();
    
    rdroid_curAcceleration = 1;
    Video_pCanvas = rdCanvas_New(2, Video_pMenuBuffer, Video_pVbufIdk, 0, 0, newW, newH, 6);
    sithRender_SetSomeRenderflag(0x2a);
    sithRender_SetGeoMode(Video_modeStruct.geoMode);
    sithRender_SetLightMode(Video_modeStruct.lightMode);
    sithRender_SetTexMode(Video_modeStruct.texMode);
    sithCamera_Open(Video_pCanvas, stdDisplay_pCurVideoMode->widthMaybe);

    stdDisplay_SetMode(0, 0, 0);

    Video_bOpened = 1;
    jkGame_isDDraw = 1;
    return 1;
}
#endif
