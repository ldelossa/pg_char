# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
	# this is the output o and .ko file name
	# and should not be named after any .c files
	# in your module directory.
	obj-m := pg_char.o 
	# these are the object files make should create
	# and link to create your module file above.
    pg_char-objs := main.o pg_char_file_ops.o
# Otherwise we were called directl from the command
# line; invoke the kernel build system.
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)
endif

test:
	gcc -o test ./pg_char_test.c

clean:	
	rm -rf *.o *~ core .depend *.mod.o .*.cmd *.ko *.mod.c \
	.tmp_versions *.markers *.symvers modules.order a.out test
