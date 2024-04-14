/*
    Copyright 2016-2023 melonDS team

    This file is part of melonDS.

    melonDS is free software: you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    melonDS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with melonDS. If not, see http://www.gnu.org/licenses/.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "Platform.h"
#include "Config.h"

#include <iostream>
#include <fstream>
#include "toml/toml.hpp"


namespace Config
{
using namespace melonDS;

int KeyMapping[12];
int JoyMapping[12];

int HKKeyMapping[HK_MAX];
int HKJoyMapping[HK_MAX];

int JoystickID;

int WindowWidth;
int WindowHeight;
bool WindowMaximized;

int ScreenRotation;
int ScreenGap;
int ScreenLayout;
bool ScreenSwap;
int ScreenSizing;
bool IntegerScaling;
int ScreenAspectTop;
int ScreenAspectBot;
bool ScreenFilter;

bool ScreenUseGL;
bool ScreenVSync;
int ScreenVSyncInterval;

int _3DRenderer;
bool Threaded3D;

int GL_ScaleFactor;
bool GL_BetterPolygons;

bool LimitFPS;
int MaxFPS;
bool AudioSync;
bool ShowOSD;

int ConsoleType;
bool DirectBoot;

#ifdef JIT_ENABLED
bool JIT_Enable = false;
int JIT_MaxBlockSize = 32;
bool JIT_BranchOptimisations = true;
bool JIT_LiteralOptimisations = true;
bool JIT_FastMemory = true;
#endif

bool ExternalBIOSEnable;

std::string BIOS9Path;
std::string BIOS7Path;
std::string FirmwarePath;

std::string DSiBIOS9Path;
std::string DSiBIOS7Path;
std::string DSiFirmwarePath;
std::string DSiNANDPath;

bool DLDIEnable;
std::string DLDISDPath;
int DLDISize;
bool DLDIReadOnly;
bool DLDIFolderSync;
std::string DLDIFolderPath;

bool DSiSDEnable;
std::string DSiSDPath;
int DSiSDSize;
bool DSiSDReadOnly;
bool DSiSDFolderSync;
std::string DSiSDFolderPath;

bool FirmwareOverrideSettings;
std::string FirmwareUsername;
int FirmwareLanguage;
int FirmwareBirthdayMonth;
int FirmwareBirthdayDay;
int FirmwareFavouriteColour;
std::string FirmwareMessage;
std::string FirmwareMAC;
std::string WifiSettingsPath = "wfcsettings.bin"; // Should this be configurable?

int MPAudioMode;
int MPRecvTimeout;

std::string LANDevice;
bool DirectLAN;

bool SavestateRelocSRAM;

int AudioInterp;
int AudioBitDepth;
int AudioVolume;
bool DSiVolumeSync;
int MicInputType;
std::string MicDevice;
std::string MicWavPath;

std::string LastROMFolder;
std::string LastBIOSFolder;

std::string RecentROMList[10];

std::string SaveFilePath;
std::string SavestatePath;
std::string CheatFilePath;

bool EnableCheats;

bool MouseHide;
int MouseHideSeconds;

bool PauseLostFocus;
std::string UITheme;

int64_t RTCOffset;

bool DSBatteryLevelOkay;
int DSiBatteryLevel;
bool DSiBatteryCharging;

bool DSiFullBIOSBoot;

#ifdef GDBSTUB_ENABLED
bool GdbEnabled;
int GdbPortARM7;
int GdbPortARM9;
bool GdbARM7BreakOnStartup;
bool GdbARM9BreakOnStartup;
#endif

CameraConfig Camera[2];


const char* kConfigFile = "melonDS.toml";

const char* kLegacyConfigFile = "melonDS.ini";
const char* kLegacyUniqueConfigFile = "melonDS.%d.ini";

toml::value RootTable;

LegacyEntry LegacyFile[] =
{
    {"Key_A",      0, "Keyboard.A", true},
    {"Key_B",      0, "Keyboard.B", true},
    {"Key_Select", 0, "Keyboard.Select", true},
    {"Key_Start",  0, "Keyboard.Start", true},
    {"Key_Right",  0, "Keyboard.Right", true},
    {"Key_Left",   0, "Keyboard.Left", true},
    {"Key_Up",     0, "Keyboard.Up", true},
    {"Key_Down",   0, "Keyboard.Down", true},
    {"Key_R",      0, "Keyboard.R", true},
    {"Key_L",      0, "Keyboard.L", true},
    {"Key_X",      0, "Keyboard.X", true},
    {"Key_Y",      0, "Keyboard.Y", true},

    {"Joy_A",      0, "Joystick.A", true},
    {"Joy_B",      0, "Joystick.B", true},
    {"Joy_Select", 0, "Joystick.Select", true},
    {"Joy_Start",  0, "Joystick.Start", true},
    {"Joy_Right",  0, "Joystick.Right", true},
    {"Joy_Left",   0, "Joystick.Left", true},
    {"Joy_Up",     0, "Joystick.Up", true},
    {"Joy_Down",   0, "Joystick.Down", true},
    {"Joy_R",      0, "Joystick.R", true},
    {"Joy_L",      0, "Joystick.L", true},
    {"Joy_X",      0, "Joystick.X", true},
    {"Joy_Y",      0, "Joystick.Y", true},

    {"HKKey_Lid",                 0, "Keyboard.HK_Lid", true},
    {"HKKey_Mic",                 0, "Keyboard.HK_Mic", true},
    {"HKKey_Pause",               0, "Keyboard.HK_Pause", true},
    {"HKKey_Reset",               0, "Keyboard.HK_Reset", true},
    {"HKKey_FastForward",         0, "Keyboard.HK_FastForward", true},
    {"HKKey_FastForwardToggle",   0, "Keyboard.HK_FastForwardToggle", true},
    {"HKKey_FullscreenToggle",    0, "Keyboard.HK_FullscreenToggle", true},
    {"HKKey_SwapScreens",         0, "Keyboard.HK_SwapScreens", true},
    {"HKKey_SwapScreenEmphasis",  0, "Keyboard.HK_SwapScreenEmphasis", true},
    {"HKKey_SolarSensorDecrease", 0, "Keyboard.HK_SolarSensorDecrease", true},
    {"HKKey_SolarSensorIncrease", 0, "Keyboard.HK_SolarSensorIncrease", true},
    {"HKKey_FrameStep",           0, "Keyboard.HK_FrameStep", true},
    {"HKKey_PowerButton",         0, "Keyboard.HK_PowerButton", true},
    {"HKKey_VolumeUp",            0, "Keyboard.HK_VolumeUp", true},
    {"HKKey_VolumeDown",          0, "Keyboard.HK_VolumeDown", true},

    {"HKJoy_Lid",                 0, "Joystick.HK_Lid", true},
    {"HKJoy_Mic",                 0, "Joystick.HK_Mic", true},
    {"HKJoy_Pause",               0, "Joystick.HK_Pause", true},
    {"HKJoy_Reset",               0, "Joystick.HK_Reset", true},
    {"HKJoy_FastForward",         0, "Joystick.HK_FastForward", true},
    {"HKJoy_FastForwardToggle",   0, "Joystick.HK_FastForwardToggle", true},
    {"HKJoy_FullscreenToggle",    0, "Joystick.HK_FullscreenToggle", true},
    {"HKJoy_SwapScreens",         0, "Joystick.HK_SwapScreens", true},
    {"HKJoy_SwapScreenEmphasis",  0, "Joystick.HK_SwapScreenEmphasis", true},
    {"HKJoy_SolarSensorDecrease", 0, "Joystick.HK_SolarSensorDecrease", true},
    {"HKJoy_SolarSensorIncrease", 0, "Joystick.HK_SolarSensorIncrease", true},
    {"HKJoy_FrameStep",           0, "Joystick.HK_FrameStep", true},
    {"HKJoy_PowerButton",         0, "Joystick.HK_PowerButton", true},
    {"HKJoy_VolumeUp",            0, "Joystick.HK_VolumeUp", true},
    {"HKJoy_VolumeDown",          0, "Joystick.HK_VolumeDown", true},

    {"JoystickID", 0, "Joystick.JoystickID", true},

    {"WindowWidth",  0, "Window0.Width", true},
    {"WindowHeight", 0, "Window0.Height", true},
    {"WindowMax",    1, "Window0.Maximized", true},

    {"ScreenRotation", 0, "Window0.ScreenRotation", true},
    {"ScreenGap",      0, "Window0.ScreenGap", true},
    {"ScreenLayout",   0, "Window0.ScreenLayout", true},
    {"ScreenSwap",     1, "Window0.ScreenSwap", true},
    {"ScreenSizing",   0, "Window0.ScreenSizing", true},
    {"IntegerScaling", 1, "Window0.IntegerScaling", true},
    {"ScreenAspectTop",0, "Window0.ScreenAspectTop", true},
    {"ScreenAspectBot",0, "Window0.ScreenAspectBot", true},
    {"ScreenFilter",   1, "Screen.Filter", true},

    {"ScreenUseGL",         1, "Screen.UseGL", false},
    {"ScreenVSync",         1, "Screen.VSync", false},
    {"ScreenVSyncInterval", 0, "Screen.VSyncInterval", false},

    {"3DRenderer", 0, "3D.Renderer", false},
    {"Threaded3D", 1, "3D.Soft.Threaded3D", false},

    {"GL_ScaleFactor", 0, "3D.GL.ScaleFactor", false},
    {"GL_BetterPolygons", 1, "3D.GL.BetterPolygons", false},

    {"LimitFPS", 1, "LimitFPS", false},
    {"MaxFPS", 0, "MaxFPS", false},
    {"AudioSync", 1, "AudioSync", false},
    {"ShowOSD", 1, "Window0.ShowOSD", false},

    {"ConsoleType", 0, "Emu.ConsoleType", false},
    {"DirectBoot", 1, "Emu.DirectBoot", false},

#ifdef JIT_ENABLED
    {"JIT_Enable", 1, "JIT.Enable", false},
    {"JIT_MaxBlockSize", 0, "JIT.MaxBlockSize", false},
    {"JIT_BranchOptimisations", 1, "JIT.BranchOptimisations", false},
    {"JIT_LiteralOptimisations", 1, "JIT.LiteralOptimisations", false},
#ifdef __APPLE__
    {"JIT_FastMemory", 1, "JIT.FastMemory", false},
#else
    {"JIT_FastMemory", 1, "JIT.FastMemory", false},
#endif
#endif

    {"ExternalBIOSEnable", 1, "Emu.ExternalBIOSEnable", false},

    {"BIOS9Path", 2, "DS.BIOS9Path", false},
    {"BIOS7Path", 2, "DS.BIOS7Path", false},
    {"FirmwarePath", 2, "DS.FirmwarePath", false},

    {"DSiBIOS9Path", 2, "DSi.BIOS9Path", false},
    {"DSiBIOS7Path", 2, "DSi.BIOS7Path", false},
    {"DSiFirmwarePath", 2, "DSi.FirmwarePath", false},
    {"DSiNANDPath", 2, "DSi.NANDPath", false},

    {"DLDIEnable", 1, "DLDI.Enable", false},
    {"DLDISDPath", 2, "DLDI.ImagePath", false},
    {"DLDISize", 0, "DLDI.ImageSize", false},
    {"DLDIReadOnly", 1, "DLDI.ReadOnly", false},
    {"DLDIFolderSync", 1, "DLDI.FolderSync", false},
    {"DLDIFolderPath", 2, "DLDI.FolderPath", false},

    {"DSiSDEnable", 1, "DSi.SD.Enable", false},
    {"DSiSDPath", 2, "DSi.SD.ImagePath", false},
    {"DSiSDSize", 0, "DSi.SD.ImageSize", false},
    {"DSiSDReadOnly", 1, "DSi.SD.ReadOnly", false},
    {"DSiSDFolderSync", 1, "DSi.SD.FolderSync", false},
    {"DSiSDFolderPath", 2, "DSi.SD.FolderPath", false},

    {"FirmwareOverrideSettings", 1, "Firmware.OverrideSettings", true},
    {"FirmwareUsername", 2, "Firmware.Username", true},
    {"FirmwareLanguage", 0, "Firmware.Language", true},
    {"FirmwareBirthdayMonth", 0, "Firmware.BirthdayMonth", true},
    {"FirmwareBirthdayDay", 0, "Firmware.BirthdayDay", true},
    {"FirmwareFavouriteColour", 0, "Firmware.FavouriteColour", true},
    {"FirmwareMessage", 2, "Firmware.Message", true},
    {"FirmwareMAC", 2, "Firmware.MAC", true},

    {"MPAudioMode", 0, "MP.AudioMode", false},
    {"MPRecvTimeout", 0, "MP.RecvTimeout", false},

    {"LANDevice", 2, "LAN.Device", false},
    {"DirectLAN", 1, "LAN.DirectMode", false},

    {"SavStaRelocSRAM", 1, "SaveState.RelocSRAM", false},

    {"AudioInterp", 0, "Audio.Interpolation", false},
    {"AudioBitDepth", 0, "Audio.BitDepth", false},
    {"AudioVolume", 0, "Audio.Volume", true},
    {"DSiVolumeSync", 1, "Audio.DSiVolumeSync", true},
    {"MicInputType", 0, "Mic.InputType", false},
    {"MicDevice", 2, "Mic.Device", false},
    {"MicWavPath", 2, "Mic.WavPath", false},

    {"LastROMFolder", 2, "LastROMFolder", false},
    {"LastBIOSFolder", 2, "LastBIOSFolder", false},

    {"RecentROM_0", 2, "RecentROM[0]", false},
    {"RecentROM_1", 2, "RecentROM[1]", false},
    {"RecentROM_2", 2, "RecentROM[2]", false},
    {"RecentROM_3", 2, "RecentROM[3]", false},
    {"RecentROM_4", 2, "RecentROM[4]", false},
    {"RecentROM_5", 2, "RecentROM[5]", false},
    {"RecentROM_6", 2, "RecentROM[6]", false},
    {"RecentROM_7", 2, "RecentROM[7]", false},
    {"RecentROM_8", 2, "RecentROM[8]", false},
    {"RecentROM_9", 2, "RecentROM[9]", false},

    {"SaveFilePath", 2, "SaveFilePath", true},
    {"SavestatePath", 2, "SavestatePath", true},
    {"CheatFilePath", 2, "CheatFilePath", true},

    {"EnableCheats", 1, "EnableCheats", true},

    {"MouseHide",        1, "Mouse.Hide", false},
    {"MouseHideSeconds", 0, "Mouse.HideSeconds", false},
    {"PauseLostFocus",   1, "PauseLostFocus", false},
    {"UITheme",          2, "UITheme", false},

    {"RTCOffset",       3, "RTC.Offset", true},

    {"DSBatteryLevelOkay",   1, "DS.Battery.LevelOkay", true},
    {"DSiBatteryLevel",    0, "DSi.Battery.Level", true},
    {"DSiBatteryCharging", 1, "DSi.Battery.Charging", true},

    {"DSiFullBIOSBoot", 1, "DSi.FullBIOSBoot", true},

#ifdef GDBSTUB_ENABLED
    {"GdbEnabled", 1, "Gdb.Enabled", false},
    {"GdbPortARM7", 0, "Gdb.ARM7.Port", true},
    {"GdbPortARM9", 0, "Gdb.ARM9.Port", true},
    {"GdbARM7BreakOnStartup", 1, "Gdb.ARM7.BreakOnStartup", true},
    {"GdbARM9BreakOnStartup", 1, "Gdb.ARM9.BreakOnStartup", true},
#endif

    {"Camera0_InputType", 0, "DSi.Camera0.InputType", false},
    {"Camera0_ImagePath", 2, "DSi.Camera0.ImagePath", false},
    {"Camera0_CamDeviceName", 2, "DSi.Camera0.DeviceName", false},
    {"Camera0_XFlip", 1, "DSi.Camera0.XFlip", false},
    {"Camera1_InputType", 0, "DSi.Camera1.InputType", false},
    {"Camera1_ImagePath", 2, "DSi.Camera1.ImagePath", false},
    {"Camera1_CamDeviceName", 2, "DSi.Camera1.DeviceName", false},
    {"Camera1_XFlip", 1, "DSi.Camera1.XFlip", false},

    {"", -1, "", false}
};

ConfigEntry ConfigFile[] =
{
    {"Key_A",      0, &KeyMapping[0],  -1, true},
    {"Key_B",      0, &KeyMapping[1],  -1, true},
    {"Key_Select", 0, &KeyMapping[2],  -1, true},
    {"Key_Start",  0, &KeyMapping[3],  -1, true},
    {"Key_Right",  0, &KeyMapping[4],  -1, true},
    {"Key_Left",   0, &KeyMapping[5],  -1, true},
    {"Key_Up",     0, &KeyMapping[6],  -1, true},
    {"Key_Down",   0, &KeyMapping[7],  -1, true},
    {"Key_R",      0, &KeyMapping[8],  -1, true},
    {"Key_L",      0, &KeyMapping[9],  -1, true},
    {"Key_X",      0, &KeyMapping[10], -1, true},
    {"Key_Y",      0, &KeyMapping[11], -1, true},

    {"Joy_A",      0, &JoyMapping[0],  -1, true},
    {"Joy_B",      0, &JoyMapping[1],  -1, true},
    {"Joy_Select", 0, &JoyMapping[2],  -1, true},
    {"Joy_Start",  0, &JoyMapping[3],  -1, true},
    {"Joy_Right",  0, &JoyMapping[4],  -1, true},
    {"Joy_Left",   0, &JoyMapping[5],  -1, true},
    {"Joy_Up",     0, &JoyMapping[6],  -1, true},
    {"Joy_Down",   0, &JoyMapping[7],  -1, true},
    {"Joy_R",      0, &JoyMapping[8],  -1, true},
    {"Joy_L",      0, &JoyMapping[9],  -1, true},
    {"Joy_X",      0, &JoyMapping[10], -1, true},
    {"Joy_Y",      0, &JoyMapping[11], -1, true},

    {"HKKey_Lid",                 0, &HKKeyMapping[HK_Lid],                 -1, true},
    {"HKKey_Mic",                 0, &HKKeyMapping[HK_Mic],                 -1, true},
    {"HKKey_Pause",               0, &HKKeyMapping[HK_Pause],               -1, true},
    {"HKKey_Reset",               0, &HKKeyMapping[HK_Reset],               -1, true},
    {"HKKey_FastForward",         0, &HKKeyMapping[HK_FastForward],         -1, true},
    {"HKKey_FastForwardToggle",   0, &HKKeyMapping[HK_FastForwardToggle],   -1, true},
    {"HKKey_FullscreenToggle",    0, &HKKeyMapping[HK_FullscreenToggle],    -1, true},
    {"HKKey_SwapScreens",         0, &HKKeyMapping[HK_SwapScreens],         -1, true},
    {"HKKey_SwapScreenEmphasis",  0, &HKKeyMapping[HK_SwapScreenEmphasis],  -1, true},
    {"HKKey_SolarSensorDecrease", 0, &HKKeyMapping[HK_SolarSensorDecrease], -1, true},
    {"HKKey_SolarSensorIncrease", 0, &HKKeyMapping[HK_SolarSensorIncrease], -1, true},
    {"HKKey_FrameStep",           0, &HKKeyMapping[HK_FrameStep],           -1, true},
    {"HKKey_PowerButton",         0, &HKKeyMapping[HK_PowerButton],         -1, true},
    {"HKKey_VolumeUp",            0, &HKKeyMapping[HK_VolumeUp],            -1, true},
    {"HKKey_VolumeDown",          0, &HKKeyMapping[HK_VolumeDown],          -1, true},

    {"HKJoy_Lid",                 0, &HKJoyMapping[HK_Lid],                 -1, true},
    {"HKJoy_Mic",                 0, &HKJoyMapping[HK_Mic],                 -1, true},
    {"HKJoy_Pause",               0, &HKJoyMapping[HK_Pause],               -1, true},
    {"HKJoy_Reset",               0, &HKJoyMapping[HK_Reset],               -1, true},
    {"HKJoy_FastForward",         0, &HKJoyMapping[HK_FastForward],         -1, true},
    {"HKJoy_FastForwardToggle",   0, &HKJoyMapping[HK_FastForwardToggle],   -1, true},
    {"HKJoy_FullscreenToggle",    0, &HKJoyMapping[HK_FullscreenToggle],    -1, true},
    {"HKJoy_SwapScreens",         0, &HKJoyMapping[HK_SwapScreens],         -1, true},
    {"HKJoy_SwapScreenEmphasis",  0, &HKJoyMapping[HK_SwapScreenEmphasis],  -1, true},
    {"HKJoy_SolarSensorDecrease", 0, &HKJoyMapping[HK_SolarSensorDecrease], -1, true},
    {"HKJoy_SolarSensorIncrease", 0, &HKJoyMapping[HK_SolarSensorIncrease], -1, true},
    {"HKJoy_FrameStep",           0, &HKJoyMapping[HK_FrameStep],           -1, true},
    {"HKJoy_PowerButton",         0, &HKJoyMapping[HK_PowerButton],         -1, true},
    {"HKJoy_VolumeUp",            0, &HKJoyMapping[HK_VolumeUp],            -1, true},
    {"HKJoy_VolumeDown",          0, &HKJoyMapping[HK_VolumeDown],          -1, true},

    {"JoystickID", 0, &JoystickID, 0, true},

    {"WindowWidth",  0, &WindowWidth,  256, true},
    {"WindowHeight", 0, &WindowHeight, 384, true},
    {"WindowMax",    1, &WindowMaximized, false, true},

    {"ScreenRotation", 0, &ScreenRotation, 0, true},
    {"ScreenGap",      0, &ScreenGap,      0, true},
    {"ScreenLayout",   0, &ScreenLayout,   0, true},
    {"ScreenSwap",     1, &ScreenSwap,     false, true},
    {"ScreenSizing",   0, &ScreenSizing,   0, true},
    {"IntegerScaling", 1, &IntegerScaling, false, true},
    {"ScreenAspectTop",0, &ScreenAspectTop,0, true},
    {"ScreenAspectBot",0, &ScreenAspectBot,0, true},
    {"ScreenFilter",   1, &ScreenFilter,   true, true},

    {"ScreenUseGL",         1, &ScreenUseGL,         false, false},
    {"ScreenVSync",         1, &ScreenVSync,         false, false},
    {"ScreenVSyncInterval", 0, &ScreenVSyncInterval, 1, false},

    {"3DRenderer", 0, &_3DRenderer, 0, false},
    {"Threaded3D", 1, &Threaded3D, true, false},

    {"GL_ScaleFactor", 0, &GL_ScaleFactor, 1, false},
    {"GL_BetterPolygons", 1, &GL_BetterPolygons, false, false},

    {"LimitFPS", 1, &LimitFPS, true, false},
    {"MaxFPS", 0, &MaxFPS, 1000, false},
    {"AudioSync", 1, &AudioSync, false},
    {"ShowOSD", 1, &ShowOSD, true, false},

    {"ConsoleType", 0, &ConsoleType, 0, false},
    {"DirectBoot", 1, &DirectBoot, true, false},

#ifdef JIT_ENABLED
    {"JIT_Enable", 1, &JIT_Enable, false, false},
    {"JIT_MaxBlockSize", 0, &JIT_MaxBlockSize, 32, false},
    {"JIT_BranchOptimisations", 1, &JIT_BranchOptimisations, true, false},
    {"JIT_LiteralOptimisations", 1, &JIT_LiteralOptimisations, true, false},
    #ifdef __APPLE__
        {"JIT_FastMemory", 1, &JIT_FastMemory, false, false},
    #else
        {"JIT_FastMemory", 1, &JIT_FastMemory, true, false},
    #endif
#endif

    {"ExternalBIOSEnable", 1, &ExternalBIOSEnable, false, false},

    {"BIOS9Path", 2, &BIOS9Path, (std::string)"", false},
    {"BIOS7Path", 2, &BIOS7Path, (std::string)"", false},
    {"FirmwarePath", 2, &FirmwarePath, (std::string)"", false},

    {"DSiBIOS9Path", 2, &DSiBIOS9Path, (std::string)"", false},
    {"DSiBIOS7Path", 2, &DSiBIOS7Path, (std::string)"", false},
    {"DSiFirmwarePath", 2, &DSiFirmwarePath, (std::string)"", false},
    {"DSiNANDPath", 2, &DSiNANDPath, (std::string)"", false},

    {"DLDIEnable", 1, &DLDIEnable, false, false},
    {"DLDISDPath", 2, &DLDISDPath, (std::string)"dldi.bin", false},
    {"DLDISize", 0, &DLDISize, 0, false},
    {"DLDIReadOnly", 1, &DLDIReadOnly, false, false},
    {"DLDIFolderSync", 1, &DLDIFolderSync, false, false},
    {"DLDIFolderPath", 2, &DLDIFolderPath, (std::string)"", false},

    {"DSiSDEnable", 1, &DSiSDEnable, false, false},
    {"DSiSDPath", 2, &DSiSDPath, (std::string)"dsisd.bin", false},
    {"DSiSDSize", 0, &DSiSDSize, 0, false},
    {"DSiSDReadOnly", 1, &DSiSDReadOnly, false, false},
    {"DSiSDFolderSync", 1, &DSiSDFolderSync, false, false},
    {"DSiSDFolderPath", 2, &DSiSDFolderPath, (std::string)"", false},

    {"FirmwareOverrideSettings", 1, &FirmwareOverrideSettings, false, true},
    {"FirmwareUsername", 2, &FirmwareUsername, (std::string)"melonDS", true},
    {"FirmwareLanguage", 0, &FirmwareLanguage, 1, true},
    {"FirmwareBirthdayMonth", 0, &FirmwareBirthdayMonth, 1, true},
    {"FirmwareBirthdayDay", 0, &FirmwareBirthdayDay, 1, true},
    {"FirmwareFavouriteColour", 0, &FirmwareFavouriteColour, 0, true},
    {"FirmwareMessage", 2, &FirmwareMessage, (std::string)"", true},
    {"FirmwareMAC", 2, &FirmwareMAC, (std::string)"", true},

    {"MPAudioMode", 0, &MPAudioMode, 1, false},
    {"MPRecvTimeout", 0, &MPRecvTimeout, 25, false},

    {"LANDevice", 2, &LANDevice, (std::string)"", false},
    {"DirectLAN", 1, &DirectLAN, false, false},

    {"SavStaRelocSRAM", 1, &SavestateRelocSRAM, false, false},

    {"AudioInterp", 0, &AudioInterp, 0, false},
    {"AudioBitDepth", 0, &AudioBitDepth, 0, false},
    {"AudioVolume", 0, &AudioVolume, 256, true},
    {"DSiVolumeSync", 1, &DSiVolumeSync, false, true},
    {"MicInputType", 0, &MicInputType, 1, false},
    {"MicDevice", 2, &MicDevice, (std::string)"", false},
    {"MicWavPath", 2, &MicWavPath, (std::string)"", false},

    {"LastROMFolder", 2, &LastROMFolder, (std::string)"", true},
    {"LastBIOSFolder", 2, &LastBIOSFolder, (std::string)"", true},

    {"RecentROM_0", 2, &RecentROMList[0], (std::string)"", true},
    {"RecentROM_1", 2, &RecentROMList[1], (std::string)"", true},
    {"RecentROM_2", 2, &RecentROMList[2], (std::string)"", true},
    {"RecentROM_3", 2, &RecentROMList[3], (std::string)"", true},
    {"RecentROM_4", 2, &RecentROMList[4], (std::string)"", true},
    {"RecentROM_5", 2, &RecentROMList[5], (std::string)"", true},
    {"RecentROM_6", 2, &RecentROMList[6], (std::string)"", true},
    {"RecentROM_7", 2, &RecentROMList[7], (std::string)"", true},
    {"RecentROM_8", 2, &RecentROMList[8], (std::string)"", true},
    {"RecentROM_9", 2, &RecentROMList[9], (std::string)"", true},

    {"SaveFilePath", 2, &SaveFilePath, (std::string)"", true},
    {"SavestatePath", 2, &SavestatePath, (std::string)"", true},
    {"CheatFilePath", 2, &CheatFilePath, (std::string)"", true},

    {"EnableCheats", 1, &EnableCheats, false, true},

    {"MouseHide",        1, &MouseHide,        false, false},
    {"MouseHideSeconds", 0, &MouseHideSeconds, 5, false},
    {"PauseLostFocus",   1, &PauseLostFocus,   false, false},
    {"UITheme",          2, &UITheme, (std::string)"", false},

    {"RTCOffset",       3, &RTCOffset,       (int64_t)0, true},

    {"DSBatteryLevelOkay",   1, &DSBatteryLevelOkay, true, true},
    {"DSiBatteryLevel",    0, &DSiBatteryLevel, 0xF, true},
    {"DSiBatteryCharging", 1, &DSiBatteryCharging, true, true},

    {"DSiFullBIOSBoot", 1, &DSiFullBIOSBoot, false, true},

#ifdef GDBSTUB_ENABLED
    {"GdbEnabled", 1, &GdbEnabled, false, false},
    {"GdbPortARM7", 0, &GdbPortARM7, 3334, true},
    {"GdbPortARM9", 0, &GdbPortARM9, 3333, true},
    {"GdbARM7BreakOnStartup", 1, &GdbARM7BreakOnStartup, false, true},
    {"GdbARM9BreakOnStartup", 1, &GdbARM9BreakOnStartup, false, true},
#endif

    // TODO!!
    // we need a more elegant way to deal with this
    {"Camera0_InputType", 0, &Camera[0].InputType, 0, false},
    {"Camera0_ImagePath", 2, &Camera[0].ImagePath, (std::string)"", false},
    {"Camera0_CamDeviceName", 2, &Camera[0].CamDeviceName, (std::string)"", false},
    {"Camera0_XFlip", 1, &Camera[0].XFlip, false, false},
    {"Camera1_InputType", 0, &Camera[1].InputType, 0, false},
    {"Camera1_ImagePath", 2, &Camera[1].ImagePath, (std::string)"", false},
    {"Camera1_CamDeviceName", 2, &Camera[1].CamDeviceName, (std::string)"", false},
    {"Camera1_XFlip", 1, &Camera[1].XFlip, false, false},

    {"", -1, nullptr, 0, false}
};


bool LoadFile(int inst, int actualinst)
{
    Platform::FileHandle* f;
    if (inst > 0)
    {
        char name[100] = {0};
        snprintf(name, 99, kLegacyUniqueConfigFile, inst+1);
        f = Platform::OpenLocalFile(name, Platform::FileMode::ReadText);

        if (!Platform::CheckLocalFileWritable(name)) return false;
    }
    else
    {
        f = Platform::OpenLocalFile(kLegacyConfigFile, Platform::FileMode::ReadText);

        if (actualinst == 0 && !Platform::CheckLocalFileWritable(kLegacyConfigFile)) return false;
    }

    if (!f) return true;

    char linebuf[1024];
    char entryname[32];
    char entryval[1024];
    while (!Platform::IsEndOfFile(f))
    {
        if (!Platform::FileReadLine(linebuf, 1024, f))
            break;

        int ret = sscanf(linebuf, "%31[A-Za-z_0-9]=%[^\t\r\n]", entryname, entryval);
        entryname[31] = '\0';
        if (ret < 2) continue;

        for (ConfigEntry* entry = &ConfigFile[0]; entry->Value; entry++)
        {
            if (!strncmp(entry->Name, entryname, 32))
            {
                if ((inst > 0) && (!entry->InstanceUnique))
                    break;

                switch (entry->Type)
                {
                case 0: *(int*)entry->Value = strtol(entryval, NULL, 10); break;
                case 1: *(bool*)entry->Value = strtol(entryval, NULL, 10) ? true:false; break;
                case 2: *(std::string*)entry->Value = entryval; break;
                case 3: *(int64_t*)entry->Value = strtoll(entryval, NULL, 10); break;
                }

                break;
            }
        }
    }

    CloseFile(f);
    return true;
}

bool LoadLegacyFile(int inst)
{
    /*Platform::FileHandle* f;
    if (inst > 0)
    {
        char name[100] = {0};
        snprintf(name, 99, kLegacyUniqueConfigFile, inst+1);
        f = Platform::OpenLocalFile(name, Platform::FileMode::ReadText);
    }
    else
    {
        f = Platform::OpenLocalFile(kLegacyConfigFile, Platform::FileMode::ReadText);
    }

    if (!f) return true;

    auto table = GetLocalTable(inst);

    char linebuf[1024];
    char entryname[32];
    char entryval[1024];
    while (!Platform::IsEndOfFile(f))
    {
        if (!Platform::FileReadLine(linebuf, 1024, f))
            break;

        int ret = sscanf(linebuf, "%31[A-Za-z_0-9]=%[^\t\r\n]", entryname, entryval);
        entryname[31] = '\0';
        if (ret < 2) continue;

        for (LegacyEntry* entry = &LegacyFile[0]; entry->Type != -1; entry++)
        {
            if (!strncmp(entry->Name, entryname, 32))
            {
                if (entry->InstanceUnique ^ (inst == -1))
                    break;

                const char* path = entry->TOMLPath;
                auto node = table.at_path(path);

                switch (entry->Type)
                {
                    case 0:
                        node.value<int>() = strtol(entryval, NULL, 10);
                        break;

                    case 1:
                        node.value<bool>() = !!strtol(entryval, NULL, 10);
                        break;

                    case 2:
                        node.value<std::string>() = entryval;
                        break;

                    case 3:
                        node.value<int64_t>() = strtoll(entryval, NULL, 10);
                        break;
                }

                break;
            }
        }
    }

    CloseFile(f);*/
    return true;
}

