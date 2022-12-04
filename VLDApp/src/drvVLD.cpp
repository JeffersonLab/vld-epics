#include <iocsh.h>

extern "C"
{
#include "jvme.h"
#include "vldLib.h"
}

#include <epicsExport.h>
#include "drvVLD.hh"

/** Constructor for the VLD class
 * The only instantiation is done by the IOC at startup
 */
VLD::VLD(const char* portName, uint32_t vme_addr, uint32_t vme_incr, uint32_t nincr, uint32_t iFlag)
  : asynPortDriver(portName, MAX_SIGNALS, //NUM_PARAMS, deprecated
		   asynInt32Mask | asynFloat64Mask | asynDrvUserMask,
		   0,
		   ASYN_CANBLOCK, 1,
		   0, 0)
{
  createParam(firmwareVersionString, asynParamInt32, &P_firmwareVersion);
  createParam(slotNumberString, asynParamInt32, &P_slotNumber);

  createParam(triggerDelayString, asynParamInt32, &P_triggerDelay);
  createParam(triggerDelayStepString, asynParamInt32, &P_triggerDelayStep);
  createParam(triggerWidthString, asynParamInt32, &P_triggerWidth);
  createParam(triggerSourceMaskString, asynParamInt32, &P_triggerSourceMask);

  createParam(clockSourceString, asynParamInt32, &P_clockSource);

  createParam(ledConnectorString, asynParamInt32, &P_ledConnector);
  createParam(ledLoChanMaskString, asynParamInt32, &P_ledLoChanMask);
  createParam(ledHiChanMaskString, asynParamInt32, &P_ledHiChanMask);
  createParam(ledLDOString, asynParamInt32, &P_ledLDO);
  createParam(ledEnableString, asynParamInt32, &P_ledEnable);

  createParam(bleachTimeString, asynParamInt32, &P_bleachTime);
  createParam(bleachTimerEnableString, asynParamInt32, &P_bleachTimerEnable);

  createParam(calibrationPulseWidthString, asynParamInt32, &P_calibrationPulserWidth);

  createParam(randomPulserPrescaleString, asynParamInt32, &P_randomPulserPrescale);
  createParam(randomPulserEnableString, asynParamInt32, &P_randomPulserEnable);

  createParam(periodicPulserPeriodString, asynParamInt32, &P_periodicPulserPeriod);
  createParam(periodicPulserNumberString, asynParamInt32, &P_periodicPulserNumber);

  createParam(triggerCountString, asynParamInt32, &P_triggerCount);

  vmeOpenDefaultWindows();

  /* Init VLD here */
  vldInit(vme_addr, vme_incr, nincr, iFlag);

  P_slotmask = vldSlotMask();

}

int32_t
VLD::addr2slot(uint32_t addr)
{
  int32_t slot = 0, rval = 0;

  /*
    Here we assume the address is related to the slot number:
      A24 = slot << 19;
  */
  slot = addr >> 19;

  /* Check if this slot has been initialized */
  if(P_slotmask & (1 << slot))
    rval = slot;
  else
    rval = -1;

  return rval;
}

asynStatus
VLD::getBounds(asynUser *pasynUser, epicsInt32 *low, epicsInt32 *high)
{
  int function = pasynUser->reason;

  *low = 0;

  if(function == P_triggerDelay)
    *high = 0xffffffff;

  if(function == P_triggerDelayStep)
    *high = 0xffffffff;

  else if(function == P_triggerWidth)
    *high = 0xffffffff;

  else if(function == P_triggerSourceMask)
    *high = 0xffffffff;

  else if(function == P_clockSource)
    *high = 0xffffffff;

  else if(function == P_ledConnector)
    *high = 0xffffffff;

  else if(function == P_ledLoChanMask)
    *high = 0xffffffff;

  else if(function == P_ledHiChanMask)
    *high = 0xffffffff;

  else if(function == P_ledLDO)
    *high = 0xffffffff;

  else if(function == P_bleachTime)
    *high = 0xffffffff;

  else if(function == P_calibrationPulserWidth)
    *high = 0xffffffff;

  else if(function == P_randomPulserPrescale)
    *high = 0xffffffff;

  else if(function == P_periodicPulserPeriod)
    *high = 0xffffffff;

  else if(function == P_periodicPulserNumber)
    *high = 0xffffffff;

  else
    return(asynError);

  return(asynSuccess);
}

