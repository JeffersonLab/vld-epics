#include        <stdio.h>
#include        <string.h>
#include        <alarm.h>
#include        <cvtTable.h>
#include        <dbDefs.h>
#include        <dbAccess.h>
#include        <recGbl.h>
#include        <recSup.h>
#include        <devSup.h>
#include        <link.h>
#include        <longoutRecord.h>

#include        <dbScan.h>
#include        <epicsExport.h>

#define NOCARD         0xc0
#define OUTPUTPORT     0x40

extern int DEBUGVLD;
/* Create the dset for devLoVLD */
static long init_record();
static long write_longout();
extern int write_VLD();

struct
{
  long number;
  DEVSUPFUN report;
  DEVSUPFUN init;
  DEVSUPFUN init_record;
  DEVSUPFUN get_ioint_info;
  DEVSUPFUN write_longout;
  DEVSUPFUN special_linconv;
} devLoVLD =
{6, NULL, NULL, init_record, NULL, write_longout, NULL};

epicsExportAddress(dset, devLoVLD);

/*****************************************************************
 * Init Record
 *****************************************************************/
static long
init_record(struct longoutRecord *plongout)
{
  int status = 0;

  /* lo.out must be VME_IO */
  switch (plongout->out.type)
    {
    case (VME_IO):
      break;

    default:
      status = S_db_badField;
      recGblRecordError(status, (void *) plongout,
			"devLoVLD (init_record) Illegal OUT field");
    }
  plongout->udf = FALSE;

  return (status);
}

static long
write_longout(struct longoutRecord *plongout)
{
  /****************************************************************
   * Write LongOut
   ****************************************************************/
  struct vmeio *pvmeio_VLD;
  int status;
  unsigned long value;

  pvmeio_VLD = &(plongout->out.value.vmeio);
  if(DEBUGVLD)
    printf("%s: pvmeio_VLD->signal = %d val = %x\n", __func__,
	   pvmeio_VLD->signal,
	   plongout->val);

  value = plongout->val;
  status = write_VLD(pvmeio_VLD->card, pvmeio_VLD->signal, &value);
  if(status != 0)
    {
      recGblSetSevr(plongout, WRITE_ALARM, INVALID_ALARM);
    }
  return (status);
}
