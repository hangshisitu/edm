//=============================================================================
// File:       applfile.cpp
// Contents:   Definitions for DwAppleFile
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

#define DW_UTIL_IMPLEMENTATION

#include <mimepp/config.h>
#include <mimepp/debug.h>
#include <mimepp/applfile.h>


DwAppleFile::DwAppleFile()
{
    mNumEntries = 0;
    mHasRealName      = DwFalse;
    mHasFileDatesInfo = DwFalse;
    mHasResourceFork  = DwFalse;
    mHasDataFork      = DwFalse;
    mFormatType = kAppleSingle;
    mCreateTime = (DwInt32) 0x80000000;
    mModifyTime = (DwInt32) 0x80000000;
    mBackupTime = (DwInt32) 0x80000000;
    mAccessTime = (DwInt32) 0x80000000;
}


int DwAppleFile::Format() const
{
    return mFormatType;
}


void DwAppleFile::SetFormat(int aFormatType)
{
    if (aFormatType == kAppleSingle || aFormatType == kAppleDoubleHeader) {
        mFormatType = aFormatType;
    }
}


DwBool DwAppleFile::HasFileName() const
{
    return mHasRealName;
}


const char* DwAppleFile::FileName() const
{
    return mRealName.c_str();
}


void DwAppleFile::SetFileName(const char* aName)
{
    mHasRealName = DwTrue;
    mRealName = aName;
}


DwBool DwAppleFile::HasFileTimes() const
{
    return mHasFileDatesInfo;
}


DwInt32 DwAppleFile::CreateTime() const
{
    return mCreateTime;
}


void DwAppleFile::SetCreateTime(DwInt32 aTime)
{
    mHasFileDatesInfo = DwTrue;
    mCreateTime = aTime;
}


DwInt32 DwAppleFile::ModifyTime() const
{
    return mModifyTime;
}


void DwAppleFile::SetModifyTime(DwInt32 aTime)
{
    mHasFileDatesInfo = DwTrue;
    mModifyTime = aTime;
}


DwInt32 DwAppleFile::BackupTime() const
{
    return mBackupTime;
}


void DwAppleFile::SetBackupTime(DwInt32 aTime)
{
    mHasFileDatesInfo = DwTrue;
    mBackupTime = aTime;
}


DwInt32 DwAppleFile::AccessTime() const
{
    return mAccessTime;
}


void DwAppleFile::SetAccessTime(DwInt32 aTime)
{
    mHasFileDatesInfo = DwTrue;
    mAccessTime = aTime;
}


DwBool DwAppleFile::HasDataFork() const
{
    return mHasDataFork;
}


const DwString& DwAppleFile::DataFork() const
{
    return mDataFork;
}


void DwAppleFile::SetDataFork(const DwString& aStr)
{
    mHasDataFork = DwTrue;
    mDataFork = aStr;
}


DwBool DwAppleFile::HasResourceFork() const
{
    return mHasResourceFork;
}


const DwString& DwAppleFile::ResourceFork() const
{
    return mResourceFork;
}


void DwAppleFile::SetResourceFork(const DwString& aStr)
{
    mHasResourceFork = DwTrue;
    mResourceFork = aStr;
}


const DwString& DwAppleFile::PackagedData() const
{
    return mAppleFileChars;
}


void DwAppleFile::SetPackagedData(const DwString& aStr)
{
    mHasFileDatesInfo = DwFalse;
    mHasRealName      = DwFalse;
    mHasResourceFork  = DwFalse;
    mHasDataFork      = DwFalse;
    mAppleFileChars = aStr;
}


