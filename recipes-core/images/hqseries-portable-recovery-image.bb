DESCRIPTION = "Hach Recovery initramfs image"
LICENSE = "MIT"

require recipes-core/images/dey-image-recovery-initramfs.bb

PACKAGE_INSTALL += " \
    kernel-modules \
    xsplash \
"

PACKAGE_INSTALL_remove += " \
    psplash \
"

export IMAGE_BASENAME = "hqseries-portable-recovery-image"
IMAGE_NAME = "hqseries-portable-recovery-image"
