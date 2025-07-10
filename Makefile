obj-m := gpio_key.o
KDIR ?= /lib/modules/$(shell uname -r)/build
EXTRA_CFLAGS += -Wno-compiler-version
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean