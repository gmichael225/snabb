/* Copyright 2012 Snabb GmbH. See the file COPYING for license details. */

#include <assert.h>
#include <fcntl.h>
#include <linux/if_tun.h>
#include <net/if.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

#include "snabb.h"
#include <net/snabb-shm-dev.h>

struct snabb_shm_dev *open_shm(const char *path)
{
    int fd;
    struct snabb_shm_dev *dev;
    assert( (fd = open(path, O_RDWR)) >= 0 );
    dev = (struct snabb_shm_dev *)
        mmap(NULL, sizeof(struct snabb_shm_dev),
             PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    assert( dev != MAP_FAILED );
    assert( dev->magic == 0x57ABB000 );
    return dev;
}

int open_tap(const char *name)
{
    struct ifreq ifr;
    int fd;
    if ((fd = open("/dev/net/tun", O_RDWR | O_NONBLOCK)) < 0) {
        perror("open /dev/net/tun");
        return -1;
    }
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name)-1);
    if (ioctl(fd, TUNSETIFF, (void*)&ifr) < 0) {
        perror("TUNSETIFF");
        return -1;
    }
    return fd;
}

uint64_t get_time_ns()
{
    /* XXX Consider using RDTSC. */
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000LL + ts.tv_nsec;
}
