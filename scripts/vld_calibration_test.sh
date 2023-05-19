#!/bin/bash
#
# vld_calibration_test.sh [0,1]
#   - Script to enable (1) / disable (0) random pulser to selected channels
#
#

###########
# Settings
###########

# VLD Slot to use
#VLDSLOT="vme4VLD17"
#VLDSLOT="vme4VLD16"
#VLDSLOT="vme4VLD15"
#VLDSLOT="vme4VLD18"
VLDSLOT="vme4VLD13"
#VLDSLOT="vme4VLD19"
#VLDSLOT="vme4VLD20"

# keep 14 for other tests
#VLDSLOT="vme4VLD14"

# Random Pulser Setting (700kHz / 2^n) n=[0,15]
#  0: 700kHz
#  1: 350kHz
#  2: 175kHz
#  3: 87.5kHz
# ...
#  7: 5.5kHz
# ...
# 15: 21 Hz
PULSER_SETTING=15
#PULSER_SETTING=10

# Trigger Source
#   0: always high (should be Periodic trigger), this is for fixed period
#   1: random trigger only; average frequency (meaning the period can vary betwen +/- something)
#   2: FP_trigger (external trigger plugged in at "IN" front pannel)
TRIG_SOURCE=1

# Connector Channel Enable Masks
C0_LOW_MASK=0x0
C0_HIGH_MASK=0x0

C1_LOW_MASK=0x0
#C1_LOW_MASK=0x1
#C1_LOW_MASK=0x2
C1_LOW_MASK=0x4
#C1_LOW_MASK=0x8
#C1_LOW_MASK=0x10
#C1_LOW_MASK=0x20
#C1_LOW_MASK=0x40
#C1_LOW_MASK=0x80
#C1_LOW_MASK=0x100
#C1_LOW_MASK=0x200
#C1_LOW_MASK=0x400
#C1_LOW_MASK=0x800
#C1_LOW_MASK=0x1000
#C1_LOW_MASK=0x2000
#C1_LOW_MASK=0x4000
#C1_LOW_MASK=0x8000
#C1_LOW_MASK=0x10000
#C1_LOW_MASK=0x20000

C1_HIGH_MASK=0x0
#C1_HIGH_MASK=0x1
#C1_HIGH_MASK=0x2
#C1_HIGH_MASK=0x4
#C1_HIGH_MASK=0x8
#C1_HIGH_MASK=0x10
#C1_HIGH_MASK=0x20
#C1_HIGH_MASK=0x40
#C1_HIGH_MASK=0x80
#C1_HIGH_MASK=0x100
#C1_HIGH_MASK=0x200
#C1_HIGH_MASK=0x400
#C1_HIGH_MASK=0x800
#C1_HIGH_MASK=0x1000
#C1_HIGH_MASK=0x2000
#C1_HIGH_MASK=0x4000
#C1_HIGH_MASK=0x8000
#C1_HIGH_MASK=0x10000
#C1_HIGH_MASK=0x20000
















#################################################

C2_LOW_MASK=0x0
#C2_LOW_MASK=0x1
#C2_LOW_MASK=0x2
#C2_LOW_MASK=0x4
#C2_LOW_MASK=0x8
#C2_LOW_MASK=0x10
#C2_LOW_MASK=0x20
#C2_LOW_MASK=0x40
#C2_LOW_MASK=0x80
#C2_LOW_MASK=0x100
#C2_LOW_MASK=0x200
#C2_LOW_MASK=0x400
#C2_LOW_MASK=0x800
#C2_LOW_MASK=0x1000
#C2_LOW_MASK=0x2000
#C2_LOW_MASK=0x4000
#C2_LOW_MASK=0x8000
#C2_LOW_MASK=0x10000
#C2_LOW_MASK=0x20000

C2_HIGH_MASK=0x0
#C2_HIGH_MASK=0x1
#C2_HIGH_MASK=0x2
#C2_HIGH_MASK=0x4
#C2_HIGH_MASK=0x8
#C2_HIGH_MASK=0x10
#C2_HIGH_MASK=0x20

#C2_HIGH_MASK=0x40
#C2_HIGH_MASK=0x80
#C2_HIGH_MASK=0x100
#C2_HIGH_MASK=0x200
#C2_HIGH_MASK=0x400
#C2_HIGH_MASK=0x800
#C2_HIGH_MASK=0x1000
#C2_HIGH_MASK=0x2000
#C2_HIGH_MASK=0x4000
#C2_HIGH_MASK=0x8000
#C2_HIGH_MASK=0x10000
#C2_HIGH_MASK=0x20000

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
	echo "Disable VLD"
	TRIG_SOURCE=0
	PULSER_ENABLE=0

	C0_LOW_MASK=0
	C0_HIGH_MASK=0
	C1_LOW_MASK=0
	C1_HIGH_MASK=0
	C2_LOW_MASK=0
	C2_HIGH_MASK=0
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
