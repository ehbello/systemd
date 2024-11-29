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
 * Copyright (C) 2012-2013 Kay Sievers <kay@vrfy.org>
 * Copyright (C) 2012 Harald Hoyer <harald@redhat.com>
 */

#include <efi.h>
#include <efilib.h>

#include "console.h"
#include "util.h"

EFI_STATUS console_key_read(UINT64 *key, BOOLEAN wait) {
        EFI_GUID EfiSimpleTextInputExProtocolGuid = EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID;
        static EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *TextInputEx;
        static BOOLEAN checked;
        UINTN index;
        EFI_INPUT_KEY k;
        EFI_STATUS err;

        if (!checked) {
                err = LibLocateProtocol(&EfiSimpleTextInputExProtocolGuid, (VOID **)&TextInputEx);
                if (EFI_ERROR(err))
                        TextInputEx = NULL;

                checked = TRUE;
        }

        /* wait until key is pressed */
        if (wait)
                uefi_call_wrapper(BS->WaitForEvent, 3, 1, &ST->ConIn->WaitForKey, &index);

        if (TextInputEx) {
                EFI_KEY_DATA keydata;
                UINT64 keypress;

                err = uefi_call_wrapper(TextInputEx->ReadKeyStrokeEx, 2, TextInputEx, &keydata);
                if (!EFI_ERROR(err)) {
                        UINT32 shift = 0;

                        /* do not distinguish between left and right keys */
                        if (keydata.KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) {
                                if (keydata.KeyState.KeyShiftState & (EFI_RIGHT_CONTROL_PRESSED|EFI_LEFT_CONTROL_PRESSED))
                                        shift |= EFI_CONTROL_PRESSED;
                                if (keydata.KeyState.KeyShiftState & (EFI_RIGHT_ALT_PRESSED|EFI_LEFT_ALT_PRESSED))
                                        shift |= EFI_ALT_PRESSED;
                        };

                        /* 32 bit modifier keys + 16 bit scan code + 16 bit unicode */
                        keypress = KEYPRESS(shift, keydata.Key.ScanCode, keydata.Key.UnicodeChar);
                        if (keypress > 0) {
                                *key = keypress;
                                return 0;
                        }
                }
        }

        /* fallback for firmware which does not support SimpleTextInputExProtocol
         *
         * This is also called in case ReadKeyStrokeEx did not return a key, because
         * some broken firmwares offer SimpleTextInputExProtocol, but never acually
         * handle any key. */
        err  = uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &k);
        if (EFI_ERROR(err))
                return err;

        *key = KEYPRESS(0, k.ScanCode, k.UnicodeChar);
        return 0;
}
