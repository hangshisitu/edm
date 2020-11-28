//=============================================================================
// File:       proto_nt.cpp
// Contents:   Definitions for DwClientProtocol
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

#ifndef DW_NET_IMPLEMENTATION
#define DW_NET_IMPLEMENTATION
#endif

#if defined WIN32


#include <mimepp/config.h>
#include <mimepp/debug.h>

#include <stdio.h>
#include <string.h>
#include <limits.h>
#ifdef DW_USE_ANSI_IOSTREAM
#  include <iostream>
#else
#  include <iostream.h>
#endif

#include <mimepp/system.h>
#include <mimepp/protocol.h>

#if defined(DW_DEBUG_PROTO)
#  define DBG_PROTO_STMT(x) x
#else
#  define DBG_PROTO_STMT(x)
#endif

static const char* get_error_text(int aErrorCode);


DwProtocolClient::DwProtocolClient()
{
    mIsDllOpen      = DwFalse;
    mIsOpen         = DwFalse;
    mSocket         = INVALID_SOCKET;
    mPort           = 0;
    mServerName     = 0;
    mReceiveTimeout = 90;
    mLastCommand    = 0;
    mFailureCode    = kFailNoFailure;
    mFailureStr     = "";
    mErrorCode      = kErrNoError;
    mErrorStr       = get_error_text(kErrNoError);

    // Open the Winsock DLL

    WORD wVersionRequested = MAKEWORD(1, 1);
    WSADATA wsaData;
    int err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        // error!
        HandleError(err, kWSAStartup);
        return;
    }

    // Confirm that the Windows Sockets DLL supports 1.1.
    // Note that if the DLL supports versions greater
    // than 1.1 in addition to 1.1, it will still return
    // 1.1 in wVersion since that is the version we
    // requested.

    // Note: If the DLL supports Winsock 1.0, but not 1.1, it will not report
    // an error.  Since we do not support Winsock 1.0, we have to detect this
    // error.

    if (LOBYTE(wsaData.wVersion) != 1 ||
        HIBYTE(wsaData.wVersion) != 1) {

        // error!
        err = kErrNoWinsock;
        HandleError(err, kWSAStartup);
        return;
    }

    mIsDllOpen = DwTrue;
}


DwProtocolClient::~DwProtocolClient()
{
    if (mIsOpen) {
        Close();
    }
    if (mServerName) {
        delete [] mServerName;
        mServerName = 0;
    }
    if (mIsDllOpen) {
        WSACleanup();
    }
}


