FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}-${PV}:"

#
# Recipe: https://layers.openembedded.org/layerindex/recipe/97507/
# FTP URL: http://microwindows.censoft.com/pub/microwindows/
# Digi URL: https://github.com/digi-embedded/meta-digi/blob/thud/meta-digi-dey/recipes-graphics/microwindows/microwindows_0.91.bb
#

#
# NOTE: Only making the change at main source level URL chagne to HTTP instead of FTP since upstream Site has changed.
#       Two pathch files are untouched and added as override here so that build works file, Patch_1: From Main BB recipe
#       and PATCH_2 is Hach created.
#

# Override the main Source URL since FTP site is not longer available.
SRC_URI = " \
    http://microwindows.censoft.com/pub/microwindows/microwindows-src-${PV}.tar.gz \
    file://0001-defconfig.patch;striplevel=2 \
    file://0002-Use-14x16-font.patch;striplevel=2 \
"
