/****************************************************************************
 * Program   : drvVLD.c
 * Programmer: Bryan Moffit
 * Modified  : 09 Dec 1999
 ****************************************************************************/

#include "jvme.h"

#include <iocsh.h>
#include <drvSup.h>
#include <math.h>

#include <dbDefs.h>
#include <link.h>
#include <registryFunction.h>
#include <epicsExport.h>

#define MAXCARDS         2
#define PORTS            2
#define MAXREGS          5
#define REGOFFSET        0x2
#define CARDLENGTH       0x10

#define JLFOK               1
#define JLFBAD              0

int DEBUGVLD = 0;
unsigned short *VLD_shortAdd[MAXCARDS][MAXREGS];
int VLD_cards = 0;
int VLD_reqcardtot = 0;

long
vmeConfigVLD(int addr_type, unsigned long base_addr, long unit_length,
	     int num_cards)
{
  short card, status, reg;
  char testR = 1;
  char *addr;
  char *flexio[MAXREGS];
  int abs(int);
  STATUS sysBusToLocalAdrs(int, char *, char **);
  STATUS vxMemProbe(char *, int, int, char *);

  status = vmeOpenDefaultWindows();
  if(status != OK)
    {
      puts("ERROR opening VME windows\n");
      return ERROR;
    }

  if(DEBUGVLD)
    printf("%s: addr_type <%d> base_addr <%lx> unit_length <%lx> num_cards <%d>\n",
	   __func__,
	   addr_type, base_addr, unit_length, num_cards);

  /* Total number of cards requested */
  VLD_reqcardtot += abs(num_cards);

  /* Check num_cards request */
  if((num_cards <= 0) || (VLD_reqcardtot > MAXCARDS))
    {
      printf("%s: Maximum number of cards exceeded.\n", __func__);
      if(VLD_cards == 1)
	{
	  printf("                   One card total is available to EPICS of %d total requested.\n",
		 VLD_reqcardtot);
	}
      else
	{
	  printf("                   %d cards total are available to EPICS of %d total requested.\n",
		 VLD_cards, VLD_reqcardtot);
	}
      return JLFBAD;
    }

  /* Check address type request */
  if(addr_type != 0)
    {
      printf("%s: Requested address type invalid.\n", __func__);
      if(VLD_cards == 1)
	{
	  printf("                   One card total is available to EPICS of %d total requested.\n",
		 VLD_reqcardtot);
	}
      else
	{
	  printf("                   %d cards total are available to EPICS of %d total requested.\n",
		 VLD_cards, VLD_reqcardtot);
	}
      return JLFBAD;
    }

  /* Check unit length  */
  if(unit_length != CARDLENGTH)
    {
      printf("%s: Unit Length invalid.\n", __func__);
      if(VLD_cards == 1)
	{
	  printf("                   One card total is available to EPICS of %d total requested.\n",
		 VLD_reqcardtot);
	}
      else
	{
	  printf("                   %d cards total are available to EPICS of %d total requested.\n",
		 VLD_cards, VLD_reqcardtot);
	}
      return JLFBAD;
    }

  /* Cycle through requested cards to get addresses and check for memory location existence */
  addr = (char *) base_addr;
  for(card = VLD_cards; card < VLD_reqcardtot; card++)
    {

      /* Cycle through card registers */
      for(reg = 0; reg < MAXREGS; reg++)
	{
	  if(DEBUGVLD)
	    printf("%s: card <%d> reg <%d> addr <%lx>\n",
		   __func__,
		   card, reg,
		   (unsigned long) addr);

	  /* Convert requested address to full address */
	  flexio[reg] = (char *) VLD_shortAdd[card][reg];
#ifdef VXWORKS
	  status = sysBusToLocalAdrs(VME_AM_SUP_SHORT_IO, addr, &flexio[reg]);
#else
	  status = vmeBusToLocalAdrs(0x29, addr, &flexio[reg]);
#endif
	  VLD_shortAdd[card][reg] = (unsigned short *) flexio[reg];
	  if(DEBUGVLD)
	    printf("%s: VLD_shortAdd[card][reg] <%lx>\n",
		   __func__,
		   (unsigned long) VLD_shortAdd[card][reg]);
	  if(status != OK)
	    {
	      printf("%s: Short address space error for JLab Flex I/O card #%d.\n",
		     __func__,
		     card);
	      return ERROR;
	    }

	  /* Check that memory location exists */
#ifdef VXWORKS
	  status = vxMemProbe((char *) VLD_shortAdd[card][reg], VX_READ, 1, &testR);
#else
	  status = vmeMemProbe((char *) VLD_shortAdd[card][reg], 1, &testR);
#endif
	  if(status == OK)
	    {
	      puts("\n... VLD located...\n");

	      if(DEBUGVLD)
		printf("%s: JLFLEXIO is present; VLD_shortAdd = %p.\n",
		       __func__,
		       VLD_shortAdd[card][reg]);
	    }
	  else
	    {
	      printf("%s: Requested address <%lx> not present.\n",
		     __func__,
		     (long) VLD_shortAdd[card][reg]);
	      if(VLD_cards == 1)
		{
		  printf("                   One card total is available to EPICS of %d total requested.\n",
			 VLD_reqcardtot);
		}
	      else
		{
		  printf("                   %d cards total are available to EPICS of %d total requested.\n",
			 VLD_cards, VLD_reqcardtot);
		}
	      return JLFBAD;
	    }

	  addr += REGOFFSET;
	}
      VLD_cards++;
    }
  puts("\n... JLFLexIO initialized...\n");
  return OK;
}

