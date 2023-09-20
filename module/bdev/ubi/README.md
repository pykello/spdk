# bdev_ubi

## Introduction

bdev_ubi provides an SPDK virtual bdev layered over another bdev, enabling copy-on-access for a base image.

This can be utilized to set up a block device initialized with an image file,
without the delay of an initial copy. With bdev_ubi, copying occurs lazily upon
block access, rather than at the provisioning time.

For instance, let's say you already have a bdev named "aio0" and you wish to
populate it with data from `/opt/images/large-image.raw`. A conventional approach
would be to copy `jammy.raw` to `aio0` using tools like spdk_dd, which can be
time-consuming. However, with bdev_ubi, you can proceed as follows:

```
rpc.py bdev_ubi_create -n ubi0 -b aio0 -i /opt/images/large-image.raw --new-disk true
```

and rather than directly using `aio0`, use `ubi0` as the block device. This action
completes almost instantly, eliminating the need for initial data copying. The
actual data copying will occur lazily during I/O requests.

## API

### bdev_ubi_create

Parameters:
* `-n/--name [name]` (required): Name of the bdev to be created.
* `-i/--image-path [path]` (required): Path to the image file.
* `-b/--base-bdev [bdev]` (required): Name of base bdev.
* `-z/--stripe-size-mb [size]` (optional): Stripe size in megabytes. Defaults to 1.

### bdev_ubi_delete

Parameters:
* `-n/--name [name]` (required): Name of the bdev to be deleted.

## Internals

### Data Layout

First 8MB of base bdev is reserved for metadata. Metadata consists of:
* Magic bytes (9 bytes): `BDEV_UBI\0`
* Metadata version major (2 bytes)
* Metadata version minor (2 bytes)
* stripe_size_mb (1 byte)
* Stripe headers: 4 byte per stripes. Currently it specifies whether a stripe has
  been fetched from image or not. 31-bits are reserved for future extension.
* Padding to make the total size 8MB.

Then at the 1MB offset the actual disk data starts.

### Read/Write I/O operations

If the stripe containing the requested block range hasn't been fetched yet, then a
stripe fetch is enqueued. Once stripe has been fetched, the actual I/O operation is
served.

### Flush (aka sync)

* Data for the requested range is flushed to base bdev.
* Once data flush is finished, and if metadata has been modified in memory, then
  metadata is first written and then flushed to base bdev.

## Testing

SPDK has a testing binary called `bdevio` which does some basic automated tests
on a bdev. To use this for testing bdev_ubi, run:

```
test/ubi/ubi.sh
```
