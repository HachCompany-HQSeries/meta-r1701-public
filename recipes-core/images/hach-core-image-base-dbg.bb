SUMMARY = "A debug image derived from core-image-base."
LICENSE = "MIT"

require hach-core-image-base.bb

IMAGE_INSTALL += " \
    suspend-resume \
    htop \
    devmem2 \
    tmux \
"

IMAGE_FEATURES += " \
    package-management \
    eclipse-debug \
    qtcreator-debug \
    debug-tweaks \
    tools-debug \
"