/**************************************************************************
 * Write VLD
 **************************************************************************/
int
write_VLD(register unsigned short card,
	  register short signal, unsigned long *pval)
{

  /*** Write new output */
  vmeBusLock();
  vmeWrite16(VLD_shortAdd[card][signal], (unsigned short) *pval);
  vmeBusUnlock();

  if(DEBUGVLD)
    printf("%s: signal=%d pval=%lx *VLD_shortAdd=%x VLD_shortAdd=%lx\n",
	   __func__,
	   signal, *pval, *VLD_shortAdd[card][signal],
	   (unsigned long) VLD_shortAdd[card][signal]);
  return (0);
}

/***************************************************************************
 * Read VLD
 ***************************************************************************/
int
read_VLD(register unsigned short card,
	 register unsigned short signal, unsigned long *pval)
{

  unsigned short temp;

#ifdef VXWORKS
  cacheInvalidate(DATA_CACHE, VLD_shortAdd[card][signal], 1);
#endif

  vmeBusLock();
  temp = vmeRead16(VLD_shortAdd[card][signal]);
  vmeBusUnlock();
  *pval = (unsigned long) temp;

  if(DEBUGVLD)
    printf("drvVLD: signal = %d temp = %x\n", signal, temp);

  return (0);
}

static  const iocshArg vmeConfigArg0 = { "addr_type",iocshArgInt};
static  const iocshArg vmeConfigArg1 = { "base_addr",iocshArgInt};
static  const iocshArg vmeConfigArg2 = { "unit_length",iocshArgInt};
static  const iocshArg vmeConfigArg3 = { "num_cards",iocshArgInt};
static const iocshArg *const drvVLDConfigureArgs[4] =
  {&vmeConfigArg0, &vmeConfigArg1, &vmeConfigArg2, &vmeConfigArg3};
static const iocshFuncDef drvVLDConfigureFuncDef =
  {"vmeConfigVLD",4,drvVLDConfigureArgs};
static void drvVLDCallConfigureFunc(const iocshArgBuf *args)
{
  vmeConfigVLD(args[0].ival,args[1].ival,args[2].ival,args[3].ival);
}

void drvVLDRegistrar(void)
{
  static int firstTime = 1;
  if (!firstTime)
    return;
  firstTime = 0;
  iocshRegister(&drvVLDConfigureFuncDef,drvVLDCallConfigureFunc);
}

epicsExportRegistrar(drvVLDRegistrar);
