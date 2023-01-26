/*
 * libbase64 is a Base64 encoding library for C.
 * Copyright (C) 2022 Alexander J. Carter
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "base64.h"

#define BASE64_OCTET_ARRAY_SIZE 3
#define BASE64_SEXTET_ARRAY_SIZE 4
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

const struct base64_standard RFC_2152 = {
	.alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
	.discard = false,
	.pad = '\0'
};

const struct base64_standard RFC_3501 = {
	.alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+,",
	.discard = false,
	.pad = '\0'
};

const struct base64_standard RFC_4648_4 = {
	.alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
	.discard = false,
	.pad = '='
};

const struct base64_standard RFC_4648_5 = {
	.alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_",
	.discard = false,
	.pad = '='
};

static int8_t *build_decode_table(const char *alphabet)
{
	if (strlen(alphabet) < 64) {
		return NULL;
	}

	unsigned char min_char = alphabet[0];
	unsigned char max_char = alphabet[0];

	for (size_t i = 1; i < strlen(alphabet); ++i) {
		min_char = (alphabet[i] < min_char) ? alphabet[i] : min_char;
		max_char = (alphabet[i] > max_char) ? alphabet[i] : max_char;
	}

	size_t table_size = max_char - min_char + 1;
	int8_t *table = malloc(table_size);

	if (table == NULL) {
		return NULL;
	}

	for (size_t i = 0; i < table_size; ++i) {
		table[i] = -1;
	}

	for (size_t i = 0; i < 64; ++i) {
		table[(uint8_t)alphabet[i]] = i;
	}

	return table;
}

static bool base64_valid_char(int c, const int8_t *decode_table)
{
	return (decode_table[c] != -1);
}

static void base64_encode_step(const void *src, size_t src_size, char *dest, const struct base64_standard standard)
{
	uint32_t octet_triad = 0;
	const unsigned char *src_arr = src;

	octet_triad |= src_arr[0] << 16;
	octet_triad |= src_arr[1] << 8;
	octet_triad |= src_arr[2];

	/* Mask four sextets out of the three-byte number and use them to index into
	 * the encoding table, producing an encoded string of length four.
	 *
	 * 0x3F = 0b00111111
	 */
	dest[0] = (src_size > 0) ? standard.alphabet[(octet_triad & (0x3F << 18)) >> 18] : standard.pad;
	dest[1] = (src_size > 0) ? standard.alphabet[(octet_triad & (0x3F << 12)) >> 12] : standard.pad;
	dest[2] = (src_size > 1) ? standard.alphabet[(octet_triad & (0x3F << 6)) >> 6] : standard.pad;
	dest[3] = (src_size > 2) ? standard.alphabet[octet_triad & 0x3F] : standard.pad;
}

static size_t base64_decode_step(const char *src, size_t src_size, void *dest, const int8_t *decode_table)
{
	/* Take the six least significant bits in each byte in src, and mask it into
	 * sextet tetrad to create a three byte number. The most significant byte is zero,
	 * and is unneeded.
	 */
	size_t dest_size = 0;
	uint32_t sextet_tetrad = 0;

	sextet_tetrad |= ((0 < src_size) ? decode_table[(uint8_t)src[dest_size++]] : 0) << 18;
	sextet_tetrad |= ((1 < src_size) ? decode_table[(uint8_t)src[dest_size++]] : 0) << 12;
	sextet_tetrad |= ((2 < src_size) ? decode_table[(uint8_t)src[dest_size++]] : 0) << 6;
	sextet_tetrad |= ((3 < src_size) ? decode_table[(uint8_t)src[dest_size]] : 0);

	/* Take the three bytes in sextet_tetrad, and map it into dest_arr. */
	unsigned char *dest_arr = dest;
	dest_arr[0] = (sextet_tetrad & (0xFF << 16)) >> 16;
	dest_arr[1] = (sextet_tetrad & (0xFF << 8)) >> 8;
	dest_arr[2] = sextet_tetrad & 0xFF;

	return dest_size;
}

char *base64_encode_standard(const void *src, size_t src_size, const struct base64_standard standard)
{
	if (!src) {
		return NULL;
	}

	const size_t dest_str_size = BASE64_SEXTET_ARRAY_SIZE * ((src_size + 2) / BASE64_OCTET_ARRAY_SIZE) + 1;
	char *const dest_str = calloc(dest_str_size, sizeof(char));

	if (dest_str == NULL) {
		return NULL;
	}

	char buf[BASE64_SEXTET_ARRAY_SIZE];

	for (size_t i = 0; i < src_size; i += BASE64_OCTET_ARRAY_SIZE) {
		base64_encode_step((uint8_t *)src + i, MIN(src_size - i, BASE64_OCTET_ARRAY_SIZE), buf, standard);
		strncat(dest_str, buf, BASE64_SEXTET_ARRAY_SIZE);
	}

	return dest_str;
}

void *base64_decode_standard(const char *src, size_t *dest_size, const struct base64_standard standard)
{
	if (!src) {
		return NULL;
	}

	size_t dest_size_tmp = 0;
	size_t src_size = strlen(src);
	char *dest = malloc((src_size * BASE64_OCTET_ARRAY_SIZE) / BASE64_SEXTET_ARRAY_SIZE);

	if (dest == NULL) {
		return NULL;
	}

	char src_buf[BASE64_SEXTET_ARRAY_SIZE];
	size_t src_buf_size = 0;

	int8_t *decode_table = build_decode_table(standard.alphabet);

	if (decode_table == NULL) {
		free(dest);
		return NULL;
	}

	for (size_t i = 0; i < src_size; ++i) {
		if (base64_valid_char(src[i], decode_table)) {
			src_buf[src_buf_size] = src[i];
			++src_buf_size;
		} else if (!standard.discard && src[i] != standard.pad) {
			free(dest);
			free(decode_table);
			return NULL;
		}

		if (src_buf_size == BASE64_SEXTET_ARRAY_SIZE) {
			size_t dest_buf_size = base64_decode_step(src_buf, src_buf_size, dest + dest_size_tmp, decode_table);
			dest_size_tmp += dest_buf_size;
			src_buf_size = 0;
		}
	}

	if (src_buf_size) {
		size_t dest_buf_size = base64_decode_step(src_buf, src_buf_size, dest + dest_size_tmp, decode_table);
		dest_size_tmp += dest_buf_size - 1;
	}

	if (dest_size) {
		*dest_size = dest_size_tmp;
	}

	free(decode_table);

	return dest;
}

char *base64_encode(const void *src, size_t src_size)
{
	return base64_encode_standard(src, src_size, RFC_4648_4);
}

void *base64_decode(const char *src, size_t *dest_size)
{
	return base64_decode_standard(src, dest_size, RFC_4648_4);
}
