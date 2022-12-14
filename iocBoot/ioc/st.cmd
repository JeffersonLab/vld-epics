#!../../bin/linux-x86_64/VLD


< envPaths

epicsEnvSet("PORT", "daqlab")

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/VLD.dbd"
VLD_registerRecordDeviceDriver pdbbase

VLDConfig("$(PORT)",0x380000,0,1,0);

## Load record instances
dbLoadTemplate($(TOP)/db/"VLD-$(PORT).substitutions","P=$(PORT),PORT=$(PORT)")

##asynSetTraceMask("$(PORT)", 7, ASYN_TRACE_DRIVER)

cd "${TOP}/iocBoot/${IOC}"
iocInit


dbpf "daqlabSlot7:BleachTime_RB.ESLO", ".02097152000"
dbpf "daqlabSlot7:TriggerWidth_RB.ESLO", "4"
dbpf "daqlabSlot7:TriggerWidth_RB.EOFF", "4"
