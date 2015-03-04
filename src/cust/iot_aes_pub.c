#include <stdio.h>
#include "string.h"
#include "types.h"
#include "iot_api.h"
#include "config.h"
#include "uip-conf.h"
#include "iot_custom.h"

/******************************************************************************
* MODULE NAME:     iot_aes_pub.c
* PROJECT CODE:    __MT7681__
* DESCRIPTION:
* DESIGNER:
* DATE:            July 2014
*
* SOURCE CONTROL:
*
* LICENSE:
*     This source code is copyright (c) 2014 Mediatek Tech. Inc.
*     All rights reserved.
*
* REVISION     HISTORY:
*   V1.0.0     July 2014   - Initial Version V1.0
*
*
* SOURCE:
* ISSUES:
*    First Implementation.
* NOTES TO USERS:
*
******************************************************************************/

/************************************************/
/* Macros */
/************************************************/

/************************************************/
/* Extern  Paramenters */
/************************************************/
XIP_ATTRIBUTE(".xipsec0") uint8 PLAIN[256] = {
    /*  0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f    */
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7 ,0xab, 0x76, /* 0 */
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4 ,0x72, 0xc0, /* 1 */
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8 ,0x31, 0x15, /* 2 */
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27 ,0xb2, 0x75, /* 3 */
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3 ,0x2f, 0x84, /* 4 */
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c ,0x58, 0xcf, /* 5 */
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c ,0x9f, 0xa8, /* 6 */
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff ,0xf3, 0xd2, /* 7 */
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d ,0x19, 0x73, /* 8 */
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e ,0x0b, 0xdb, /* 9 */
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95 ,0xe4, 0x79, /* a */
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a ,0xae, 0x08, /* b */
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd ,0x8b, 0x8a, /* c */
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1 ,0x1d, 0x9e, /* d */
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55 ,0x28, 0xdf, /* e */
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54 ,0xbb, 0x16, /* f */
};

uint8 CIPTER[256];
uint8 PLAIN2[256];


extern void aes_cbc_complete_decrypt (
    IN  uint8 CipherText[],
    IN  uint32 CipherTextLength,
    IN  uint8 PaddingFlag,
    IN  AesCbc *Aes,
    OUT uint8 PlainText[],
    INOUT uint32 *PlainTextLength);

/************************************************/
/* Functions */
/************************************************/

/*========================================================================
    Routine    Description:
        aes_sample

    Arguments:
    Return Value:
========================================================================*/
void aes_sample(void)
{
    uint32 PlainLen=241, PlainLen2=256,  CipterLen=256;

    uint8 AES_CBC_Default_IV[16] = {0x56, 0x2e, 0x17, 0x99, 0x6d, 0x09, 0x3d, 0x28,
                                    0xdd, 0xb3, 0xba, 0x69, 0x5a, 0x2e, 0x6f, 0x58
                                   };

    uint8 KEY[16] = {32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47};
    uint8 INPUT[32] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
                       17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
                      };

    AesCbc AesTest;
#if 0  /*test case1 for one Block Encrypt/Decrypt*/
    printf_high("AES_CBC_Default_IV:");
    dump(AES_CBC_Default_IV, 16);

    RT_AES_Encrypt( AES_CBC_Default_IV,    16,
                    KEY,    16,
                    CIPTER,    &CipterLen);
    dump(CIPTER, CipterLen);

    RT_AES_Decrypt( CIPTER, 16,
                    KEY,    16,
                    PLAIN2, &PlainLen2);
    dump(PLAIN2, PlainLen2);
    memset(PLAIN2, 0, PlainLen2);
#endif

#if 0  /*test case1 for ECB*/
    /*CipterLen >= PlainLen,    CipterLen%AES_BLOCK_SIZES == 0 */
    aes_ecb_encry_test(PLAIN, &PlainLen, CIPTER, &CipterLen);
    printf_high("PlainLen=%d, CipterLen=%d\n", PlainLen,CipterLen);

    /*PlainLen == CipterLen,  (CipterLen or  PlainLen)%AES_BLOCK_SIZES == 0 */
    aes_ecb_decry_test(CIPTER, &CipterLen, PLAIN2, &PlainLen2);
    printf_high("PlainLen2=%d, CipterLen=%d\n", PlainLen2,CipterLen);

    dump(PLAIN,  PlainLen);
    dump(PLAIN2, PlainLen2);
    dump(CIPTER, CipterLen);
