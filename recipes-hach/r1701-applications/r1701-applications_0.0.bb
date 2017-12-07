# Yocto generated CMake toolchain file:
# /opt/hachdev/buildsystem/r1701/build/tmp/work/cortexa7hf-neon-dey-linux-gnueabi/r1701-applications/0.0-r0/toolchain.make


DESCRIPTION = "A recipe for building the Hach r1701 target applications."

LICENSE = "CLOSED"

inherit cmake

SRC_URI = "git://git@stash.hach.ewqg.com:7999/r1701/r1701_fw.git;branch=develop;protocol=ssh"
SRCREV = "${AUTOREV}"

# Source directory where the code will be placed.
S = "${WORKDIR}/git"

EXTRA_OECMAKE = "-DUNIT_TEST=OFF"
