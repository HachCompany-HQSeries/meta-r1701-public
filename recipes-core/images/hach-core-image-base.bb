SUMMARY = "A QT5 image derived from core-image-base."
LICENSE = "MIT"

# dey-image-qt.bb is a good starting point for our custom r1701 image. For example, it includes the packagegroup
#   dey-bluetooth: adds “meta-digi-dey/recipes-connectivity/packagegroups/packagegroup-dey-bluetooth.bb”
#   dey-qt: adds “meta-digi-dey/recipes-graphics/packagegroups/packagegroup-dey-qt.bb”
#   etc.
require recipes-core/images/core-image-base.bb

inherit populate_sdk_qt5

R1701_INSTALL = " \
    rsync \
    tmux \
    devmem2 \
    htop \
    zeromq \
    boost \
    protobuf \
    lighttpd \
    fcgi \
    sqlite3 \
"

# Add predefined runtime package groups. 
IMAGE_FEATURES += " \
    dey-network \
    package-management \
    ssh-server-dropbear \
    ${@bb.utils.contains('MACHINE_FEATURES', 'accel-video', 'dey-gstreamer', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'alsa', 'dey-audio', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'bluetooth', 'dey-bluetooth', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'wifi', 'dey-wireless', '', d)} \
    debug-tweaks \
    eclipse-debug \
    qtcreator-debug \
    ssh-server-openssh \
    tools-debug \
    dey-qt \
"

# Add individual recipes.
IMAGE_INSTALL_append = " \
    ${R1701_INSTALL} \
"

# From meta-digi/meta-digi-dey/recipes-core/images/core-image-base.bbappend:
# SDK features (for toolchains generated from an image with populate_sdk).
SDKIMAGE_FEATURES ?= "dev-pkgs dbg-pkgs staticdev-pkgs"

# From meta-digi/meta-digi-dey/recipes-core/images/core-image-base.bbappend:
# Add our dey-image tweaks to the final image (like /etc/build info).
inherit dey-image

# From meta-digi/meta-digi-dey/recipes-core/images/core-image-base.bbappend:
# Do not install udev-cache.
BAD_RECOMMENDATIONS += "udev-cache"

# Add a new filesystem
IMAGE_FSTYPES_append = " sdcard"

DISTRO_FEATURES_remove = "x11"

export IMAGE_BASENAME = "hach-image"
