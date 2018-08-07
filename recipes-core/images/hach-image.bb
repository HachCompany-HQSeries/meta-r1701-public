SUMMARY = "A QT5 image derived from dey-image-qt"
LICENSE = "MIT"

require recipes-core/images/dey-image-qt.bb

inherit populate_sdk_qt5
inherit populate_sdk_qt5_base

POKY_DEFAULT_DISTRO_FEATURES_remove = "ptest wayland cellular bluez5"
GRAPHICAL_BACKEND = "directfb"

# DEY image features (alphabetical order)
FEATURE_PACKAGES_dey-audio = ""
FEATURE_PACKAGES_dey-bluetooth = ""
FEATURE_PACKAGES_dey-debug = "packagegroup-dey-debug"
FEATURE_PACKAGES_dey-examples = ""
FEATURE_PACKAGES_dey-gstreamer = ""
FEATURE_PACKAGES_dey-network = "packagegroup-dey-network"
FEATURE_PACKAGES_dey-qt = "packagegroup-dey-qt"
FEATURE_PACKAGES_dey-trustfence = ""
FEATURE_PACKAGES_dey-wireless = ""

QEMU_TARGETS = "arm x86_64"
DISTRO_EXTRA_RDEPENDS_append_qemuarm = " ${POKYQEMUDEPS}"
DISTRO_EXTRA_RDEPENDS_append_qemuarm64 = " "
DISTRO_EXTRA_RDEPENDS_append_qemumips = " "
DISTRO_EXTRA_RDEPENDS_append_qemuppc = " "
DISTRO_EXTRA_RDEPENDS_append_qemux86 = " "
DISTRO_EXTRA_RDEPENDS_append_qemux86-64 = " ${POKYQEMUDEPS}"

# Remove some features to reduce the rootfs size to fit in the internal storage.
IMAGE_FEATURES_remove_r1701 = "package-management"

# Remove distribution feature.
DISTRO_FEATURES_remove = "x11 wayland wifi bluetooth"

# We only use direct framebuffer. No need to have x11 realted tools and libraries.
CONFLICT_DISTRO_FEATURES = "directfb x11"

IMAGE_LINGUAS = "en-us"

# Remove X11, wireless and sound related packages.
# NOTE - SOC_PACKAGES contains only IMX graphics demo tools and examples. We do not need them.
IMAGE_FEATURES_remove += " \
    ${SOC_PACKAGES} \
    ${VIRTUAL-RUNTIME_touchscreen} \
    x11 \
    x11-base \
    x11-sato \
    dey-gstreamer \
    dey-audio \
    dey-bluetooth \
    dey-wireless \
    awsiotsdk-c \
    cloudconnector \
    libdigiapix \
    evtest \
    fbtest \
    i2c-tools \
    memwatch \
    dey-examples \
    tcpdump \
    ethtool \
    modemmanager \
    libdrm \
"

IMAGE_INSTALL += " \
    boost \
    zeromq \
    sqlite3 \
    busybox-udhcpd \
    protobuf \
    recovery-utils \
    cpio \
    cppzmq \
    e2fsprogs-mke2fs \
    util-linux-sfdisk \
    tzdata \
    sysinfo \
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

export IMAGE_BASENAME = "hach-image"
