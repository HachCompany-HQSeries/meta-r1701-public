SUMMARY = "QT packagegroup for HACH HqSeries Handheld Meter image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

# Install Freescale QT demo applications
QT5_APPS = ""
QT5_APPS_imxgpu3d = ""

# Install fonts (TODO: Remove them later...)
QT5_FONTS = " \
    ttf-dejavu-mathtexgyre \
    ttf-dejavu-sans \
    ttf-dejavu-sans-condensed \
    ttf-dejavu-sans-mono \
    ttf-dejavu-serif \
    ttf-dejavu-serif-condensed \
"

# uncomment the following line to add webengine support
# but remind to add also meta-python2 to the bblayere
# QT5_IMAGE_INSTALL_append = " packagegroup-qt5-webengine"

# uncomment the following line to add webkit support
# but remind that is considered obsolete since Qt 5.7
# QT5_IMAGE_INSTALL_append = " packagegroup-qt5-webkit"

# Install Freescale QT demo applications for X11 backend only
MACHINE_QT5_MULTIMEDIA_APPS = ""
QT5_RDEPENDS = ""
QT5_RDEPENDS_common = " \
    packagegroup-qt5-demos \
    ${QT5_FONTS} \
    ${QT5_APPS} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11', 'libxkbcommon', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'qtwayland qtwayland-plugins', '', d)}\
"

QT5_RDEPENDS_imxgpu2d = "${@bb.utils.contains('DISTRO_FEATURES', 'x11','${QT5_RDEPENDS_common}', \
    'qtbase qtbase-plugins', d)}"

# qtbase-examples, Do we need --> qtquickcontrols2 qtquickcontrols2-qmlplugins
QT5_RDEPENDS_imxpxp = "${@bb.utils.contains('DISTRO_FEATURES', 'x11','${QT5_RDEPENDS_common}', \
    'qtbase qtbase-plugins qtquickcontrols2 qtquickcontrols2-qmlplugins', d)}"

QT5_RDEPENDS_imxgpu3d = " \
    ${QT5_RDEPENDS_common} \
    gstreamer1.0-plugins-good-qt \
"

QT5_IMAGE_INSTALL_append_imxgpu3d = ""

# Add packagegroup-qt5-webengine to QT5_RDEPENDS_mx6 and comment out the line below to install qtwebengine to the rootfs.
QT5_RDEPENDS_remove = " packagegroup-qt5-webengine packagegroup-qt5-webkit"

RDEPENDS_${PN} += " \
    liberation-fonts \
    ${QT5_RDEPENDS} \
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