void DwAppleFile::Pack()
{
    mAppleFileChars = "";

    int length =
          26                       // header
        + 4*12                     // entry descriptors
        + 16                       // File Dates Info entry
        + mRealName.length()       // Real Name entry
        + mResourceFork.length();  // Resource Fork entry
    if (mFormatType == kAppleSingle) {
        length += mDataFork.length(); // Data Fork entry
    }
    DwString buffer;
    buffer.reserve(length);

    // Magic number

    if (mFormatType == kAppleSingle) {
        buffer += (char) 0x00;
        buffer += (char) 0x05;
        buffer += (char) 0x16;
        buffer += (char) 0x00;
    }
    else /* if (mFormatType == kAppleDoubleHeader) */ {
        buffer += (char) 0x00;
        buffer += (char) 0x05;
        buffer += (char) 0x16;
        buffer += (char) 0x07;
	}

    // Version number

    buffer += (char) 0x00;
    buffer += (char) 0x02;
    buffer += (char) 0x00;
    buffer += (char) 0x00;

    // Filler (all zeros)

    for (int i=0; i < 16; ++i) {
        buffer += (char) 0;
    }

    // Number of entries

    mNumEntries = 0;
    if (mHasRealName) {
        ++mNumEntries;
    }
    if (mHasFileDatesInfo) {
        ++mNumEntries;
    }
    if (mHasResourceFork) {
        ++mNumEntries;
    }
    if (mHasDataFork && mFormatType == kAppleSingle) {
        ++mNumEntries;
    }
    buffer += (char) ((mNumEntries >> 8) & 0xff);
    buffer += (char) ((mNumEntries     ) & 0xff);

    // Real Name entry descriptor

    size_t realNameOffsetPos = 0;
    if (mHasRealName) {
        DwInt32 entryId = 3;
        DwInt32 entryOffset = 0;
        DwInt32 entryLength = mRealName.length();
        buffer += (char) ((entryId >> 24) & 0xff);
        buffer += (char) ((entryId >> 16) & 0xff);
        buffer += (char) ((entryId >>  8) & 0xff);
        buffer += (char) ((entryId >>  0) & 0xff);
        realNameOffsetPos = buffer.length();
        buffer += (char) ((entryOffset >> 24) & 0xff);
        buffer += (char) ((entryOffset >> 16) & 0xff);
        buffer += (char) ((entryOffset >>  8) & 0xff);
        buffer += (char) ((entryOffset >>  0) & 0xff);
        buffer += (char) ((entryLength >> 24) & 0xff);
        buffer += (char) ((entryLength >> 16) & 0xff);
        buffer += (char) ((entryLength >>  8) & 0xff);
        buffer += (char) ((entryLength >>  0) & 0xff);
    }

    // File Dates Info entry descriptor

    size_t fileDatesInfoOffsetPos = 0;
    if (mHasFileDatesInfo) {
        DwInt32 entryId = 8;
        DwInt32 entryOffset = 0;
        DwInt32 entryLength = 16;
        buffer += (char) ((entryId >> 24) & 0xff);
        buffer += (char) ((entryId >> 16) & 0xff);
        buffer += (char) ((entryId >>  8) & 0xff);
        buffer += (char) ((entryId >>  0) & 0xff);
        fileDatesInfoOffsetPos = buffer.length();
        buffer += (char) ((entryOffset >> 24) & 0xff);
        buffer += (char) ((entryOffset >> 16) & 0xff);
        buffer += (char) ((entryOffset >>  8) & 0xff);
        buffer += (char) ((entryOffset >>  0) & 0xff);
        buffer += (char) ((entryLength >> 24) & 0xff);
        buffer += (char) ((entryLength >> 16) & 0xff);
        buffer += (char) ((entryLength >>  8) & 0xff);
        buffer += (char) ((entryLength >>  0) & 0xff);
    }

    // Resource Fork entry descriptor

    size_t resourceForkOffsetPos = 0;
    if (mHasResourceFork) {
        DwUint32 entryId = 2;
        DwUint32 entryOffset = 0;
        DwUint32 entryLength = mResourceFork.length();
        buffer += (char) ((entryId >> 24) & 0xff);
        buffer += (char) ((entryId >> 16) & 0xff);
        buffer += (char) ((entryId >>  8) & 0xff);
        buffer += (char) ((entryId >>  0) & 0xff);
        resourceForkOffsetPos = buffer.length();
        buffer += (char) ((entryOffset >> 24) & 0xff);
        buffer += (char) ((entryOffset >> 16) & 0xff);
        buffer += (char) ((entryOffset >>  8) & 0xff);
        buffer += (char) ((entryOffset >>  0) & 0xff);
        buffer += (char) ((entryLength >> 24) & 0xff);
        buffer += (char) ((entryLength >> 16) & 0xff);
        buffer += (char) ((entryLength >>  8) & 0xff);
        buffer += (char) ((entryLength >>  0) & 0xff);
    }

    // Data Fork entry descriptor

    size_t dataForkOffsetPos = 0;
    if (mHasDataFork && mFormatType == kAppleSingle) {
        DwUint32 entryId = 1;
        DwUint32 entryOffset = 0;
        DwUint32 entryLength = mDataFork.length();
        buffer += (char) ((entryId >> 24) & 0xff);
        buffer += (char) ((entryId >> 16) & 0xff);
        buffer += (char) ((entryId >>  8) & 0xff);
        buffer += (char) ((entryId >>  0) & 0xff);
        dataForkOffsetPos = buffer.length();
        buffer += (char) ((entryOffset >> 24) & 0xff);
        buffer += (char) ((entryOffset >> 16) & 0xff);
        buffer += (char) ((entryOffset >>  8) & 0xff);
        buffer += (char) ((entryOffset >>  0) & 0xff);
        buffer += (char) ((entryLength >> 24) & 0xff);
        buffer += (char) ((entryLength >> 16) & 0xff);
        buffer += (char) ((entryLength >>  8) & 0xff);
        buffer += (char) ((entryLength >>  0) & 0xff);
    }

    // Real Name entry

    if (mHasRealName) {
        DwUint32 entryOffset = buffer.length();
        size_t pos = realNameOffsetPos;
        buffer[pos++] = (char) ((entryOffset >> 24) & 0xff);
        buffer[pos++] = (char) ((entryOffset >> 16) & 0xff);
        buffer[pos++] = (char) ((entryOffset >>  8) & 0xff);
        buffer[pos++] = (char) ((entryOffset >>  0) & 0xff);
        buffer += mRealName;
    }

    // File Dates Info entry

    if (mHasFileDatesInfo) {
        DwUint32 entryOffset = buffer.length();
        size_t pos = fileDatesInfoOffsetPos;
        buffer[pos++] = (char) ((entryOffset >> 24) & 0xff);
        buffer[pos++] = (char) ((entryOffset >> 16) & 0xff);
        buffer[pos++] = (char) ((entryOffset >>  8) & 0xff);
        buffer[pos++] = (char) ((entryOffset >>  0) & 0xff);
        buffer += (char) ((mCreateTime >> 24) & 0xff);
        buffer += (char) ((mCreateTime >> 16) & 0xff);
        buffer += (char) ((mCreateTime >>  8) & 0xff);
        buffer += (char) ((mCreateTime >>  0) & 0xff);
        buffer += (char) ((mModifyTime >> 24) & 0xff);
        buffer += (char) ((mModifyTime >> 16) & 0xff);
        buffer += (char) ((mModifyTime >>  8) & 0xff);
        buffer += (char) ((mModifyTime >>  0) & 0xff);
        buffer += (char) ((mBackupTime >> 24) & 0xff);
        buffer += (char) ((mBackupTime >> 16) & 0xff);
        buffer += (char) ((mBackupTime >>  8) & 0xff);
        buffer += (char) ((mBackupTime >>  0) & 0xff);
        buffer += (char) ((mAccessTime >> 24) & 0xff);
        buffer += (char) ((mAccessTime >> 16) & 0xff);
        buffer += (char) ((mAccessTime >>  8) & 0xff);
        buffer += (char) ((mAccessTime >>  0) & 0xff);
    }

    // Resource Fork entry

    if (mHasResourceFork) {
        DwUint32 entryOffset = buffer.length();
        size_t pos = resourceForkOffsetPos;
        buffer[pos++] = (char) ((entryOffset >> 24) & 0xff);
        buffer[pos++] = (char) ((entryOffset >> 16) & 0xff);
        buffer[pos++] = (char) ((entryOffset >>  8) & 0xff);
        buffer[pos++] = (char) ((entryOffset >>  0) & 0xff);
        buffer += mResourceFork;
    }

    // Data Fork entry

    if (mHasDataFork && mFormatType == kAppleSingle) {
        DwUint32 entryOffset = buffer.length();
        size_t pos = dataForkOffsetPos;
        buffer[pos++] = (char) ((entryOffset >> 24) & 0xff);
        buffer[pos++] = (char) ((entryOffset >> 16) & 0xff);
        buffer[pos++] = (char) ((entryOffset >>  8) & 0xff);
        buffer[pos++] = (char) ((entryOffset >>  0) & 0xff);
        buffer += mDataFork;
    }

    mAppleFileChars = buffer;
}


