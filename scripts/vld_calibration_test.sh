#!/bin/bash
#
# vld_calibration_test.sh
#
#

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

###########
# Settings
###########

# VLD Slot to use
VLDSLOT="vme4VLD17"

# Random Pulser Setting
# 0: 700kHz
# 1: 350kHz
# 2: 175kHz
# 3: 87.5kHz
# ...
# 7: 5.5kHz
PULSER_SETTING=7

# Trigger Source
#   0: always high (should be Periodic trigger),
#   1: random trigger only
#   2: FP_trigger
TRIG_SOURCE=1

# Connector Channel Enable Masks
C0_LOW_MASK=0xa
C0_HIGH_MASK=0x0

C1_LOW_MASK=0x1
C1_HIGH_MASK=0x0

C2_LOW_MASK=0x1
C2_HIGH_MASK=0x0

C3_LOW_MASK=0x1
C3_HIGH_MASK=0x0

C4_LOW_MASK=0x1
C4_HIGH_MASK=0x0

case $STATE in
    "disable" )
	echo "Disable VLD"
	PULSER_ENABLE=0

	C0_LOW_MASK=0
	C0_HIGH_MASK=0
	C1_LOW_MASK=0
	C1_HIGH_MASK=0
	C2_LOW_MASK=0
	C2_HIGH_MASK=0
	C3_LOW_MASK=0
	C3_HIGH_MASK=0
	C4_LOW_MASK=0
	C4_HIGH_MASK=0
	;;
    "enable" )
	echo "Enable VLD"
	PULSER_ENABLE=1

esac

# Execute
$CAPUT $VLDSLOT:TriggerSourceMask $TRIG_SOURCE
$CAPUT $VLDSLOT:RandomPulserRateSet $PULSER_SETTING
$CAPUT $VLDSLOT:RandomPulserEnable $PULSER_ENABLE

# convert hex to dec for caput args
$CAPUT $VLDSLOT:C0:LowChannelMask $(printf "%d" $C0_LOW_MASK)
$CAPUT $VLDSLOT:C0:HighChannelMask $(printf "%d" $C0_HIGH_MASK)

$CAPUT $VLDSLOT:C1:LowChannelMask $(printf "%d" $C1_LOW_MASK)
$CAPUT $VLDSLOT:C1:HighChannelMask $(printf "%d" $C1_HIGH_MASK)

$CAPUT $VLDSLOT:C2:LowChannelMask $(printf "%d" $C2_LOW_MASK)
$CAPUT $VLDSLOT:C2:HighChannelMask $(printf "%d" $C2_HIGH_MASK)

$CAPUT $VLDSLOT:C3:LowChannelMask $(printf "%d" $C3_LOW_MASK)
$CAPUT $VLDSLOT:C3:HighChannelMask $(printf "%d" $C3_HIGH_MASK)

$CAPUT $VLDSLOT:C4:LowChannelMask $(printf "%d" $C4_LOW_MASK)
$CAPUT $VLDSLOT:C4:HighChannelMask $(printf "%d" $C4_HIGH_MASK)
