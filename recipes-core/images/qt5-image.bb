SUMMARY = "A Qt5 image derived from dey-image-qt"
LICENSE = "MIT"

require recipes-core/images/dey-image-qt.bb

inherit populate_sdk_qt5

EXTRA_TOOLS_INSTALL = " \
    rsync \
    tmux \
    devmem2 \
    htop \
"

HQD_EXTRAS = " \
    zeromq \
    boost \
    protobuf \
"

IMAGE_FEATURES = " \
    debug-tweaks \
    tools-debug \
    eclipse-debug \
    ssh-server-openssh \
    qtcreator-debug \
    dey-qt \
"

IMAGE_INSTALL += " \
    ${EXTRA_TOOLS_INSTALL} \
    ${HQD_EXTRAS} \
"

DISTRO_FEATURES_remove = "x11"

export IMAGE_BASENAME = "qt5-image"
