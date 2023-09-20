/*   SPDX-License-Identifier: BSD-3-Clause
 *   Copyright (C) 2017 Intel Corporation. All rights reserved.
 *   Copyright (c) 2019 Mellanox Technologies LTD. All rights reserved.
 */

#ifndef SPDK_BDEV_NULL_H
#define SPDK_BDEV_NULL_H

#include "spdk/stdinc.h"

#define DEFAULT_STRIPE_SIZE_MB 1

typedef void (*spdk_delete_ubi_complete)(void *cb_arg, int bdeverrno);

struct spdk_bdev;
struct spdk_uuid;

/*
 * Parameters to create a ubi bdev.
 */
struct spdk_ubi_bdev_opts {
	const char *name;
	const char *image_path;
	const char *base_bdev_name;
	uint32_t stripe_size_mb;
};

struct ubi_create_context {
	void (*done_fn)(void *cb_arg, struct spdk_bdev *bdev, int status);
	void *done_arg;

	struct ubi_bdev *ubi_bdev;
	bool registerd;

	/* temporary channel used to read metadata */
	struct spdk_io_channel *base_ch;
};

/*
 * Functions called by rpc methods.
 */
void bdev_ubi_create(const struct spdk_ubi_bdev_opts *opts,
					 struct ubi_create_context *context);
void bdev_ubi_delete(const char *bdev_name, spdk_delete_ubi_complete cb_fn, 
					 void *cb_arg);

#endif /* SPDK_BDEV_NULL_H */
