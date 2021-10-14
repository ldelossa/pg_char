#include <linux/fs.h>
#include <linux/module.h>

#include "pg_char_dev.h"

// keep track of open count
int open;

int pg_char_open(struct inode *, struct file *) {
    open++;
	printk(KERN_ALERT "device opened. count: %d\n", open);
    return 0;
}

int pg_char_release(struct inode *, struct file *) {
    open--;
	printk(KERN_ALERT "device released. count: %d", open);
    return 0;
}

// global fops containg our char device's implementation.
struct file_operations pg_char_fops = {
    .owner = THIS_MODULE,
    .open  = pg_char_open,
    .release = pg_char_release
}; 