asynStatus
VLD::readInt32(asynUser *pasynUser, epicsInt32 *value)
{
  int id = 0;
  int addr;
  int function = pasynUser->reason;
  int status=0;
  static const char *functionName = "readInt32";

  this->getAddress(pasynUser, &addr);
  id = addr2slot(addr);

  if(function == P_firmwareVersion)
    {
      uint32_t fw = 0;
      vmeBusLock();
      status = vldGetFirmwareVersion(id, &fw);
      vmeBusUnlock();

      setIntegerParam(addr, P_firmwareVersion, fw);
      *value = fw;

    }

  else if((function == P_triggerDelay) || (function == P_triggerWidth) || (function == P_triggerDelayStep))
    {
      int32_t delay = 0, delayStep = 0, width = 0;
      vmeBusLock();
      status = vldGetTriggerDelayWidth(id, &delay, &delayStep, &width);
      vmeBusUnlock();

      setIntegerParam(addr, P_triggerDelay, delay);
      setIntegerParam(addr, P_triggerDelayStep, delayStep);
      setIntegerParam(addr, P_triggerWidth, width);

      if(function == P_triggerDelay)
	*value = delay;
      else if(function == P_triggerDelayStep)
	*value = delayStep;
      else if(function == P_triggerWidth)
	*value = width;

    }

  else if (function == P_triggerSourceMask)
    {
      uint32_t trigSrc = 0;
      vmeBusLock();
      status = vldGetTriggerSourceMask(id, &trigSrc);
      vmeBusUnlock();

      setIntegerParam(addr, P_triggerSourceMask, (epicsInt32) trigSrc);

      *value = trigSrc;

    }

  else if (function == P_clockSource)
    {
      uint32_t clkSrc = 0;
      vmeBusLock();
      status = vldGetClockSource(id, &clkSrc);
      vmeBusUnlock();

      setIntegerParam(addr, P_clockSource, (epicsInt32) clkSrc);
      *value = clkSrc;
    }
#ifdef NOT_YET_DEFINED
  else if ((function == P_ledConnector) || (function == P_ledLoChanMask) ||
	   (function == P_ledHiChanMask) || (function == P_ledLDO) ||
	   (function == P_ledEnable))
    {
      uint32_t ledConnector = 0, ledLoChanMask = 0, ledHiChanMask = 0, ledLDO = 0, ledEnable = 0;
      vmeBusLock();
      status = ;
      vmeBusUnlock();

      //....

    }
#endif
  else if ((function == P_bleachTime) || (function == P_bleachTimerEnable))
    {
      uint32_t timer = 0, enable = 0;
      vmeBusLock();
      status = vldGetBleachTime(id, &timer, &enable);
      vmeBusUnlock();

      setIntegerParam(addr, P_bleachTime, timer);
      setIntegerParam(addr, P_bleachTimerEnable, enable);

      if(function == P_bleachTime)
	*value = timer;
      else if (function == P_bleachTimerEnable)
	*value = enable;

    }

  else if (function == P_calibrationPulserWidth)
    {
      uint32_t pulsewidth = 0;
      vmeBusLock();
      status = vldGetCalibrationPulseWidth(id, &pulsewidth);
      vmeBusUnlock();

      setIntegerParam(addr, P_calibrationPulserWidth, pulsewidth);
      *value = pulsewidth;
    }

  else if ((function == P_randomPulserPrescale) || (function == P_randomPulserEnable))
    {
      uint32_t prescale = 0, enable = 0;
      vmeBusLock();
      status = vldGetRandomPulser(id, &prescale, &enable);
      vmeBusUnlock();

      setIntegerParam(addr, P_randomPulserPrescale, prescale);
      setIntegerParam(addr, P_randomPulserEnable, enable);

      if (function == P_randomPulserPrescale)
	*value = prescale;
      else if (function == P_randomPulserEnable)
	*value = enable;
    }

  else if ((function == P_periodicPulserPeriod) || (function == P_periodicPulserNumber))
    {
      uint32_t period = 0, number = 0;
      vmeBusLock();
      status = vldGetPeriodicPulser(id, &period, &number);
      vmeBusUnlock();

      setIntegerParam(addr, P_periodicPulserPeriod, period);
      setIntegerParam(addr, P_periodicPulserNumber, number);

      if (function == P_periodicPulserPeriod)
	*value = period;
      else if (function == P_periodicPulserNumber)
	*value = number;

    }

  else if (function == P_triggerCount)
    {
      uint32_t trigcnt = 0;
      vmeBusLock();
      status = vldGetTriggerCount(id, &trigcnt);
      vmeBusUnlock();

      setIntegerParam(addr, P_triggerCount, trigcnt);
      *value = trigcnt;
    }

  else
    {
      // Other functions we call the base class method
      status = asynPortDriver::readInt32(pasynUser, value);
    }

  if (status == 0)
    {
      asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
		"%s:%s, port %s, read %d from address %d\n",
		driverName, functionName, this->portName, *value, addr);
    }
  else
    {
      asynPrint(pasynUser, ASYN_TRACE_ERROR,
		"%s:%s, port %s, ERROR reading from address %d, status=%d\n",
		driverName, functionName, this->portName, addr, status);
    }

  callParamCallbacks(addr);
  return (status==0) ? asynSuccess : asynError;
}

