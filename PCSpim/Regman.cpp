// SPIM S20 MIPS simulator.
// Definitions for the SPIM S20.
//
// Copyright (c) 1990-2010, James R. Larus.
// Changes for DOS and Windows versions by David A. Carley (dac@cs.wisc.edu)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation and/or
// other materials provided with the distribution.
//
// Neither the name of the James R. Larus nor the names of its contributors may be
// used to endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//


#include "stdafx.h"
#include "regman.h"

LONG RegMan_CreateKey(HKEY hKey, LPCTSTR lpstrKeyName, PHKEY phKeyDest)
{
    DWORD	disp = 0;

    return RegCreateKeyEx(hKey,
        lpstrKeyName,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS,
        NULL,
        phKeyDest,
        &disp);
}


LONG RegMan_DeleteKey(HKEY hKey, LPCTSTR lpstrKeyName)
{
    return RegDeleteKey(hKey, lpstrKeyName);
}


LONG RegMan_OpenKey(HKEY hKey, LPCTSTR lpstrKeyName, PHKEY phKeyDest)
{
    return RegOpenKeyEx(hKey, lpstrKeyName, 0, KEY_ALL_ACCESS, phKeyDest);
}


LONG RegMan_GetDWORDValue(HKEY hKey, LPCTSTR lpstrValueName, LPDWORD pdwDest)
{
    LONG retVal;

    DWORD dwType;
    DWORD dwcbDest = sizeof(DWORD);

    retVal = RegQueryValueEx(hKey,
        lpstrValueName,
        0,
        &dwType,
        (LPBYTE)pdwDest,
        &dwcbDest);

    if ((retVal == ERROR_SUCCESS) && (dwType != REG_DWORD))
        return ERROR_CANTREAD;
    else
        return retVal;
}


LONG RegMan_SetSZValue(HKEY hKey, LPCTSTR lpstrValueName, LPCTSTR lpstrSrc)
{
    return RegSetValueEx(hKey,
        lpstrValueName,
        0,
        REG_SZ,
        (LPBYTE)lpstrSrc,
        (DWORD)lstrlen(lpstrSrc) + 1);
}


LONG RegMan_GetSZValue(HKEY hKey,
    LPCTSTR lpstrValueName,
    LPTSTR lpstrDest,
    LPDWORD plcbDest)
{
    LONG retVal;
    DWORD dwType;

    retVal = RegQueryValueEx(hKey,
        lpstrValueName,
        0,
        &dwType,
        (LPBYTE)lpstrDest,
        plcbDest);

    if ((retVal == ERROR_SUCCESS) && (dwType != REG_SZ))
        return ERROR_CANTREAD;
    else
        return retVal;
}


LONG RegMan_GetNumValues(HKEY hKey, LPDWORD plcValues)
{
    return RegQueryInfoKey(hKey,
        NULL,
        NULL,
        0,
        NULL,
        NULL,
        NULL,
        plcValues,
        NULL,
        NULL,
        NULL,
        NULL);
}


LONG RegMan_EnumValueName(HKEY hKey,
    DWORD dwIndex,
    LPTSTR lpstrValueName,
    LPDWORD pdwcbValueName)
{
    return RegEnumValue(hKey,
        dwIndex,
        lpstrValueName,
        pdwcbValueName,
        0,
        NULL,
        NULL,
        NULL);
}

LONG RegMan_EnumSZValue(HKEY hKey,
    DWORD dwIndex,
    LPTSTR lpstrValueName,
    LPDWORD pdwcbValueName,
    LPTSTR lpstrData,
    LPDWORD pdwcbData)
{
    LONG retVal;
    DWORD dwType;

    retVal = RegEnumValue(hKey,
        dwIndex,
        lpstrValueName,
        pdwcbValueName,
        0,
        &dwType,
        (LPBYTE)lpstrData,
        pdwcbData);

    if ((retVal == ERROR_SUCCESS) && (dwType != REG_SZ))
        return ERROR_CANTREAD;
    else
        return retVal;
}


