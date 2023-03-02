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
		   asynInt32Mask | asynInt32ArrayMask | asynDrvUserMask,
		   asynInt32Mask | asynInt32ArrayMask | asynDrvUserMask,
		   ASYN_CANBLOCK | ASYN_MULTIDEVICE, 1,
		   0, 0)
{
  // VLD Crate variables
  createParam(crateSlotMaskString, asynParamInt32, &P_crateSlotMask);
  createParam(crateNVLDString, asynParamInt32, &P_crateNVLD);
  createParam(crateActiveMaskString, asynParamInt32, &P_crateActiveMask);
  createParam(crateModeString, asynParamInt32, &P_crateMode);
  createParam(crateModeEnableString, asynParamInt32, &P_crateModeEnable);

  // Individual VLD module variables
  createParam(firmwareVersionString, asynParamInt32, &P_firmwareVersion);
  createParam(boardIDString, asynParamInt32, &P_boardID);

  createParam(triggerDelayString, asynParamInt32, &P_triggerDelay);
  createParam(triggerDelayStepString, asynParamInt32, &P_triggerDelayStep);
  createParam(triggerWidthString, asynParamInt32, &P_triggerWidth);
  createParam(triggerSourceMaskString, asynParamInt32, &P_triggerSourceMask);

  createParam(clockSourceString, asynParamInt32, &P_clockSource);
  printf("P_clockSource = %d\n", P_clockSource); // 11

  for(int32_t iconn = 0; iconn < VLD_CONNECTOR_NUM; iconn++)
    {
      char tmpStr[100];
      sprintf(tmpStr, "%s%d:%s", connectorString, iconn, loChanMaskString);
      createParam(tmpStr, asynParamInt32, &P_loChanMask[iconn]);

      sprintf(tmpStr, "%s%d:%s", connectorString, iconn, hiChanMaskString);
      createParam(tmpStr, asynParamInt32, &P_hiChanMask[iconn]);
      printf("P_hiChanMask[%d] = %d\n", iconn, P_hiChanMask[iconn]); //

      sprintf(tmpStr, "%s%d:%s", connectorString, iconn, LDOString);
      createParam(tmpStr, asynParamInt32, &P_LDO[iconn]);

      sprintf(tmpStr, "%s%d:%s", connectorString, iconn, LDOEnableString);
      createParam(tmpStr, asynParamInt32, &P_LDOEnable[iconn]);
    }


  createParam(bleachTimeString, asynParamInt32, &P_bleachTime);
  printf("P_bleachTime = %d\n", P_bleachTime); // 32
  createParam(bleachTimerEnableString, asynParamInt32, &P_bleachTimerEnable);

  createParam(calibrationPulseWidthString, asynParamInt32, &P_calibrationPulserWidth);

  createParam(randomPulserPrescaleString, asynParamInt32, &P_randomPulserPrescale);
  createParam(randomPulserEnableString, asynParamInt32, &P_randomPulserEnable);

  createParam(periodicPulserPeriodString, asynParamInt32, &P_periodicPulserPeriod);
  createParam(periodicPulserPeriodRangeString, asynParamInt32, &P_periodicPulserPeriodRange);
  createParam(periodicPulserNumberString, asynParamInt32, &P_periodicPulserNumber);

  createParam(triggerCountString, asynParamInt32, &P_triggerCount);

  createParam(pulseWaveformTypeString, asynParamInt32, &P_pulseWaveformType);
  createParam(pulseWaveformString, asynParamInt32Array, &P_pulseWaveform);

  createParam(squareWaveAmplitudeString, asynParamInt32, &P_squareWaveAmplitude);
  createParam(squareWaveWidthString, asynParamInt32, &P_squareWaveWidth);

  createParam(resetString, asynParamInt32, &P_reset);

  vld_ActiveMask = 0;

  vmeOpenDefaultWindows();

  /* Init VLD here */
  int32_t status = vldInit(vme_addr, vme_incr, nincr, iFlag);
  if(status == 0)
    {
      /* set initiallized slots mask */
      setIntegerParam(P_crateSlotMask, vldSlotMask());
      vldGStatus(1);
    }

}