bool LoadLegacy()
{
    for (int i = -1; i < 16; i++)
        LoadLegacyFile(i);

    return true;
}

bool Load()
{
    auto cfgpath = Platform::GetLocalFilePath(kConfigFile);

    if (!Platform::CheckFileWritable(cfgpath))
        return false;

    /*RootTable = toml::table();

    if (!Platform::FileExists(cfgpath))
        return LoadLegacy();

    try
    {
        RootTable = toml::parse_file(cfgpath);
    }
    catch (toml::parse_error& err)
    {
        //RootTable = toml::table();
    }*/

    return true;
}

void Save()
{
    auto cfgpath = Platform::GetLocalFilePath(kConfigFile);
printf("save\n");
    if (!Platform::CheckFileWritable(cfgpath))
        return;
    printf("zirz\n");
    RootTable["test"] = 4444;
    //std::string derp = "sfsdf";
    //toml::serializer<std::string> vorp(RootTable);
    //toml::serializer<toml::string> zarp;

    std::cout << RootTable;
    printf("blarg\n");
    std::ofstream file;
    file.open(cfgpath, std::ofstream::out | std::ofstream::trunc);
    file << RootTable;
    file.close();
}


/*toml::node_view<toml::node> GetLocalTable(int instance)
{
    if (instance == -1)
        return toml::node_view(RootTable);
    else
        return RootTable["Instance" + std::to_string(instance)];
}*/

}
