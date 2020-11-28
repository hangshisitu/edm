//=============================================================================
// File:       dw_mime.cpp
// Contents:   Various functions
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

#define DW_CORE_IMPLEMENTATION

#include <mimepp/config.h>
#include <mimepp/debug.h>

#include <mimepp/string.h>
#include <mimepp/msgcmp.h>
#include <mimepp/enum.h>
#include <mimepp/utility.h>

static size_t calc_crlf_buff_size(const char* srcBuf, size_t srcLen);
static int to_crlf(const char* srcBuf, size_t srcLen, char* destBuf,
    size_t destSize, size_t* destLen);
static int to_lf(const char* srcBuf, size_t srcLen, char* destBuf, 
    size_t destSize, size_t* destLen);
static int to_cr(const char* srcBuf, size_t srcLen, char* destBuf,
    size_t destSize, size_t* destLen);


int DwToCrLfEol(const DwString& aSrcStr, DwString& aDestStr)
{
    // Estimate required destination buffer size
    size_t srcLen = aSrcStr.length();
    const char* srcBuf = aSrcStr.data();
    size_t destSize = calc_crlf_buff_size(srcBuf, srcLen);

    // Allocate destination buffer
    DwString destStr(destSize, (char)0);
    char* destBuf = (char*) destStr.data();

    // Encode source to destination
    size_t destLen;
    to_crlf(srcBuf, srcLen, destBuf, destSize, &destLen);
    aDestStr.assign(destStr, 0, destLen);
    return 0;
}


int DwToLfEol(const DwString& aSrcStr, DwString& aDestStr)
{
    size_t srcLen = aSrcStr.length();
    const char* srcBuf = aSrcStr.data();
    size_t destSize = srcLen;
    
    // Allocate destination buffer
    DwString destStr(destSize, (char)0);
    char* destBuf = (char*) destStr.data();

    // Encode source to destination
    size_t destLen;
    to_lf(srcBuf, srcLen, destBuf, destSize, &destLen);
    aDestStr.assign(destStr, 0, destLen);
    return 0;
}


int DwToCrEol(const DwString& aSrcStr, DwString& aDestStr)
{
    size_t srcLen = aSrcStr.length();
    const char* srcBuf = aSrcStr.data();
    size_t destSize = srcLen;
    
    // Allocate destination buffer
    DwString destStr(destSize, (char)0);
    char* destBuf = (char*) destStr.data();

    // Encode source to destination
    size_t destLen;
    to_cr(srcBuf, srcLen, destBuf, destSize, &destLen);
    aDestStr.assign(destStr, 0, destLen);
    return 0;
}


int DwToLocalEol(const DwString& aSrcStr, DwString& aDestStr)
{
#if defined(DW_EOL_CRLF)
    return DwToCrLfEol(aSrcStr, aDestStr);
#elif defined(DW_EOL_LF)
    return DwToLfEol(aSrcStr, aDestStr);
#else
#   error "Must define DW_EOL_CRLF, DW_EOL_LF"
#endif
}


int DwCteStrToEnum(const DwString& aStr)
{
    int cte = DwMime::kCteUnknown;
    int ch = aStr[0];
    switch (ch) {
    case '7':
        if (DwStrcasecmp(aStr, "7bit") == 0) {
            cte = DwMime::kCte7bit;
        }
        break;
    case '8':
        if (DwStrcasecmp(aStr, "8bit") == 0) {
            cte = DwMime::kCte8bit;
        }
        break;
    case 'B':
    case 'b':
        if (DwStrcasecmp(aStr, "base64") == 0) {
            cte = DwMime::kCteBase64;
        }
        else if (DwStrcasecmp(aStr, "binary") == 0) {
            cte = DwMime::kCteBinary;
        }
        break;
    case 'Q':
    case 'q':
        if (DwStrcasecmp(aStr, "quoted-printable") == 0) {
            cte = DwMime::kCteQuotedPrintable;
        }
        break;
    }
    return cte;
}


