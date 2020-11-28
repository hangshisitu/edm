/* base64.c */
/*--------------------------------------------------------------------------*\
  Copyright (C) 1999-2000 Douglas W. Sauder

  This software is provided "as is," without any express or implied
  warranty.  In no event will the author be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
  claim that you wrote the original software. If you use this software
  in a product, an acknowledgment in the product documentation would be
  appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
  misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  The original distribution can be obtained from www.hunnysoft.com.
  You can email the author at dwsauder@hunnysoft.com.

  $RCSfile: base64.c,v $
  $Revision: 1.2 $
  $Date: 2001/12/03 09:21:30 $
\*--------------------------------------------------------------------------*/

#include "mimepp/base64.h"

static char base64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz0123456789+/";

#define SKIP '\177'
#define BAD  '\277'
#define END  '\100'

static char base64Map[] = {
  /*  00     01     02     03     04     05     06     07 */
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
  /*  08     09     0A     0B     0C     0D     0E     0F */
     BAD,  SKIP,  SKIP,   BAD,  SKIP,  SKIP,   BAD,   BAD,
  /*  10     11     12     13     14     15     16     17 */
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
  /*  18     19     1A     1B     1C     1D     1E     1F */
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
  /*  SP     !      "      #      $      %      &      '  */
    SKIP,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
  /*  (      )      *      +      ,      -      .      /  */
     BAD,   BAD,   BAD,    62,   BAD,   BAD,   BAD,    63,
  /*  0      1      2      3      4      5      6      7 */
      52,    53,    54,    55,    56,    57,    58,    59,
  /*  8      9      :      ;      <      =      >      ?  */
      60,    61,   BAD,   BAD,   BAD,   END,   BAD,   BAD,
  /*  @      A      B      C      D      E      F      G  */
     BAD,    0 ,    1 ,    2 ,    3 ,    4 ,    5 ,    6 ,
  /*  H      I      J      K      L      M      N      O  */
      7 ,    8 ,    9 ,    10,    11,    12,    13,    14,
  /*  P      Q      R      S      T      U      V      W  */
      15,    16,    17,    18,    19,    20,    21,    22,
  /*  X      Y      Z      [      \      ]      ^      _  */
      23,    24,    25,   BAD,   BAD,   BAD,   BAD,   BAD,
  /*  `      a      b      c      d      e      f      g  */
     BAD,    26,    27,    28,    29,    30,    31,    32,
  /*  h      i      j      k      l      m      n      o  */
      33,    34,    35,    36,    37,    38,    39,    40,
  /*  p      q      r      s      t      u      v      w  */
      41,    42,    43,    44,    45,    46,    47,    48,
  /*  x      y      z      {      |      }      ~     DEL */
      49,    50,    51,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD
};


/*--------------------------------------------------------------------------*\
 * dw_b64encoder implementation
\*--------------------------------------------------------------------------*/


/*
 * The following routine is "fast".  To minimize branching, very
 * little checking is done of the input.  The calling procedure
 * bears the responsibility to guard against bad input.
 *
 * The number of bytes in the input buffer MUST be a multiple of 3.
 *
 * The output buffer MUST be big enough -- at least of size
 * (aInBufferLen / 3 * 4)
 */
static void fast_encode(char *aInBuffer, int aInBufferLen, char *aOutBuffer)
{
    int i, by, n, inPos, outPos;

    inPos = 0;
    outPos = 0;
    while (inPos < aInBufferLen) {
        by = aInBuffer[inPos++] & 0xff;
        n = by;
        n <<= 8;
        by = aInBuffer[inPos++] & 0xff;
        n += by;
        n <<= 8;
        by = aInBuffer[inPos++] & 0xff;
        n += by;
        outPos += 4;
        i = outPos;
        aOutBuffer[--i] = base64Chars[n&0x3f];
        n >>= 6;
        aOutBuffer[--i] = base64Chars[n&0x3f];
        n >>= 6;
        aOutBuffer[--i] = base64Chars[n&0x3f];
        n >>= 6;
        aOutBuffer[--i] = base64Chars[n&0x3f];
    }
}