int DwAppleFile::Unpack()
{
    int errorCode = IsValid();
    if (errorCode != kErrNoError) {
        return errorCode;
    }
    errorCode = GetFileDatesInfo();
    if (errorCode != kErrNoError) {
        return errorCode;
    }
    errorCode = GetRealName();
    if (errorCode != kErrNoError) {
        return errorCode;
    }
    errorCode = GetResourceFork();
    if (errorCode != kErrNoError) {
        return errorCode;
    }
    errorCode = GetDataFork();
    if (errorCode != kErrNoError) {
        return errorCode;
    }
    return errorCode;
}


DwInt32 DwAppleFile::UnixTimeToAppleFileTime(DwUint32 aUnixTime)
{
    return aUnixTime - (DwInt32) 946684800L;
}


DwUint32 DwAppleFile::AppleFileTimeToUnixTime(DwInt32 aASTime)
{
    // If the Unix time would be negative, then set it to zero.

    // Note: AppleSingle/AppleDouble uses 0x80000000 to denote an invalid date.
    // If we see an AppleFile time that would cause the Unix time to be less
    // than zero, this probably is the case of 0x80000000.  We are not likely
    // to see real file times before 1970-01-01.

    if (aASTime <  (DwInt32) -946684800L) {
        return 0;
    }
    else {
        return aASTime + (DwInt32) 946684800L;
    }
}


