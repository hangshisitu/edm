//=============================================================================
// File:       utility.h
// Contents:   Declarations of utility functions for MIME++
// Maintainer: Doug Sauder <dwsauder@hunnysoft.com>
// WWW:        http://www.hunnysoft.com/mimepp/
//
// Copyright (c) 1996-2000 Douglas W. Sauder
// All rights reserved.
// 
// IN NO EVENT SHALL DOUGLAS W. SAUDER BE LIABLE TO ANY PARTY FOR DIRECT,
// INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF
// THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF DOUGLAS W. SAUDER
// HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// DOUGLAS W. SAUDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT
// NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
// BASIS, AND DOUGLAS W. SAUDER HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
// SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
//
//=============================================================================

#ifndef DW_UTILITY_H
#define DW_UTILITY_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif


DwBool DW_CORE_EXPORT DwInitialize();
void DW_CORE_EXPORT DwFinalize();
int  DW_CORE_EXPORT DwCteStrToEnum(const DwString& aStr);
void DW_CORE_EXPORT DwCteEnumToStr(int aEnum, DwString& aStr);
int  DW_CORE_EXPORT DwTypeStrToEnum(const DwString& aStr);
void DW_CORE_EXPORT DwTypeEnumToStr(int aEnum, DwString& aStr);
int  DW_CORE_EXPORT DwSubtypeStrToEnum(const DwString& aStr);
void DW_CORE_EXPORT DwSubtypeEnumToStr(int aEnum, DwString& aStr);
int  DW_CORE_EXPORT DwToCrLfEol(const DwString& aSrcStr, DwString& aDestStr);
int  DW_CORE_EXPORT DwToLfEol(const DwString& aSrcStr, DwString& aDestStr);
int  DW_CORE_EXPORT DwToCrEol(const DwString& aSrcStr, DwString& aDestStr);
int  DW_CORE_EXPORT DwToLocalEol(const DwString& aSrcStr, DwString& aDestStr);
int  DW_CORE_EXPORT DwEncodeBase64(const DwString& aSrcStr, DwString& aDestStr);
int  DW_CORE_EXPORT DwDecodeBase64(const DwString& aSrcStr, DwString& aDestStr);
int  DW_CORE_EXPORT DwEncodeQuotedPrintable(const DwString& aSrcStr, DwString& aDestStr);
int  DW_CORE_EXPORT DwDecodeQuotedPrintable(const DwString& aSrcStr, DwString& aDestStr, bool bHeaderSubjectField=false);
int  DW_CORE_EXPORT DwBEncode(const DwString& aSrcStr, DwString& aDestStr);
int  DW_CORE_EXPORT DwQEncode(const DwString& aSrcStr, DwString& aDestStr);
//int  DW_CORE_EXPORT DwQDecode(const DwString& aSrcStr, DwString& aDestStr);
//Modify by pqf, this function has a bug, will trim the SPACE&TAB
void DW_CORE_EXPORT DwUnfoldLine(DwString& aLine);
void DW_CORE_EXPORT DwFoldLine(DwString& aLine, int aOffset);

#endif
