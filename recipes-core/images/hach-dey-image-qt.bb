SUMMARY = "A QT5 image derived from dey-image-qt"
LICENSE = "MIT"

# dey-image-qt.bb is a good starting point for our custom r1701 image. For example, it includes the packagegroup
#   dey-bluetooth: adds “meta-digi-dey/recipes-connectivity/packagegroups/packagegroup-dey-bluetooth.bb”
#   dey-qt: adds “meta-digi-dey/recipes-graphics/packagegroups/packagegroup-dey-qt.bb”
#   etc.
require recipes-core/images/dey-image-qt.bb

inherit populate_sdk_qt5

R1701_INSTALL = " \
    rsync \
    tmux \
    devmem2 \
    htop \
    zeromq \
    boost \
    protobuf \
    fcgi \
    sqlite3 \
    busybox-udhcpd \
"

# Add predefined runtime package groups. 
IMAGE_FEATURES += " \
    debug-tweaks \
    eclipse-debug \
    qtcreator-debug \
    ssh-server-openssh \
    tools-debug \
"

# Add individual recipes.
IMAGE_INSTALL_append = " \
    ${R1701_INSTALL} \
"

# Add a new filesystem
IMAGE_FSTYPES_append = " sdcard"

DISTRO_FEATURES_remove = "x11"

export IMAGE_BASENAME = "hach-image"