int DwAppleFile::IsValid()
{
    int errorCode = kErrNoError;

    const DwString& buffer = mAppleFileChars;
    mHasFileDatesInfo = DwFalse;
    mHasRealName      = DwFalse;
    mHasDataFork      = DwFalse;
    mHasResourceFork  = DwFalse;

    // Check that the buffer is long enough for at least the header

    if (buffer.length() < 26) {
        errorCode = kErrTooShort;
        return errorCode;
    }

    // Check that the magic number is valid

    DwUint32 magic;
    magic  = buffer[0] & 0xff;
    magic <<= 8;
    magic |= buffer[1] & 0xff;
    magic <<= 8;
    magic |= buffer[2] & 0xff;
    magic <<= 8;
    magic |= buffer[3] & 0xff;
    if (magic == (DwUint32) 0x00051600) {
        mFormatType = kAppleSingle;
    }
    else if (magic == (DwUint32) 0x00051607) {
        mFormatType = kAppleDoubleHeader;
    }
    else {
        errorCode = kErrBadMagicNumber;
        return errorCode;
    }

    // Check that the version number is valid

    DwUint32 version;
    version  = buffer[4] & 0xff;
    version <<= 8;
    version |= buffer[5] & 0xff;
    version <<= 8;
    version |= buffer[6] & 0xff;
    version <<= 8;
    version |= buffer[7] & 0xff;
    if (version != (DwUint32) 0x00020000) {
        errorCode = kErrWrongVersion;
        return errorCode;
    }

    // Get the number of entries and check that the buffer is long enough
    // for the number of entry descriptors

    mNumEntries = ((buffer[24] << 8) & 0xff00) | (buffer[25] & 0x00ff);
    if (buffer.length() < 26 + 12u*mNumEntries) {
        errorCode = kErrCorruptData;
        return errorCode;
    }

    // Get each entry and make sure the offset and length are such that the
    // file can contain it

    DwUint32 entryId;
    DwUint32 entryOffset;
    DwUint32 entryLength;
    DwUint32 entrySectionEnd = 26 + mNumEntries*12;
    for (int i=0; i < mNumEntries; ++i) {
        int pos = 26 + i*12;
        // entry ID
        entryId  = buffer[pos++] & 0xff;
        entryId <<= 8;
        entryId |= buffer[pos++] & 0xff;
        entryId <<= 8;
        entryId |= buffer[pos++] & 0xff;
        entryId <<= 8;
        entryId |= buffer[pos++] & 0xff;
        // entry offset
        entryOffset  = buffer[pos++] & 0xff;
        entryOffset <<= 8;
        entryOffset |= buffer[pos++] & 0xff;
        entryOffset <<= 8;
        entryOffset |= buffer[pos++] & 0xff;
        entryOffset <<= 8;
        entryOffset |= buffer[pos++] & 0xff;
        // entry length
        entryLength  = buffer[pos++] & 0xff;
        entryLength <<= 8;
        entryLength |= buffer[pos++] & 0xff;
        entryLength <<= 8;
        entryLength |= buffer[pos++] & 0xff;
        entryLength <<= 8;
        entryLength |= buffer[pos++] & 0xff;
        // Check that the offset and length are valid
        if (entryOffset < entrySectionEnd
            || buffer.length() < entryOffset + entryLength) {
            errorCode = kErrCorruptData;
            return errorCode;
        }
        switch (entryId) {
        case kDataFork:
            mHasDataFork = DwTrue;
            break;
        case kResourceFork:
            mHasResourceFork = DwTrue;
            break;
        case kRealName:
            mHasRealName = DwTrue;
            break;
        case kFileDatesInfo:
            mHasFileDatesInfo = DwTrue;
            break;
        }
    }
    return errorCode;
}


