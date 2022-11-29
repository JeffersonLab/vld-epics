TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

LIBRARY_IOC += VLD

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# install VLD.dbd into <top>/dbd
DBD += VLD.dbd

# specify all source files to be compiled and added to the library
VLD_SRCS += devLiVLD.c
VLD_SRCS += devLoVLD.c
VLD_SRCS += drvVLD.c

VLD_LIBS += $(EPICS_BASE_IOC_LIBS)

# Where to find jvme + supported libraries/headers
USR_INCLUDES_Linux += ${LINUXVME_INC}
VLD_LDFLAGS += -L${LINUXVME_LIB} -ljvme -lvld


#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