void DwCteEnumToStr(int aEnum, DwString& aStr)
{
    switch (aEnum) {
    case DwMime::kCte7bit:
        aStr = "7bit";
        break;
    case DwMime::kCte8bit:
        aStr = "8bit";
        break;
    case DwMime::kCteBinary:
        aStr = "binary";
        break;
    case DwMime::kCteBase64:
        aStr = "base64";
        break;
    case DwMime::kCteQuotedPrintable:
        aStr = "quoted-printable";
        break;
    }
}


int DwTypeStrToEnum(const DwString& aStr)
{
    int type = DwMime::kTypeUnknown;
    int ch = aStr[0];
    switch (ch) {
    case 'A':
    case 'a':
        if (DwStrcasecmp(aStr, "application") == 0) {
            type = DwMime::kTypeApplication;
        }
        else if (DwStrcasecmp(aStr, "audio") == 0) {
            type = DwMime::kTypeAudio;
        }
        break;
    case 'I':
    case 'i':
        if (DwStrcasecmp(aStr, "image") == 0) {
            type = DwMime::kTypeImage;
        }
        break;
    case 'M':
    case 'm':
        if (DwStrcasecmp(aStr, "message") == 0) {
            type = DwMime::kTypeMessage;
        }
        else if (DwStrcasecmp(aStr, "model") == 0) {
            type = DwMime::kTypeModel;
        }
        else if (DwStrcasecmp(aStr, "multipart") == 0) {
            type = DwMime::kTypeMultipart;
        }
        break;
    case 'T':
    case 't':
        if (DwStrcasecmp(aStr, "text") == 0) {
            type = DwMime::kTypeText;
        }
        break;
    case 'V':
    case 'v':
        if (DwStrcasecmp(aStr, "video") == 0) {
            type = DwMime::kTypeVideo;
        }
        break;
    case 0:
        type = DwMime::kTypeNull;
        break;
    }
    return type;
}


void DwTypeEnumToStr(int aEnum, DwString& aStr)
{
    switch (aEnum) {
    case DwMime::kTypeNull:
        aStr = "";
        break;
    case DwMime::kTypeUnknown:
    default:
        aStr = "Unknown";
        break;
    case DwMime::kTypeText:
        aStr = "Text";
        break;
    case DwMime::kTypeMultipart:
        aStr = "Multipart";
        break;
    case DwMime::kTypeMessage:
        aStr = "Message";
        break;
    case DwMime::kTypeApplication:
        aStr = "Application";
        break;
    case DwMime::kTypeImage:
        aStr = "Image";
        break;
    case DwMime::kTypeAudio:
        aStr = "Audio";
        break;
    case DwMime::kTypeVideo:
        aStr = "Video";
        break;
    case DwMime::kTypeModel:
        aStr = "Model";
        break;
    }
}


