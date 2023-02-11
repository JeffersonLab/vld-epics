#!/bin/bash
#
# vld_square_pulse.sh   [width]   [amplitude/dac]
#   - Script to set square pulse parameters for a VLD
#
#

# VLD Slot to use
VLDSLOT="vme4VLD17"

# CAPUT exec path
CAPUT=caput

WIDTH=0
DAC=0

if [ $# != 2 ]; then
    echo "Usage:"
    echo "  `basename $0` [width]  [dac]"
    echo "                     width: 2ns steps  [0,1000]"
    echo "                     dac: amplitude of pulse [0, 63]"
    exit
else
    WIDTH=$1
    DAC=$2
fi


# Execute
$CAPUT $VLDSLOT:PulseWaveformType 0
$CAPUT $VLDSLOT:SquareWaveWidth $WIDTH
$CAPUT $VLDSLOT:SquareWaveAmplitude $DAC
$CAPUT $VLDSLOT:PulseWaveformType 1