int32_t
VLD::addr2slot(uint32_t addr)
{

  return addr;
}

bool
VLD::isConnectorFunction(int32_t function,
			 int32_t &connector,
			 bool &isLoMask, bool &isHiMask, bool &isLDO, bool &isLDOEnable)
{
  /* initialize returned values */
  connector = -1;
  isLoMask = false;
  isHiMask = false;
  isLDO = false;
  isLDOEnable = false;

  for(int32_t iconn = 0; iconn < VLD_CONNECTOR_NUM; iconn++)
    {
      /* Check the lo mask */
      if(function == P_loChanMask[iconn])
	{
	  connector = iconn;
	  isLoMask = true;
	  return true;
	}

      /* Check the hi mask */
      if(function == P_hiChanMask[iconn])
	{
	  connector = iconn;
	  isHiMask = true;
	  return true;
	}

      /* Check the LDO */
      if(function == P_LDO[iconn])
	{
	  connector = iconn;
	  isLDO = true;
	  return true;
	}

      /* Check the LDOEnable */
      if(function == P_LDOEnable[iconn])
	{
	  connector = iconn;
	  isLDOEnable = true;
	  return true;
	}
    }

  return false;
}

asynStatus
VLD::getBounds(asynUser *pasynUser, epicsInt32 *low, epicsInt32 *high)
{
  int32_t function = pasynUser->reason;
  asynPrint(pasynUser, ASYN_TRACE_FLOW, "getbounds for function %d", function);

  *low = 0;

  if(function == P_triggerDelay)
    *high = 0x7f;

  else if(function == P_triggerDelayStep)
    *high = 0x1;

  else if(function == P_triggerWidth)
    *high = 0x1f;

  else if(function == P_triggerSourceMask)
    *high = 0x1F;

  else if(function == P_clockSource)
    *high = 0x1;

  else if((function >= P_LDO[0]) && (function <=P_LDO[VLD_CONNECTOR_NUM-1]))
    *high = 0x7;

  else if((function >= P_LDOEnable[0]) && (function <=P_LDOEnable[VLD_CONNECTOR_NUM-1]))
    *high = 0x1;

  else if((function >= P_loChanMask[0]) && (function <=P_loChanMask[VLD_CONNECTOR_NUM-1]))
    *high = 0x3fffff;

  else if((function >= P_hiChanMask[0]) && (function <=P_hiChanMask[VLD_CONNECTOR_NUM-1]))
    *high = 0x3fffff;

  else if(function == P_bleachTime)
    *high = 0x0FFFFFFF;

  else if(function == P_bleachTimerEnable)
    *high = 0x1;

  else if(function == P_calibrationPulserWidth)
    *high = 0x3ff;

  else if(function == P_randomPulserPrescale)
    *high = 0xF;

  else if(function == P_randomPulserEnable)
    *high = 0x1;

  else if(function == P_periodicPulserPeriod)
    *high = 0x7fff;

  else if(function == P_periodicPulserPeriodRange)
    *high = 0x1;

  else if(function == P_periodicPulserNumber)
    *high = 0xffff;

  else if(function == P_pulseWaveformType)
    *high = 1;

  else if(function == P_squareWaveAmplitude)
    *high = 0x3F;

  else if(function == P_squareWaveWidth)
    *high = 2048;

  else if(function == P_reset)
    *high = 1;

  else
    return(asynError);

  return(asynSuccess);
}

