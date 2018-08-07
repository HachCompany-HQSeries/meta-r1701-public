# Set virtual runtimes depending on X11 feature
VIRTUAL-RUNTIME_touchscreen = ""
VIRTUAL-RUNTIME_keymaps = ""

RDEPENDS_${PN} = "\
    base-files \
    base-passwd \
    busybox \
    busybox-acpid \
    busybox-static-nodes \
    ${@bb.utils.contains("MACHINE_FEATURES", "rtc", "busybox-hwclock", "", d)} \
    init-ifupdown \
    modutils-initscripts \
    netbase \
    networkmanager \
    os-release \
    recovery-utils \
    usbutils \
    ${VIRTUAL-RUNTIME_dev_manager} \
    ${VIRTUAL-RUNTIME_init_manager} \
    ${VIRTUAL-RUNTIME_initscripts} \
    ${VIRTUAL-RUNTIME_login_manager} \
    ${VIRTUAL-RUNTIME_passwd_manager} \
    ${VIRTUAL-RUNTIME_update-alternatives} \
    ${MACHINE_ESSENTIAL_EXTRA_RDEPENDS} \
    ${MACHINE_EXTRA_RDEPENDS} \
"

RRECOMMENDS_${PN} = "\
    ${MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS} \
    ${MACHINE_EXTRA_RRECOMMENDS} \
"
