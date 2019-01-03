/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
 * Copyright (c) 2018 Artur Pacholec
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string.h>

#include "py/objproperty.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/bleio/UUID.h"

//| .. currentmodule:: bleio
//|
//| :class:`UUID` -- BLE UUID
//| =========================================================
//|
//| A 16-bit or 128-bit UUID. Can be used for services, characteristics, descriptors and more.
//|

//| .. class:: UUID(value)
//|
//|   Create a new UUID or UUID object encapsulating the uuid value.
//|   The value can be one of:
//|
//|   - an `int` value in range 0 to 0xFFFF (Bluetooth SIG 16-bit UUID)
//|   - a buffer object (bytearray, bytes)  of 16 bytes in little-endian order (128-bit UUID)
//|
//|   :param int/buffer value: The uuid value to encapsulate

STATIC mp_obj_t bleio_uuid_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    bleio_uuid_obj_t *self = m_new_obj(bleio_uuid_obj_t);
    self->base.type = type;

    const mp_obj_t value = pos_args[0];
    uint8_t uuid128[16];

    if (MP_OBJ_IS_INT(value)) {
        mp_int_t uuid16 = mp_obj_get_int(value);
        if (uuid16 < 0 || uuid16 > 0xffff) {
            mp_raise_ValueError(translate("UUID integer value not in range 0 to 0xffff"));
        }

        // NULL means no 128-bit value.
        common_hal_bleio_uuid_construct(self, uuid16, NULL);

    } else {
        mp_buffer_info_t bufinfo;
        if (!mp_get_buffer(value, &bufinfo, MP_BUFFER_READ)) {
            mp_raise_ValueError(translate("UUID value is not int or byte buffer"));
        }

        if (bufinfo.len != 16) {
            mp_raise_ValueError(translate("Byte buffer must be 16 bytes."));
        }

        memcpy(uuid128, bufinfo.buf, 16);
        uint32_t uuid16 = (uuid128[13] << 8) | uuid128[12];
        uuid128[12] = 0;
        uuid128[13] = 0;
        common_hal_bleio_uuid_construct(self, uuid16, bufinfo.buf);
    }

    return MP_OBJ_FROM_PTR(self);
}

//|   .. attribute:: uuid16
//|
//|     The 16-bit part of the UUID. (read-only)
//|
STATIC mp_obj_t bleio_uuid_get_uuid16(mp_obj_t self_in) {
    bleio_uuid_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_bleio_uuid_get_uuid16(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(bleio_uuid_get_uuid16_obj, bleio_uuid_get_uuid16);

const mp_obj_property_t bleio_uuid_uuid16_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&bleio_uuid_get_uuid16_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: uuid128
//|
//|     The 128-bit value of the UUID, return as a bytes().
//|     Throws AttributeError if this is a 16-bit UUID. (read-only)
//|
STATIC mp_obj_t bleio_uuid_get_uuid128(mp_obj_t self_in) {
    bleio_uuid_obj_t *self = MP_OBJ_TO_PTR(self_in);

    uint8_t uuid128[16];
    if (!common_hal_bleio_uuid_get_uuid128(self, uuid128)) {
        mp_raise_AttributeError(translate("not a 128-bit UUID"));
    }
    return mp_obj_new_bytes(uuid128, 16);
}

MP_DEFINE_CONST_FUN_OBJ_1(bleio_uuid_get_uuid128_obj, bleio_uuid_get_uuid128);

const mp_obj_property_t bleio_uuid_uuid128_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&bleio_uuid_get_uuid128_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: uuid128_reference
//|
//|     An opaque reference representing the 128-bit UUID. (read-only)
//|     Returns None if this is a 16-bit UUID.
//|
STATIC mp_obj_t bleio_uuid_get_uuid128_reference(mp_obj_t self_in) {
    bleio_uuid_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t reference = common_hal_bleio_uuid_get_uuid128_reference(self);
    return reference == 0 ? mp_const_none : MP_OBJ_NEW_SMALL_INT(reference);
}

MP_DEFINE_CONST_FUN_OBJ_1(bleio_uuid_get_uuid128_reference_obj, bleio_uuid_get_uuid128_reference);

const mp_obj_property_t bleio_uuid_uuid128_reference_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&bleio_uuid_get_uuid128_reference_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: size
//|
//|     Returns 128 if this UUID represents a 128-bit vendor-specific UUID.
//|     Returns 16 if this UUID represents a 16-bit Bluetooth SIG assigned UUID. (read-only)
//|     32-bit UUIDs are not currently supported.
//|
STATIC mp_obj_t bleio_uuid_get_size(mp_obj_t self_in) {
    bleio_uuid_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_bleio_uuid_get_size(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(bleio_uuid_get_size_obj, bleio_uuid_get_size);

const mp_obj_property_t bleio_uuid_size_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&bleio_uuid_get_size_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t bleio_uuid_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_uuid16), MP_ROM_PTR(&bleio_uuid_uuid16_obj) },
    { MP_ROM_QSTR(MP_QSTR_uuid128), MP_ROM_PTR(&bleio_uuid_uuid128_obj) },
    { MP_ROM_QSTR(MP_QSTR_uuid128_reference), MP_ROM_PTR(&bleio_uuid_uuid128_reference_obj) },
    { MP_ROM_QSTR(MP_QSTR_size), MP_ROM_PTR(&bleio_uuid_size_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_uuid_locals_dict, bleio_uuid_locals_dict_table);

STATIC mp_obj_t bleio_uuid_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    bleio_uuid_obj_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
    case MP_UNARY_OP_HASH:
        if (common_hal_bleio_uuid_get_size(self) == 16) {
            return MP_OBJ_NEW_SMALL_INT(common_hal_bleio_uuid_get_uuid16(self));
        } else {
            union {
                uint8_t uuid128_bytes[16];
                uint16_t uuid128_uint16[8];
            } uuid128;
            common_hal_bleio_uuid_get_uuid128(self, uuid128.uuid128_bytes);
            int hash = 0;
            for (size_t i = 0; i < MP_ARRAY_SIZE(uuid128.uuid128_uint16); i++) {
                hash += uuid128.uuid128_uint16[i];
            }
            return MP_OBJ_NEW_SMALL_INT(hash);
        }
    default:
        return MP_OBJ_NULL; // op not supported
    }
}

STATIC mp_obj_t bleio_uuid_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    switch (op) {
        // Two UUID's are equal if their uuid16 values and uuid128 references match.
        case MP_BINARY_OP_EQUAL:
            if (MP_OBJ_IS_TYPE(rhs_in, &bleio_uuid_type)) {
                return mp_obj_new_bool(
                    common_hal_bleio_uuid_get_uuid16(lhs_in) == common_hal_bleio_uuid_get_uuid16(rhs_in) &&
                    common_hal_bleio_uuid_get_uuid128_reference(lhs_in) ==
                    common_hal_bleio_uuid_get_uuid128_reference(rhs_in));
            } else {
                return mp_const_false;
            }

        default:
            return MP_OBJ_NULL; // op not supported
    }
}

const mp_obj_type_t bleio_uuid_type = {
    { &mp_type_type },
    .name = MP_QSTR_UUID,
    .make_new = bleio_uuid_make_new,
    .unary_op = bleio_uuid_unary_op,
    .binary_op = bleio_uuid_binary_op,
    .locals_dict = (mp_obj_dict_t*)&bleio_uuid_locals_dict,
};
