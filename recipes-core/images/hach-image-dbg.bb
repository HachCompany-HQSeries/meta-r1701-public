SUMMARY = "A QT5 debug image derived from dey-image-qt"
LICENSE = "MIT"

require hach-image.bb

IMAGE_INSTALL += " \
    suspend-resume \
    rsync \
    htop \
    devmem2 \
    mtd-utils \
"

# Add predefined runtime package groups.
IMAGE_FEATURES += " \
    package-management \
    eclipse-debug \
    qtcreator-debug \
    debug-tweaks \
    tools-debug \
"

# Remove X11, wireless and sound related packages.
# NOTE - SOC_PACKAGES contains only IMX graphics demo tools and examples. We do not need them.
IMAGE_FEATURES_remove += " \
    ${SOC_PACKAGES} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11', 'x11-base x11-sato', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'accel-video', 'dey-gstreamer', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'alsa', 'dey-audio', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'bluetooth', 'dey-bluetooth', '', d)} \
    ${@bb.utils.contains('MACHINE_FEATURES', 'wifi', 'dey-wireless', '', d)} \
"

export IMAGE_BASENAME = "hach-image-dbg"
