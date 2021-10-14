#ifndef _PG_CHAR_H_
#define _PG_CHAR_H_

#include <linux/cdev.h>

// globals
extern int pg_char_major;
extern int pg_char_minor;
extern int pg_char_nr_dev;

// defined in file_ops.c
extern struct file_operations pg_char_fops;

// a structure representing out
// character device.
typedef struct pg_char {
    struct cdev cdev;
} pg_char_dev;

#endif /* _PG_CHAR_H_ */
