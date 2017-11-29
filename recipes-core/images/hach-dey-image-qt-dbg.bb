SUMMARY = "A QT5 debug image derived from dey-image-qt"
LICENSE = "MIT"

require recipes-core/images/dey-image-qt.bb

IMAGE_INSTALL += " \
    suspend-resume \
    rsync \
    htop \
    devmem2 \
    mtd-utils \  
    dey-examples-spidev \
"

# Add predefined runtime package groups. 
IMAGE_FEATURES += " \
    package-management \
    eclipse-debug \
    qtcreator-debug \
    debug-tweaks \
    tools-debug \
    ssh-server-openssh \

export IMAGE_BASENAME = "hach-dey-image-qt-dbg"