int DwSubtypeStrToEnum(const DwString& aStr)
{
    if (aStr == "") {
        return DwMime::kSubtypeNull;
    }
    int type = DwMime::kSubtypeUnknown;
    int ch = aStr[0];
    switch (ch) {
    case 'A':
    case 'a':
        if (DwStrcasecmp(aStr, "alternative") == 0) {
            type = DwMime::kSubtypeAlternative;
        }
        if (DwStrcasecmp(aStr, "appledouble") == 0) {
            type = DwMime::kSubtypeAppledouble;
        }
        if (DwStrcasecmp(aStr, "applefile") == 0) {
            type = DwMime::kSubtypeApplefile;
        }
        break;
    case 'B':
    case 'b':
        if (DwStrcasecmp(aStr, "basic") == 0) {
            type = DwMime::kSubtypeBasic;
        }
        break;
    case 'D':
    case 'd':
        if (DwStrcasecmp(aStr, "delivery-status") == 0) {
            type = DwMime::kSubtypeDeliveryStatus;
        }
        else if (DwStrcasecmp(aStr, "digest") == 0) {
            type = DwMime::kSubtypeDigest;
        }
        break;
    case 'E':
    case 'e':
        if (DwStrcasecmp(aStr, "encrypted") == 0) {
            type = DwMime::kSubtypeEncrypted;
        }
        else if (DwStrcasecmp(aStr, "enriched") == 0) {
            type = DwMime::kSubtypeEnriched;
        }
        else if (DwStrcasecmp(aStr, "external-body") == 0) {
            type = DwMime::kSubtypeExternalBody;
        }
        break;
    case 'G':
    case 'g':
        if (DwStrcasecmp(aStr, "gif") == 0) {
            type = DwMime::kSubtypeGif;
        }
        break;
    case 'H':
    case 'h':
        if (DwStrcasecmp(aStr, "html") == 0) {
            type = DwMime::kSubtypeHtml;
        }
        break;
    case 'J':
    case 'j':
        if (DwStrcasecmp(aStr, "jpeg") == 0) {
            type = DwMime::kSubtypeJpeg;
        }
        break;
    case 'M':
    case 'm':
        if (DwStrcasecmp(aStr, "mixed") == 0) {
            type = DwMime::kSubtypeMixed;
        }
        else if (DwStrcasecmp(aStr, "mpeg") == 0) {
            type = DwMime::kSubtypeMpeg;
        }
        break;
    case 'O':
    case 'o':
        if (DwStrcasecmp(aStr, "octet-stream") == 0) {
            type = DwMime::kSubtypeOctetStream;
        }
        break;
    case 'P':
    case 'p':
        if (DwStrcasecmp(aStr, "plain") == 0) {
            type = DwMime::kSubtypePlain;
        }
        else if (DwStrcasecmp(aStr, "parallel") == 0) {
            type = DwMime::kSubtypeParallel;
        }
        else if (DwStrcasecmp(aStr, "partial") == 0) {
            type = DwMime::kSubtypePartial;
        }
        else if (DwStrcasecmp(aStr, "postscript") == 0) {
            type = DwMime::kSubtypePostscript;
        }
        break;
    case 'R':
    case 'r':
        if (DwStrcasecmp(aStr, "report") == 0) {
            type = DwMime::kSubtypeReport;
        }
        else if (DwStrcasecmp(aStr, "related") == 0) {
            type = DwMime::kSubtypeRelated;
        }
        else if (DwStrcasecmp(aStr, "richtext") == 0) {
            type = DwMime::kSubtypeRichtext;
        }
        else if (DwStrcasecmp(aStr, "rfc822") == 0) {
            type = DwMime::kSubtypeRfc822;
        }
        else if (DwStrcasecmp(aStr, "rfc822-headers") == 0) {
            type = DwMime::kSubtypeRfc822Headers;
        }
        break;
    case 'S':
    case 's':
        if (DwStrcasecmp(aStr, "signed") == 0) {
            type = DwMime::kSubtypeSigned;
        }
    }
    return type;
}