void dw_b64encoder_set_defaults(dw_b64encoder *aEncoder)
{
    aEncoder->mMaxLineLen = 72;
    aEncoder->mOutputCrLf = 0;
    aEncoder->mNoFinalNewline = 0;
}


void dw_b64encoder_start(dw_b64encoder *aEncoder)
{
    aEncoder->mState = DW_B64_STATE_0;
    aEncoder->mLastByte = 0;
    aEncoder->mLineLen = 0;
}


void dw_b64encoder_encode(dw_b64encoder *aEncoder)
{
    int by, n, inLen, outLen, inPos, outPos, maxLineLen;
    char *inBuffer, *outBuffer;

    inBuffer = aEncoder->mInBuffer;
    inLen = aEncoder->mInLen;
    inPos = aEncoder->mInPos;
    outBuffer = aEncoder->mOutBuffer;
    outLen = aEncoder->mOutLen;
    outPos = aEncoder->mOutPos;
    maxLineLen = aEncoder->mMaxLineLen & 0x7ffffffc;

    while (inPos < inLen && outPos < outLen) {
        switch (aEncoder->mState) {
        case DW_B64_STATE_0:
            aEncoder->mState = DW_B64_STATE_1;
            by = inBuffer[inPos++] & 0xff;
            n = by >> 2;
            outBuffer[outPos++] = base64Chars[n];
            aEncoder->mLastByte = by;
            ++aEncoder->mLineLen;
            break;
        case DW_B64_STATE_1:
            aEncoder->mState = DW_B64_STATE_2;
            by = inBuffer[inPos++] & 0xff;
            n = (aEncoder->mLastByte & 0x03) << 4;
            n += by >> 4;
            outBuffer[outPos++] = base64Chars[n];
            aEncoder->mLastByte = by;
            ++aEncoder->mLineLen;
            break;
        case DW_B64_STATE_2:
            aEncoder->mState = DW_B64_STATE_3;
            by = inBuffer[inPos++] & 0xff;
            n = (aEncoder->mLastByte & 0x0f) << 2;
            n += by >> 6;
            outBuffer[outPos++] = base64Chars[n];
            aEncoder->mLastByte = by;
            ++aEncoder->mLineLen;
            break;
        case DW_B64_STATE_3:
            aEncoder->mState = DW_B64_STATE_0;
            n = aEncoder->mLastByte & 0x3f;
            outBuffer[outPos++] = base64Chars[n];
            ++aEncoder->mLineLen;
            if (aEncoder->mLineLen >= maxLineLen) {
                aEncoder->mLineLen = 0;
                if (aEncoder->mOutputCrLf) {
                    aEncoder->mState = DW_B64_STATE_OUTPUT_CRLF;
                }
                else /* if (! aEncoder->mOutputCrLf) */ {
                    aEncoder->mState = DW_B64_STATE_OUTPUT_LF;
                }
            }
            break;
        case DW_B64_STATE_OUTPUT_LF:
            aEncoder->mState = DW_B64_STATE_0;
            outBuffer[outPos++] = '\n';
            break;
        case DW_B64_STATE_OUTPUT_CRLF:
            aEncoder->mState = DW_B64_STATE_OUTPUT_LF;
            outBuffer[outPos++] = '\r';
            break;
        }
    }
    while (outPos < outLen) {
        switch (aEncoder->mState) {
        case DW_B64_STATE_0:
        case DW_B64_STATE_1:
        case DW_B64_STATE_2:
            goto LOOP_EXIT;
            break;
        case DW_B64_STATE_3:
            aEncoder->mState = DW_B64_STATE_0;
            n = aEncoder->mLastByte & 0x3f;
            outBuffer[outPos++] = base64Chars[n];
            ++aEncoder->mLineLen;
            if (aEncoder->mLineLen >= maxLineLen) {
                aEncoder->mLineLen = 0;
                if (aEncoder->mOutputCrLf) {
                    aEncoder->mState = DW_B64_STATE_OUTPUT_CRLF;
                }
                else /* if (! aEncoder->mOutputCrLf) */ {
                    aEncoder->mState = DW_B64_STATE_OUTPUT_LF;
                }
            }
            break;
        case DW_B64_STATE_OUTPUT_LF:
            aEncoder->mState = DW_B64_STATE_0;
            outBuffer[outPos++] = '\n';
            break;
        case DW_B64_STATE_OUTPUT_CRLF:
            aEncoder->mState = DW_B64_STATE_OUTPUT_LF;
            outBuffer[outPos++] = '\r';
            break;
        }
    }
LOOP_EXIT:
    aEncoder->mInPos = inPos;
    aEncoder->mOutPos = outPos;
}


