SUMMARY = "A QT5 image derived from dey-image-qt"
LICENSE = "MIT"

require recipes-core/images/dey-image-qt.bb

inherit populate_sdk_qt5

IMAGE_INSTALL += " \
    rsync \
    zeromq \
    boost \
    protobuf \
    fcgi \
    sqlite3 \
    busybox-udhcpd \
"

# Add predefined runtime package groups. 
IMAGE_FEATURES += " \
    ssh-server-openssh \
"

# Add a new filesystem
IMAGE_FSTYPES_append = " sdcard"

DISTRO_FEATURES_remove = "x11"

export IMAGE_BASENAME = "hach-image"