LONG RegMan_EnumKey(HKEY hKey,
    DWORD dwIndex,
    LPTSTR lpstrName,
    LPDWORD pdwcbName)
{
    return RegEnumKeyEx(hKey,
        dwIndex,
        lpstrName,
        pdwcbName,
        0,
        NULL,
        NULL,
        NULL);
}


LONG RegMan_DeleteValue(HKEY hKey, LPCTSTR lpstrValueName)
{
    return RegDeleteValue(hKey, lpstrValueName);
}


LONG RegMan_SetDWORDValue(HKEY hKey, LPCTSTR lpstrValueName, DWORD dwData)
{
    return RegSetValueEx(hKey,
        lpstrValueName,
        0,
        REG_DWORD,
        (LPBYTE)&dwData,
        sizeof(DWORD));
}


LONG RegMan_CloseKey(HKEY hKey)
{
    return RegCloseKey(hKey);
}


LONG RegMan_DeleteTree(HKEY hKey, LPCTSTR szTreeName)
{
    char szName[MAX_PATH];
    DWORD cbName;
    HKEY hKeyDel;

    RegMan_OpenKey(hKey, szTreeName, &hKeyDel);

    // Delete all values.
    cbName = MAX_PATH;
    while (S_OK == RegMan_EnumValueName(hKeyDel, 0, szName, &cbName))
    {
        RegMan_DeleteValue(hKeyDel, szName);
        cbName = MAX_PATH;
    }

    // Delete all sub keys

    cbName = MAX_PATH;
    while (S_OK == RegMan_EnumKey(hKeyDel, 0, szName, &cbName))
    {
        RegMan_DeleteTree(hKeyDel, szName);
        cbName = MAX_PATH;
    }

    RegMan_CloseKey(hKeyDel);
    RegMan_DeleteKey(hKey, szTreeName);

    return S_OK;
}


LONG RegMan_CopyTree(HKEY hKeySrc, HKEY hKeyDest)
{
    LPTSTR szName;
    LPBYTE rgbData;
    DWORD cbName, cbData, i, dwType;
    HKEY hKeyCopy, hKeyNew;

    DWORD cSubKeys, cbMaxSubKeyNameLen, cValues, cbMaxValueNameLen, cbMaxValueLen;
    RegQueryInfoKey(hKeySrc,
        NULL,
        NULL,
        NULL,
        &cSubKeys,
        &cbMaxSubKeyNameLen,
        NULL,
        &cValues,
        &cbMaxValueNameLen,
        &cbMaxValueLen,
        NULL,
        NULL);

    ++cbMaxValueNameLen;
    ++cbMaxValueLen;
    ++cbMaxSubKeyNameLen;

    szName = new TCHAR[cbMaxValueNameLen];
    rgbData = new BYTE[cbMaxValueLen];

    // Copy the values
    cbName = cbMaxValueNameLen;
    cbData = cbMaxValueLen;
    for (i = 0; ; ++i)
    {
        // The the source value info
        if (S_OK != RegEnumValue(hKeySrc,
            i,
            szName,
            &cbName,
            NULL,
            &dwType,
            rgbData,
            &cbData))
            break;

        // Create the destination value
        RegSetValueEx(hKeyDest, szName, NULL, dwType, rgbData, cbData);

        // Reset for the next call.
        cbName = cbMaxValueNameLen;
        cbData = cbMaxValueLen;
    }

    delete [] szName;
    delete [] rgbData;

    szName = new TCHAR[cbMaxSubKeyNameLen];
    cbName = cbMaxSubKeyNameLen;
    for (i = 0; ; ++i)
    {
        if (S_OK != RegEnumKeyEx(hKeySrc,
            i,
            szName,
            &cbName,
            NULL,
            NULL,
            NULL,
            NULL))
            break;

        RegOpenKeyEx(hKeySrc, szName, NULL, KEY_ALL_ACCESS, &hKeyCopy);
        RegCreateKeyEx(hKeyDest, szName, NULL, NULL, REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS, NULL, &hKeyNew, NULL);

        RegMan_CopyTree(hKeyCopy, hKeyNew);

        RegCloseKey(hKeyCopy);
        RegCloseKey(hKeyNew);

        cbName = cbMaxSubKeyNameLen;
    }

    delete [] szName;

    return S_OK;
}

