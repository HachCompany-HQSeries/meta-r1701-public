#!/bin/sh
case "$1" in
  start)
        # set cpu to the performance level, maximum frequency i.e. 528 MHz
        echo performance > /sys/bus/cpu/devices/cpu0/cpufreq/scaling_governor

        # Update MCA firmware and set W4PK mode
        MCA_VERSION=$(cat /sys/class/i2c-dev/i2c-0/device/0-007e/fw_version)
        echo "MCA FW Ver # $MCA_VERSION"

        # NOTE:: Do not do MCA FW update from here otherwise some first time initialization will corrupt NAND. This
        #        Update will hard boot as soon as MCA FW update is done and this will leave below mentioned first time
        #        INIT service write operation on NAND in corrupt state. DO NOT DO MCA FW UPDATE
        #        Ex.
        #        onfiguring packages on first boot....
        #        (This may take several minutes. Please do not power off the machine.)
        #        Running postinst /etc/rpm-postinsts/100-init-ifupdown...
        #        Running postinst /etc/rpm-postinsts/101-u-boot-fw-utils...
        #        update-rc.d: /etc/init.d/run-postinsts exists during rc.d purge (continuing)
        #        Removing any system startup links for run-postinsts ...
        #        /etc/rcS.d/S99run-postinsts
        #        INIT: Entering runlevel: 5

        #if [ $MCA_VERSION != '1.15' ]; then
        #    echo "Current MCA FW version (${MCA_VERSION}) is not latest, has to be >= 1.15. Updating to latest..."
        #    echo "Meter will reboot automatically after successful MCA FW update..."
        #    sync
        #    sync
        #    mca_fw_updater -f /opt/hach/bin/mca_cc6ul.bin
        #fi

        # Make sure we have set W4PK (Wait for Power key press for boot mode.)
        MCA_BOOT_MODE=$(mca_config_tool --boot_mode)
        echo $MCA_BOOT_MODE
        MCA_BOOT_MODE=`echo $MCA_BOOT_MODE | awk '{print $5}'`
        echo "Extracted MCA boot flag # $MCA_BOOT_MODE"
        if [ "$MCA_BOOT_MODE" != "W4PK" ]; then
            echo "Setting MCA Boot mode to W4PK..."
            mca_config_tool --boot_mode=W4PK &
        fi

        # TODO:: We have to make sure that we have correct "/etc/fw_env.config" file. Word count has to be close to 480
        #        If it is 0 then we have issue with RFS where Kernel/RFS not able to read u-boot ENV variables. This 
        #        will not only affects the SD card partition but run time HQD application as well due to unable to read
        #        model name, type, SN and many more other ENV variables that we use during APP runtime.
        UBOOT_ENV_VER_FILE_WORD_CNT=$(cat /etc/fw_env.config | wc -c)
        echo "U-Boot ENV file --> /etc/fw_env.config SIZE ### $UBOOT_ENV_VER_FILE_WORD_CNT"
        if [ "$UBOOT_ENV_VER_FILE_WORD_CNT" != "480" ]; then
            echo "HQ Series meter application can not run properly without it!!!"
        fi

        # Handle SD card. Format SD card if first partition (ext4 - For user) and second partition (ext4 - Internal)
        # is not available.
        sh /opt/hach/bin/formatsd

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

        # if software update required is true then we have to restore settings back.
        swUpdateReq=$(fw_printenv -n "sw-update-req" 2>/dev/null)
        if [ "${swUpdateReq}" = "1" ]; then
            echo "Software update done... restoring settings..."
            mkdir -p /opt/hach/settings

            # NOTE:: Let's not restore settings for now, we are constantly changing PROTOBUF, restorng might break unit.
            #cp -fa /run/media/mmcblk1p2/backup/settings/* /opt/hach/settings/.

            # NOTE:: Do not remove SWU file, if for some reason update does not work then customer can upgrade again.
            #rm -rf /run/media/mmcblk1p1/*.swu
            sync

            # since factory restore is done, reset u-boot variable to 0.
            fw_setenv sw-update-req 0

            # Make sure that we have reset the flag.
            swUpdateReq=$(fw_printenv -n "sw-update-req" 2>/dev/null)
            if [ "${swUpdateReq}" = "1" ]; then
                echo "ERROR # Restore settings failed!!!"
                fw_setenv sw-update-req 0
            else
                echo "Settings restored..."
            fi
        fi

        # Manage factory restore.
        factoryRestore=$(fw_printenv -n "factory-restore" 2>/dev/null)
        if [ "${factoryRestore}" = "1" ]; then
            echo "Factory restore requested..."

            # Delete all user settings
            rm -rf /opt/hach/settings/*

            # Delete all exceptions files.
            rm -rf /opt/hach/exceptions/*

            # Delete data base and other stuff on SD card.
            rm -rf /run/media/mmcblk1p2/*
            rm -rf /run/media/mmcblk1p1/*
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
