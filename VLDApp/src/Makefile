TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

# build an ioc application

PROD_IOC = VLD
# VLD.dbd will be created and installed
DBD += VLD.dbd

VLD_DBD += base.dbd

# VLD_registerRecordDeviceDriver.cpp will be created from VLD.dbd
VLD_SRCS += VLD_registerRecordDeviceDriver.cpp
VLD_SRCS_DEFAULT += VLDMain.cpp
VLD_SRCS_vxWorks += -nil-

# Add vxWorks support stuff
VLD_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Add the support libraries needed by this application
VLD_LIBS += asyn

# Finally include the libraries from base
VLD_LIBS += $(EPICS_BASE_IOC_LIBS)

VLD_LDFLAGS += -L$(LINUXVME_LIB) -ljvme -lvld
USR_INCLUDES += -I$(LINUXVME_INC)


include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
