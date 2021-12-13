#pragma once

#include <stdbool.h>

struct lab2_address_space
{
    int nr_pages, mapped;
    bool unevictable, exiting, supports_thp;
};

struct lab2_vfsmount
{
    int flags;
};

#define BUF_LEN 256
#define MAJOR_NUM 100
#define DEVICE_NAME "char_dev"
#define DEVICE_FILE_NAME "char_dev"

#define IOCTL_GET_ADDRESS_SPACE _IOWR(MAJOR_NUM, 0, int)
#define IOCTL_GET_VFSMOUNT _IOWR(MAJOR_NUM, 1, int)