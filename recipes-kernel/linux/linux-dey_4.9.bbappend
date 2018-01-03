FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}-${PV}:"

SRC_URI_append = " \
	file://0001-Adding-r1701-device-trees.patch \
	file://0002-Adding-hach-r1701-as-carrier-board-in-to-OTP-kernel.patch \
	file://0003-Adding-sharp-MIP-display-support-to-Kconfig-file.patch \
"
