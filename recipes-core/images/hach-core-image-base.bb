SUMMARY = "A QT5 image derived from core-image-base."
LICENSE = "MIT"

require recipes-core/images/core-image-base.bb

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
    dey-network \
    package-management \
    ssh-server-dropbear \
    ${@bb.utils.contains('MACHINE_FEATURES', 'accel-video', 'dey-gstreamer', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'alsa', 'dey-audio', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'bluetooth', 'dey-bluetooth', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'wifi', 'dey-wireless', '', d)} \
    ssh-server-openssh \
    dey-qt \
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
