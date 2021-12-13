#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>

#include "lab2_common.h"

#define MNT_NOSUID	0x01
#define MNT_NODEV	0x02
#define MNT_NOEXEC	0x04
#define MNT_NOATIME	0x08
#define MNT_NODIRATIME	0x10
#define MNT_RELATIME	0x20
#define MNT_READONLY	0x40	/* does the user want this to be r/o? */
#define MNT_NOSYMFOLLOW	0x80

#define MNT_SHRINKABLE	0x100
#define MNT_WRITE_HOLD	0x200

#define MNT_SHARED	0x1000	/* if the vfsmount is a shared mount */
#define MNT_UNBINDABLE	0x2000	/* if the vfsmount is a unbindable mount */

#define MNT_INTERNAL	0x4000

#define MNT_LOCK_ATIME		0x040000
#define MNT_LOCK_NOEXEC		0x080000
#define MNT_LOCK_NOSUID		0x100000
#define MNT_LOCK_NODEV		0x200000
#define MNT_LOCK_READONLY	0x400000
#define MNT_LOCKED		0x800000
#define MNT_DOOMED		0x1000000
#define MNT_SYNC_UMOUNT		0x2000000
#define MNT_MARKED		0x4000000
#define MNT_UMOUNT		0x8000000
#define MNT_CURSOR		0x10000000

static void lab2_vfsmount_print(struct lab2_vfsmount *mnt)
{
    if (mnt->flags & MNT_NOSUID)
        printf("vfsmount: %s\n", "flag MNT_NOSUID used");
    if (mnt->flags & MNT_NODEV)
        printf("vfsmount: %s\n", "flag MNT_NODEV used");
    if (mnt->flags & MNT_NOEXEC)
        printf("vfsmount: %s\n", "flag MNT_NOEXEC used");
    if (mnt->flags & MNT_NOATIME)
        printf("vfsmount: %s\n", "flag MNT_NOATIME used");
    if (mnt->flags & MNT_NODIRATIME)
        printf("vfsmount: %s\n", "flag MNT_NODIRATIME used");
    if (mnt->flags & MNT_RELATIME)
        printf("vfsmount: %s\n", "flag MNT_RELATIME used");
    if (mnt->flags & MNT_READONLY)
        printf("vfsmount: %s\n", "flag MNT_READONLY used");
    if (mnt->flags & MNT_NOSYMFOLLOW)
        printf("vfsmount: %s\n", "flag MNT_NOSYMFOLLOW used");
    if (mnt->flags & MNT_SHRINKABLE)
        printf("vfsmount: %s\n", "flag MNT_SHRINKABLE used");
    if (mnt->flags & MNT_WRITE_HOLD)
        printf("vfsmount: %s\n", "flag MNT_WRITE_HOLD used");
    if (mnt->flags & MNT_SHARED)
        printf("vfsmount: %s\n", "flag MNT_SHARED used");
    if (mnt->flags & MNT_UNBINDABLE)
        printf("vfsmount: %s\n", "flag MNT_UNBINDABLE used");
    if (mnt->flags & MNT_INTERNAL)
        printf("vfsmount: %s\n", "flag MNT_INTERNAL used");
    if (mnt->flags & MNT_LOCK_ATIME)
        printf("vfsmount: %s\n", "flag MNT_LOCK_ATIME used");
    if (mnt->flags & MNT_LOCK_NOEXEC)
        printf("vfsmount: %s\n", "flag MNT_LOCK_NOEXEC used");
    if (mnt->flags & MNT_LOCK_NOSUID)
        printf("vfsmount: %s\n", "flag MNT_LOCK_NOSUID used");
    if (mnt->flags & MNT_LOCK_NODEV)
        printf("vfsmount: %s\n", "flag MNT_LOCK_NODEV used");
    if (mnt->flags & MNT_LOCK_READONLY)
        printf("vfsmount: %s\n", "flag MNT_LOCK_READONLY used");
    if (mnt->flags & MNT_LOCKED)
        printf("vfsmount: %s\n", "flag MNT_LOCKED used");
    if (mnt->flags & MNT_DOOMED)
        printf("vfsmount: %s\n", "flag MNT_DOOMED used");
    if (mnt->flags & MNT_SYNC_UMOUNT)
        printf("vfsmount: %s\n", "flag MNT_SYNC_UMOUNT used");
    if (mnt->flags & MNT_MARKED)
        printf("vfsmount: %s\n", "flag MNT_MARKED used");
    if (mnt->flags & MNT_UMOUNT)
        printf("vfsmount: %s\n", "flag MNT_UMOUNT used");
    if (mnt->flags & MNT_CURSOR)
        printf("vfsmount: %s\n", "flag MNT_CURSOR used");
}

static void lab2_address_space_print(struct lab2_address_space * as)
{
    printf("address_space: nr_pages: %d\n", as->nr_pages);
    printf("address_space: mapped: %d\n", as->mapped);
    printf("address_space: unevictable: %d\n", as->unevictable);
    printf("address_space: exiting: %d\n", as->exiting);
    printf("address_space: supports thp: %d\n", as->supports_thp);
}

static int print_help()
{
    printf("Usage: ./lab2_client [-a|-v]... PATH\n");
}

int main(int argc, char * argv[])
{
    bool print_vfsmount = false;
    bool print_address_space = false;
    char * path = NULL;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-a") == 0) {
            print_address_space = true;
        }
        else if (strcmp(argv[i], "-v") == 0) {
            print_vfsmount = true;
        }
        else if (!path) {
            path = argv[i];
        }
        else {
            return print_help();
        }
    }

    if (!print_vfsmount && !print_address_space) {
        print_address_space = true;
        print_vfsmount = true;
    }

    int fd = fopen("/dev/" DEVICE_FILE_NAME, O_RDONLY);

    if (fd == -1) {
        printf("Cannot open /dev/" DEVICE_FILE_NAME " device, please check your permissions");
        return -1;
    }

    if (print_vfsmount) {
        char buf[BUF_LEN];
        strcpy(buf, path);

        if (ioctl(fd, IOCTL_GET_VFSMOUNT, buf) != 0) {
            printf("Cannot get struct vfsmount for path %s\n", path);
            return -1;
        }

        struct lab2_vfsmount * vfsmount = (struct lab2_vfsmount *) buf;
        lab2_vfsmount_print(vfsmount);
    }

    if (print_address_space) {
        char buf[BUF_LEN];
        strcpy(buf, path);

        if (ioctl(fd, IOCTL_GET_ADDRESS_SPACE, buf) != 0) {
            printf("Cannot get struct vfsmount for path %s\n", path);
            return -1;
        }

        struct lab2_address_space * as = (struct lab2_address_space *) buf;
        lab2_address_space_print(as);
    }

    return 0;
}