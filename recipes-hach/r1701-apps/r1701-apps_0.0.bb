# Yocto generated CMake toolchain file:
# /opt/hachdev/buildsystem/r1701/build/tmp/work/cortexa7hf-neon-dey-linux-gnueabi/r1701-applications/0.0-r0/toolchain.make

DESCRIPTION = "A recipe for building the Hach r1701 target applications."

LICENSE = "CLOSED"

DEPENDS += "zeromq"

inherit cmake pkgconfig

SRC_URI = "git://git@stash.hach.ewqg.com:7999/r1701/r1701_fw.git;branch=tim-work;protocol=ssh \
           file://r1701-apps.sh \
"
SRCREV = "${AUTOREV}"

inherit update-rc.d
INITSCRIPT_NAME = "r1701-apps.sh"
INITSCRIPT_PARAMS = "defaults 99"

# Source directory where the code will be placed.
S = "${WORKDIR}/git"

EXTRA_OECMAKE += "-DUNIT_TEST=OFF \
                  -DCMAKE_CXX_STANDARD=11 \
                  -DCMAKE_CXX_STANDARD_REQUIRED=ON \
                  -DCMAKE_CXX_EXTENSIONS=OFF \
                  -DOE_QMAKE_PATH_EXTERNAL_HOST_BINS=/opt/hachdev/buildsystem/r1701/build/tmp/sysroots/x86_64-linux/usr/bin/qt5 \
"

do_install_append() {
    # Install Noto font into /usr/share/fonts
    install -m 0755 -d ${D}${datadir}/fonts
    install -m 0755 ${S}/fonts/NotoSansPlus.ttf ${D}${datadir}/fonts

	install -d ${D}${sysconfdir} ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/r1701-apps.sh ${D}${sysconfdir}/init.d
}
