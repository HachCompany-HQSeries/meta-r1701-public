# Set virtual runtimes depending on X11 feature
VIRTUAL-RUNTIME_touchscreen = ""
VIRTUAL-RUNTIME_keymaps = ""

RDEPENDS_${PN} = "\
    base-files \
    base-passwd \
    ${@bb.utils.contains("MACHINE_FEATURES", "keyboard", "${VIRTUAL-RUNTIME_keymaps}", "", d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', '', bb.utils.contains("MACHINE_FEATURES", "rtc", "${VIRTUAL-RUNTIME_base-utils-hwclock}", "", d), d)} \
    ${@bb.utils.contains("MACHINE_FEATURES", "touchscreen", "${VIRTUAL-RUNTIME_touchscreen}", "",d)} \
    init-ifupdown \
    modutils-initscripts \
    netbase \
    networkmanager \
    os-release \
    recovery-utils \
    sysinfo \
    usbutils \
    ${VIRTUAL-RUNTIME_base-utils} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', '', '${VIRTUAL-RUNTIME_base-utils-acpid}', d)} \
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
    ${VIRTUAL-RUNTIME_base-utils-syslog} \
    ${MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS} \
    ${MACHINE_EXTRA_RRECOMMENDS} \
"