void dw_b64encoder_encode_fast(dw_b64encoder *aEncoder)
{
    int by, n, numEolChars, inSegmentLen, outSegmentLen;
    int inLen, outLen, inPos, outPos, inLimit, maxLineLen;
    char *inBuffer, *outBuffer;

    inBuffer = aEncoder->mInBuffer;
    inLen = aEncoder->mInLen;
    inPos = aEncoder->mInPos;
    outBuffer = aEncoder->mOutBuffer;
    outLen = aEncoder->mOutLen;
    outPos = aEncoder->mOutPos;
    maxLineLen = aEncoder->mMaxLineLen & 0x7ffffffc;

    numEolChars = (aEncoder->mOutputCrLf) ? 2 : 1;
    /*
     * Finish the current input triplet
     */
    while (inPos < inLen && outPos < outLen) {
        switch (aEncoder->mState) {
        case DW_B64_STATE_0:
            goto LOOP_EXIT;
            break;
        case DW_B64_STATE_1:
            aEncoder->mState = DW_B64_STATE_2;
            by = inBuffer[inPos++] & 0xff;
            n = (aEncoder->mLastByte & 0x03) << 4;
            n += by >> 4;
            outBuffer[outPos++] = base64Chars[n];
            aEncoder->mLastByte = by;
            ++aEncoder->mLineLen;
            break;
        case DW_B64_STATE_2:
            aEncoder->mState = DW_B64_STATE_3;
            by = inBuffer[inPos++] & 0xff;
            n = (aEncoder->mLastByte & 0x0f) << 2;
            n += by >> 6;
            outBuffer[outPos++] = base64Chars[n];
            aEncoder->mLastByte = by;
            ++aEncoder->mLineLen;
            break;
        case DW_B64_STATE_3:
            aEncoder->mState = DW_B64_STATE_0;
            n = aEncoder->mLastByte & 0x3f;
            outBuffer[outPos++] = base64Chars[n];
            ++aEncoder->mLineLen;
            if (aEncoder->mLineLen >= maxLineLen) {
                aEncoder->mLineLen = 0;
                if (aEncoder->mOutputCrLf) {
                    aEncoder->mState = DW_B64_STATE_OUTPUT_CRLF;
                }
                else /* if (! aEncoder->mOutputCrLf) */ {
                    aEncoder->mState = DW_B64_STATE_OUTPUT_LF;
                }
            }
            break;
        case DW_B64_STATE_OUTPUT_LF:
            aEncoder->mState = DW_B64_STATE_0;
            outBuffer[outPos++] = '\n';
            break;
        case DW_B64_STATE_OUTPUT_CRLF:
            aEncoder->mState = DW_B64_STATE_OUTPUT_LF;
            outBuffer[outPos++] = '\r';
            break;
        }
    }
LOOP_EXIT:
    /*
     * Finish the current line of output
     */
    outSegmentLen = (maxLineLen - aEncoder->mLineLen) & 0x7ffffffc;
    inSegmentLen = outSegmentLen / 4 * 3;
    if (inPos + inSegmentLen <= inLen
        && outPos + outSegmentLen + numEolChars <= outLen) {
        fast_encode(&inBuffer[inPos], inSegmentLen, &outBuffer[outPos]);
        inPos += inSegmentLen;
        outPos += outSegmentLen;
        if (aEncoder->mOutputCrLf) {
            outBuffer[outPos++] = '\r';
        }
        outBuffer[outPos++] = '\n';
    }
    /*
     * Encode one line at a time, as fast as possible
     */
    outSegmentLen = maxLineLen;
    inSegmentLen = outSegmentLen / 4 * 3;
    inLimit = inPos + (outLen - outPos) / (outSegmentLen + numEolChars)
        * inSegmentLen;
    if (inLimit > inLen) {
        inLimit = inLen;
    }
    inLimit -= inSegmentLen;
    while (inPos <= inLimit) {
        fast_encode(&inBuffer[inPos], inSegmentLen, &outBuffer[outPos]);
        inPos += inSegmentLen;
        outPos += outSegmentLen;
        if (aEncoder->mOutputCrLf) {
            outBuffer[outPos++] = '\r';
        }
        outBuffer[outPos++] = '\n';
    }
    /*
     * Encode remaining partial line
     */
    outSegmentLen = (outLen - outPos) & 0x7ffffffc;
    inSegmentLen = outSegmentLen / 4 * 3;
    if (inSegmentLen > inLen - inPos) {
        inSegmentLen = (inLen - inPos) / 3 * 3;
        outSegmentLen = inSegmentLen / 3 * 4;
    }
    if (inSegmentLen > 0) {
        fast_encode(&inBuffer[inPos], inSegmentLen, &outBuffer[outPos]);
        inPos += inSegmentLen;
        outPos += outSegmentLen;
    }
    /*
     * Encode remaining partial triplet
     */
    aEncoder->mInPos = inPos;
    aEncoder->mOutPos = outPos;
    dw_b64encoder_encode(aEncoder);
}


