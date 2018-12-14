/*
    Copyright 2016-2019 StapleButter

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

#include <stdlib.h>
#include <stdio.h>

#include "libui/ui.h"

#include "../types.h"
#include "../Config.h"

#include "DlgAudioSettings.h"


namespace DlgAudioSettings
{

bool opened;
uiWindow* win;

//


int OnCloseWindow(uiWindow* window, void* blarg)
{
    opened = false;
    return 1;
}

void OnCancel(uiButton* btn, void* blarg)
{
    uiControlDestroy(uiControl(win));
    opened = false;
}

void OnOk(uiButton* btn, void* blarg)
{
    /*Config::DirectBoot = uiCheckboxChecked(cbDirectBoot);
    Config::Threaded3D = uiCheckboxChecked(cbThreaded3D);
    Config::SocketBindAnyAddr = uiCheckboxChecked(cbBindAnyAddr);*/

    Config::Save();

    uiControlDestroy(uiControl(win));
    opened = false;
}

void Open()
{
    if (opened)
    {
        uiControlSetFocus(uiControl(win));
        return;
    }

    opened = true;
    win = uiNewWindow("Audio settings - melonDS", 400, 100, 0, 0);
    uiWindowSetMargined(win, 1);
    uiWindowOnClosing(win, OnCloseWindow, NULL);

    uiBox* top = uiNewVerticalBox();
    uiWindowSetChild(win, uiControl(top));
    uiBoxSetPadded(top, 1);

    {
        uiGroup* grp = uiNewGroup("Audio output");
        uiBoxAppend(top, uiControl(grp), 0);
        uiGroupSetMargined(grp, 1);

        uiBox* in_ctrl = uiNewVerticalBox();
        uiGroupSetChild(grp, uiControl(in_ctrl));

        uiLabel* label_vol = uiNewLabel("Volume:");
        uiBoxAppend(in_ctrl, uiControl(label_vol), 0);

        uiSlider* volslider = uiNewSlider(0, 255);
        uiBoxAppend(in_ctrl, uiControl(volslider), 0);
    }

    {
        uiGroup* grp = uiNewGroup("Microphone input");
        uiBoxAppend(top, uiControl(grp), 0);
        uiGroupSetMargined(grp, 1);

        uiBox* in_ctrl = uiNewVerticalBox();
        uiGroupSetChild(grp, uiControl(in_ctrl));

        uiRadioButtons* mictypes = uiNewRadioButtons();
        uiRadioButtonsAppend(mictypes, "None");
        uiRadioButtonsAppend(mictypes, "Microphone");
        uiRadioButtonsAppend(mictypes, "White noise");
        uiRadioButtonsAppend(mictypes, "WAV file:");
        uiBoxAppend(in_ctrl, uiControl(mictypes), 0);

        uiBox* path_box = uiNewHorizontalBox();
        uiBoxAppend(in_ctrl, uiControl(path_box), 0);

        uiEntry* path_entry = uiNewEntry();
        uiBoxAppend(path_box, uiControl(path_entry), 1);

        uiButton* path_browse = uiNewButton("...");
        uiBoxAppend(path_box, uiControl(path_browse), 0);
    }

    {
        uiBox* in_ctrl = uiNewHorizontalBox();
        uiBoxSetPadded(in_ctrl, 1);
        uiBoxAppend(top, uiControl(in_ctrl), 0);

        uiLabel* dummy = uiNewLabel("");
        uiBoxAppend(in_ctrl, uiControl(dummy), 1);

        uiButton* btncancel = uiNewButton("Cancel");
        uiButtonOnClicked(btncancel, OnCancel, NULL);
        uiBoxAppend(in_ctrl, uiControl(btncancel), 0);

        uiButton* btnok = uiNewButton("Ok");
        uiButtonOnClicked(btnok, OnOk, NULL);
        uiBoxAppend(in_ctrl, uiControl(btnok), 0);
    }

    // shit

    uiControlShow(uiControl(win));
}

}