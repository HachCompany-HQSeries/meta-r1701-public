YOCTO	:= Y

obj-m	:= serial-modbus.o
obj-m	+= mipdisplay-2-7-inch.o
obj-m	+= mipdisplay-3-2-inch.o

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

export KROOT=/opt/hachdev/kernel/r1701/
all:
	@$(MAKE) -C $(KROOT) M=$(shell pwd)
modules_install:
	@$(MAKE) -C $(KROOT) M=$(shell pwd) LOCALVERSION= INSTALL_MOD_PATH=/srv/nfs/r1701 modules_install
clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers

endif