#endif

#if 1  /*test case1 for CBC*/
    printf_high("\n AES CBC \n");
    aes_cbc_encrypt(INPUT,   sizeof(INPUT),
                    KEY,     sizeof(KEY),
                    AES_CBC_Default_IV, sizeof(AES_CBC_Default_IV),
                    CIPTER,  &CipterLen);

    aes_cbc_decrypt(CIPTER,  CipterLen,
                    KEY,     sizeof(KEY),
                    AES_CBC_Default_IV, sizeof(AES_CBC_Default_IV),
                    PLAIN2,  &PlainLen2);

    printf_high("PlainLen2=%d, CipterLen=%d\n", PlainLen2,CipterLen);
    dump(INPUT, sizeof(INPUT));
    dump(CIPTER, CipterLen);
    dump(PLAIN2, PlainLen2);
#endif

#if 1   /*test case2 for CBC,  split cipher steam into many parts, and descrypt each part in-sequencely*/
    printf_high("\n AES CBC complete decrypt\n");

    /*generate cipher stream (include padding) according to the inputed plain text and AES KEY, IV*/
    /*Notice:  if PlainTextLength=16*n,   CipherTextLength=16*(n+1) 
                   if PlainTextLength=16*(n-1) + x (0<x<16),    CipherTextLength = 16*n */
    aes_cbc_encrypt(INPUT,   sizeof(INPUT),
                                 KEY,     sizeof(KEY),
                                 AES_CBC_Default_IV, sizeof(AES_CBC_Default_IV),
                                 CIPTER,  &CipterLen);
    dump(CIPTER, CipterLen);

    /*Step1: initial structure object of AesCbc,  includes the cipher stream length,
        CBC key length, CBC IV, CBC IV length,  and a buffer to store plain text*/
    memcpy(AesTest.key, KEY, 16);
    AesTest.keyLen = 16;
    memcpy(AesTest.iv, AES_CBC_Default_IV, 16);
    AesTest.ivLen = 16;
    AesTest.outBufSize = sizeof(PLAIN2);

    /*Step2: decrypt the ciphter stream in-sequencely*/
    aes_cbc_complete_decrypt(CIPTER,  16, 0, &AesTest, PLAIN2,  &PlainLen2);
    dump(PLAIN2, PlainLen2);
    aes_cbc_complete_decrypt(CIPTER+16,  16, 0, &AesTest, PLAIN2,  &PlainLen2);
    dump(PLAIN2, PlainLen2);
    aes_cbc_complete_decrypt(CIPTER+32,  16, 1, &AesTest, PLAIN2,  &PlainLen2);
    dump(PLAIN2, PlainLen2);
#endif

}


/*
========================================================================
Routine Description:
    AES (ECB) Decryption test

Arguments:
    pCipter       The cipher text
    pCipterLen  The length of cipher text in bytes
    pPlainLen    The length of allocated plain buffer in bytes

Return Value:
    pPlain        Return plain text
    pPlainLen    Return the length of plain text in bytes

Notice:
   PlainLen == CipterLen,  (CipterLen or  PlainLen)%AES_BLOCK_SIZES == 0
========================================================================
*/
void aes_ecb_decry_test(
    IN  puchar pCipter,
    IN  puint32 pCipterLen,
    OUT puchar pPlain,
    INOUT puint32 pPlainLen)
{
    uint32 index = 0;
    uint8  Key[AES_BLOCK_SIZES] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};
    uint32 iPlainBlkLen = AES_BLOCK_SIZES;

    printf_high("AES_DecryTest\n");

    /*
      * 1. Check the input parameters
      *    - CipherTextLength must be divided with no remainder by block
      */
    if ((*pCipterLen % AES_BLOCK_SIZES) != 0) {
        printf_high("aes_ecb_decry_test: cipher text length is %d bytes, it can't be divided with no remainder by block size(%d).\n",
                    *pCipterLen, AES_BLOCK_SIZES);
        return;
    }

    if (*pPlainLen != *pCipterLen)  {
        printf_high("aes_ecb_decry_test: cipher text length is %d bytes, should smae as .\n",
                    *pCipterLen, AES_BLOCK_SIZES);
        return;
    }

    /*
     * 2. Main algorithm
     *    - Cypher text divide into serveral blocks (16 bytes/block)
     *    - Execute RT_AES_Decrypt procedure.
     */
    for (index=0; index<(*pCipterLen/AES_BLOCK_SIZES); index++) {
        RT_AES_Decrypt(
            pCipter + (index*AES_BLOCK_SIZES),
            AES_BLOCK_SIZES,
            Key,
            AES_BLOCK_SIZES,
            pPlain + (index*AES_BLOCK_SIZES),
            &iPlainBlkLen);
    }
}


