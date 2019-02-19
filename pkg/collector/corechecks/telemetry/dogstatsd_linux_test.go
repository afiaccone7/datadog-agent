// Unless explicitly stated otherwise all files in this repository are licensed
// under the Apache License Version 2.0.
// This product includes software developed at Datadog (https://www.datadoghq.com/).
// Copyright 2016-2019 Datadog, Inc.

package telemetry

import (
	"strings"
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestDogstatsdDrops(t *testing.T) {
	procUDPReader := strings.NewReader(`  sl  local_address rem_address   st tx_queue rx_queue tr tm->when retrnsmt   uid  timeout inode ref pointer drops
  188: 0100007F:E3E8 0100007F:1FBD 01 00000000:00000000 00:00000000 00000000     0        0 108614 2 0000000000000000 0
 1406: 0100007F:E8AA 0100007F:1FBD 01 00000000:00000000 00:00000000 00000000   112        0 318569992 2 0000000000000000 0
 1613: 0100007F:B979 0100007F:1FBD 01 00000000:00000000 00:00000000 00000000   112        0 318570502 2 0000000000000000 0
 2038: 0100007F:DB22 0100007F:1FBD 01 00000000:00000000 00:00000000 00000000  5001        0 318654169 2 0000000000000000 0
 2111: 0100007F:9B6B 0100007F:1FBD 01 00000000:00000000 00:00000000 00000000  5001        0 104828 2 0000000000000000 0
 2411: 0100007F:8C97 0100007F:1FBD 01 00000000:00000000 00:00000000 00000000   112        0 110381 2 0000000000000000 0
 3217: 0100007F:1FBD 00000000:0000 07 00000000:00000000 00:00000000 00000000   112        0 318570567 2 0000000000000000 1234567
 3337: 00000000:0035 00000000:0000 07 00000000:00000000 00:00000000 00000000     0        0 33503 2 0000000000000000 0
 3352: 00000000:0044 00000000:0000 07 00000000:00000000 00:00000000 00000000     0        0 12241 2 0000000000000000 0
 3607: 0100007F:0143 00000000:0000 07 00000000:00000000 00:00000000 00000000   113        0 224787939 2 0000000000000000 0
 3692: 0100007F:2198 00000000:0000 07 00000000:00000000 00:00000000 00000000     0        0 108634 2 0000000000000000 0
 3798: 00000000:0202 00000000:0000 07 00000000:00000000 00:00000000 00000000     0        0 252621798 2 0000000000000000 0
 3859: 0100007F:823F 0100007F:1FBD 01 00000000:00000000 00:00000000 00000000     0        0 319092668 2 0000000000000000 0`)

	drops, err := readDogstatsdUDPDrops(procUDPReader)

	assert.Nil(t, err)
	assert.Equal(t, drops, 1234567)
}