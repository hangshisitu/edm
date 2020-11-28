//=============================================================================
// File:       applfile.h
// Contents:   Declarations for DwAppleFile
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

#ifndef DW_APPLFILE_H
#define DW_APPLFILE_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

#ifndef DW_STRING_H
#include <mimepp/string.h>
#endif


//=============================================================================
//+ Name DwAppleFile -- Class for packing/unpacking AppleSingle and AppleDouble data
//+ Description
//. <var>DwAppleFile</var> packs or unpacks file data to or from AppleSingle or
//. AppleDouble format.  The AppleSingle format allows a file's contents to be
//. packaged together with related file information, such as file dates,
//. resources, icons, and so forth, into a single file.  The AppleDouble
//. format is similar, but different in that the file's contents are stored
//. in one file, and the remaining file information (dates, resources, icons,
//. and so forth) is stored in a separate AppleDouble header file.
//.
//. In the MIME world, AppleSingle and AppleDouble are the preferred ways to
//. transfer Macintosh files, with AppleDouble preferred over AppleSingle
//. where possible.  In actual practice, the Binhex format is common (most
//. likely due to the fact that most Macintosh archives store files in Binhex
//. format).  The use of AppleSingle and AppleDouble in MIME is described
//. in RFC-1740 and is referred to as MacMIME.  MacMIME is an official
//. Internet standards track protocol.
//.
//. To use <var>DwAppleFile</var> for packing file data to the AppleDouble
//. header format, first call
//. <var>SetFormat(DwAppleFile::kAppleDoubleHeader)</var> to set the format
//. type to AppleDouble header, then call one or more of the following
//. functions to set the file data:
//. <var>SetFileName()</var>,
//. <var>SetCreateTime()</var>,
//. <var>SetModifyTime()</var>,
//. <var>SetBackupTime()</var>,
//. <var>SetAccessTime()</var>
//. <var>SetResourceFork()</var>.
//. Next, call <var>Pack()</var> to package all the file information into
//. a single buffer.  Finally, call <var>PackagedData()</var> to get the
//. packaged AppleDouble header data.
//.
//. To use <var>DwAppleFile</var> for packing file data to the AppleSingle
//. format, first call <var>SetFormat(DwAppleFile::kAppleSingle)</var>
//. to set the format type to AppleSingle, then call one or more of the
//. following functions to set the file data:
//. <var>SetFileName()</var>,
//. <var>SetCreateTime()</var>,
//. <var>SetModifyTime()</var>,
//. <var>SetBackupTime()</var>,
//. <var>SetAccessTime()</var>
//. <var>SetResourceFork()</var>,
//. <var>SetDataFork()</var>.
//. Next, call <var>Pack()</var> to package all the file information into
//. a single buffer.  Finally, call <var>PackagedData()</var> to get the
//. packaged AppleSingle data.
//.
//. Note: The definitive reference for AppleSingle and AppleDouble is
//. <i>AppleSingle/AppleDouble Formats: Developer's Note</i>, available from
//. Apple Computer.
//=============================================================================
// Last modified 1998-12-04
//+ Noentry ~DwAppleFile

class DW_UTIL_EXPORT DwAppleFile {

public:

    enum Error {
        kErrNoError = 0,
        kErrTooShort,
        kErrBadMagicNumber,
        kErrWrongVersion,
        kErrCorruptData
    };

    enum FormatType {
        kAppleSingle,
        kAppleDoubleHeader
    };

    DwAppleFile();
    //. Default constructor.

    int Format() const;
    void SetFormat(int aFormatType);
    //. Gets or sets the format type.  When using <var>DwAppleFile</var>
    //. to pack file data, you should specify a format type of
    //. <var>kAppleSingle</var> or <var>kAppleDoubleHeader</var>.
    //. When using <var>DwAppleFile</var> to unpack file data, the format type
    //. is determined by interpreting a magic number in the packaged data.
    //.
    //. The AppleDouble header and AppleSingle formats are almost identical.
    //. They are different in two respects: the magic number is different, and
    //. an AppleDouble header is not permitted to contain a data fork.

    DwBool HasFileName() const;
    //. Returns a true value if the packaged file data contains a file name.
    //. If it does contain a file name, you can get the file name by calling
    //. <var>FileName()</var>.

    const char* FileName() const;
    void SetFileName(const char* aName);
    //. Gets or sets the file name that is included in the packaged file data.

    DwBool HasFileTimes() const;
    //. Returns true value if the packaged file data contains a file times
    //. entry.  If it does contain a file times entry, you can get the
    //. file times by calling <var>CreateTime()</var>,
    //. <var>ModificationTime()</var>, <var>BackupTime()</var>, and
    //. <var>AccessTime()</var>.
    //.
    //. If a file times entry is present, that does not mean that all the
    //. times (create time, modify time, backup time, and access time)
    //. are all valid.  An invalid time is indicated by a value of
    //. 0x80000000.