void dw_b64encoder_finish(dw_b64encoder *aEncoder)
{
    int n, outPos, outLen, maxLineLen;
    char *outBuffer;

    outBuffer = aEncoder->mOutBuffer;
    outLen = aEncoder->mOutLen;
    outPos = aEncoder->mOutPos;
    maxLineLen = aEncoder->mMaxLineLen & 0x7ffffffc;

    /*
     * Note: this function requires that there be space in the output
     * buffer, even if there will be no output
     */
    while (outPos < outLen) {
        switch (aEncoder->mState) {
        case DW_B64_STATE_0:
            aEncoder->mState = DW_B64_STATE_DONE;
            if (! aEncoder->mNoFinalNewline) {
                aEncoder->mLineLen = 0;
                if (aEncoder->mOutputCrLf) {
                    aEncoder->mState = DW_B64_STATE_OUTPUT_LF;
                    outBuffer[outPos++] = '\r';
                }
                else /* if (! aEncoder->mOutputCrLf) */ {
                    outBuffer[outPos++] = '\n';
                }
            }
            break;
        case DW_B64_STATE_1:
            aEncoder->mState = DW_B64_STATE_OUTPUT_EQ_EQ;
            n = (aEncoder->mLastByte & 0x03) << 4;
            outBuffer[outPos++] = base64Chars[n];
            ++aEncoder->mLineLen;
            break;
        case DW_B64_STATE_2:
            aEncoder->mState = DW_B64_STATE_OUTPUT_EQ;
            n = (aEncoder->mLastByte & 0x0f) << 2;
            outBuffer[outPos++] = base64Chars[n];
            ++aEncoder->mLineLen;
            break;
        case DW_B64_STATE_3:
            aEncoder->mState = DW_B64_STATE_DONE;
            n = aEncoder->mLastByte & 0x3f;
            outBuffer[outPos++] = base64Chars[n];
            ++aEncoder->mLineLen;
            if (aEncoder->mLineLen >= maxLineLen
                || ! aEncoder->mNoFinalNewline) {
                aEncoder->mLineLen = 0;
                if (aEncoder->mOutputCrLf) {
                    aEncoder->mState = DW_B64_STATE_OUTPUT_CRLF;
                }
                else /* if (! aEncoder->mOutputCrLf) */ {
                    aEncoder->mState = DW_B64_STATE_OUTPUT_LF;
                }
            }
            break;
        case DW_B64_STATE_OUTPUT_EQ_EQ:
            aEncoder->mState = DW_B64_STATE_OUTPUT_EQ;
            outBuffer[outPos++] = '=';
            break;
        case DW_B64_STATE_OUTPUT_EQ:
            aEncoder->mState = DW_B64_STATE_DONE;
            outBuffer[outPos++] = '=';
            if (aEncoder->mLineLen >= maxLineLen
                || ! aEncoder->mNoFinalNewline) {
                aEncoder->mLineLen = 0;
                if (aEncoder->mOutputCrLf) {
                    aEncoder->mState = DW_B64_STATE_OUTPUT_CRLF;
                }
                else /* if (! aEncoder->mOutputCrLf) */ {
                    aEncoder->mState = DW_B64_STATE_OUTPUT_LF;
                }
            }
            break;
        case DW_B64_STATE_OUTPUT_LF:
            aEncoder->mState = DW_B64_STATE_DONE;
            outBuffer[outPos++] = '\n';
            break;
        case DW_B64_STATE_OUTPUT_CRLF:
            aEncoder->mState = DW_B64_STATE_OUTPUT_LF;
            outBuffer[outPos++] = '\r';
            break;
        case DW_B64_STATE_DONE:
            goto LOOP_EXIT;
            break;
        }
    }
LOOP_EXIT:
    aEncoder->mOutPos = outPos;
}