/*
========================================================================
Routine Description:
    AES (ECB) encryption test

Arguments:
    pPlain        The block of plain text
    pPlainLen   The length of plain text in bytes
    pCipterLen The length of allocated cipher buffer in bytes

Return Value:
    pCipter      Return cipher text
    pCipterLen Return the length of real used cipher buffer in bytes

Notice:
   CipterLen >= PlainLen,    CipterLen%AES_BLOCK_SIZES == 0
========================================================================
*/
void aes_ecb_encry_test(
    IN  puchar pPlain,
    IN  puint32 pPlainLen,
    OUT puchar pCipter,
    INOUT puint32 pCipterLen)
{
    uint32 index = 0;
    uint32 iBlockCount = 0;
    uint32 PaddingSize=0, PlainBlockEnd=0;

    uint8  Key[AES_BLOCK_SIZES] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};
    uint32 iCipterBlkLen = AES_BLOCK_SIZES;

    uint8  Block[AES_BLOCK_SIZES];
    uint32 BlockSize = 0;

    printf_high("aes_ecb_encry_test\n");

    if ((pPlain == NULL) || (pCipter == NULL))  {
        printf_high("aes_ecb_encry_test invalid data.\n");
        return;
    }

    if (*pPlainLen % ((uint32)AES_BLOCK_SIZES) > 0) {
        PaddingSize = ((uint32) AES_BLOCK_SIZES) - ( *pPlainLen % ((uint32)AES_BLOCK_SIZES));
        if (*pCipterLen < (*pPlainLen + PaddingSize)) {
            printf_high("aes_ecb_encry_test: cipher text length is %d bytes < (plain text length %d bytes + padding size %d bytes).\n",
                        *pCipterLen, *pPlainLen, PaddingSize);
            return;
        } /* End of if */
    }

    iBlockCount   = (*pPlainLen + AES_BLOCK_SIZES - 1)/AES_BLOCK_SIZES;
    printf_high("iBlockCount = %d \n",iBlockCount);

    for (index=0; index<iBlockCount; index++) {
        PlainBlockEnd += AES_BLOCK_SIZES;
        BlockSize = AES_BLOCK_SIZES;

        if (PlainBlockEnd > *pPlainLen)  {
            /*Set Padding value*/
            memset(Block, 0 , sizeof(Block));
            BlockSize = *pPlainLen%AES_BLOCK_SIZES;
        }
        memcpy(Block, pPlain+(index*AES_BLOCK_SIZES), BlockSize);

        //printf_high("index=%d  BlockSize=%d\n", index,BlockSize);
        //dump(Block, BlockSize);

        RT_AES_Encrypt(
            Block,
            AES_BLOCK_SIZES,
            Key,
            AES_BLOCK_SIZES,
            pCipter + (index*AES_BLOCK_SIZES),
            &iCipterBlkLen);
    }

    return;
}