int DwProtocolClient::Open(const char* aServer, DwUint16 aPort)
{
    mFailureCode = kFailNoFailure;
    mFailureStr  = "";
    mErrorCode   = kErrNoError;
    mErrorStr    = get_error_text(mErrorCode);

    if (! mIsDllOpen) {
        // error!
        mErrorCode = kErrBadUsage;
        mErrorStr = get_error_text(mErrorCode);
        return -1;
    }
    if (mIsOpen) {
        // error!
        mErrorCode = kErrBadUsage;
        mErrorStr = get_error_text(mErrorCode);
        return -1;
    }
    if (aServer == 0 || aServer[0] == 0) {
        // error!
        mErrorCode = kErrBadParameter;
        mErrorStr = get_error_text(mErrorCode);
        return -1;
    }
    if (mServerName) {
        delete [] mServerName;
        mServerName = 0;
    }
    mServerName = new char[strlen(aServer)+1];
    strcpy(mServerName, aServer);
    mPort = aPort;

    // Open the socket

    mSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (mSocket == INVALID_SOCKET) {
        // error!
        int err = WSAGetLastError();
        HandleError(err, ksocket);
        return -1;
    }

    // Set socket option to timeout on receives
    int millisecs = 0;
    if (mReceiveTimeout < INT_MAX/1000) {
        millisecs = mReceiveTimeout*1000;
    }
    else {
        millisecs = INT_MAX;
    }
    int err = setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO,
        (const char*)&millisecs, sizeof(millisecs));
    if (err == SOCKET_ERROR) {
        // error!
        int err = WSAGetLastError();
        HandleError(err, ksetsockopt);
        return -1;
    }

    // If the server is specified by an IP number in dotted decimal form,
    // then try to connect to that IP number.

    err = -1;
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(struct sockaddr_in));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(mPort);
    serverAddr.sin_addr.s_addr = inet_addr(mServerName);
    if (serverAddr.sin_addr.s_addr != INADDR_NONE) {
        DBG_PROTO_STMT(cout << "Trying connection to " << mServerName << endl;)
        err = connect(mSocket, (struct sockaddr*)&serverAddr,
            sizeof(struct sockaddr_in));
    }

    // Otherwise, do a host name lookup.

    else {
        DwUint32 addresses[100];
        int numAddresses =
            DwGetHostByName(mServerName, 0, 0, addresses, 100, &err);
        if (err != 0) {
            // error!
            closesocket(mSocket);
            mSocket = INVALID_SOCKET;
            HandleError(err, kgethostbyname);
            return -1;
        }

        // Connect to the server.  Try each IP number until one succeeds.

        for (int i=0; i < numAddresses; ++i) {
            DwUint32 addr = addresses[i];
            serverAddr.sin_addr.s_addr = addr;
            DBG_PROTO_STMT(cout << "Trying connection to " << mServerName;)
            DBG_PROTO_STMT(cout << " (" << (((char*)&addr)[0]&0xff);)
            DBG_PROTO_STMT(cout << "." << (((char*)&addr)[1]&0xff);)
            DBG_PROTO_STMT(cout << "." << (((char*)&addr)[2]&0xff);)
            DBG_PROTO_STMT(cout << "." << (((char*)&addr)[3]&0xff);)
            DBG_PROTO_STMT(cout << ")" << endl;)
            err = connect(mSocket, (struct sockaddr*)&serverAddr,
                sizeof(struct sockaddr_in));
            if (err != SOCKET_ERROR) {
                break;
            }
        }
    }

    if (err == SOCKET_ERROR) {
        // error!
        int err = WSAGetLastError();
        closesocket(mSocket);
        mSocket = -1;
        HandleError(err, kconnect);
        return -1;
    }
    DBG_PROTO_STMT(cout << "Connection okay" << endl;)
    mIsOpen = DwTrue;
    return 0;
}


DwBool DwProtocolClient::IsOpen() const
{
    return mIsOpen;
}


int DwProtocolClient::Close()
{
    mFailureCode = kFailNoFailure;
    mFailureStr  = "";
    mErrorCode   = kErrNoError;
    mErrorStr    = get_error_text(mErrorCode);

    if (! mIsDllOpen) {
        // error!
        mErrorCode = kErrBadUsage;
        mErrorStr = get_error_text(mErrorCode);
        return -1;
    }
    if (! mIsOpen) {
        // error!
        mErrorCode = kErrBadUsage;
        mErrorStr = get_error_text(mErrorCode);
        return -1;
    }
    int err = closesocket(mSocket);
    if (err == SOCKET_ERROR) {
        // error!
        int err = WSAGetLastError();
        HandleError(err, kclosesocket);
        return -1;
    }
    mIsOpen = DwFalse;
    return 0;
}


int DwProtocolClient::SetReceiveTimeout(int aSecs)
{
    mReceiveTimeout = aSecs;
    if (mIsOpen) {
        // Set socket option to timeout on receives
        int millisecs = 0;
        if (mReceiveTimeout < INT_MAX/1000) {
            millisecs = mReceiveTimeout*1000;
        }
        else {
            millisecs = INT_MAX;
        }
        int err = setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO,
            (const char*)&millisecs, sizeof(millisecs));
        if (err == SOCKET_ERROR) {
            // error!
            int err = WSAGetLastError();
            HandleError(err, ksetsockopt);
            return -1;
        }
    }
    return 0;
}


int DwProtocolClient::LastCommand() const
{
    return mLastCommand;
}


int DwProtocolClient::LastFailure() const
{
    return mFailureCode;
}


const char* DwProtocolClient::LastFailureStr() const
{
    return mFailureStr;
}


int DwProtocolClient::LastError() const
{
    return mErrorCode;
}


const char* DwProtocolClient::LastErrorStr() const
{
    return mErrorStr;
}


