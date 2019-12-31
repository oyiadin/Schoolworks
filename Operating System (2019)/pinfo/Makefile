ifneq ($(KERNELRELEASE),)

	obj-m := pinfo.o

else

	KERN_DIR ?= ../linux-4.15/
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERN_DIR) M=$(PWD) modules

endif


clean:
	$(MAKE) -C $(KERN_DIR) M=$(PWD) clean