asynStatus
VLD::readInt32(asynUser *pasynUser, epicsInt32 *value)
{
  int id = 0;
  int addr;
  int32_t function = pasynUser->reason;
  int status=0;
  static const char *functionName = "readInt32";
  const char *paramName;
  int32_t connector = 0;
  bool isLoMask = false, isHiMask = false, isLDO = false, isLDOEnable = false;

  this->getAddress(pasynUser, &addr);
  id = addr2slot(addr);

  getParamName(function, &paramName);
  asynPrint(pasynUser, ASYN_TRACE_FLOW, "readInt32 for 0x%x function = %d (%s)\n",
	    addr, function, paramName);

  if(function == P_crateSlotMask)
    {
      uint32_t slotmask = 0;
      vmeBusLock();
      slotmask = vldSlotMask();
      vmeBusUnlock();

      if(slotmask == 0xFFFFFFFF)
	status = -1;
      else
	{
	  setIntegerParam(addr, P_crateSlotMask, slotmask);
	  *value = slotmask;
	}
    }
  else if(function == P_crateNVLD)
    {
      vmeBusLock();
      *value = vldGetNVLD();
      status = 0;
      vmeBusUnlock();

      setIntegerParam(P_crateNVLD, *value);
    }

  else if(function == P_crateActiveMask)
    {
      status = 0;
      *value = vld_ActiveMask;

      setIntegerParam(P_crateNVLD, *value);
    }

  else if(function == P_firmwareVersion)
    {
      uint32_t fw = 0;
      vmeBusLock();
      status = vldGetFirmwareVersion(id, &fw);
      vmeBusUnlock();

      setIntegerParam(addr, P_firmwareVersion, fw);
      *value = fw;

    }

  else if(function == P_boardID)
    {
      uint32_t boardID = 0;
      vmeBusLock();
      status = vldGetBoardID(id, &boardID);
      vmeBusUnlock();

      setIntegerParam(addr, P_boardID, boardID);
      *value = boardID;

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

  else if (isConnectorFunction(function, connector, isLoMask, isHiMask, isLDO, isLDOEnable))
    {
      vmeBusLock();
      if(isLoMask || isHiMask)
	{
	  uint32_t loChanEnableMask = 0, hiChanEnableMask = 0;
	  status = vldGetChannelMask(id, connector, &loChanEnableMask, &hiChanEnableMask);

	  setIntegerParam(addr, P_loChanMask[connector], (epicsInt32) loChanEnableMask);
	  setIntegerParam(addr, P_hiChanMask[connector], (epicsUInt32) hiChanEnableMask);

	  if(isLoMask)
	    *value = loChanEnableMask;

	  if(isHiMask)
	    *value = hiChanEnableMask;
	}

      if(isLDO || isLDOEnable)
	{
	  uint32_t ctrlLDO = 0, enableLDO = 0;
	  status = vldGetBleachCurrent(id, connector, &ctrlLDO, &enableLDO);

	  setIntegerParam(addr, P_LDO[connector], (epicsInt32) ctrlLDO);
	  setIntegerParam(addr, P_LDOEnable[connector], (epicsInt32) enableLDO);

	  if(isLDO)
	    *value = ctrlLDO;

	  if(isLDOEnable)
	    *value = enableLDO;
	}
      vmeBusUnlock();
    }
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

      if(enable)
	vld_ActiveMask |= (1 << id);
      else
	vld_ActiveMask &= ~(1 << id);
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

      if(enable)
	vld_ActiveMask |= (1 << id);
      else
	vld_ActiveMask &= ~(1 << id);

    }

  else if ((function == P_periodicPulserPeriod) || (function == P_periodicPulserPeriodRange) || (function == P_periodicPulserNumber))
    {
      uint32_t period = 0, period_range = 0, number = 0;
      vmeBusLock();
      status = vldGetPeriodicPulser(id, &period, &period_range, &number);
      vmeBusUnlock();

      setIntegerParam(addr, P_periodicPulserPeriod, period);
      setIntegerParam(addr, P_periodicPulserPeriodRange, period_range);
      setIntegerParam(addr, P_periodicPulserNumber, number);

      if (function == P_periodicPulserPeriod)
	*value = period;
      else if (function == P_periodicPulserPeriodRange)
	*value = period_range;
      else if (function == P_periodicPulserNumber)
	*value = number;

      if(number != 0)
	vld_ActiveMask |= (1 << id);
      else
	vld_ActiveMask &= ~(1 << id);
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

  else if ((function == P_squareWaveAmplitude) || (function == P_squareWaveWidth))
    {
      // These are soft channels, values used by P_Pulsewaveformtype
      // Just return the previously written value
      status = 0;
    }

  else if (function == P_pulseWaveformType)
    {
      // Just return the previously written value
      status = 0;
    }

  else if (function == P_reset)
    {
      // w/o readback always zero
      *value = 0;
      status = 0;
    }

  else
    {
      // Other functions we call the base class method
      status = asynPortDriver::readInt32(pasynUser, value);
    }

  if (status == 0)
    {
      asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
		"%s:%s, %s Slot %d %s, read %d\n",
		driverName, functionName, this->portName,
		id, paramName, *value);
    }
  else
    {
      asynPrint(pasynUser, ASYN_TRACE_ERROR,
		"%s:%s, %s Slot %d %s, ERROR reading from function (%d), status=%d\n",
		driverName, functionName, this->portName,
		id, paramName, function, status);
    }

  callParamCallbacks(addr);
  return (status==0) ? asynSuccess : asynError;
}