int DwProtocolClient::PSend(const char* aBuf, int aBufLen)
{
    mFailureCode = kFailNoFailure;
    mFailureStr  = "";
    mErrorCode   = kErrNoError;
    mErrorStr    = get_error_text(mErrorCode);

    if (! mIsDllOpen) {
        // error!
        mErrorCode = kErrBadUsage;
        mErrorStr = get_error_text(mErrorCode);
        return 0;
    }
    if (! mIsOpen) {
        // error!
        mErrorCode = kErrBadUsage;
        mErrorStr = get_error_text(mErrorCode);
        return 0;
    }
    int ret;
    int numToSend = aBufLen;
    int numSent = 0;
    while (numToSend > 0) {
        ret = send(mSocket, &aBuf[numSent], numToSend, 0);
        if (ret == SOCKET_ERROR) {
            // error!
            int err = WSAGetLastError();
            HandleError(err, ksend);
            break;
        }
        else {
            numSent += ret;
            numToSend -= ret;
        }
    }
    return numSent;
}


int DwProtocolClient::PReceive(char* aBuf, int aBufSize)
{
    mFailureCode = kFailNoFailure;
    mFailureStr  = "";
    mErrorCode   = kErrNoError;
    mErrorStr    = get_error_text(mErrorCode);

    if (! mIsDllOpen) {
        // error!
        mErrorCode = kErrBadUsage;
        mErrorStr = get_error_text(mErrorCode);
        return 0;
    }
    if (! mIsOpen) {
        // error!
        mErrorCode = kErrBadUsage;
        mErrorStr = get_error_text(mErrorCode);
        return 0;
    }
    int numReceived = 0;
    int ret = recv(mSocket, aBuf, aBufSize, 0);
    if (ret == SOCKET_ERROR) {
        // error!
        int err = WSAGetLastError();
        HandleError(err, krecv);
        numReceived = 0;
    }
    else {
        numReceived = ret;
    }
    return numReceived;
}


