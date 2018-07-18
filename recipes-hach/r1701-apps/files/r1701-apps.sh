#!/bin/sh
case "$1" in
  start)
        modeltype=$(fw_printenv -n "model-type" 2>/dev/null)
        if [ "${modeltype}" = "HQ_MPP" ]; then
            modprobe -r mipdisplay-3-2-inch
            modprobe -r mxsfb
            modprobe mipdisplay-3-2-inch
        else
            modprobe -r mipdisplay-3-2-inch
        fi
        echo "Model Type: ${modeltype}"
        export QT_QPA_PLATFORM=linuxfb:fb=/dev/fb0
        export QT_QPA_FB_DISABLE_INPUT=1
        export QMLSCENE_DEVICE=softwarecontext
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
