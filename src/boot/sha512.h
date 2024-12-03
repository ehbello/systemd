/* SPDX-License-Identifier: LGPL-2.1+ */
/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * Copyright (C) 2017 kitsunyan <kitsunyan@inbox.ru>
 */

#ifndef __SDBOOT_SHA512_H
#define __SDBOOT_SHA512_H

#include <efi.h>

typedef struct {
        uint64_t h[8], w[16];
        size_t size, total;
} Sha512Context;

void sha512_create(Sha512Context *context);
void sha512_update(Sha512Context *context, void *data, size_t length);
void sha512_finish(Sha512Context *context, uint8_t *output);

void sha512_compute(void *data, size_t length, uint8_t *output);

#endif
