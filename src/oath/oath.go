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
	key := strings.ToUpper(secret);

	bsecret, err := base32.StdEncoding.DecodeString(key);
	if err != nil {
		return nil, fmt.Errorf("Could not decode key: %s", err);
	}

	return bsecret, nil;
}

func HOTP(secret []byte, count int64) int {
	mac := hmac.New(sha1.New, secret);

	binary.Write(mac, binary.BigEndian, count);

	sum   := mac.Sum(nil);
	off   := sum[len(sum) - 1] & 0xf;
	trunc := binary.BigEndian.Uint32(sum[off : off + 4]);
	code  := (trunc & 0x7fffffff) % 1000000;

	return int(code);
}

func TOTP(secret []byte) int {
	step  := int64(30);
	count := int64(math.Floor(float64(time.Now().Unix() / step)));

	return HOTP(secret, count);
}
