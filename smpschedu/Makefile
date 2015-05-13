obj-m := module.o
KDIR ?= /mnt/data/omap/linux-omap

PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
	@rm -rf *.ko *.o *.order *.symvers .*.cmd *.mod.c .tmp_versions 
