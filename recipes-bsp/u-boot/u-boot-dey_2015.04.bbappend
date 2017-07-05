FILESEXTRAPATHS_prepend := "${THISDIR}/u-boot-dey-2015.04:"

SRCREV = "0de0373ade11801e9ce9eed20d31ab3fd37306dd"
SRC_URI = " \
    git://github.com/digi-embedded/u-boot.git;branch=v2015.04/maint \
    file://0001-Add-new-platform-for-r1701.patch \
"
