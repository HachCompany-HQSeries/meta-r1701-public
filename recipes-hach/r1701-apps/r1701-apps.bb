# The following recipe builds the r1701 applications and installs them into the target rootfs according to the CMake
# definitions within the firmware project.
#
# Yocto generated CMake toolchain file:
# /opt/hachdev/buildsystem/r1701/build/tmp/work/cortexa7hf-neon-dey-linux-gnueabi/r1701-applications/0.0-r0/toolchain.make

DESCRIPTION = "A recipe for building the Hach r1701 target applications."
LICENSE = "CLOSED"

# Build time dependencies with other recipes.
DEPENDS += "zeromq cppzmq boost sqlite3 protobuf protobuf-native qtbase qtbase-native recovery-utils gtest gtest-native cmake cmake-native gmock gmock-native"

# Use Yocto to perform CMake configuration.
inherit cmake pkgconfig

# Location of source code and artifacts used by this recipe.
SRC_URI = "gitsm://git@stash.waterqualitytools.com:7999/r1701/r1701_fw.git;branch=${FW_BRANCH};protocol=ssh \
           file://r1701-apps.sh \
           file://mca_cc6ul.bin \
           file://public.pem \
"
SRCREV = "${FW_COMMIT}"

# Configure the SysV initialization service for the sys_mgr within the r1701_applications.
inherit update-rc.d
INITSCRIPT_NAME = "r1701-apps.sh"
INITSCRIPT_PARAMS = "defaults 99"

# Source directory where the code will be placed.
S = "${WORKDIR}/git"

# Additional configuration of CMake.
EXTRA_OECMAKE += " -DUNIT_TEST=OFF \
                   -DDEBUG=ON \
                   -DENABLE_CLANG_FORMATTER=OFF \
                   -DCMAKE_CXX_STANDARD=17 \
                   -DCMAKE_CXX_STANDARD_REQUIRED=ON \
                   -DCMAKE_CXX_EXTENSIONS=OFF \
                   -DCMAKE_VERBOSE_MAKEFILE=ON \
                   -DOE_QMAKE_PATH_EXTERNAL_HOST_BINS=${STAGING_BINDIR_NATIVE}\
                   -DCMAKE_INSTALL_PREFIX=${base_prefix}/opt/hach \
                   -DCMAKE_INSTALL_SYSCONFDIR=${base_prefix}/opt/hach/ \
                   -DCMAKE_INSTALL_BINDIR=bin \
"

# Special compiler flags this appliction uses.
CFLAGS_R1701 = "-Wno-poison-system-directories -Wall -Wpointer-arith -Wno-psabi \
                -ffunction-sections -fdata-sections -feliminate-unused-debug-types"

# These definitions are required for easylogging so that when it is turned on it will include all of the needed features.
# These are added explicitly to allow the addition of the TIMED_ functions which make timing the performance of
# functions and scopes very easy.
TARGET_CFLAGS += "${CFLAGS_R1701} -DELPP_DISABLE_PERFORMANCE_TRACKING -DELPP_THREAD_SAFE -DINI_PATH=/opt/hach/configs/sys_r1701.cfg"
TARGET_CXXFLAGS += "${CFLAGS_R1701} -DELPP_DISABLE_PERFORMANCE_TRACKING -DELPP_THREAD_SAFE -DINI_PATH=/opt/hach/configs/sys_r1701.cfg"

do_install_append() {
    # Install the SysV init script to the rootfs.
    install -d ${D}${sysconfdir} ${D}${sysconfdir}/init.d
    install -m 0755 ${WORKDIR}/r1701-apps.sh ${D}${sysconfdir}/init.d

    install -m 0755 ${WORKDIR}/mca_cc6ul.bin ${D}/opt/hach/bin

    # Install RSA public key for verifying firmware images
    install -d ${D}${sysconfdir}/ssl/certs
    install -m 0600 ${WORKDIR}/public.pem ${D}${sysconfdir}/ssl/certs/key.pub
}

# Create /opt directory for r1701 apps installation.
FILES_${PN} += " \
    ${base_prefix}/opt \
    ${base_prefix}/opt/hach/bin/mca_cc6ul.bin \
"
