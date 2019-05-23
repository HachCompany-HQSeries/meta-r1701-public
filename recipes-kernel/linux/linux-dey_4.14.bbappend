FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}-${PV}:"

# Making GIT clone protocol as HTTPS. Somehow github cloning using default git protocol is quite slow. JTTPS seems to be
# working faster.
SRC_URI = " \
    ${LINUX_GIT_URI};protocol=https;nobranch=1 \
    file://defconfig \
"

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
    file://0010-Adding-HPP-and-MPP-device-trees.patch \
"