/*--------------------------------------------------------------------------*\
 * dw_b64decoder implementation
\*--------------------------------------------------------------------------*/


static void dw_b64decoder_decode4(dw_b64decoder *aDecoder)
{
    int ch, by, n, inLen, outLen, inPos, outPos;
    char *inBuffer, *outBuffer;

    inBuffer = aDecoder->mInBuffer;
    inLen = aDecoder->mInLen;
    inPos = aDecoder->mInPos;
    outBuffer = aDecoder->mOutBuffer;
    outLen = aDecoder->mOutLen;
    outPos = aDecoder->mOutPos;

    while (inPos < inLen && outPos < outLen) {
        ch = inBuffer[inPos++];
        n = base64Map[ch];
        if (! (n & 0xc0)) {
            switch (aDecoder->mState) {
            case DW_B64_STATE_0:
                aDecoder->mState = DW_B64_STATE_1;
                break;
            case DW_B64_STATE_1:
                aDecoder->mState = DW_B64_STATE_2;
                by = aDecoder->mLastByte << 2;
                by += n >> 4;
                outBuffer[outPos++] = by;
                break;
            case DW_B64_STATE_2:
                aDecoder->mState = DW_B64_STATE_3;
                by = aDecoder->mLastByte << 4;
                by += n >> 2;
                outBuffer[outPos++] = by;
                break;
            case DW_B64_STATE_3:
                aDecoder->mState = DW_B64_STATE_0;
                by = aDecoder->mLastByte << 6;
                by += n;
                outBuffer[outPos++] = by;
                goto LOOP_EXIT;
                break;
            }
            aDecoder->mLastByte = n;
        }
        else if (n == SKIP) {
        }
        else if (n == END) {
            aDecoder->mState = DW_B64_STATE_DONE;
            goto LOOP_EXIT;
        }
        else if (n == BAD) {
            aDecoder->mError = 1;
        }
    }
LOOP_EXIT:
    aDecoder->mInPos = inPos;
    aDecoder->mOutPos = outPos;
}


void dw_b64decoder_start(dw_b64decoder *aDecoder)
{
    aDecoder->mState = DW_B64_STATE_0;
    aDecoder->mError = 0;
    aDecoder->mLastByte = 0;
}


void dw_b64decoder_decode(dw_b64decoder *aDecoder)
{
    int ch, by, n, inLen, outLen, inPos, outPos;
    char *inBuffer, *outBuffer;

    if (aDecoder->mState == DW_B64_STATE_DONE) {
        return;
    }

    inBuffer = aDecoder->mInBuffer;
    inLen = aDecoder->mInLen;
    inPos = aDecoder->mInPos;
    outBuffer = aDecoder->mOutBuffer;
    outLen = aDecoder->mOutLen;
    outPos = aDecoder->mOutPos;

    while (inPos < inLen && outPos < outLen) {
        ch = inBuffer[inPos++];
        n = base64Map[ch];
        if (! (n & 0xc0)) {
            switch (aDecoder->mState) {
            case DW_B64_STATE_0:
                aDecoder->mState = DW_B64_STATE_1;
                break;
            case DW_B64_STATE_1:
                aDecoder->mState = DW_B64_STATE_2;
                by = aDecoder->mLastByte << 2;
                by += n >> 4;
                outBuffer[outPos++] = by;
                break;
            case DW_B64_STATE_2:
                aDecoder->mState = DW_B64_STATE_3;
                by = aDecoder->mLastByte << 4;
                by += n >> 2;
                outBuffer[outPos++] = by;
                break;
            case DW_B64_STATE_3:
                aDecoder->mState = DW_B64_STATE_0;
                by = aDecoder->mLastByte << 6;
                by += n;
                outBuffer[outPos++] = by;
                break;
            }
            aDecoder->mLastByte = n;
        }
        else if (n == SKIP) {
        }
        else if (n == END) {
            aDecoder->mState = DW_B64_STATE_DONE;
            goto LOOP_EXIT;
        }
        else if (n == BAD) {
            aDecoder->mError = 1;
        }
    }
LOOP_EXIT:
    aDecoder->mInPos = inPos;
    aDecoder->mOutPos = outPos;
}


