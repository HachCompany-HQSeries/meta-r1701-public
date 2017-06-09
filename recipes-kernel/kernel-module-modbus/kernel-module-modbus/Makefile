YOCTO	:= Y

obj-m 	:= serial-modbus.o

ifeq ($(YOCTO), Y)

SRC 	:= $(shell pwd)
all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)
modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install
clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers

else

export KROOT=/opt/hachdev/buildsystem/r1701/build/tmp/work/ccimx6ulsbc-dey-linux-gnueabi/linux-dey/4.1-r0/build/
all:
	@$(MAKE) -C $(KROOT) M=$(shell pwd)
modules_install:
	@$(MAKE) -C $(KROOT) M=$(shell pwd) INSTALL_MOD_PATH=/srv/nfs/r1701 modules_install
clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers

endif

