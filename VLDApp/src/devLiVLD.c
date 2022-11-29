#include        <stdio.h>
#include        <string.h>

#include        <alarm.h>
#include        <cvtTable.h>
#include        <dbDefs.h>
#include        <dbAccess.h>
#include        <recSup.h>
#include        <recGbl.h>
#include        <devSup.h>
#include        <link.h>
#include        <longinRecord.h>

#include        <dbScan.h>
#include        <epicsExport.h>

extern int DEBUGVLD;
/* Create the dset for devLiVLD */
static long init_record();
static long read_longin();
extern int read_VLD();

struct
{
  long number;
  DEVSUPFUN report;
  DEVSUPFUN init;
  DEVSUPFUN init_record;
  DEVSUPFUN get_ioint_info;
  DEVSUPFUN read_longin;
  DEVSUPFUN special_linconv;
} devLiVLD =
{6, NULL, NULL, init_record, NULL, read_longin, NULL};

epicsExportAddress(dset, devLiVLD);

static long
init_record(struct longinRecord *plongin)
{
/********************************************************
 * Init Record
 ********************************************************/

  /* li.inp must be VME_IO */
  switch (plongin->inp.type)
    {
    case (VME_IO):
      break;
    default:
      recGblRecordError(S_db_badField, (void *) plongin,
			"devLiVLD (init_record) Illegal INP field");
      return (S_db_badField);
    }
  plongin->udf = FALSE;
  return (0);
}

static long
read_longin(struct longinRecord *plongin)
{
/**********************************************************
 * Read LongIn
 **********************************************************/
  struct vmeio *pvmeio_VLD;
  int status;
  unsigned long value;

    /****** Get value and status */
  pvmeio_VLD = (struct vmeio *) &(plongin->inp.value);
  status = read_VLD(pvmeio_VLD->card, pvmeio_VLD->signal, &value);

  if(DEBUGVLD)
    printf("%s: signal = %d value = %lx\n", __func__,
	   pvmeio_VLD->signal, value);

  if(status == 0)
    {
      plongin->val = value;
    }
  else
    {
      recGblSetSevr(plongin, READ_ALARM, INVALID_ALARM);
    }
  return (status);
}