void DwProtocolClient::HandleError(int aErrorCode, int aSystemCall)
{
    mErrorCode = aErrorCode;
    mErrorStr = get_error_text(aErrorCode);
    switch (aSystemCall) {
    case kWSAStartup:
        switch (aErrorCode) {
        case kErrNoWinsock:
        case WSASYSNOTREADY:
        case WSAVERNOTSUPPORTED:
            mFailureCode = kFailNoWinsock;
            mFailureStr = "A usable Winsock DLL could not be found";
            break;
        default:
            break;
        }
        break;
    case ksocket:
        switch (aErrorCode) {
        case WSAENETDOWN:
            mFailureCode = kFailNetDown;
            mFailureStr = "The network is down";
            break;
        case WSAEMFILE:
        case WSAENOBUFS:
            mFailureCode = kFailNoResources;
            mFailureStr = "Cannot get required system resources";
            break;
        case WSANOTINITIALISED:
        case WSAEAFNOSUPPORT:
        case WSAEINPROGRESS:
        case WSAEPROTONOSUPPORT:
        case WSAEPROTOTYPE:
        case WSAESOCKTNOSUPPORT:
        default:
            break;
        }
        break;
    case kgethostbyname:
        switch (aErrorCode) {
        case WSAENETDOWN:
            mFailureCode = kFailNetDown;
            mFailureStr = "The network is down";
            break;
        case WSAHOST_NOT_FOUND:  // Host name not found by DNS server
        case WSATRY_AGAIN:       // No reply from DNS server
        case WSANO_RECOVERY:     // Error reported by DNS server
        case WSANO_DATA:         // DNS server has no data record for host
            mFailureCode = kFailHostNotFound;
            mFailureStr = "The server was not found";
            break;
        case WSANOTINITIALISED:
        case WSAEINPROGRESS:
        case WSAEINTR:
        default:
            break;
        }
        break;
    case ksetsockopt:
        switch (aErrorCode) {
        case WSAENETDOWN:
            mFailureCode = kFailNetDown;
            mFailureStr = "The network is down";
            break;
        case WSAENETRESET:
            mFailureCode = kFailConnDropped;
            mFailureStr = "The connection was dropped by the server";
            break;
        case WSANOTINITIALISED:
        case WSAEFAULT:
        case WSAEINPROGRESS:
        case WSAEINVAL:
        case WSAENOPROTOOPT:
        case WSAENOTCONN:
        case WSAENOTSOCK:
        default:
            break;
        }
        break;
    case kconnect:
        switch (aErrorCode) {
        case WSAENETDOWN:
            mFailureCode = kFailNetDown;
            mFailureStr = "The network is down";
            break;
        case WSAECONNREFUSED:
            mFailureCode = kFailConnRefused;
            mFailureStr = "The connection was refused by the server";
            break;
        case WSAENETUNREACH:
            mFailureCode = kFailNetUnreachable;
            mFailureStr = "The network is unreachable";
            break;
        case WSAETIMEDOUT:
            mFailureCode = kFailTimedOut;
            mFailureStr = "The connection attempt to the server timed out";
            break;
        case WSANOTINITIALISED:
        case WSAEADDRINUSE:
        case WSAEINTR:
        case WSAEINPROGRESS:
        case WSAEADDRNOTAVAIL:
        case WSAEAFNOSUPPORT:
        case WSAEDESTADDRREQ:
        case WSAEFAULT:
        case WSAEINVAL:
        case WSAEISCONN:
        case WSAEMFILE:
        case WSAENOBUFS:
        case WSAENOTSOCK:
        case WSAEWOULDBLOCK:
        default:
            break;
        }
        break;
    case ksend:
        switch (aErrorCode) {
        case WSAENETDOWN:
            mFailureCode = kFailNetDown;
            mFailureStr = "The network is down";
            break;
        case WSAENETRESET:
        case WSAECONNABORTED:
        case WSAECONNRESET:
            mFailureCode = kFailConnDropped;
            mFailureStr = "The connection was dropped by the server";
            break;
        case WSANOTINITIALISED:
        case WSAEACCES:
        case WSAEINTR:
        case WSAEINPROGRESS:
        case WSAEFAULT:
        case WSAENOBUFS:
        case WSAENOTCONN:
        case WSAENOTSOCK:
        case WSAEOPNOTSUPP:
        case WSAESHUTDOWN:
        case WSAEWOULDBLOCK:
        case WSAEMSGSIZE:
        case WSAEINVAL:
        default:
            break;
        }
        break;
    case krecv:
        switch (aErrorCode) {
        case WSAENETDOWN:
            mFailureCode = kFailNetDown;
            mFailureStr = "The network is down";
            break;
        case WSAENETRESET:
        case WSAECONNABORTED:
        case WSAECONNRESET:
            mFailureCode = kFailConnDropped;
            mFailureStr = "The connection was dropped by the server";
            break;
        case WSANOTINITIALISED:
        case WSAEACCES:
        case WSAEINTR:
        case WSAEINPROGRESS:
        case WSAEFAULT:
        case WSAENOBUFS:
        case WSAENOTCONN:
        case WSAENOTSOCK:
        case WSAEOPNOTSUPP:
        case WSAESHUTDOWN:
        case WSAEWOULDBLOCK:
        case WSAEMSGSIZE:
        case WSAEINVAL:
        default:
            break;
        }
        break;
    case kclosesocket:
        switch (aErrorCode) {
        case WSAENETDOWN:
            mFailureCode = kFailNetDown;
            mFailureStr = "The network is down";
            break;
        case WSANOTINITIALISED:
        case WSAENOTSOCK:
        case WSAEINPROGRESS:
        case WSAEINTR:
        case WSAEWOULDBLOCK:
        default:
            break;
        }
        break;
    default:
        break;
    }
}


