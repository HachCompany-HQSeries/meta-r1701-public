DESCRIPTION = "A recipe for building Lightweight Media Transfer Protocol (MTP) responder daemon for GNU/Linux."
LICENSE = "GPLv3"
LIC_FILES_CHKSUM = "file://LICENSE;md5=d32239bcb673463ab874e80d47fae504"

# Location of source code and artifacts used by this recipe. From ORIGIN
# https://github.com/viveris/uMTP-Responder.git
#SRC_URI = "git://github.com/viveris/uMTP-Responder.git;branch=master;protocol=https \
#           file://umtprd.conf \
#           file://umtprd.sh \
#"

# Location of source code and artifacts used by this recipe from HACH forked address.
# https://github.com/viveris/uMTP-Responder.git
SRC_URI = "git://github.com/HachCompany-HQSeries/uMTP-Responder.git;branch=master;protocol=https \
           file://umtprd.conf \
           file://umtprd.sh \
"

# Version - v0.14.1
# https://github.com/viveris/uMTP-Responder/commit/78d244b30dd4c2a8f04a86a92f9ec51cf08756e6 (ORIGIN Link)
# https://github.com/HachCompany-HQSeries/uMTP-Responder/commit/78d244b30dd4c2a8f04a86a92f9ec51cf08756e6 (Forked Link)
SRCREV = "78d244b30dd4c2a8f04a86a92f9ec51cf08756e6"

# Configure the SysV initialization service for the sys_mgr within the r1701_applications.
inherit update-rc.d
INITSCRIPT_NAME = "umtprd.sh"
INITSCRIPT_PARAMS = "defaults 98"

# Source directory where the code will be placed.
S = "${WORKDIR}/git"

inherit pkgconfig

# Additional configuration of Make. Note: syslog support and debug output options can be enabled separately.
#TARGET_CFLAGS += "-DUSE_SYSLOG -DDEBUG"
TARGET_CFLAGS += "-DUSE_SYSLOG"
#TARGET_CFLAGS += "-DDEBUG"

# Create /opt directory for UMTP scripts installation.
FILES_${PN} += " \
    ${base_prefix}/opt \
"

do_compile() {
    oe_runmake
}

do_install() {
    # Install umtprd.conf file in "/etc/umtprd/umtprd.conf", this file must be here for umtprd daemon to start properly.
    install -d ${D}${sysconfdir} ${D}${sysconfdir}/umtprd
    install ${WORKDIR}/umtprd.conf ${D}${sysconfdir}/umtprd

    # Install compiled DAEMON file to /usr/bin
    install -d ${D}${bindir}
    install -m 0755 umtprd ${D}${bindir}

    #Install other scripts in /opt/hach/bin
    install -d ${D}/opt/hach ${D}/opt/hach/bin

    # Install the SysV init script to the rootfs.
    install -d ${D}${sysconfdir} ${D}${sysconfdir}/init.d
    install -m 0755 ${WORKDIR}/umtprd.sh ${D}${sysconfdir}/init.d
}
