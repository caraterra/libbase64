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

#ifndef BASE64_H
#define BASE64_H

#include <stdbool.h>
#include <stdlib.h>

#define BASE64_OCTET_ARRAY_SIZE 3
#define BASE64_SEXTET_ARRAY_SIZE 4

bool base64_valid_char(int);

void base64_encode_step(const void *, size_t, char *, bool);
size_t base64_decode_step(const char *, size_t, void *);

char *base64_encode(const void *, size_t, bool);
void *base64_decode(const char *, size_t *, bool);

#endif