static const char* get_error_text(int aErrorCode)
{
    const char* msg = "";
    switch (aErrorCode) {
    case DwProtocolClient::kErrNoError:
        msg = "No error";
        break;
    case DwProtocolClient::kErrUnknownError:
        msg = "Unknown error";
        break;
    case DwProtocolClient::kErrBadParameter:
        msg = "(MIME++) bad parameter passed to function";
        break;
    case DwProtocolClient::kErrBadUsage:
        msg = "(MIME++) bad library usage";
        break;
    case DwProtocolClient::kErrNoWinsock:
        msg = "(MIME++) incompatible Winsock version";
        break;
    case WSAEINTR:
        msg = "Interrupted system call";
        break;
    case WSAEBADF:
        msg = "Bad file number";
        break;
    case WSAEACCES:
        msg = "Permission denied";
        break;
    case WSAEFAULT:
        msg = "Bad address";
        break;
    case WSAEINVAL:
        msg = "Invalid argument";
        break;
    case WSAEMFILE:
        msg = "Too many open files";
        break;
    case WSAEWOULDBLOCK:
        msg = "Operation would block";
        break;
    case WSAEINPROGRESS:
        msg = "Operation now in progress";
        break;
    case WSAEALREADY:
        msg = "Operation already in progress";
        break;
    case WSAENOTSOCK:
        msg = "Socket operation on nonsocket";
        break;
    case WSAEDESTADDRREQ:
        msg = "Destination address required";
        break;
    case WSAEMSGSIZE:
        msg = "Message to long";
        break;
    case WSAEPROTOTYPE:
        msg = "Protocol wrong type for socket";
        break;
    case WSAENOPROTOOPT:
        msg = "Bad protocol option";
        break;
    case WSAEPROTONOSUPPORT:
        msg = "Protocol not supported";
        break;
    case WSAESOCKTNOSUPPORT:
        msg = "Socket type not supported";
        break;
    case WSAEOPNOTSUPP:
        msg = "Operation not supported on socket";
        break;
    case WSAEPFNOSUPPORT:
        msg = "Protocol family not supported";
        break;
    case WSAEAFNOSUPPORT:
        msg = "Address family not supported by protocol family";
        break;
    case WSAEADDRINUSE:
        msg = "Adress already in use";
        break;
    case WSAEADDRNOTAVAIL:
        msg = "Cannot assign requested address";
        break;
    case WSAENETDOWN:
        msg = "Network is down";
        break;
    case WSAENETUNREACH:
        msg = "Network is unreachable";
        break;
    case WSAENETRESET:
        msg = "Net dropped connection or reset";
        break;
    case WSAECONNABORTED:
        msg = "Software caused connection abort";
        break;
    case WSAECONNRESET:
        msg = "Connection reset by peer";
        break;
    case WSAENOBUFS:
        msg = "No buffer space available";
        break;
    case WSAEISCONN:
        msg = "Socket is already connected";
        break;
    case WSAENOTCONN:
        msg = "Socket is not connected";
        break;
    case WSAESHUTDOWN:
        msg = "Cannot send after socket is shutdown";
        break;
    case WSAETOOMANYREFS:
        msg = "Too many references, cannot splice";
        break;
    case WSAETIMEDOUT:
        msg = "Connection timed out";
        break;
    case WSAECONNREFUSED:
        msg = "Connection refused";
        break;
    case WSAELOOP:
        msg = "Too many levels of symbolic links";
        break;
    case WSAENAMETOOLONG:
        msg = "File name too long";
        break;
    case WSAEHOSTDOWN:
        msg = "Host is down";
        break;
    case WSAEHOSTUNREACH:
        msg = "No route to host";
        break;
    case WSAENOTEMPTY:
        msg = "Directory not empty";
        break;
    case WSAEPROCLIM:
        "Too many processes";
        break;
    case WSAEUSERS:
        msg = "Too many users";
        break;
    case WSAEDQUOT:
        msg = "Disk quota exceeded";
        break;
    case WSAESTALE:
        msg = "Stale NFS file handle";
        break;
    case WSAEREMOTE:
        msg = "Too many levels of remote in path";
        break;
    case WSASYSNOTREADY:
        msg = "Network subsystem is unavailable";
        break;
    case WSAVERNOTSUPPORTED:
        msg = "WINSOCK DLL version out of range";
        break;
    case WSANOTINITIALISED:
        msg = "Successful WSASTARTUP not yet performed";
        break;
    case WSAHOST_NOT_FOUND:
        msg = "Host not found";
        break;
    case WSATRY_AGAIN:
        msg = "Nonauthoritative host not found";
        break;
    case WSANO_RECOVERY:
        msg = "Nonrecoverable errors: FORMERR, REFUSED, NOTIMP";
        break;
    case WSANO_DATA:
        msg = "Valid name, no data record of requested type";
        break;
//    case WSANO_ADDRESS:
//        msg = "No address, look for MX record";
//        break;
    default:
        msg = "Unknown error";
        break;
    }
    return msg;
}

#endif
