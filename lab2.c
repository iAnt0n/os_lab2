#include <linux/init.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mount.h>
#include <linux/path.h>
#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/pagemap.h>

#include "lab2_common.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("anton");
MODULE_DESCRIPTION("Lab2");
MODULE_VERSION("1.0");

static struct class *cls; 

static int device_open(struct inode *inode, struct file *file) 
{ 
    pr_info("device_open(%p)\n", file); 
 
    // /* We don't want to talk to two processes at the same time. */ 
    // if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN)) 
    //     return -EBUSY; 
 
    // try_module_get(THIS_MODULE); 
    return 0; 
} 
 
static int device_release(struct inode *inode, struct file *file) 
{ 
    pr_info("device_release(%p,%p)\n", inode, file); 
 
    // /* We're now ready for our next caller */ 
    // atomic_set(&already_open, CDEV_NOT_USED); 
 
    // module_put(THIS_MODULE); 
    return 0; 
}

static long device_ioctl(
        struct file *file,
        unsigned int cmd,
        unsigned long arg)
{
    int retval = 0;
    int i = 0;
    char __user *tmp = (char __user *) arg; 
    char path_arg[BUF_LEN];

    struct path path;

    switch (cmd) {
    case IOCTL_GET_ADDRESS_SPACE:
        copy_from_user(path_arg, tmp, BUF_LEN);

        if (kern_path(path_arg, LOOKUP_FOLLOW, &path) != 0) {
            return -EINVAL;
        }

        struct lab2_address_space address_space_for_user;
        address_space_for_user.nr_pages = path.dentry->d_inode->i_mapping->nrpages;
        address_space_for_user.mapped = mapping_mapped(path.dentry->d_inode->i_mapping);
        address_space_for_user.unevictable = mapping_unevictable(path.dentry->d_inode->i_mapping);
        address_space_for_user.exiting = mapping_exiting(path.dentry->d_inode->i_mapping);
        address_space_for_user.supports_thp = mapping_thp_support(path.dentry->d_inode->i_mapping);
        copy_to_user((struct lab2_address_space *) arg, &address_space_for_user, sizeof(address_space_for_user));

        break;

    case IOCTL_GET_VFSMOUNT:
        copy_from_user(path_arg, tmp, BUF_LEN);

        if (kern_path(path_arg, LOOKUP_FOLLOW, &path) != 0) {
            return -EINVAL;
        }

        struct lab2_vfsmount vfsmount_for_user;
        vfsmount_for_user.flags = path.mnt->mnt_flags;
        copy_to_user((struct lab2_vfsmount *) arg, &vfsmount_for_user, sizeof(vfsmount_for_user));

        break;
    default:
        return -ENOTTY;
    }

    return 0;
}

static struct file_operations fops = {  
    .open = device_open, 
    .release = device_release,
    .unlocked_ioctl = device_ioctl
}; 

static int __init lab2_init(void)
{
    pr_info("INIT MODULE\n");

    int retval = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops); 
 
    /* Negative values signify an error */ 
    if (retval < 0) { 
        pr_alert("%s failed with %d\n",
                 "Sorry, registering the character device ", retval); 
        return retval; 
    } 
 
    cls = class_create(THIS_MODULE, DEVICE_FILE_NAME); 
    device_create(cls, NULL, MKDEV(MAJOR_NUM, 0), NULL, DEVICE_FILE_NAME); 
 
    pr_info("Device created on /dev/%s\n", DEVICE_FILE_NAME); 

    // struct path path;

    // kern_path("/home/anton/Downloads/7a348f", LOOKUP_FOLLOW, &path);
    // kern_path("/home/anton/os", LOOKUP_FOLLOW, &path);

    // pr_info("ADDRESS SPACE NRPAGES: %d\n", path.dentry->d_inode->i_mapping->nrpages);
    // pr_info("MIGHT MAPPED?: %d\n", mapping_mapped(path.dentry->d_inode->i_mapping));
    // pr_info("VFS MOUNT: %d", path.mnt);
    
    return 0;
}

static void __exit lab2_exit(void)
{
    device_destroy(cls, MKDEV(MAJOR_NUM, 0)); 
    class_destroy(cls); 
 
    /* Unregister the device */ 
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);

    pr_info("DEINIT MODULE\n");
}

module_init(lab2_init);
module_exit(lab2_exit);
