#!/bin/sh
case "$1" in
  start)
        # Handle Display based on model type. Screen size is the total visible area. These numbers are taken from data
        # sheet.
        screen_active_area_size="mmsize=42.672x68.072"
        modeltype=$(fw_printenv -n "model-type" 2>/dev/null)
        if [ "${modeltype}" = "HQ_MPP" ]; then
            modprobe -r mipdisplay-3-2-inch
            modprobe -r mxsfb pwm_bl
            modprobe mipdisplay-3-2-inch
            screen_active_area_size="mmsize=42.672x68.072"
        else
            modprobe -r mipdisplay-3-2-inch
            screen_active_area_size="mmsize=53.28x71.04"
        fi

        # Handle RJ45 (Ethernet module), firmware will disable ethernet power control line if test mode is not set.
        testmode=$(fw_printenv -n "test-mode" 2>/dev/null)
        if [ "${testmode}" = "On" ]; then
            echo "r1701 - Ethernet enabled..."
        else
            ifconfig eth0 down
            modprobe -r fec
            echo "r1701 - Ethernet disabled..."
        fi

        # Make sure that all USB related kernel modules are not loaded. Firmware will handle the USB gadget connection
        # based on meter settings.
        modprobe -r g_ether usb_f_rndis usb_f_ecm libcomposite u_ether
        ifdown usb0

        echo "Model Type: ${modeltype}"
        echo "Test Mode: ${testmode}"

        # export correct QT flags.
        export QT_QPA_PLATFORM="linuxfb:fb=/dev/fb0:${screen_active_area_size}"
        export QT_QPA_FB_DISABLE_INPUT=1
        export QMLSCENE_DEVICE=softwarecontext

        # set cpu to the performance level
        echo performance > /sys/bus/cpu/devices/cpu0/cpufreq/scaling_governor


        # Start system manager as daemon.
        start-stop-daemon --start --quiet --make-pidfile --pidfile /var/run/sys_mgr.pid --exec /opt/hach/bin/sys_mgr -- -d
        echo "r1701 - started as deamon"
        ;;
  stop)
        start-stop-daemon --stop --quiet --pidfile /var/run/sys_mgr.pid
        echo "r1701 - deamon stopped"
        ;;
  *)
        echo "Usage: $0 {start|stop}"
        exit 1
esac
exit $?
