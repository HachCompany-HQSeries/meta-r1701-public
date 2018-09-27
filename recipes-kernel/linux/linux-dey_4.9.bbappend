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
    file://0005-Add-HACH-log-image.patch \
    file://0006-Do-not-add-Power-Key-SLEEP-event.patch \
    file://0007-Update-the-battery-charger-driver-for-R1701.patch \
    file://0008-NAND-suspend-resume-initialization-issue-patch.patch \
    file://0009-Adding-HPP-and-MPP-device-trees.patch \
"
