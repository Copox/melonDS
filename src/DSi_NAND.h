/*
    Copyright 2016-2021 Arisotura

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

#ifndef DSI_NAND_H
#define DSI_NAND_H

#include "types.h"
#include <vector>
#include <string>

namespace DSi_NAND
{

bool Init(FILE* nand, u8* es_keyY);
void DeInit();

void GetIDs(u8* emmc_cid, u64& consoleid);

void PatchTSC();

void ListTitles(u32 category, std::vector<u32>& titlelist);
bool TitleExists(u32 category, u32 titleid);
void GetTitleInfo(u32 category, u32 titleid, u32& version, u8* header, u8* banner);
bool ImportTitle(const char* appfile, u8* tmd, bool readonly);
void DeleteTitle(u32 category, u32 titleid);

}

#endif // DSI_NAND_H