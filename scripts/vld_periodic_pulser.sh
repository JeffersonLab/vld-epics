#!/bin/bash
#
# vld_periodic_pulser.sh  [period]  [period_range]  [number]
#   - Script to modify VLD periodic pulser parameters
#                     [period]       :  Pulser period in units of period_range
#                     [period_range] : 0: 8 nanoseconds
#                                    : 1: 8.2 microseconds
#                     [number]       : Number of pulses.  65535 for unlimited
#
#

###########
# Settings
###########

# VLD Slot to use
VLDSLOT="vme4VLD17"

# Periodic Pulser Period
#    period = PULSER_PERIOD * PULSER_PERIOD_RANGE
#   PULSER_PERIOD_RANGE = 0 (for 8ns steps)
#                         1 (for 8192ns steps)
PULSER_PERIOD=9
PULSER_PERIOD_RANGE=1
PULSER_NUMBER=65535

# Trigger Source
#   0: always high (should be Periodic trigger), this is for fixed period
#   1: random trigger only; average frequency (meaning the period can vary betwen +/- something)
#   2: FP_trigger (external trigger plugged in at "IN" front pannel)
TRIG_SOURCE=0

if [ -z "$3" ]; then
    echo "Usage:"
    echo "  `basename $0` [period]  [period_range]  [number]"
    echo "                     [period]       :  Pulser period in units of period_range"
    echo "                     [period_range] : 0: 8 nanoseconds"
    echo "                                    : 1: 8.2 microseconds"
    echo "                     [number]       : Number of pulses.  65535 for unlimited"
    exit
else
    PULSER_PERIOD=$1
    PULSER_PERIOD_RANGE=$2
    PULSER_NUMBER=$3
fi

# CAPUT exec path
CAPUT=caput


# Printout
echo "Set VLD Periodic pulser "

# Execute
$CAPUT $VLDSLOT:TriggerSourceMask $TRIG_SOURCE
$CAPUT $VLDSLOT:PeriodicPulserPeriod $PULSER_PERIOD
$CAPUT $VLDSLOT:PeriodicPulserPeriodRange $PULSER_PERIOD_RANGE
$CAPUT $VLDSLOT:PeriodicPulserNumber $PULSER_NUMBER