void dw_b64decoder_decode_fast(dw_b64decoder *aDecoder)
{
    int ch, i, n, m, c1, c2, inLimit, saveInPos;
    int inLen, outLen, inPos, outPos;
    char *inBuffer, *outBuffer;

    if (aDecoder->mState == DW_B64_STATE_DONE) {
        return;
    }
    /*
     * First, get in sync.  That is, make sure we are at the beginning
     * of an input quartet of base64 chars.
     */
    dw_b64decoder_decode4(aDecoder);
    if (aDecoder->mState == DW_B64_STATE_DONE) {
        return;
    }
    inBuffer = aDecoder->mInBuffer;
    inLen = aDecoder->mInLen;
    inPos = aDecoder->mInPos;
    outBuffer = aDecoder->mOutBuffer;
    outLen = aDecoder->mOutLen;
    outPos = aDecoder->mOutPos;
    /*
     * Now, decode input quartets as fast as possible
     */
    while (inPos + 4 <= inLen && outPos + 3 <= outLen) {
        /*
         * Calculate a conservative max length of the input for the given
         * available space in the output buffer.  After this calculation,
         * we only need to compare the input pos to the input limit;
         * we don't have to compare the output pos to the output limit.
         */
        inLimit = inPos + (outLen - outPos) / 3 * 4;
        if (inLimit > inLen) {
            inLimit = inLen;
        }
        inLimit -= 4;
        while (inPos <= inLimit) {
            saveInPos = inPos;
            c1 = base64Map[inBuffer[inPos++] & 0xff];
            m = c1;
            n = c1;
            n <<= 6;
            c2 = base64Map[inBuffer[inPos++] & 0xff];
            m |= c2;
            n |= c2;
            n <<= 6;
            ch = base64Map[inBuffer[inPos++] & 0xff];
            m |= ch;
            n |= ch;
            n <<= 6;
            ch = base64Map[inBuffer[inPos++] & 0xff];
            m |= ch;
            n |= ch;
            /*
             * If all four input chars are "normal" base64 chars (that is,
             * not CR, LF, or other characters) then decode them.
             */
            if (! (0xc0 & m)) {
                outPos += 3;
                i = outPos;
                outBuffer[--i] = (char) n;
                n >>= 8;
                outBuffer[--i] = (char) n;
                n >>= 8;
                outBuffer[--i] = (char) n;
            }
            /*
             * Otherwise, handle this case by advancing by one input quartet,
             * skipping characters as necessary
             */
            else {
                inPos = saveInPos;
                /* Optimization: in most cases, we just skip LF or CR LF */
                if (c1 == SKIP) {
                    ++inPos;
                    if (c2 == SKIP) {
                        ++inPos;
                    }
                }
                else {
                    aDecoder->mInPos = inPos;
                    aDecoder->mOutPos = outPos;
                    dw_b64decoder_decode4(aDecoder);
                    if (aDecoder->mState == DW_B64_STATE_DONE) {
                        return;
                    }
                    inPos = aDecoder->mInPos;
                    outPos = aDecoder->mOutPos;
                }
            }
        }
    }
    /*
     * Now, finish up any remaining partial quartet
     */
    aDecoder->mInPos = inPos;
    aDecoder->mOutPos = outPos;
    dw_b64decoder_decode(aDecoder);
}
