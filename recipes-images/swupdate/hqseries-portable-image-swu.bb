DESCRIPTION = "Compound image for HQ-Series Handheld Meter platform."
SECTION = ""

# Note: sw-description is mandatory
SRC_URI = " \
    file://sw-description \
    file://update.sh \
"

inherit swupdate

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

# IMAGE_DEPENDS: list of Yocto images that contains a root filesystem it'll be ensured they're built before creating 
#                swupdate image
IMAGE_DEPENDS = "hqseries-portable-image-swupdate"

# SWUPDATE_IMAGES: list of images that will be part of the compound image the list can have any binaries - images must 
#                  be in the DEPLOY directory.
SWUPDATE_IMAGES = " \
    hqseries-portable-image-swupdate \
"

# Images can have multiple formats - define which image must be taken to be put in the compound image
SWUPDATE_IMAGES_FSTYPES[var-image-swupdate] = ".tar.gz"
