/*
 * libbase64 is a tiny Base64 encoding library for C.
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

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

const char ENCODE_TABLE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const char DECODE_TABLE[] = {
	['+'] = 62,
	['/'] = 63,
	['0'] = 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
	['A'] = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	['a'] = 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};

bool base64_valid_char(int c)
{
	if (isalnum(c) || (c == '+' || c == '/' || c == '=')) {
		return true;
	}
	return false;
}

void base64_encode_step(const void *src, size_t src_size, char *dest, bool pad)
{
	char pad_char = (pad) ? '=' : '\0';

	uint32_t octet_triad = 0;
	const unsigned char *src_arr = src;
	octet_triad |= ((0 < src_size) ? src_arr[0] : 0) << 16;
	octet_triad |= ((1 < src_size) ? src_arr[1] : 0) << 8;
	octet_triad |= (2 < src_size) ? src_arr[2] : 0;

	dest[0] = (src_size > 0) ? ENCODE_TABLE[(octet_triad & (0x3F << 18)) >> 18] : pad_char;
	dest[1] = (src_size > 0) ? ENCODE_TABLE[(octet_triad & (0x3F << 12)) >> 12] : pad_char;
	dest[2] = (src_size > 1) ? ENCODE_TABLE[(octet_triad & (0x3F << 6)) >> 6] : pad_char;
	dest[3] = (src_size > 2) ? ENCODE_TABLE[octet_triad & 0x3F] : pad_char; // 0x3F = 0b00111111
}

size_t base64_decode_step(const char *src, size_t src_size, void *dest)
{
	size_t dest_size = 0;

	uint32_t sextet_tetrad = 0;
	sextet_tetrad |= ((0 < src_size) ? DECODE_TABLE[(uint8_t)src[dest_size]] : 0) << 18;
	sextet_tetrad |= ((1 < src_size && src[1] != '=') ? DECODE_TABLE[(uint8_t)src[++dest_size]] : 0) << 12;
	sextet_tetrad |= ((2 < src_size && src[2] != '=') ? DECODE_TABLE[(uint8_t)src[++dest_size]] : 0) << 6;
	sextet_tetrad |= ((3 < src_size && src[3] != '=') ? DECODE_TABLE[(uint8_t)src[++dest_size]] : 0);

	unsigned char *dest_arr = dest;
	dest_arr[0] = (sextet_tetrad & (0xFF << 16)) >> 16;
	dest_arr[1] = (sextet_tetrad & (0xFF << 8)) >> 8;
	dest_arr[2] = sextet_tetrad & 0xFF;

	return dest_size;
}

char *base64_encode(const void *src, size_t src_size, bool pad)
{
	if (!src) {
		return NULL;
	}

	const unsigned char *src_str = src;

	const size_t dest_str_size = 4 * ((src_size + 2) / 3) + 1;
	char *const dest_str = malloc(dest_str_size);
	dest_str[0] = 0;

	char buf[BASE64_SEXTET_ARRAY_SIZE];

	for (size_t i = 0; i < src_size; i += BASE64_OCTET_ARRAY_SIZE) {
		base64_encode_step(src_str + i, MIN(src_size - i, BASE64_OCTET_ARRAY_SIZE), buf, pad);
		strncat(dest_str, buf, BASE64_SEXTET_ARRAY_SIZE);
	}

	return dest_str;
}

void *base64_decode(const char *src, size_t *dest_size, bool strict)
{
	if (!src) {
		return NULL;
	}

	size_t dest_size_tmp = 0;
	size_t src_size = strlen(src);
	char *dest = malloc((src_size * BASE64_OCTET_ARRAY_SIZE) / BASE64_SEXTET_ARRAY_SIZE);

	char src_buf[BASE64_SEXTET_ARRAY_SIZE];
	size_t src_buf_size = 0;
	for (size_t i = 0; i < src_size; ++i) {
		if (base64_valid_char(src[i])) {
			src_buf[src_buf_size] = src[i];
			++src_buf_size;
		} else if (strict) {
			free(dest);
			return NULL;
		}

		if (src_buf_size == BASE64_SEXTET_ARRAY_SIZE) {
			size_t dest_buf_size = base64_decode_step(src_buf, src_buf_size, dest + dest_size_tmp);
			dest_size_tmp += dest_buf_size;
			src_buf_size = 0;
		}
	}

	if (src_buf_size) {
		size_t dest_buf_size = base64_decode_step(src_buf, src_buf_size, dest + dest_size_tmp);
		dest_size_tmp += dest_buf_size;
	}

	if (dest_size) {
		*dest_size = dest_size_tmp;
	}

	return dest;
}
