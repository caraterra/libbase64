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

#ifndef BASE64_H
#define BASE64_H

#include <stdbool.h>
#include <stdlib.h>

struct base64_standard {
	char *alphabet;
	bool discard;
	char pad;
};

extern const struct base64_standard RFC_2152;
extern const struct base64_standard RFC_3501;
extern const struct base64_standard RFC_4648_4;
extern const struct base64_standard RFC_4648_5;

char *base64_encode_standard(const void *, size_t, const struct base64_standard);
void *base64_decode_standard(const char *, size_t *, const struct base64_standard);

char *base64_encode(const void *, size_t);
void *base64_decode(const char *, size_t *);

#endif