void DwSubtypeEnumToStr(int aEnum, DwString& aStr)
{
    switch (aEnum) {
    case DwMime::kSubtypeNull:
        aStr = "";
        break;
    case DwMime::kSubtypeUnknown:
    default:
        aStr = "Unknown";
        break;
    case DwMime::kSubtypePlain:
        aStr = "Plain";
        break;
    case DwMime::kSubtypeRichtext:
        aStr = "Richtext";
        break;
    case DwMime::kSubtypeEnriched:
        aStr = "Enriched";
        break;
    case DwMime::kSubtypeHtml:
        aStr = "HTML";
        break;
    case DwMime::kSubtypeMixed:
        aStr = "Mixed";
        break;
    case DwMime::kSubtypeAlternative:
        aStr = "Alternative";
        break;
    case DwMime::kSubtypeDigest:
        aStr = "Digest";
        break;
    case DwMime::kSubtypeParallel:
        aStr = "Parallel";
        break;
    case DwMime::kSubtypeRfc822:
        aStr = "Rfc822";
        break;
    case DwMime::kSubtypePartial:
        aStr = "Partial";
        break;
    case DwMime::kSubtypeExternalBody:
        aStr = "External-body";
        break;
    case DwMime::kSubtypePostscript:
        aStr = "Postscript";
        break;
    case DwMime::kSubtypeOctetStream:
        aStr = "Octet-stream";
        break;
    case DwMime::kSubtypeJpeg:
        aStr = "jpeg";
        break;
    case DwMime::kSubtypeGif:
        aStr = "gif";
        break;
    case DwMime::kSubtypeBasic:
        aStr = "basic";
        break;
    case DwMime::kSubtypeMpeg:
        aStr = "mpeg";
        break;
    case DwMime::kSubtypeSigned:
        aStr = "signed";
        break;
    case DwMime::kSubtypeEncrypted:
        aStr = "encrypted";
        break;
    case DwMime::kSubtypeRfc822Headers:
        aStr = "rfc822-headers";
        break;
    case DwMime::kSubtypeReport:
        aStr = "report";
        break;
    case DwMime::kSubtypeDeliveryStatus:
        aStr = "delivery-status";
        break;
    case DwMime::kSubtypeRelated:
        aStr = "related";
        break;
    case DwMime::kSubtypeAppledouble:
        aStr = "appledouble";
        break;
    case DwMime::kSubtypeApplefile:
        aStr = "applefile";
        break;
    }
}


void DwUnfoldLine(DwString& aLine)
{
    const char kTAB = 9;
    const char kLF = 10;
    const char kCR = 13;
    const DwString& line = aLine;
    size_t lineLen = aLine.length();
    DwString unfoldedLine;
    unfoldedLine.reserve(lineLen);
    char lastCh = 0;
    for (int i=0; i < (int) lineLen; ++i) {
        char ch = line[i];
        if (ch == kCR || ch == kLF) {
        }
        else if (ch == kTAB && lastCh == kLF) {
            unfoldedLine += ' ';
        }
        else {
            unfoldedLine += ch;
        }
        lastCh = ch;
    }
    aLine = unfoldedLine;
}


void DwFoldLine(DwString& aLine, int aOffset)
{
    const DwString& line = aLine;
    int offset = aOffset;
    DwString foldedLines;
    DwString curLine;
    DwString word;
    foldedLines.reserve(line.length()+10);
    curLine.reserve(80);
    word.reserve(80);
    size_t wordStart = 0;
    size_t wordEnd = 0;
    size_t curLineLen = 0;
    while (DwTrue) {
        wordStart = line.find_first_not_of(" \t\r\n", wordEnd);
        if (wordStart == DwString::npos) {
            break;
        }
        wordEnd = line.find_first_of(" \t\r\n", wordStart);
        if (wordEnd == DwString::npos) {
            wordEnd = line.length();
        }
        word = line.substr(wordStart, wordEnd - wordStart);
        curLineLen = curLine.length();
        if (curLineLen == 0) {
            curLine += word;
        }
        else if (curLineLen + 1 + word.length() + offset < 76) {
            curLine += " ";
            curLine += word;
        }
        else {
            if (foldedLines.length() > 0) {
                foldedLines += DW_EOL " ";
            }
            foldedLines += curLine;
            curLine = word;
            offset = 0;
        }
    }
    if (curLine.length() > 0) {
        if (foldedLines.length() > 0) {
            foldedLines += DW_EOL " ";
        }
        foldedLines += curLine;
    }
    aLine = foldedLines;
}


//============================================================================
// Everything below this line is private to this file (static)
//============================================================================


static size_t calc_crlf_buff_size(const char* srcBuf, size_t srcLen)
{
    size_t i, extra;

    if (!srcBuf) return 0;
    extra = 0;
    for (i=0; i < srcLen; ) {
        switch (srcBuf[i]) {
        /* Bare LF (UNIX or C text) */
        case '\n':
            ++extra;
            ++i;
            break;
        case '\r':
            /* CR LF (DOS, Windows, or MIME text) */
            if (i+1 < srcLen && srcBuf[i+1] == '\n') {
                i += 2;
            }
            /* Bare CR (Macintosh text) */
            else {
                ++extra;
                ++i;
            }
            break;
        default:
            ++i;
        }
    }
    return srcLen + extra;
}


