//=============================================================================
// File:       enum.h
// Contents:   Declarations of global constants and function prototypes
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

#ifndef DW_ENUM_H
#define DW_ENUM_H

#ifndef DW_CONFIG_H
#include <mimepp/config.h>
#endif

//-----------------------------------------------------------------------------
// Enumerated values
//-----------------------------------------------------------------------------

#if defined(DW_USE_NAMESPACES)
namespace DwMime {
#else
struct DwMime {
#endif

// Content transfer encoding

enum {
    kCteNull,
    kCteUnknown,
    kCte7bit,
    kCte8bit,
    kCteBinary,
    kCteQuotedPrintable,
    kCteQp = kCteQuotedPrintable,
    kCteBase64,
    kCteLast
};

// Content types

enum {
    kTypeNull,
    kTypeUnknown,
    kTypeText,
    kTypeMultipart,
    kTypeMessage,
    kTypeApplication,
    kTypeImage,
    kTypeAudio,
    kTypeVideo,
    kTypeModel,
    kTypeLast
};

// Content subtypes

enum {
    kSubtypeNull,
    kSubtypeUnknown,
    // Text
    kSubtypePlain,           // text/plain
    kSubtypeRichtext,        // text/richtext (RFC-1341)
    kSubtypeEnriched,        // text/enriched
    kSubtypeHtml,            // text/html
    // Multipart
    kSubtypeMixed,           // multipart/mixed
    kSubtypeAlternative,     // multipart/alternative
    kSubtypeDigest,          // multipart/digest
    kSubtypeParallel,        // multipart/parallel
    // Message
    kSubtypeRfc822,          // message/rfc822
    kSubtypePartial,         // message/partial
    kSubtypeExternalBody,    // message/external-body
    // Application
    kSubtypePostscript,      // application/postscript
    kSubtypeOctetStream,     // application/octet-stream
    // Image
    kSubtypeJpeg,            // image/jpeg
    kSubtypeGif,             // image/gif
    // Audio
    kSubtypeBasic,           // audio/basic
    // Video
    kSubtypeMpeg,            // video/mpeg
    // RFC 1847
    kSubtypeSigned,          // multipart/signed
    kSubtypeEncrypted,       // multipart/encrypted
    kSubtypeRfc822Headers,   // text/rfc822-headers
    // RFC-1892
    kSubtypeReport,          // multipart/report
    // RFC-1894
    kSubtypeDeliveryStatus,  // message/delivery-status
    // RFC-2112
    kSubtypeRelated,         // multipart/related
    // RFC-1740
    kSubtypeAppledouble,     // multipart/appledouble
    kSubtypeApplefile,       // application/applefile
    // Last
    kSubtypeLast
};

// Well-known header fields

enum {
    kFldNull,
    kFldUnknown,
    // RFC-822
    kFldBcc,
    kFldCc,
    kFldComments,
    kFldDate,
    kFldEncrypted,
    kFldFrom,
    kFldInReplyTo,
    kFldKeywords,
    kFldMessageId,
    kFldReceived,
    kFldReferences,
    kFldReplyTo,
    kFldResentBcc,
    kFldResentCc,
    kFldResentDate,
    kFldResentFrom,
    kFldResentMessageId,
    kFldResentReplyTo,
    kFldResentSender,
    kFldResentTo,
    kFldReturnPath,
    kFldSender,
    kFldTo,
    kFldSubject,
    // RFC-1036
    kFldApproved,
    kFldControl,
    kFldDistribution,
    kFldExpires,
    kFldFollowupTo,
    kFldLines,
    kFldNewsgroups,
    kFldOrganization,
    kFldPath,
    kFldSummary,
    kFldXref,
    // RFC-1521
    kFldContentDescription,
    kFldContentId,
    kFldContentTransferEncoding,
    kFldCte = kFldContentTransferEncoding,
    kFldContentType,
    kFldMimeVersion,
    // RFC-1544
    kFldContentMd5,
    // RFC-1806
    kFldContentDisposition,
    // Last
    kFldLast
};


// Disposition type (Content-Disposition header field, see RFC-1806)
enum {
    kDispTypeNull,
    kDispTypeUnknown,
    kDispTypeInline,
    kDispTypeAttachment
};


#if defined(DW_USE_NAMESPACES)
}  // end namespace DwMime
#else
}; // end DwMime class declaration
#endif

#endif