int DwAppleFile::GetFileDatesInfo()
{
    // Note: To get a Unix time from an Applesingle time, add 946684800
    // (0x386d4380)

    int errorCode = kErrNoError;
    const DwString& buffer = mAppleFileChars;

    // Set all dates to invalid date

    mCreateTime = (DwInt32) 0x80000000;
    mModifyTime = (DwInt32) 0x80000000;
    mBackupTime = (DwInt32) 0x80000000;
    mAccessTime = (DwInt32) 0x80000000;
    if (!mHasFileDatesInfo) {
        return errorCode;
    }

    // Scan for the File Dates Info entry

    DwUint32 entryId;
    DwUint32 entryOffset;
    DwUint32 entryLength;
    DwBool isFound = DwFalse;
    for (int i=0; i < mNumEntries; ++i) {
        int pos = 26 + i*12;
        // entry ID
        entryId  = buffer[pos++] & 0xff;
        entryId <<= 8;
        entryId |= buffer[pos++] & 0xff;
        entryId <<= 8;
        entryId |= buffer[pos++] & 0xff;
        entryId <<= 8;
        entryId |= buffer[pos++] & 0xff;
        // entry offset
        entryOffset  = buffer[pos++] & 0xff;
        entryOffset <<= 8;
        entryOffset |= buffer[pos++] & 0xff;
        entryOffset <<= 8;
        entryOffset |= buffer[pos++] & 0xff;
        entryOffset <<= 8;
        entryOffset |= buffer[pos++] & 0xff;
        // entry length
        entryLength  = buffer[pos++] & 0xff;
        entryLength <<= 8;
        entryLength |= buffer[pos++] & 0xff;
        entryLength <<= 8;
        entryLength |= buffer[pos++] & 0xff;
        entryLength <<= 8;
        entryLength |= buffer[pos++] & 0xff;
        if (entryId == kFileDatesInfo) {
            isFound = DwTrue;
            break;
        }
    }
    if (!isFound) {
        return errorCode;
    }
    if (entryOffset+entryLength > buffer.length()) {
        mErrorCode = kErrCorruptData;
        return errorCode;
    }

    // Create time

    mCreateTime  = buffer[entryOffset+0] & 0xff;
    mCreateTime <<= 8;
    mCreateTime |= buffer[entryOffset+1] & 0xff;
    mCreateTime <<= 8;
    mCreateTime |= buffer[entryOffset+2] & 0xff;
    mCreateTime <<= 8;
    mCreateTime |= buffer[entryOffset+3] & 0xff;

    // Modify time

    mModifyTime  = buffer[entryOffset+4] & 0xff;
    mModifyTime <<= 8;
    mModifyTime |= buffer[entryOffset+5] & 0xff;
    mModifyTime <<= 8;
    mModifyTime |= buffer[entryOffset+6] & 0xff;
    mModifyTime <<= 8;
    mModifyTime |= buffer[entryOffset+7] & 0xff;

    // Backup time

    mBackupTime  = buffer[entryOffset+ 8] & 0xff;
    mBackupTime <<= 8;
    mBackupTime |= buffer[entryOffset+ 9] & 0xff;
    mBackupTime <<= 8;
    mBackupTime |= buffer[entryOffset+10] & 0xff;
    mBackupTime <<= 8;
    mBackupTime |= buffer[entryOffset+11] & 0xff;

    // Access time

    mAccessTime  = buffer[entryOffset+12] & 0xff;
    mAccessTime <<= 8;
    mAccessTime |= buffer[entryOffset+13] & 0xff;
    mAccessTime <<= 8;
    mAccessTime |= buffer[entryOffset+14] & 0xff;
    mAccessTime <<= 8;
    mAccessTime |= buffer[entryOffset+15] & 0xff;

    return errorCode;
}


