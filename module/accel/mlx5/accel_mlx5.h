/*   SPDX-License-Identifier: BSD-3-Clause
 *   Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 */

#include "spdk/stdinc.h"

struct accel_mlx5_attr {
	/* The number of entries in qp submission/receive queue */
	uint16_t qp_size;
	/* The number of requests in the global pool */
	uint32_t num_requests;
	/* Comma separated list of allowed device names */
	char *allowed_devs;
	/* Apply crypto operation for each X data blocks. Works only if multiblock crypto operation is supported by HW.
	 * 0 means no limit */
	uint16_t crypto_split_blocks;
};

void accel_mlx5_get_default_attr(struct accel_mlx5_attr *attr);
int accel_mlx5_enable(struct accel_mlx5_attr *attr);