    DwInt32 CreateTime() const;
    void SetCreateTime(DwInt32 aTime);
    DwInt32 ModifyTime() const;
    void SetModifyTime(DwInt32 aTime);
    DwInt32 BackupTime() const;
    void SetBackupTime(DwInt32 aTime);
    DwInt32 AccessTime() const;
    void SetAccessTime(DwInt32 aTime);
    //. Gets or sets file times that are included in the packaged file data.
    //. The <i>create time</i> is the time that the file was created.
    //. The <i>modify time</i> is the time the file was last modified.
    //. The <i>backup time</i> is the time the file was last saved to a
    //. backup medium.
    //. The <i>access time</i> is the time the file was last accessed.
    //.
    //. A file time's value represents the number of seconds before or after
    //. 1&nbsp;Jan&nbsp;2000 00:00:00&nbsp;UTC.  The value 0 represents
    //. 1&nbsp;Jan&nbsp;2000 00:00:00&nbsp;UTC; a negative time represents
    //. the number of seconds until 1&nbsp;Jan&nbsp;2000 00:00:00&nbsp;UTC;
    //. and a positive value represents the number of seconds after
    //. 1&nbsp;Jan&nbsp;2000 00:00:00&nbsp;UTC.
    //. The value 0x80000000 is a special value that indicates an invalid
    //. value.
    //.
    //. You can use the functions <var>UnixTimeToAppleFileTime()</var> and
    //. <var>AppleFileTimeToUnixTime()</var> to convert between the apple
    //. file time representation (signed number of seconds since
    //. 1&nbsp;Jan&nbsp;2000) and the Unix time representation (unsigned
    //. number of seconds since 1&nbsp;Jan&nbsp;1970).

    DwBool HasDataFork() const;
    //. Returns a true value if the packaged file data contains a Macintosh
    //. data fork entry.  If it does contain a data fork, you can get
    //. the data fork by calling <var>DataFork()</var>.
    //.
    //. An AppleDouble header does not contain a data fork entry.

    const DwString& DataFork() const;
    void SetDataFork(const DwString& aStr);
    //. Gets or sets the data fork.  The data fork contains the file's "data,"
    //. such as the text in a text file, or the image data in a GIF file.  In
    //. contrast, the resource fork contains resources that are used only by
    //. the Macintosh operating system (font information, code, icons, and so
    //. forth). An AppleDouble header does not contain a data fork entry.

    DwBool HasResourceFork() const;
    //. Returns a true value if the packaged file data contains a Macintosh
    //. resource fork entry.  If it does contain a resource fork, you can get
    //. the resource fork by calling <var>ResourceFork()</var>.

    const DwString& ResourceFork() const;
    void SetResourceFork(const DwString& aStr);
    //. Gets or sets the resource fork.  The resource fork contains resources
    //. that are used only by Macintosh operating system (font information,
    //. code, icons, and so forth).

    const DwString& PackagedData() const;
    void SetPackagedData(const DwString& aStr);
    //. Gets or sets the buffer of packaged file data.

    void Pack();
    //. Packages the file data into a single buffer.  After calling
    //. <var>Pack()</var>, you can get the packaged data by calling 
    //. <var>PackagedData()</var>.

    int Unpack();
    //. Unpacks the packaged file data.  Before calling <var>Unpack()</var>,
    //. you should call <var>SetPackagedData()</var> to set the buffer of
    //. packaged file data.  After calling <var>Unpack()</var>, you can call
    //. other functions to retrieve the unpacked file information.
    //.
    //. If the packaged file data is successfully unpacked, <var>Unpack()</var>
    //. returns 0.  If an error occurs, it returns an error code:
    //.
    //. <pre>
    //.&nbsp;    kErrTooShort        premature end of file detected 
    //.&nbsp;    kErrBadMagicNumber  the magic number indicates that the data is not in
    //.&nbsp;                        AppleDouble or AppleSingle format
    //.&nbsp;    kErrWrongVersion    the version number indicates a version of the AppleDouble
    //.&nbsp;                        or AppleSingle format that is not understood
    //.&nbsp;    kErrCorruptData     that data is corrupted in some way
    //. </pre>

    static DwInt32 UnixTimeToAppleFileTime(DwUint32 aUnixTime);
    static DwUint32 AppleFileTimeToUnixTime(DwInt32 aASTime);
    //. Converts between the scalar time used in the AppleSingle and
    //. AppleDouble formats ("Apple file time") and the scalar time used
    //. in Unix ("Unix time"). An Apple file time is defined to be the
    //. signed number of seconds before or after 1&nbsp;Jan&nbsp;2000
    //. 00:00:00&nbsp;UTC. The value 0 represents 1&nbsp;Jan&nbsp;2000
    //. 00:00:00&nbsp;UTC; a negative time represents the number of seconds
    //. until 1&nbsp;Jan&nbsp;2000 00:00:00&nbsp;UTC; and a positive value
    //. represents the number of seconds after 1&nbsp;Jan&nbsp;2000
    //. 00:00:00&nbsp;UTC.
    //. A Unix time is the number of seconds since 1&nbsp;Jan&nbsp;1970
    //. 00:00:00&nbsp;UTC.

private:

    enum {
        kDataFork          =  1,
        kResourceFork      =  2,
        kRealName          =  3,
        kComment           =  4,
        kIconBW            =  5,
        kIconColor         =  6,
        kIconColour        =  6,
        kFileDatesInfo     =  8,
        kFinderInfo        =  9,
        kMacintoshFileInfo = 10,
        kProDosFileInfo    = 11,
        kMsDosFileInfo     = 12,
        kShortName         = 13
    };

    int IsValid();
    int GetFileDatesInfo();
    int GetRealName();
    int GetResourceFork();
    int GetDataFork();

    int mNumEntries;

    DwBool   mHasRealName;
    DwBool   mHasFileDatesInfo;
    DwBool   mHasResourceFork;
    DwBool   mHasDataFork;

    int      mErrorCode;

    int      mFormatType;
    DwInt32  mCreateTime;
    DwInt32  mModifyTime;
    DwInt32  mBackupTime;
    DwInt32  mAccessTime;
    DwString mRealName;
    DwString mResourceFork;
    DwString mDataFork;

    DwString mAppleFileChars;
};

#endif
