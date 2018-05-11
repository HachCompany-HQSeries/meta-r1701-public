# The following recipe builds the r1701 applications and installs them into the target rootfs according to the CMake
# definitions within the firmware project.
#
# Yocto generated CMake toolchain file:
# /opt/hachdev/buildsystem/r1701/build/tmp/work/cortexa7hf-neon-dey-linux-gnueabi/r1701-applications/0.0-r0/toolchain.make

DESCRIPTION = "A recipe for building the Hach r1701 target applications."
LICENSE = "CLOSED"

# Build time dependencies with other recipes.
DEPENDS += "zeromq boost sqlite3 protobuf protobuf-native qtbase qtbase-native recovery-utils"

# Use Yocto to perform CMake configuration.
inherit cmake pkgconfig

# Location of source code and artifacts used by this recipe.
SRC_URI = "git://git@stash.hach.ewqg.com:7999/r1701/r1701_fw.git;branch=develop;protocol=ssh \
           file://r1701-apps.sh \
"
SRCREV = "${AUTOREV}"

# Configure the SysV initialization service for the sys_mgr within the r1701_applications.
inherit update-rc.d
INITSCRIPT_NAME = "r1701-apps.sh"
INITSCRIPT_PARAMS = "defaults 99"

# Source directory where the code will be placed.
S = "${WORKDIR}/git"

# Additional configuration of CMake.
EXTRA_OECMAKE += "-DUNIT_TEST=OFF \
                  -DENABLE_CLANG_FORMATTER=OFF \
                  -DCMAKE_CXX_STANDARD=11 \
                  -DCMAKE_CXX_STANDARD_REQUIRED=ON \
                  -DCMAKE_CXX_EXTENSIONS=OFF \
                  -DOE_QMAKE_PATH_EXTERNAL_HOST_BINS=${STAGING_BINDIR_NATIVE}/qt5 \
                  -DCMAKE_INSTALL_PREFIX=${base_prefix}/opt/hach \
                  -DCMAKE_INSTALL_SYSCONFDIR=${base_prefix}/opt/hach/ \
                  -DCMAKE_INSTALL_BINDIR=bin \
"

do_install_append() {
    # Install the SysV init script to the rootfs.
    install -d ${D}${sysconfdir} ${D}${sysconfdir}/init.d
    install -m 0755 ${WORKDIR}/r1701-apps.sh ${D}${sysconfdir}/init.d
}

# Create /opt directory for r1701 apps installation.
FILES_${PN} += " \
    ${base_prefix}/opt \
"
