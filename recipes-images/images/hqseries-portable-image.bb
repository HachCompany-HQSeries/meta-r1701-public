SUMMARY = "An image based on Qt5 for graphical use."
LICENSE = "MIT"

# Inherit Yocto's core-image and feature check class.
inherit core-image features_check

# Inherit Qt5 core.
inherit populate_sdk_qt5 populate_sdk_qt5_base

# SDK features (for toolchains generated from an image with populate_sdk)
SDKIMAGE_FEATURES ?= "dev-pkgs dbg-pkgs staticdev-pkgs"

# We use only Linux FB, No X11, Wayland or XWayland.
CONFLICT_DISTRO_FEATURES = "directfb"

# Do not install udev-cache
BAD_RECOMMENDATIONS += "udev-cache"
BAD_RECOMMENDATIONS += "eudev-hwdb"

# Default US english.
IMAGE_LINGUAS = "en-us"

# Audio support.
AUDIO_SUPPORT = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'alsa',       'packagegroup-imx-tools-audio', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'pulseaudio', 'packagegroup-imx-tools-audio', '', d)} \
"

# Wifi  Bluetooth support.
WIFI_BT_SUPPORT = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'bluetooth', 'packagegroup-tools-bluetooth', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wifi',      '', '', d)} \
"

# Hardware accelerated Video support.
VIDEO_SUPPORT = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'accel-video', 'packagegroup-fsl-gstreamer1.0 packagegroup-fsl-gstreamer1.0-full', '', d)} \
"

# Backend Graphics distro support. Ex. X11 or X11 + Wayland Or Wayland Or Linux FB
GRAPHICS_BE_SUPPORT = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11', 'xterm', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11 wayland', 'weston-xwayland xterm', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'weston-init', \
        bb.utils.contains('DISTRO_FEATURES',    'x11', '', '', d), d)} \
"
GRAPHICS_FEATURES_SUPPORT = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', '', \
       bb.utils.contains('DISTRO_FEATURES',     'x11', 'x11-base x11-sato', \
                                                       '', d), d)} \
"

# GPU support.
GPU_SUPPORT = " \
    packagegroup-fsl-tools-gpu-external \
"

CORE_IMAGE_EXTRA_INSTALL += " \
    packagegroup-core-full-cmdline \
    \
    packagegroup-r1701-qt \
    \
    ${AUDIO_SUPPORT} \
    ${WIFI_BT_SUPPORT} \
    ${GPU_SUPPORT} \
    ${VIDEO_SUPPORT} \
    \
    ${GRAPHICS_BE_SUPPORT} \
"

# Image feature supports.
IMAGE_FEATURES += " \
    ssh-server-openssh \
    \
    eclipse-debug \
    debug-tweaks \
    tools-debug \
    nfs-server \
    \
    splash \
    hwcodecs \
    ${GRAPHICS_FEATURES_SUPPORT} \
    \
"

# Base Image tools & software supports.
IMAGE_INSTALL += " \
    boost \
    zeromq \
    sqlite3 \
    protobuf \
    cppzmq \
    \
    e2fsprogs-mke2fs \
    util-linux-sfdisk \
    tzdata \
    dosfstools  \
    htop \
    p7zip \
    sysstat \
    \
    mtd-utils \
    openssl-bin \
    \
    screen \
    tcf-agent \
    openssh-sftp-server \
    \
    libgpiod \
    libgpiod-tools \
"

# Remove X11, wireless and sound related packages.
# NOTE - SOC_PACKAGES contains only IMX graphics demo tools and examples. We do not need them.
IMAGE_FEATURES_remove += " \
    x11 \
    x11-base \
    x11-sato \
    evtest \
    fbtest \
    i2c-tools \
    memwatch \
    tcpdump \
    ethtool \
    modemmanager \
    ssh-server-dropbear \
"

# Disable Console on main display. We do not want to see kernel logs, boot messages on main display.
systemd_disable_vt () {
    rm ${IMAGE_ROOTFS}${sysconfdir}/systemd/system/getty.target.wants/getty@tty*.service
}
IMAGE_PREPROCESS_COMMAND_append = " ${@ 'systemd_disable_vt;' if bb.utils.contains('DISTRO_FEATURES', 'systemd', True, False, d) and bb.utils.contains('USE_VT', '0', True, False, d) else ''} "

# Due to the Qt samples the resulting image will not fit the default NAND size. Removing default ubi creation for this
# image
IMAGE_FSTYPES_remove = "multiubi"

# export base name.
export IMAGE_BASENAME = "hqseries-portable-image"

# Use custom name.
#IMAGE_NAME = "hqseries-portable-image"
