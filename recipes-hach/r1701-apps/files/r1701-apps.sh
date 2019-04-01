#!/bin/sh
case "$1" in
  start)
        # set cpu to the performance level, maximum frequency i.e. 528 MHz
        echo performance > /sys/bus/cpu/devices/cpu0/cpufreq/scaling_governor

        # Handle Display based on model type. Screen size is the total visible area. These numbers are taken from data
        # sheet.
        screen_active_area_size="mmsize=42.672x68.072"
        modeltype=$(fw_printenv -n "model-type" 2>/dev/null)
        modelname=$(fw_printenv -n "model-name" 2>/dev/null)
        testmode=$(fw_printenv -n "test-mode" 2>/dev/null)
        if [ "${modeltype}" = "HQ_MPP" ]; then
            screen_active_area_size="mmsize=42.672x68.072"
        else
            # Unbind virtual console from framebuffer device so that unwanted image/characters do not show up on screen.
            # detach framebuffer console from console layer
            echo 0 > sys/class/vtconsole/vtcon1/bind
            screen_active_area_size="mmsize=53.28x71.04"
        fi

        # Manage factory restore.
        factoryRestore=$(fw_printenv -n "factory-restore" 2>/dev/null)
        if [ "${factoryRestore}" = "1" ]; then
            echo "Factory restore requested..."

            # Delete Database.
            rm -rf /mnt/update/*

            # Delete all user settings
            rm -rf /opt/hach/settings/*

            # Delete all exceptions files.
            rm -rf /opt/hach/exceptions/*
            sync

            # since factory restore is done, reset u-boot variable to 0.
            fw_setenv factory-restore 0

            # Make sure that we have reset the flag.
            factoryRestore=$(fw_printenv -n "factory-restore" 2>/dev/null)
            if [ "${factoryRestore}" = "1" ]; then
                echo "ERROR # Factory restore flag did not reset!"
                fw_setenv factory-restore 0
            else
                echo "Factory restore done."
            fi
        fi

        # Handle RJ45 (Ethernet module), firmware will disable ethernet power control line if test mode is not set.
        #if [ "${testmode}" = "On" ]; then
            #echo "r1701 - Ethernet enabled..."
        #else
            # ifconfig eth0 down
            # modprobe -r fec smsc libphy
            #echo "r1701 - Ethernet disabled..."
        #fi

        # Make sure that all USB related kernel modules are not loaded. Firmware will handle the USB gadget connection
        # based on meter settings.
        modprobe -r g_ether usb_f_rndis usb_f_ecm libcomposite u_ether
        ifdown usb0

        # export correct QT flags.
        export QT_QPA_PLATFORM="linuxfb:fb=/dev/fb0:${screen_active_area_size}"
        export QMLSCENE_DEVICE=softwarecontext

        # Start system manager as daemon.
        start-stop-daemon --start --quiet --make-pidfile --pidfile /var/run/sys_mgr.pid --exec /opt/hach/bin/sys_mgr -- -d
        echo "${modeltype} - started as deamon"

        # Print model name and type as well as test mode
        echo "Model Type: ${modeltype}"
        echo "Model Name: ${modelname}"
        echo "Test Mode: ${testmode}"
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
