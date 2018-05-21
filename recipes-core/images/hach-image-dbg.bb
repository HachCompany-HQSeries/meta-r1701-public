SUMMARY = "A QT5 debug image derived from dey-image-qt"
LICENSE = "MIT"

require hach-image.bb

IMAGE_INSTALL += " \
    suspend-resume \
    rsync \
    htop \
    devmem2 \
    mtd-utils \
	tzdata \
"

# Add predefined runtime package groups.
# qtcreator-debug
IMAGE_FEATURES += " \
    eclipse-debug \
    debug-tweaks \
    tools-debug \
"

export IMAGE_BASENAME = "hach-image-dbg"
