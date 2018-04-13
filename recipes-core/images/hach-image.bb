SUMMARY = "A QT5 image derived from dey-image-qt"
LICENSE = "MIT"

require recipes-core/images/dey-image-qt.bb

inherit populate_sdk_qt5
inherit populate_sdk_qt5_base

IMAGE_LINGUAS = "en-us"

IMAGE_INSTALL += " \
    boost \
    zeromq \
    fcgi \
    sqlite3 \
    busybox-udhcpd \
    protobuf \
    recovery-utils \
    r1701-apps \
"

# Add predefined runtime package groups.
IMAGE_FEATURES += " \
    ssh-server-openssh \
"

# Assign a password to root account.
inherit extrausers
EXTRA_USERS_PARAMS += " \
    usermod -P hqd123 root; \
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
    ${@bb.utils.contains('MACHINE_FEATURES', 'touchscreen', '${VIRTUAL-RUNTIME_touchscreen}', '', d)} \
    awsiotsdk-c \
    cloudconnector \
    libdigiapix \
    evtest \
    fbtest \
    i2c-tools \
    memwatch \
    dey-examples \
    tcpdump \
"

# Remove distribution feature.
DISTRO_FEATURES_remove = "x11 wifi bluetooth"

# Do not install udev-cache
BAD_RECOMMENDATIONS += "udev-cache"
BAD_RECOMMENDATIONS += "eudev-hwdb"

export IMAGE_BASENAME = "hach-image"