int DwAppleFile::GetRealName()
{
    int errorCode = kErrNoError;
    const DwString& buffer = mAppleFileChars;

    // Set to empty value

    mRealName = "";
    if (!mHasRealName) {
        return errorCode;
    }

    // Scan for the Real Name entry

    DwUint32 entryId;
    DwUint32 entryOffset;
    DwUint32 entryLength;
    DwBool isFound = DwFalse;
    for (int i=0; i < mNumEntries; ++i) {
        int pos = 26 + i*12;
        // entry ID
        entryId  = buffer[pos++] & 0xff;
        entryId <<= 8;
        entryId |= buffer[pos++] & 0xff;
        entryId <<= 8;
        entryId |= buffer[pos++] & 0xff;
        entryId <<= 8;
        entryId |= buffer[pos++] & 0xff;
        // entry offset
        entryOffset  = buffer[pos++] & 0xff;
        entryOffset <<= 8;
        entryOffset |= buffer[pos++] & 0xff;
        entryOffset <<= 8;
        entryOffset |= buffer[pos++] & 0xff;
        entryOffset <<= 8;
        entryOffset |= buffer[pos++] & 0xff;
        // entry length
        entryLength  = buffer[pos++] & 0xff;
        entryLength <<= 8;
        entryLength |= buffer[pos++] & 0xff;
        entryLength <<= 8;
        entryLength |= buffer[pos++] & 0xff;
        entryLength <<= 8;
        entryLength |= buffer[pos++] & 0xff;
        if (entryId == kRealName) {
            isFound = DwTrue;
            break;
        }
    }

    if (!isFound) {
        return errorCode;
    }
    if (entryOffset+entryLength > buffer.length()) {
        errorCode = kErrCorruptData;
        return errorCode;
    }

    // Set real name

    mRealName = buffer.substr(entryOffset, entryLength);

    return errorCode;
}


