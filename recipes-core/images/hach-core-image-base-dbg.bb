SUMMARY = "A debug image derived from core-image-base."
LICENSE = "MIT"

require hach-core-image-base.bb

IMAGE_INSTALL += " \
    suspend-resume \
    rsync \
    htop \
    devmem2 \
    mtd-utils \  
    dey-examples-spidev \
"

IMAGE_FEATURES += " \
    package-management \
    eclipse-debug \
    qtcreator-debug \
    debug-tweaks \
    tools-debug \
"

#export IMAGE_BASENAME = "hach-core-image-base-dbg"
