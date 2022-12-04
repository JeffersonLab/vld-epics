#pragma once
#include <asynPortDriver.h>

static const char *driverName = "VLD";

#define firmwareVersionString "FirmwareVersion" /* asynInt32 r/o */
#define boardIDString         "ModuleID" /* asynInt32 r/o */
#define slotNumberString      "SlotNumber"      /* asynInt32 r/o */

#define triggerDelayString      "TriggerDelay"        /* asynInt32 r/w */
#define triggerDelayStepString  "TriggerDelayStep"        /* asynInt32 r/w */
#define triggerWidthString      "TriggerWidth"        /* asynInt32 r/w */
#define triggerSourceMaskString "TriggerSourceMask"   /* asynInt32 r/w */

#define clockSourceString       "ClockSource"         /* asynInt32 r/w */

#define VLD_CONNECTOR_NUM 5
#define connectorString  ":C"
#define loChanMaskString "LowChannelMask"  /* asynInt32 r/w */
#define hiChanMaskString "HighChannelMask" /* asynInt32 r/w */
#define LDOString        "LDOCurrentSetting"   /* asynInt32 r/w */
#define LDOEnableString  "LDOEnable"            /* asynInt32 r/w */

#define bleachTimeString        "BleachTime"                /* asynInt32 r/w */
#define bleachTimerEnableString "BleachTimerEnable"        /* asynInt32 r/w */

#define calibrationPulseWidthString "CalibrationPulseWidth" /* asynInt32 r/w */

#define randomPulserPrescaleString "RandomPulserPrescale" /* asynInt32 r/w */
#define randomPulserEnableString   "RandomPulserEnable"   /* asynInt32 r/w */

#define periodicPulserPeriodString "PeriodicPulserPeriod" /* asynInt32 r/w */
#define periodicPulserNumberString "PeriodicPulserNumber" /* asynInt32 r/w */

#define triggerCountString "TriggerCount" /* asynInt32 r/o */


#define MAX_SIGNALS 40

/** Class definition for the VLD class
 */
class VLD : public asynPortDriver {

public:
  VLD(const char *portName, uint32_t vme_addr, uint32_t vme_incr, uint32_t nincr, uint32_t iFlag);

  virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
  virtual asynStatus writeInt32Array(asynUser *pasynUser, epicsInt32 *value, size_t nElements);
  virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
  virtual asynStatus getBounds(asynUser *pasynUser, epicsInt32 *low, epicsInt32 *high);

protected:
  int32_t P_firmwareVersion;
  int32_t P_boardID;
  int32_t P_slotNumber;

  int32_t P_triggerDelay;
  int32_t P_triggerDelayStep;
  int32_t P_triggerWidth;
  int32_t P_triggerSourceMask;

  int32_t P_clockSource;

  int32_t P_loChanMask[VLD_CONNECTOR_NUM];
  int32_t P_hiChanMask[VLD_CONNECTOR_NUM];
  int32_t P_LDO[VLD_CONNECTOR_NUM];
  int32_t P_LDOEnable[VLD_CONNECTOR_NUM];

  int32_t P_bleachTime;
  int32_t P_bleachTimerEnable;

  int32_t P_calibrationPulserWidth;

  int32_t P_randomPulserPrescale;
  int32_t P_randomPulserEnable;

  int32_t P_periodicPulserPeriod;
  int32_t P_periodicPulserNumber;

  int32_t P_triggerCount;

private:
  int32_t P_boardNum;
  uint32_t P_slotmask;

  int32_t addr2slot(uint32_t addr);
  bool    isConnectorFunction(int32_t function,
			      int32_t &connector,
			      bool &isLoMask, bool &isHiMask, bool &isLDO, bool &isLDOEnable);
};
