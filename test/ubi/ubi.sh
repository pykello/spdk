#!/usr/bin/env bash
#  SPDX-License-Identifier: BSD-3-Clause
#  Copyright (C) 2019 Intel Corporation
#  All rights reserved.
#
set -e

testdir=$(readlink -f $(dirname $0))
rootdir=$(readlink -f $testdir/../..)
plugindir=$rootdir/examples/bdev/fio_plugin
source "$rootdir/scripts/common.sh"
source "$rootdir/test/common/autotest_common.sh"
source "$rootdir/test/nvmf/common.sh"

# Declare rpc_py here, because its default value points to rpc_cmd function,
# which does not tolerate piping arguments into it.
rpc_py="$rootdir/scripts/rpc.py"


function run_bdevio() {
	$rootdir/test/bdev/bdevio/bdevio -w &
	bdevio_pid=$!
	trap 'killprocess $bdevio_pid; exit 1' SIGKILL SIGINT SIGTERM EXIT
	waitforlisten $bdevio_pid
	$rpc_py bdev_malloc_create -b Malloc0 128 512
	rm -rf /tmp/ubi_image
	touch /tmp/ubi_image
	truncate -s 2M /tmp/ubi_image
	$rpc_py bdev_ubi_create -n ubi0 -b Malloc0 -i /tmp/ubi_image -z 1
	$rootdir/test/bdev/bdevio/tests.py perform_tests -b ubi0
	trap - SIGKILL SIGINT SIGTERM EXIT
	kill $bdevio_pid
}

run_bdevio