/*
========================================================================
Routine Description:
    AES-CBC encryption

Arguments:
    PlainText        Plain text
    PlainTextLength  The length of plain text in bytes
    Key              Cipher key, it may be 16, 24, or 32 bytes (128, 192, or 256 bits)
    KeyLength        The length of cipher key in bytes
    IV               Initialization vector, it may be 16 bytes (128 bits)
    IVLength         The length of initialization vector in bytes
    CipherTextLength The length of allocated cipher text in bytes

Return Value:
    CipherText           Return cipher text
    CipherTextLength Return the length of real used cipher text in bytes   (including padding)
                               if PlainTextLength=16*n,   CipherTextLength=16*(n+1)
                               if PlainTextLength=16*(n-1) + x (0<x<16),    CipherTextLength = 16*n

Note:
    Reference to RFC 3602 and NIST 800-38A
========================================================================
*/
void aes_cbc_encrypt (
    IN  uint8 PlainText[],
    IN  uint32 PlainTextLength,
    IN  uint8 Key[],
    IN  uint32 KeyLength,
    IN  uint8 IV[],
    IN  uint32 IVLength,
    OUT uint8 CipherText[],
    INOUT  uint32 *CipherTextLength)
{
    uint32 PaddingSize, PlainBlockStart, CipherBlockStart, CipherBlockSize;
    uint32 Index;
    uint8 Block[AES_BLOCK_SIZES];
    uint32 LastBlockSize=0;

    /*
     * 1. Check the input parameters
     *    - CipherTextLength > (PlainTextLength + Padding size), Padding size = block size - (PlainTextLength % block size)
     *    - Key length must be 16, 24, or 32 bytes(128, 192, or 256 bits)
     *    - IV length must be 16 bytes(128 bits)
     */
    LastBlockSize = PlainTextLength % ((uint32)AES_BLOCK_SIZES);
    PaddingSize = ((uint32) AES_BLOCK_SIZES) - LastBlockSize;

    if (*CipherTextLength < (PlainTextLength + PaddingSize)) {
        printf_high("aes_cbc_encrypt: cipher text length is %d bytes < (plain text length %d bytes + padding size %d bytes).\n",
                    *CipherTextLength, PlainTextLength, PaddingSize);
        return;
    } /* End of if */
    if ((KeyLength != AES_KEY128_LENGTH) && (KeyLength != AES_KEY192_LENGTH) && (KeyLength != AES_KEY256_LENGTH)) {
        printf_high("aes_cbc_encrypt: key length is %d bytes, it must be %d, %d, or %d bytes(128, 192, or 256 bits).\n",
                    KeyLength, AES_KEY128_LENGTH, AES_KEY192_LENGTH, AES_KEY256_LENGTH);
        return;
    } /* End of if */
    if (IVLength != AES_CBC_IV_LENGTH) {
        printf_high("aes_cbc_encrypt: IV length is %d bytes, it must be %d bytes(128bits).\n",
                    IVLength, AES_CBC_IV_LENGTH);
        return;
    } /* End of if */


    /*
     * 2. Main algorithm
     *    - Plain text divide into serveral blocks (16 bytes/block)
     *    - If plain text is divided with no remainder by block, add a new block and padding size = block(16 bytes)
     *    - If plain text is not divided with no remainder by block, padding size = (block - remainder plain text)
     *    - Execute RT_AES_Encrypt procedure.
     *
     *    - Padding method: The remainder bytes will be filled with padding size (1 byte)
     */
    PlainBlockStart = 0;
    CipherBlockStart = 0;
    while ((PlainTextLength - PlainBlockStart) >= AES_BLOCK_SIZES) {
        if (CipherBlockStart == 0) {
            for (Index = 0; Index < AES_BLOCK_SIZES; Index++)
                Block[Index] = PlainText[PlainBlockStart + Index]^IV[Index];
        } else {
            for (Index = 0; Index < AES_BLOCK_SIZES; Index++)
                Block[Index] = PlainText[PlainBlockStart + Index]^CipherText[CipherBlockStart - ((uint32) AES_BLOCK_SIZES) + Index];
        } /* End of if */

        CipherBlockSize = *CipherTextLength - CipherBlockStart;
        RT_AES_Encrypt(
            Block,
            AES_BLOCK_SIZES ,
            Key,
            KeyLength,
            CipherText + CipherBlockStart,
            &CipherBlockSize);

        PlainBlockStart += ((uint32) AES_BLOCK_SIZES);
        CipherBlockStart += CipherBlockSize;
    } /* End of while */

    NdisMoveMemory(Block, (&PlainText[0] + PlainBlockStart), (PlainTextLength - PlainBlockStart));
    memset((Block + (((uint32) AES_BLOCK_SIZES) -PaddingSize)), (uint8) PaddingSize, PaddingSize);
    if (CipherBlockStart == 0) {
        for (Index = 0; Index < AES_BLOCK_SIZES; Index++)
            Block[Index] ^= IV[Index];
    } else {
        for (Index = 0; Index < AES_BLOCK_SIZES; Index++)
            Block[Index] ^= CipherText[CipherBlockStart - ((uint32) AES_BLOCK_SIZES) + Index];
    } /* End of if */
    CipherBlockSize = *CipherTextLength - CipherBlockStart;
    RT_AES_Encrypt(Block, AES_BLOCK_SIZES , Key, KeyLength, CipherText + CipherBlockStart, &CipherBlockSize);
    CipherBlockStart += CipherBlockSize;
    *CipherTextLength = CipherBlockStart;
} /* End of aes_cbc_encrypt */


