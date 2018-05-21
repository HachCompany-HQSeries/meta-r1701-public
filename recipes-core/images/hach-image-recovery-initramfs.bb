# Copyright (C) 2018, HACH Company

DESCRIPTION = "Hach Recovery initramfs image"
LICENSE = "MIT"

require recipes-core/images/dey-image-recovery-initramfs.bb

PACKAGE_INSTALL += " \
    kernel-modules \
    kernel-module-mipdisplay-3-2-inch \
"
PACKAGE_INSTALL_remove += " \
 	psplash \
"

export IMAGE_BASENAME = "hach-image-recovery-initramfs"
