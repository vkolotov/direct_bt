/**
 * Author: Vlad Kolotoff
 * Copyright (c) 2026 Vlad Kolotoff
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
package org.direct_bt;

/**
 * Validation policy for the GATT layout seed cache, settable per device.
 * <p>
 * The seed cache is session-scoped (process lifetime) and keyed by device address:
 * a completed GATT discovery captures a passive handle/UUID table, and subsequent
 * connections rebuild the GATT layout from it instead of a full discovery walk.
 * The mode governs how the cached layout is validated before use.
 * </p>
 * <p>
 * See {@link #get(byte)} for its native integer mapping.
 * </p>
 * @see BTDevice#setGattCacheMode(GattCacheMode)
 * @since 3.3.5
 */
public enum GattCacheMode {
    /** Never apply nor store the seed cache: full discovery on every connection, value 0. */
    OFF   ((byte)0),
    /**
     * Validate the seed before use (default): Database Hash (0x2B2A) in one round trip when the
     * peer exposes it, otherwise a service/characteristic declaration re-read and compare, value 1.
     */
    AUTO  ((byte)1),
    /**
     * Apply the seed without any validation round trips, value 2. Unsafe across peer GATT layout
     * changes: a changed layout can reuse handle numbers, silently addressing the wrong attributes.
     */
    TRUST ((byte)2);

    public final byte value;

    /**
     * Maps the specified name to a constant of GattCacheMode.
     * <p>
     * Implementation simply returns {@link #valueOf(String)}.
     * This maps the constant names itself to their respective constant.
     * </p>
     * @param name the string name to be mapped to a constant of this enum type.
     * @return the corresponding constant of this enum type.
     * @throws IllegalArgumentException if the specified name can't be mapped to a constant of this enum type
     *                                  as described above.
     */
    public static GattCacheMode get(final String name) throws IllegalArgumentException {
        return valueOf(name);
    }

    /**
     * Maps the specified integer value to a constant of {@link GattCacheMode}.
     * @param value the integer value to be mapped to a constant of this enum type.
     * @return the corresponding constant of this enum type, using {@link #AUTO} if not supported.
     */
    public static GattCacheMode get(final byte value) {
        switch(value) {
            case (byte)0x00: return OFF;
            case (byte)0x02: return TRUST;
            default: return AUTO;
        }
    }

    GattCacheMode(final byte v) {
        value = v;
    }
}
