SUMMARY = "A QT5 debug image derived from dey-image-qt"
LICENSE = "MIT"

require recipes-core/images/dey-image-qt.bb

IMAGE_INSTALL += " \
    tmux \
    devmem2 \
    htop \
"

# Add predefined runtime package groups. 
IMAGE_FEATURES += " \
    debug-tweaks \
    eclipse-debug \
    qtcreator-debug \
    tools-debug \
    ssh-server-openssh \
"