int DwAppleFile::GetResourceFork()
{
    int errorCode = kErrNoError;
    const DwString& buffer = mAppleFileChars;

    // Set to empty value

    mResourceFork = "";
    if (!mHasResourceFork) {
        return errorCode;
    }

    // Scan for the Resource Fork entry

    DwUint32 entryId;
    DwUint32 entryOffset;
    DwUint32 entryLength;
    DwBool isFound = DwFalse;
    for (int i=0; i < mNumEntries; ++i) {
        int pos = 26 + i*12;
        // entry ID
        entryId  = buffer[pos++] & 0xff;
        entryId <<= 8;
        entryId |= buffer[pos++] & 0xff;
        entryId <<= 8;
        entryId |= buffer[pos++] & 0xff;
        entryId <<= 8;
        entryId |= buffer[pos++] & 0xff;
        // entry offset
        entryOffset  = buffer[pos++] & 0xff;
        entryOffset <<= 8;
        entryOffset |= buffer[pos++] & 0xff;
        entryOffset <<= 8;
        entryOffset |= buffer[pos++] & 0xff;
        entryOffset <<= 8;
        entryOffset |= buffer[pos++] & 0xff;
        // entry length
        entryLength  = buffer[pos++] & 0xff;
        entryLength <<= 8;
        entryLength |= buffer[pos++] & 0xff;
        entryLength <<= 8;
        entryLength |= buffer[pos++] & 0xff;
        entryLength <<= 8;
        entryLength |= buffer[pos++] & 0xff;
        if (entryId == kResourceFork) {
            isFound = DwTrue;
            break;
        }
    }

    if (!isFound) {
        return errorCode;
    }
    if (entryOffset+entryLength > buffer.length()) {
        errorCode = kErrCorruptData;
        return errorCode;
    }

    // Set resource fork string

    mResourceFork = buffer.substr(entryOffset, entryLength);

    return errorCode;
}


int DwAppleFile::GetDataFork()
{
    int errorCode = kErrNoError;
    const DwString& buffer = mAppleFileChars;

    // Set to empty value

    mDataFork = "";
    if (!mHasDataFork) {
        return errorCode;
    }

    // Scan for the Data Fork entry

    DwUint32 entryId;
    DwUint32 entryOffset;
    DwUint32 entryLength;
    DwBool isFound = DwFalse;
    for (int i=0; i < mNumEntries; ++i) {
        int pos = 26 + i*12;
        // entry ID
        entryId  = buffer[pos++] & 0xff;
        entryId <<= 8;
        entryId |= buffer[pos++] & 0xff;
        entryId <<= 8;
        entryId |= buffer[pos++] & 0xff;
        entryId <<= 8;
        entryId |= buffer[pos++] & 0xff;
        // entry offset
        entryOffset  = buffer[pos++] & 0xff;
        entryOffset <<= 8;
        entryOffset |= buffer[pos++] & 0xff;
        entryOffset <<= 8;
        entryOffset |= buffer[pos++] & 0xff;
        entryOffset <<= 8;
        entryOffset |= buffer[pos++] & 0xff;
        // entry length
        entryLength  = buffer[pos++] & 0xff;
        entryLength <<= 8;
        entryLength |= buffer[pos++] & 0xff;
        entryLength <<= 8;
        entryLength |= buffer[pos++] & 0xff;
        entryLength <<= 8;
        entryLength |= buffer[pos++] & 0xff;
        if (entryId == kDataFork) {
            isFound = DwTrue;
            break;
        }
    }

    if (!isFound) {
        return errorCode;
    }
    if (entryOffset+entryLength > buffer.length()) {
        errorCode = kErrCorruptData;
        return errorCode;
    }

    // Set resource fork string

    mDataFork = buffer.substr(entryOffset, entryLength);

    return errorCode;
}
