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


#ifndef _REGMAN_H_
#define _REGMAN_H_

#include <windows.h>
#include "winreg.h"

//
//	RegMan_CreateKey
//
//	Purpose:	Use to create a key in the registry.  This function works
//				on both Win32 and Win16.
//
//	Return Value:	ERROR_SUCCESS if successful
//					Error value otherwise(in winerror.h)
//
LONG RegMan_CreateKey(HKEY hKey, LPCTSTR lpstrKeyName, PHKEY phKeyDest);


//
//	RegMan_DeleteKey
//
//	Purpose:	Use to delete a key in the registry.  This function works
//				on both Win32 and Win16.
//
//	Return Value:	ERROR_SUCCESS if successful
//					Error value otherwise(in winerror.h)
//
LONG RegMan_DeleteKey(HKEY hKey, LPCTSTR lpstrKeyName);


//
//	RegMan_OpenKey
//
//	Purpose:	Use to open a key in the registry.  This function works
//				on both Win32 and Win16
//
//	Return Value:	ERROR_SUCCESS if successful
//					Error value otherwise(in winerror.h)
//
LONG RegMan_OpenKey(HKEY hKey, LPCTSTR lpstrKeyName, PHKEY phKeyDest);


//
//	RegMan_GetDWORDValue
//
//	Purpose:	Use to retrieve a DWORD from a registry "value".  This function
//				works on both Win32 and Win16
//
//	Return Value:	ERROR_SUCCESS if successful
//					Error value otherwise(in winerror.h)
//
LONG RegMan_GetDWORDValue(HKEY hKey, LPCTSTR lpstrValueName, LPDWORD pdwDest);


//
//	RegMan_SetSZValue
//
//	Purpose:	Use to set a string to a registry "value".  This function
//				works on both Win32 and Win16
//
//	Return Value:	ERROR_SUCCESS if successful
//					Error value otherwise(in winerror.h)
//
LONG RegMan_SetSZValue(HKEY hKey, LPCTSTR lpstrValueName, LPCTSTR lpstrSrc);


//
//	RegMan_GetSZValue
//
//	Purpose:	Use to retrieve a string from a registry "value".  This function
//				works on both Win32 and Win16
//
//	Return Value:	ERROR_SUCCESS if successful
//					Error value otherwise(in winerror.h)
//
LONG RegMan_GetSZValue(HKEY hKey, LPCTSTR lpstrValueName, LPTSTR lpstrDest, LPDWORD plcbDest);


//
//	RegMan_GetNumValues
//
//	Purpose:	Use to retrieve the number of "values" directly under a key
//				in the registry.  This function works on both Win32 and Win16
//
//	Return Value:	ERROR_SUCCESS if successful
//					Error value otherwise(in winerror.h)
//
LONG RegMan_GetNumValues(HKEY hKey, LPDWORD plcValues);


//
//	RegMan_EnumValueName
//
//	Purpose:	Use to enumerate the names of the "values" under a key
//				in the registry.  This function works in both Win32 and Win16.
//
//	Return Value:	ERROR_SUCCESS if successful
//					Error value otherwise(in winerror.h)
//
LONG RegMan_EnumValueName(HKEY hKey, DWORD dwIndex, LPTSTR lpstrValueName, LPDWORD pdwcbValueName);


//
//	RegMan_EnumSZValue
//
//	Purpose:	Use to enumerate the names _and_ string contents of the "values"
//				under a key in the registry.  This function works in both Win32 and Win16.
//
//	Return Value:	ERROR_SUCCESS if successful
//					Error value otherwise(in winerror.h)
//
LONG RegMan_EnumSZValue(HKEY hKey, DWORD dwIndex, LPTSTR lpstrValueName, LPDWORD pdwcbValueName, LPTSTR lpstrData, LPDWORD pdwcbData);

//
//	RegMan_EnumSZValue
//
//	Purpose:	Use to enumerate the names of the subkeys of a key in the registry.
//				This function works in both Win32 and Win16.
//
//	Return Value:	ERROR_SUCCESS if successful
//					Error value otherwise(in winerror.h)
//
LONG RegMan_EnumKey(HKEY hKey, DWORD dwIndex, LPTSTR lpstrName, LPDWORD pdwcbName);

//
//	RegMan_EnumSZValue
//
//	Purpose:	Use to delete a "value" under a key in the registry.
//				This function works in both Win32 and Win16.
//
//	Return Value:	ERROR_SUCCESS if successful
//					Error value otherwise(in winerror.h)
//
LONG RegMan_DeleteValue(HKEY hKey, LPCTSTR lpstrValueName);

//
//	RegMan_EnumSZValue
//
//	Purpose:	Use to retrieve a DWORD from a registry "value".
//				This function works in both Win32 and Win16.
//
//	Return Value:	ERROR_SUCCESS if successful
//					Error value otherwise(in winerror.h)
//
LONG RegMan_SetDWORDValue(HKEY hKey, LPCTSTR lpstrSubKey, DWORD dwData);

//
//	RegMan_EnumSZValue
//
//	Purpose:	Use to retrieve a DWORD from a registry "value".
//				This function works in both Win32 and Win16.
//
//	Return Value:	ERROR_SUCCESS if successful
//					Error value otherwise(in winerror.h)
//
LONG RegMan_CloseKey(HKEY hKey);

// Copies a registry tree.
LONG RegMan_CopyTree(HKEY hKeySrc, HKEY hKeyDest);

// Deletes the specified registry tree.
LONG RegMan_DeleteTree(HKEY hKey, LPCTSTR szTreeName);

#endif // _REGMAN_H

