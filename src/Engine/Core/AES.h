/****************************************************************************
**
** Copyright 2019 neuromore co
** Contact: https://neuromore.com/contact
**
** Commercial License Usage
** Licensees holding valid commercial neuromore licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and neuromore. For licensing terms
** and conditions see https://neuromore.com/licensing. For further
** information use the contact form at https://neuromore.com/contact.
**
** neuromore Public License Usage
** Alternatively, this file may be used under the terms of the neuromore
** Public License version 1 as published by neuromore co with exceptions as 
** appearing in the file neuromore-class-exception.md included in the 
** packaging of this file. Please review the following information to 
** ensure the neuromore Public License requirements will be met: 
** https://neuromore.com/npl
**
****************************************************************************/

#ifndef __CORE_AES_H
#define __CORE_AES_H

#include "ByteArray.h"


namespace Core
{

#define N_ROW			4
#define N_COL			4
#define N_BLOCK			(N_ROW * N_COL)
#define N_MAX_ROUNDS	14

typedef int8 aes_result;

/*
 * AES-128 encryption in CBC-mode and pkcs#7 padding.
 */
class ENGINE_API AES
{
	public:
		// constructor & destructor
		AES();
		~AES();

		ByteArray Encrypt(ByteArray inputData, ByteArray key);
		ByteArray Encrypt(ByteArray inputData, ByteArray key, ByteArray iv);

		ByteArray Decrypt(ByteArray inputData, ByteArray key);
		ByteArray Decrypt(ByteArray inputData, ByteArray key, ByteArray iv);

	private:
		// padding helpers
		void RemovePadding(ByteArray* inputData);
		void AddPadding(ByteArray* inputData);

		typedef struct
		{
			uint8 ksch[(N_MAX_ROUNDS + 1) * N_BLOCK];
			uint8 rnd;
		} aes_context;

		// encryption functions
		aes_result aes_set_key(const unsigned char key[], int keylen, aes_context ctx[1]);
		aes_result aes_encrypt(const unsigned char in[N_BLOCK], unsigned char out[N_BLOCK], const aes_context ctx[1]);
		aes_result aes_decrypt(const unsigned char in[N_BLOCK], unsigned char out[N_BLOCK], const aes_context ctx[1]);
		aes_result aes_cbc_encrypt(const unsigned char *in, unsigned char *out, unsigned long size, unsigned char iv[N_BLOCK], const aes_context ctx[1]);
		aes_result aes_cbc_decrypt(const unsigned char *in, unsigned char *out, unsigned long size, unsigned char iv[N_BLOCK], const aes_context ctx[1]);

		// helper functions
		void xor_block(void *d, const void *s);
		void copy_and_key(void *d, const void *s, const void *k);
		void add_round_key(uint8 d[N_BLOCK], const uint8 k[N_BLOCK]);
		void shift_sub_rows(uint8 st[N_BLOCK]);
		void inv_shift_sub_rows(uint8 st[N_BLOCK]);
		void mix_sub_columns(uint8 dt[N_BLOCK]);
		void inv_mix_sub_columns(uint8 dt[N_BLOCK]);
};

}; // namespace Core


#endif
