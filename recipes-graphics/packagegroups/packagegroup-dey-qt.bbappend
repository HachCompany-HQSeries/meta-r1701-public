#
# Copyright (C) 2013-2017, Digi International Inc.
#
SUMMARY = "QT packagegroup for HACH image"

# Do not install.
QT5_APPS_imxgpu3d = ""

# Do not install fonts.
QT5_FONTS = ""

QT5_RDEPENDS_common = ""

QT5_RDEPENDS_imxgpu2d = "${@bb.utils.contains('DISTRO_FEATURES', 'x11','${QT5_RDEPENDS_common}', \
    'qtbase qtbase-plugins', d)}"

QT5_RDEPENDS_imxpxp = "${@bb.utils.contains('DISTRO_FEATURES', 'x11','${QT5_RDEPENDS_common}', \
    'qtbase qtbase-plugins', d)}"

QT5_RDEPENDS_imxgpu3d = " \
    ${QT5_RDEPENDS_common} \
    gstreamer1.0-plugins-good-qt \
"

# We do not want any example and demos in final image.
QT5_EXAMPLES = ""
QT5_DEMOS = ""
MACHINE_QT5_EXTRA_INSTALL = ""

# No need to have these dependancies.
RDEPENDS_${PN}_remove += " \
    liberation-fonts \
    ${QT5_DEMOS} \
    ${QT5_EXAMPLES} \
"