asynStatus
VLD::readInt32Array(asynUser *pasynUser, epicsInt32 *value, size_t nElements, size_t *nIn)
{
  int id = 0;
  int addr;
  int32_t function = pasynUser->reason;
  int32_t status = 0;
  static const char *functionName = __func__;

  this->getAddress(pasynUser, &addr);
  id = addr2slot(addr);

  asynPrint(pasynUser, ASYN_TRACE_FLOW, "%s for 0x%x function = %d\n",
	    __func__, addr, function);


  if (function == P_pulseWaveform)
    {
      uint32_t nsamples = 0;
      vmeBusLock();
      status = vldGetExamplePulse((uint32_t *)value, &nsamples);
      nElements = *nIn = nsamples;
      vmeBusUnlock();
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
		"%s:%s, port %s, ERROR reading from function = %d, address %d, status=%d\n",
		driverName, functionName, this->portName, function , addr, status);
    }

  callParamCallbacks(addr);
  return (status==0) ? asynSuccess : asynError;


}

asynStatus
VLD::writeInt32Array(asynUser *pasynUser, epicsInt32 *value, size_t nElements)
{
  int id = 0;
  int addr;
  int function = pasynUser->reason;
  int32_t status = 0;

  this->getAddress(pasynUser, &addr);
  id = addr;

  asynPrint(pasynUser, ASYN_TRACE_FLOW, "%s for 0x%x function = %d\n",
	    __func__, addr, function);

  if(function == P_pulseWaveform)
    {
      vmeBusLock();
      status = vldLoadPulse32(id, (uint32_t *)value, nElements);
      vmeBusUnlock();
    }
  else
    {

      // Other functions we call the base class method
      status = asynPortDriver::writeInt32Array(pasynUser, value, nElements);
    }

  callParamCallbacks(addr);
  if (status == 0) {
    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
	      "%s:%s, port %s, wrote array[%d] to address %d\n",
	      driverName, __func__, this->portName, (int)nElements, addr);
  } else {
    asynPrint(pasynUser, ASYN_TRACE_ERROR,
	      "%s:%s, port %s, ERROR writing array[%d] to address %d, status=%d\n",
	      driverName, __func__, this->portName, (int)nElements, addr, status);
  }
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
  int32_t connector = 0;
  bool isLoMask = false, isHiMask = false, isLDO = false, isLDOEnable = false;

  this->getAddress(pasynUser, &addr);
  id = addr;

  asynPrint(pasynUser, ASYN_TRACE_FLOW, "writeInt32 for 0x%x function = %d\n", addr, function);

  /* Set it now, may overwrite it later */
  setIntegerParam(addr, function, value);

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
  else if (function == P_triggerSourceMask)
    {
      uint32_t trigSrc = value;

      vmeBusLock();
      status = vldSetTriggerSourceMask(id, trigSrc);
      vmeBusUnlock();

    }

  else if (function == P_clockSource)
    {
      uint32_t clkSrc = value;

      vmeBusLock();
      status = vldSetClockSource(id, clkSrc);
      vmeBusUnlock();

    }

  else if (isConnectorFunction(function, connector, isLoMask, isHiMask, isLDO, isLDOEnable))
    {
      if(isLoMask || isHiMask)
	{
	  uint32_t loChanEnableMask = 0, hiChanEnableMask = 0;
	  getIntegerParam(addr, P_loChanMask[connector], (epicsInt32 *)&loChanEnableMask);
	  getIntegerParam(addr, P_hiChanMask[connector], (epicsInt32 *)&hiChanEnableMask);

	  if(isLoMask)
	    loChanEnableMask = value;

	  if(isHiMask)
	    hiChanEnableMask = value;

	  vmeBusLock();
	  status = vldSetChannelMask(id, connector, loChanEnableMask, hiChanEnableMask);
	  vmeBusUnlock();

	}

      if(isLDO || isLDOEnable)
	{
	  uint32_t ctrlLDO = 0, enableLDO = 0;
	  getIntegerParam(addr, P_LDO[connector], (epicsInt32 *)&ctrlLDO);
	  getIntegerParam(addr, P_LDOEnable[connector], (epicsInt32 *)&enableLDO);

	  if(isLDO)
	    ctrlLDO = value;

	  if(isLDOEnable)
	    enableLDO = value;

	  vmeBusLock();
	  status = vldSetBleachCurrent(id, connector, ctrlLDO, enableLDO);
	  vmeBusUnlock();

	}
    }

  else if ((function == P_bleachTime) || (function == P_bleachTimerEnable))
    {
      uint32_t timer = 0, enable = 0;
      getIntegerParam(addr, P_bleachTime, (epicsInt32 *)&timer);
      getIntegerParam(addr, P_bleachTimerEnable, (epicsInt32 *)&enable);

      if(function == P_bleachTime)
	timer = value;
      else if (function == P_bleachTimerEnable)
	enable = value;

      vmeBusLock();
      status = vldSetBleachTime(id, timer, enable);
      vmeBusUnlock();

    }

  else if (function == P_calibrationPulserWidth)
    {
      uint32_t pulsewidth = value;

      vmeBusLock();
      status = vldSetCalibrationPulseWidth(id, pulsewidth);
      vmeBusUnlock();

    }

  else if ((function == P_randomPulserPrescale) || (function == P_randomPulserEnable))
    {
      uint32_t prescale = 0, enable = 0;

      getIntegerParam(addr, P_randomPulserPrescale, (epicsInt32 *)&prescale);
      getIntegerParam(addr, P_randomPulserEnable, (epicsInt32 *)&enable);

      if (function == P_randomPulserPrescale)
	prescale = value;
      else if (function == P_randomPulserEnable)
	enable = value;

      vmeBusLock();
      status = vldSetRandomPulser(id, prescale, enable);
      vmeBusUnlock();

    }

  else if ((function == P_periodicPulserPeriod) || (function == P_periodicPulserPeriodRange) || (function == P_periodicPulserNumber))
    {
      uint32_t period = 0, period_range = 0, number = 0;

      getIntegerParam(addr, P_periodicPulserPeriod, (epicsInt32 *)&period);
      getIntegerParam(addr, P_periodicPulserPeriodRange, (epicsInt32 *)&period_range);
      getIntegerParam(addr, P_periodicPulserNumber, (epicsInt32 *)&number);

      if (function == P_periodicPulserPeriod)
	period = value;
      else if (function == P_periodicPulserPeriodRange)
	period_range = value;
      else if (function == P_periodicPulserNumber)
	number = value;

      vmeBusLock();
      status = vldSetPeriodicPulser(id, period, period_range, number);
      vmeBusUnlock();

    }

  else if (function == P_pulseWaveformType)
    {
      uint32_t waveform_type = 0, square_wave_amp = 0, square_wave_width = 0;

      getIntegerParam(addr, P_squareWaveAmplitude, (epicsInt32 *)&square_wave_amp);
      getIntegerParam(addr, P_squareWaveWidth, (epicsInt32 *)&square_wave_width);

      waveform_type = value;

      vmeBusLock();
      switch(waveform_type)
	{
	case 0: // Waveform from William Gu
	  status = vldLoadExamplePulse(id);
	  break;

	case 1: // Square Wave
	  status = vldLoadSquarePulse(id, square_wave_width, square_wave_amp);
	  break;

	case 2: // User defined
	default:
	  status = -1;
	}

      vmeBusUnlock();

    }

  else if (function == P_reset)
    {
      // ignore if 0
      if(value > 0)
	{
	  vmeBusLock();
	  vldSoftReset(id);
	  vmeBusUnlock();
	}
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
