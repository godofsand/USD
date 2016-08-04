//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#include "pxr/base/arch/fileSystem.h"
#include "pxr/base/arch/nap.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#if defined(ARCH_OS_WINDOWS)
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#include <csignal>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void
crash(int sig) {
        printf("crashed!\n");
        exit(sig);
}

#if defined(ARCH_OS_WINDOWS)
bool rmdir(const std::string& strDir)
{
    WIN32_FIND_DATA fdFile = {0};
    std::string strSearch = strDir + "\\*.*" ;

    HANDLE hFind = ::FindFirstFile (strSearch.data (), &fdFile);
    if(hFind == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    do {    
        std::string strDelete = strDir + "\\" + fdFile.cFileName;
        if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (fdFile.cFileName == "." || fdFile.cFileName == "..")
            {
                continue;
            }
            rmdir (strDelete);
        }
        else
        {
            ::DeleteFile (strDelete.data ());
        }
    } while (::FindNextFile (hFind, &fdFile));

    ::FindClose (hFind);
    ::RemoveDirectory (strDir.data ());
    return true;
}
#endif 

int main()
{
    (void) signal(SIGABRT,crash);

    std::string firstName = ArchMakeTmpFileName("archFS");
    FILE *firstFile;

    (void)firstStat;
    (void)secondStat;
    (void)firstStatAgain;
    // Open a file, check that its length is 0, write to it, close it, and then check that
    // its length is now the number of characters written.
    assert((firstFile = fopen(firstName.c_str(), "w")) != NULL);
    fflush(firstFile);
    assert(ArchGetFileLength(firstName.c_str()) == 0);
    fputs("text in a file\n", firstFile);
    fclose(firstFile);
    assert(ArchGetFileLength(firstName.c_str()) == 15);

    ArchUnlinkFile(firstName.c_str());
    ArchUnlinkFile(secondName.c_str());
    ArchUnlinkFile(thirdName.c_str());

    // create and remove a tmp subdir
    std::string retpath;
    retpath = ArchMakeTmpSubdir(ArchGetTmpDir(), "myprefix");
    assert (retpath != "");
    rmdir(retpath.c_str());
    return 0;
}