asynStatus
VLD::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
  int id = 0;
  int addr;
  int function = pasynUser->reason;
  int status=0;
  static const char *functionName = "writeInt32";

  this->getAddress(pasynUser, &addr);

  if((function == P_triggerDelay) || (function == P_triggerWidth) || (function == P_triggerDelayStep))
    {
      int32_t delay = 0, delayStep = 0, width = 0;

      getIntegerParam(addr, P_triggerDelay, &delay);
      getIntegerParam(addr, P_triggerDelayStep, &delayStep);
      getIntegerParam(addr, P_triggerWidth, &width);

      if(function == P_triggerDelay)
	delay = value;
      else if(function == P_triggerDelayStep)
	delayStep = value;
      else if(function == P_triggerWidth)
	width = value;

      vmeBusLock();
      status = vldSetTriggerDelayWidth(id, delay, delayStep, width);
      vmeBusUnlock();

    }
  else
    {
      // Other functions we call the base class method
      status = asynPortDriver::writeInt32(pasynUser, value);
    }

  callParamCallbacks(addr);
  if (status == 0) {
    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
	      "%s:%s, port %s, wrote %d to address %d\n",
	      driverName, functionName, this->portName, value, addr);
  } else {
    asynPrint(pasynUser, ASYN_TRACE_ERROR,
	      "%s:%s, port %s, ERROR writing %d to address %d, status=%d\n",
	      driverName, functionName, this->portName, value, addr, status);
  }
  return (status==0) ? asynSuccess : asynError;
}

/** Configuration command, called directly or from iocsh */
extern "C" int VLDConfig(const char *portName,
			 uint32_t vme_addr, uint32_t vme_incr, uint32_t nincr, uint32_t iFlag)
{
  VLD *pVLD = new VLD(portName, vme_addr, vme_incr, nincr, iFlag);
  pVLD = NULL;  /* This is just to avoid compiler warnings */
  return(asynSuccess);
}


static const iocshArg configArg0 = { "Port name",      iocshArgString};
static const iocshArg configArg1 = { "Starting VME Address", iocshArgInt};
static const iocshArg configArg2 = { "Incrementing Address", iocshArgInt};
static const iocshArg configArg3 = { "Number of increments", iocshArgInt};
static const iocshArg configArg4 = { "Initialization Flag", iocshArgInt};
static const iocshArg * const configArgs[] = {&configArg0,
                                              &configArg1,
                                              &configArg2,
					      &configArg3,
                                              &configArg4};

static const iocshFuncDef configFuncDef = {"VLDConfig", 5, configArgs};
static void configCallFunc(const iocshArgBuf *args)
{
  VLDConfig(args[0].sval, args[1].ival, args[2].ival, args[3].ival, args[4].ival);
}

void drvVLDRegister(void)
{
  iocshRegister(&configFuncDef,configCallFunc);
}

extern "C" {
  epicsExportRegistrar(drvVLDRegister);
}