/*
========================================================================
Routine Description:
    AES-CBC decryption

Arguments:
    CipherText       Cipher text
    CipherTextLength The length of cipher text in bytes
    Key              Cipher key, it may be 16, 24, or 32 bytes (128, 192, or 256 bits)
    KeyLength        The length of cipher key in bytes
    IV               Initialization vector, it may be 16 bytes (128 bits)
    IVLength         The length of initialization vector in bytes
    PlainTextLength  The length of allocated plain text in bytes

Return Value:
    PlainText        Return plain text
    PlainTextLength  Return the length of real used plain text in bytes

Note:
    Reference to RFC 3602 and NIST 800-38A
========================================================================
*/
void aes_cbc_decrypt (
    IN  uint8 CipherText[],
    IN  uint32 CipherTextLength,
    IN  uint8 Key[],
    IN  uint32 KeyLength,
    IN  uint8 IV[],
    IN  uint32 IVLength,
    OUT uint8 PlainText[],
    INOUT uint32 *PlainTextLength)
{
    uint32 PaddingSize, PlainBlockStart, CipherBlockStart, PlainBlockSize;
    uint32 Index;

    /*
     * 1. Check the input parameters
     *    - CipherTextLength must be divided with no remainder by block
     *    - Key length must be 16, 24, or 32 bytes(128, 192, or 256 bits)
     *    - IV length must be 16 bytes(128 bits)
     */
    if ((CipherTextLength % AES_BLOCK_SIZES) != 0) {
        printf("aes_cbc_decrypt: cipher text length is %d bytes, it can't be divided with no remainder by block size(%d).\n",
               CipherTextLength, AES_BLOCK_SIZES);
        return;
    } /* End of if */
    if ((KeyLength != AES_KEY128_LENGTH) && (KeyLength != AES_KEY192_LENGTH) && (KeyLength != AES_KEY256_LENGTH)) {
        printf("aes_cbc_decrypt: key length is %d bytes, it must be %d, %d, or %d bytes(128, 192, or 256 bits).\n",
               KeyLength, AES_KEY128_LENGTH, AES_KEY192_LENGTH, AES_KEY256_LENGTH);
        return;
    } /* End of if */
    if (IVLength != AES_CBC_IV_LENGTH) {
        printf("aes_cbc_decrypt: IV length is %d bytes, it must be %d bytes(128bits).\n",
               IVLength, AES_CBC_IV_LENGTH);
        return;
    } /* End of if */


    /*
     * 2. Main algorithm
     *    - Cypher text divide into serveral blocks (16 bytes/block)
     *    - Execute RT_AES_Decrypt procedure.
     *    - Remove padding bytes, padding size is the last byte of plain text
     */
    CipherBlockStart = 0;
    PlainBlockStart = 0;
    while ((CipherTextLength - CipherBlockStart) >= AES_BLOCK_SIZES) {
        PlainBlockSize = *PlainTextLength - PlainBlockStart;
        RT_AES_Decrypt(CipherText + CipherBlockStart,
                       AES_BLOCK_SIZES ,
                       Key,
                       KeyLength,
                       PlainText + PlainBlockStart,
                       &PlainBlockSize);

        if (PlainBlockStart == 0) {
            for (Index = 0; Index < AES_BLOCK_SIZES; Index++)
                PlainText[PlainBlockStart + Index] ^= IV[Index];
        } else {
            for (Index = 0; Index < AES_BLOCK_SIZES; Index++)
                PlainText[PlainBlockStart + Index] ^= CipherText[CipherBlockStart + Index - ((uint32) AES_BLOCK_SIZES)];
        } /* End of if */

        CipherBlockStart += AES_BLOCK_SIZES;
        PlainBlockStart += PlainBlockSize;
    } /* End of while */

    PaddingSize = (uint8) PlainText[PlainBlockStart -1];
    *PlainTextLength = PlainBlockStart - PaddingSize;	
} /* End of aes_cbc_encrypt */

