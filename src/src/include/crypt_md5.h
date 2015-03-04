/****************************************************************************
 * Mediatek.
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002, Mediatek, Inc.
 *
 * All rights reserved. Mediatek's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Mediatek. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Mediatek, Inc. is obtained.
 ***************************************************************************/

/****************************************************************************
    Module Name:
    MD5

    Abstract:
    RFC1321: The MD5 Message-Digest Algorithm

    Revision History:
    Who         When            What
    --------    ----------      ------------------------------------------
    Eddy        2008/11/24      Create md5
***************************************************************************/
#ifndef __CRYPT_MD5_H__
#define __CRYPT_MD5_H__

/* Algorithm options */
#define MD5_SUPPORT

#ifdef MD5_SUPPORT
#define MD5_BLOCK_SIZE    64 /* 512 bits = 64 bytes */
#define MD5_DIGEST_SIZE   16 /* 128 bits = 16 bytes */
typedef struct {
    uint32 HashValue[4];
    uint64 MessageLen;
    uint8  Block[MD5_BLOCK_SIZE];
    uint32   BlockLen;
} MD5_CTX_STRUC, *PMD5_CTX_STRUC;

void RT_MD5_Init (
    MD5_CTX_STRUC *pMD5_CTX);
void RT_MD5_Hash (
    MD5_CTX_STRUC *pMD5_CTX);
void RT_MD5_Append (
    MD5_CTX_STRUC *pMD5_CTX,
    const uint8 Message[],
    uint32 MessageLen);
void RT_MD5_End (
    MD5_CTX_STRUC *pMD5_CTX,
    uint8 DigestMessage[]);
void RT_MD5 (
    const uint8 Message[],
    uint32 MessageLen,
    uint8 DigestMessage[]);
#endif /* MD5_SUPPORT */

#endif /* __CRYPT_MD5_H__ */

