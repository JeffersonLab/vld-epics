#!/bin/bash
#
# vld_bleach.sh [0,1]
#   - Script to enable (1) / disable (0) bleaching to selected channels
#
#

# VLD Slot to use
VLDSLOT="vme4VLD14"

# Bleach Time units [20ns * 1024 * 1024], 43980 is about 925 seconds
BLEACH_TIME=43980

# LDO Regulator setting [0,7]
BLEACH_LDO_SETTING=1

# Connector to enable / disable
BLEACH_CONNECTOR="C0"

STATE=disable

if [ -z "$1" ]; then
    echo "Usage:"
    echo "  `basename $0` [0,1]"
    echo "                     0: disable"
    echo "                     1: enable"
    exit
else
    case $1 in
	"0" ) STATE=disable;;
	"1" ) STATE=enable;;
	* )
	    echo "Usage:"
	    echo "  `basename $0` [0,1]"
	    echo "                     0: disable"
	    echo "                     1: enable"
	    exit;;
    esac
fi

# CAPUT exec path
CAPUT=caput

case $STATE in
    "disable" )
	echo "Disable VLD Bleaching on $VLDSLOT"
	# Enable Bleach Timer
	$CAPUT $VLDSLOT:BleachTime 0
	$CAPUT $VLDSLOT:BleachTimerEnable 0

	# Set LDO and Enable Bleaching
	$CAPUT $VLDSLOT:${BLEACH_CONNECTOR}:LDOCurrentSetting 0
	$CAPUT $VLDSLOT:${BLEACH_CONNECTOR}:LDOEnable 0
	;;
    "enable" )
	echo "Enable VLD Bleaching on $VLDSLOT ${BLEACH_CONNECTOR}"
	# Enable Bleach Timer
	$CAPUT $VLDSLOT:BleachTime $BLEACH_TIME
	$CAPUT $VLDSLOT:BleachTimerEnable 1

	# Set LDO and Enable Bleaching
	$CAPUT $VLDSLOT:${BLEACH_CONNECTOR}:LDOCurrentSetting ${BLEACH_LDO_SETTING}
	$CAPUT $VLDSLOT:${BLEACH_CONNECTOR}:LDOEnable 1
	;;
esac
