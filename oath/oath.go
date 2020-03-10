/*
 * Pretty command-line password manager.
 *
 * Copyright (c) 2014, Alessandro Ghedini
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package oath

import "encoding/base32"
import "encoding/binary"
import "crypto/hmac"
import "crypto/sha1"
import "fmt"
import "math"
import "strings"
import "time"

func Base32Secret(secret string) ([]byte, error) {
    key := strings.ToUpper(secret)

    bsecret, err := base32.StdEncoding.DecodeString(key)
    if err != nil {
        return nil, fmt.Errorf("Could not decode key: %s", err)
    }

    return bsecret, nil
}

func HOTP(secret []byte, count int64) int {
    mac := hmac.New(sha1.New, secret)

    binary.Write(mac, binary.BigEndian, count)

    sum   := mac.Sum(nil)
    off   := sum[len(sum)-1] & 0xf
    trunc := binary.BigEndian.Uint32(sum[off : off+4])
    code  := (trunc & 0x7fffffff) % 1000000

    return int(code)
}

func TOTP(secret []byte) int {
    step  := int64(30)
    count := int64(math.Round(float64(time.Now().Unix() / step)))

    return HOTP(secret, count)
}
