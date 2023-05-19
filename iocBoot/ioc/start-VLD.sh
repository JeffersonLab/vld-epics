#!/bin/bash
#
# startIOC
#
export LINUXVME=${HOME}/nps-vme/linuxvme
export LINUXVME_INC=${LINUXVME}/include
export LINUXVME_LIB=${LINUXVME}/${MACHINE}/lib
export LINUXVME_BIN=${LINUXVME}/${MACHINE}/bin

export TELNETLIB_PATH=${HOME}/nps-vme/EPICS/R3-15-9/extensions/lib/linux-x86_64
export LD_LIBRARY_PATH=.:${LINUXVME_LIB}:${LD_LIBRARY_PATH}:${TELNETLIB_PATH}

source ${HOME}/nps-vme/EPICS/nps_epics_setup.sh
cd ${HOME}/nps-vme/EPICS/apps/vld-epics/iocBoot/ioc

STCMD=./st.cmd

case $HOSTNAME in
    nps-vme1*)
	STCMD=./st.cmd.vme1
    ;;
    nps-vme2*)
	STCMD=./st.cmd.vme2
    ;;
    nps-vme5*)
	STCMD=./st.cmd.vme5
    ;;
    *)
	echo "Not configured for $HOSTNAME"
	exit
    ;;
esac
echo STCMD = $STCMD

$STCMD