/*
========================================================================
Routine Description:
    AES-CBC decryption

Arguments:
    CipherText          Cipher text
    CipherTextLength The length of cipher text in bytes
    PaddingFlag        0:Cipher text no padding, 1:Cipher text include padding
    Aes                    include Cipher key, keyLen, iv, ivLen, outBufSize. 
    PlainText            Plain text
    PlainTextLength  The length of allocated plain text in bytes

Return Value:
    PlainText        Return plain text
    PlainTextLength  Return the length of real used plain text in bytes

Note:
    Reference to RFC 3602 and NIST 800-38A
========================================================================
*/
void aes_cbc_complete_decrypt (
    IN  uint8 CipherText[],
    IN  uint32 CipherTextLength,
    IN  uint8 PaddingFlag,
    IN  AesCbc *Aes,
    OUT uint8 PlainText[],
    INOUT uint32 *PlainTextLength)
{
    uint32 PaddingSize, PlainBlockStart, CipherBlockStart, PlainBlockSize;
    uint32 Index;

    /*
     * 1. Check the input parameters
     *    - CipherTextLength must be divided with no remainder by block
     *    - Key length must be 16, 24, or 32 bytes(128, 192, or 256 bits)
     *    - IV length must be 16 bytes(128 bits)
     */
    if ((CipherTextLength % AES_BLOCK_SIZES) != 0) {
        printf("aes_cbc_decrypt: cipher text length is %d bytes, it can't be divided with no remainder by block size(%d).\n",
               CipherTextLength, AES_BLOCK_SIZES);
        return;
    } /* End of if */
    if ((Aes->keyLen != AES_KEY128_LENGTH) && (Aes->keyLen != AES_KEY192_LENGTH) && (Aes->keyLen != AES_KEY256_LENGTH)) {
        printf("aes_cbc_decrypt: key length is %d bytes, it must be %d, %d, or %d bytes(128, 192, or 256 bits).\n",
               Aes->keyLen, AES_KEY128_LENGTH, AES_KEY192_LENGTH, AES_KEY256_LENGTH);
        return;
    } /* End of if */
    if (Aes->ivLen != AES_CBC_IV_LENGTH) {
        printf("aes_cbc_decrypt: IV length is %d bytes, it must be %d bytes(128bits).\n",
               Aes->ivLen, AES_CBC_IV_LENGTH);
        return;
    } /* End of if */

    /*
     * 2. Main algorithm
     *    - Cypher text divide into serveral blocks (16 bytes/block)
     *    - Execute RT_AES_Decrypt procedure.
     *    - Remove padding bytes, padding size is the last byte of plain text
     */
    CipherBlockStart = 0;
    PlainBlockStart = 0;
    *PlainTextLength = Aes->outBufSize;//init *PlainTextLength
    if(*PlainTextLength < CipherTextLength)
        *PlainTextLength = CipherTextLength;

    while ((CipherTextLength - CipherBlockStart) >= AES_BLOCK_SIZES) {
        PlainBlockSize = *PlainTextLength - PlainBlockStart;
        RT_AES_Decrypt(CipherText + CipherBlockStart,
                       AES_BLOCK_SIZES ,
                       Aes->key,
                       Aes->keyLen,
                       PlainText + PlainBlockStart,
                       &PlainBlockSize);

        if (PlainBlockStart == 0) {
            for (Index = 0; Index < AES_BLOCK_SIZES; Index++)
                PlainText[PlainBlockStart + Index] ^= Aes->iv[Index];
        } else {
            for (Index = 0; Index < AES_BLOCK_SIZES; Index++)
                PlainText[PlainBlockStart + Index] ^= CipherText[CipherBlockStart + Index - ((uint32) AES_BLOCK_SIZES)];
        } /* End of if */

        CipherBlockStart += AES_BLOCK_SIZES;
        PlainBlockStart += PlainBlockSize;
    } /* End of while */

    if(!PaddingFlag){
        PaddingSize = 0;
        memcpy(Aes->iv, CipherText+(CipherTextLength-AES_BLOCK_SIZES), AES_BLOCK_SIZES);
    }else{
        PaddingSize = (uint8) PlainText[PlainBlockStart -1];
    }
    *PlainTextLength = PlainBlockStart - PaddingSize;
} /* End of aes_cbc_complete_encrypt */


