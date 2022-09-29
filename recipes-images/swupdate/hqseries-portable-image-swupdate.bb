# Copyright (C) 2022 HACH Company
# Released under the MIT license (see COPYING.MIT for the terms)

DESCRIPTION = "HACH HQ-Series Handheld Meter Image based on SWUPDATE."
LICENSE = "MIT"

require recipes-images/images/hqseries-portable-image.bb

CORE_IMAGE_EXTRA_INSTALL += " \
    swupdate \
    swupdate-www \
    kernel-image \
    kernel-devicetree \
"

IMAGE_FSTYPES = "tar.gz"
