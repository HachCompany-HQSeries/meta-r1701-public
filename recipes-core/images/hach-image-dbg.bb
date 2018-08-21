SUMMARY = "A QT5 debug image derived from dey-image-qt"
LICENSE = "MIT"

require hach-image.bb

# devmem2
IMAGE_INSTALL += " \
    htop \
    mtd-utils \
    suspend-resume \
"

# Add predefined runtime package groups.
# qtcreator-debug
# eclipse-debug
IMAGE_FEATURES += " \
    debug-tweaks \
    tools-debug \
"

export IMAGE_BASENAME = "hach-image-dbg"
