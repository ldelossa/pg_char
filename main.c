#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/slab.h>

#include "pg_char_dev.h"

MODULE_LICENSE("GPL");

// fwd declarations
void cleanup(void);
static int alloc_major(void);
static int pg_char_devices_init(struct file_operations *fops);

// major,minor and number of device constants
// declared in header and defined here.
int pg_char_major = 0;
int pg_char_minor = 0;
int pg_char_nr_dev = 1;

// global array of created devices.
pg_char_dev *pg_char_devs;

// module init function
static int pg_char_init(void)
{

	printk(KERN_ALERT "dynamically allocating device numbers\n");
    if (alloc_major() < 0) {
	    printk(KERN_ALERT "allocating device numbers failed\n");
        goto fail;

    }

	printk(KERN_ALERT "initializing and registering character devices\n");
    if (pg_char_devices_init(&pg_char_fops) < 0) {
	    printk(KERN_ALERT "allocating device numbers failed\n");
        goto fail;
    }

	printk(KERN_ALERT "pg_char_dev initialized\n");

	return 0;

fail:
    cleanup();
    return -1;
}

// module exit function
static void pg_char_exit(void)
{
    cleanup();
	printk(KERN_ALERT "pg_char_dev unregistered\n");
}

// dynamically allocate a major number from the kernel.
//
// minor numbers still start at 0.
static int alloc_major(void)
{
	// we can always get a handle to dev_t
	// with the MKDEV macro, so just stack alloc.
	dev_t dev;
	if (alloc_chrdev_region(&dev, pg_char_minor, pg_char_nr_dev, "pg_char") <
	    0) {
		return -1;
	}
    pg_char_major = MAJOR(dev);
    pg_char_minor = MINOR(dev);
	printk(KERN_ALERT "allocated major number: %d first minor: %d\n", pg_char_major, pg_char_minor);
    return 0;
}

// kmalloc enough memory to hold pg_char_nr_dev devices
// and initialize each one.
//
// cdev structures are intialized and registered with the kernel in this
// function.
//
// if a cdev registration with the kernel fails pg_char_devs will be freed, 
// any registered cdevs will be removed.
static int pg_char_devices_init(struct file_operations *fops) {
    int i = 0;
    
    pg_char_devs = kmalloc(sizeof(pg_char_dev)*pg_char_nr_dev, GFP_KERNEL);
    if (!pg_char_devs) {
        return -1;
    }
    memset(pg_char_devs, 0, sizeof(pg_char_dev)*pg_char_nr_dev);
    
    for (; i < pg_char_nr_dev; i++) {
        cdev_init(&pg_char_devs[i].cdev, fops);
        pg_char_devs[i].cdev.owner = THIS_MODULE;
        pg_char_devs[i].cdev.ops = &pg_char_fops;
        
	    printk(KERN_ALERT "allocating cdev for major: %d, minor: %d\n", pg_char_major, pg_char_minor+i);
        if (cdev_add(&pg_char_devs[i].cdev, MKDEV(pg_char_major, pg_char_minor+i), 1) < 0){
	        printk(KERN_ALERT "failed to allocate cdev for major: %d, minor: %d\n", pg_char_major, pg_char_minor+i);
            goto fail; 
        }
    }
    return 0;

fail:
    // free any added devices
    for (;i > 0; i--) {
        cdev_del(&pg_char_devs[i-1].cdev);
    }
    kfree(pg_char_devs);
    return -2;
}

void cleanup(void)
{
	if (pg_char_major) {
        unregister_chrdev_region(MKDEV(pg_char_major, pg_char_minor), pg_char_nr_dev);
    }
};

module_init(pg_char_init);
module_exit(pg_char_exit);
