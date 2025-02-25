FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}-${PV}:"

# Making GIT clone protocol as HTTPS. Somehow github cloning using default git protocol is quite slow. JTTPS seems to be
# working faster.
SRC_URI = " \
    ${LINUX_GIT_URI};protocol=https;nobranch=1 \
    file://defconfig \
"

# Using special version of Linux. See git log for why.. see the fixes they have done. We need those.
LOCALVERSION = "-dey"
SRCBRANCH = "v4.14/dey-2.6/maint"
SRCREV = "761bac6ff9eadb90c1acacdb120b749de7e724b1"

SRC_URI_append = " \
    file://0001-Adding-hach-r1701-as-carrier-board-in-to-OTP-kernel.patch \
    file://0002-Adding-sharp-MIP-display-support.patch \
    file://0003-Fix-for-matrix-keypad-column-gpio-scanning-issue.patch \
    file://0004-Color-LCD-driver-patch.patch \
    file://0005-Adding-HPP-and-MPP-Hach-logos.patch \
    file://0006-Update-the-battery-charger-driver-for-R1701.patch \
    file://0007-NAND-suspend-resume-initialization-issue-patch.patch \
    file://0008-Fix-pressure-sensor-crc4-check.patch \
    file://0009-Exporting-fix-regulator-GPIO-pin-to-sysfs-for-usersp.patch \
    file://0010-Fix-for-SDMA-firmware-loading-during-kernel-boot-rat.patch \
    file://0011-Adding-HPP-and-MPP-device-trees.patch \
    file://0012-Configure-PWM-output-to-GPIO-in-disabled-state.patch \
"
