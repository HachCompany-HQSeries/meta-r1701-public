FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}-${PV}:"

# We are overwriting the new commit id. This SHA is just one commit further than the one DIGI has defined in meta-digi
# layer. This commit ID add WATCHDOG_MAGICCLOSE feature to MCA watch dog driver. It is needed for is. We can crate a
# patch for it for since digi has done and this commit ID has only this watch dog realted change so why not use override
# SRCREV.
SRCREV = "4e73388cd70e3071cb4806f1fbecde2c87a5c15d"

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
    file://0009-Adding-r1701-device-trees.patch \
"