static int to_crlf(const char* srcBuf, size_t srcLen, char* destBuf,
    size_t destSize, size_t* destLen)
{
    size_t iSrc, iDest;

    if (!srcBuf || !destBuf || !destLen) return -1;
    iSrc = iDest = 0;
    while (iSrc < srcLen && iDest < destSize) {
        switch (srcBuf[iSrc]) {
        /* Bare LF (UNIX or C text) */
        case '\n':
            destBuf[iDest++] = '\r';
            if (iDest < destSize) {
                destBuf[iDest++] = srcBuf[iSrc++];
            }
            break;
        case '\r':
            /* CR LF (DOS, Windows, or MIME text) */
            if (iSrc+1 < srcLen && srcBuf[iSrc+1] == '\n') {
                destBuf[iDest++] = srcBuf[iSrc++];
                if (iDest < destSize) {
                    destBuf[iDest++] = srcBuf[iSrc++];
                }
            }
            /* Bare CR (Macintosh text) */
            else {
                destBuf[iDest++] = srcBuf[iSrc++];
                if (iDest < destSize) {
                    destBuf[iDest++] = '\n';
                }
            }
            break;
        default:
            destBuf[iDest++] = srcBuf[iSrc++];
        }
    }
    *destLen = iDest;
    if (iDest < destSize) {
        destBuf[iDest] = 0;
    }
    return 0;
}


static int to_lf(const char* srcBuf, size_t srcLen, char* destBuf,
    size_t destSize, size_t* destLen)
{
    size_t iSrc, iDest;

    if (!srcBuf || !destBuf || !destLen) return -1;
    iSrc = iDest = 0;
    while (iSrc < srcLen && iDest < destSize) {
        switch (srcBuf[iSrc]) {
        /* Bare LF (UNIX or C text) */
        case '\n':
            destBuf[iDest++] = srcBuf[iSrc++];
            break;
        case '\r':
            /* CR LF (DOS, Windows, or MIME text) */
            if (iSrc+1 < srcLen && srcBuf[iSrc+1] == '\n') {
                ++iSrc;
                destBuf[iDest++] = srcBuf[iSrc++];
            }
            /* Bare CR (Macintosh text) */
            else {
                destBuf[iDest++] = '\n';
                ++iSrc;
            }
            break;
        default:
            destBuf[iDest++] = srcBuf[iSrc++];
        }
    }
    *destLen = iDest;
    if (iDest < destSize) {
        destBuf[iDest] = 0;
    }
    return 0;
}


static int to_cr(const char* srcBuf, size_t srcLen, char* destBuf,
    size_t destSize, size_t* destLen)
{
    size_t iSrc, iDest;

    if (!srcBuf || !destBuf || !destLen) return -1;
    iSrc = iDest = 0;
    while (iSrc < srcLen && iDest < destSize) {
        switch (srcBuf[iSrc]) {
        /* Bare LF (UNIX or C text) */
        case '\n':
            destBuf[iDest++] = '\r';
            ++iSrc;
            break;
        case '\r':
            /* CR LF (DOS, Windows, or MIME text) */
            if (iSrc+1 < srcLen && srcBuf[iSrc+1] == '\n') {
                destBuf[iDest++] = srcBuf[iSrc++];
                ++iSrc;
            }
            /* Bare CR (Macintosh text) */
            else {
                destBuf[iDest++] = srcBuf[iSrc++];
            }
            break;
        default:
            destBuf[iDest++] = srcBuf[iSrc++];
        }
    }
    *destLen = iDest;
    if (iDest < destSize) {
        destBuf[iDest] = 0;
    }
    return 0;
}
