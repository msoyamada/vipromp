

#include <math.h>
#include <assert.h>
#include "power.hpp"
#include "sim.hpp"
#include "machine.hpp"
#include <string.h>

power::power(sim *S)
{
	Sim = S;
	/* set scale for crossover (vdd->gnd) currents */
	crossover_scaling = 1.2;
	/* set non-ideal turnoff percentage */
	turnoff_factor = 0.1;

	opcode_length = 8;
	inst_length = 32;

	 rename_power=0;
	 bpred_power=0;
	 window_power=0;
	 lsq_power=0;
	 regfile_power=0;
	 icache_power=0;
	 dcache_power=0;
	 dcache2_power=0;
	 alu_power=0;
	 falu_power=0;
	 resultbus_power=0;
	 clock_power=0;

	 rename_power_cc1=0;
	 bpred_power_cc1=0;
	 window_power_cc1=0;
	 lsq_power_cc1=0;
	 regfile_power_cc1=0;
	 icache_power_cc1=0;
	 dcache_power_cc1=0;
	 dcache2_power_cc1=0;
	 alu_power_cc1=0;
	 resultbus_power_cc1=0;
	 clock_power_cc1=0;

	 rename_power_cc2=0;
	 bpred_power_cc2=0;
	 window_power_cc2=0;
	 lsq_power_cc2=0;
	 regfile_power_cc2=0;
	 icache_power_cc2=0;
	 dcache_power_cc2=0;
	 dcache2_power_cc2=0;
	 alu_power_cc2=0;
	 resultbus_power_cc2=0;
	 clock_power_cc2=0;

	 rename_power_cc3=0;
	 bpred_power_cc3=0;
	 window_power_cc3=0;
	 lsq_power_cc3=0;
	 regfile_power_cc3=0;
	 icache_power_cc3=0;
	 dcache_power_cc3=0;
	 dcache2_power_cc3=0;
	 alu_power_cc3=0;
	 resultbus_power_cc3=0;
	 clock_power_cc3=0;

	 total_cycle_power;
	 total_cycle_power_cc1;
	 total_cycle_power_cc2;
	 total_cycle_power_cc3;

	 last_single_total_cycle_power_cc1 = 0.0;
	 last_single_total_cycle_power_cc2 = 0.0;
	 last_single_total_cycle_power_cc3 = 0.0;
	 current_total_cycle_power_cc1;
	 current_total_cycle_power_cc2;
	 current_total_cycle_power_cc3;

	 max_cycle_power_cc1 = 0.0;
	 max_cycle_power_cc2 = 0.0;
	 max_cycle_power_cc3 = 0.0;

	 total_rename_access=0;
	 total_bpred_access=0;
	 total_window_access=0;
	 total_lsq_access=0;
	 total_regfile_access=0;
	 total_icache_access=0;
	 total_dcache_access=0;
	 total_dcache2_access=0;
	 total_alu_access=0;
	 total_resultbus_access=0;

	 power0 = (power_result_type *) malloc(sizeof(power_result_type));

	 /* CACTI 4.1 */

	 TECH_FACTOR = new char[8];

	 // inicializando com tecnologia de 350nanometros
	 strcpy(TECH_FACTOR, "TECH_350");

	 GEN_POWER_FACTOR =  1.31;

	 /* TECH_POINT800nm */

	 if (!Sim->MISC->mystricmp(TECH_FACTOR, "TECH_800")){
	 	/* scaling factors */
	 	CSCALE =			1.0;			/* wire capacitance scaling factor */
	 	RSCALE =			1.0;			/* wire resistance scaling factor */
	 	LSCALE =			1.0;			/* length (feature) scaling factor */
	 	ASCALE =			(LSCALE*LSCALE);/* area scaling factor */
	 	VSCALE =			1.0;			/* voltage scaling factor */
	 	VTSCALE =			1.0;			/* threshold voltage scaling factor */
	 	SSCALE =			1.0;			/* sense voltage scaling factor */
	 	TECH_LENGTH =		0.8;         	/* TECH LENGTH */
	 	Mhz =		    	600e6;			/* defined by Wattch */
	 	GEN_POWER_SCALE = 	(GEN_POWER_FACTOR * GEN_POWER_FACTOR * GEN_POWER_FACTOR * GEN_POWER_FACTOR);
	 }

	 /* TECH_POINT400nm */

	 else if (!Sim->MISC->mystricmp(TECH_FACTOR, "TECH_400")){
	 	CSCALE =			1.0;			/* wire capacitance scaling factor */
	 	RSCALE =			1.0;			/* wire resistance scaling factor */
	 	LSCALE =			0.5;			/* length (feature) scaling factor */
	 	ASCALE =			(LSCALE*LSCALE);/* area scaling factor */
	 	VSCALE =			1.0;			/* voltage scaling factor */
	 	VTSCALE =			1.0;			/* threshold voltage scaling factor */
	 	SSCALE =			1.0;			/* sense voltage scaling factor */
	 	TECH_LENGTH =		0.4;         	/* TECH LENGTH */
	 	Mhz =				600e6;			/* defined by Wattch */
	 	GEN_POWER_SCALE =	(GEN_POWER_FACTOR * GEN_POWER_FACTOR * GEN_POWER_FACTOR);
	 }

	 /* TECH_POINT350nm */

	 else if (!Sim->MISC->mystricmp(TECH_FACTOR, "TECH_350")){
	 	CSCALE =			(5.2197);		/* wire capacitance scaling factor */
	 	RSCALE =			(5.2571);		/* wire resistance scaling factor */
	 	LSCALE = 			0.4375;			/* length (feature) scaling factor */
	 	ASCALE =			(LSCALE*LSCALE);/* area scaling factor */

	 	VSCALE =			0.5;			/* voltage scaling factor */
	 	VTSCALE =			0.6147;			/* threshold voltage scaling factor */
	 	SSCALE =			0.95;			/* sense voltage scaling factor */
	 	TECH_LENGTH =			0.35;         	/* TECH LENGTH */

	 	Mhz =				600e6;			/* defined by Wattch */
	 	GEN_POWER_SCALE =	(GEN_POWER_FACTOR*GEN_POWER_FACTOR);
	 }

	 /* TECH_POINT250nm */

	 else if (!Sim->MISC->mystricmp(TECH_FACTOR, "TECH_250"))
	 {
	 	CSCALE =			(10.2197);		/* wire capacitance scaling factor */
	 	RSCALE =			(10.2571);		/* wire resistance scaling factor */
	 	LSCALE =			0.3571;			/* length (feature) scaling factor */
	 	ASCALE =			(LSCALE*LSCALE);/* area scaling factor */
	 	VSCALE =			0.45;			/* voltage scaling factor */
	 	VTSCALE =			0.5596;			/* threshold voltage scaling factor */
	 	SSCALE =			0.90;			/* sense voltage scaling factor */
	 	TECH_LENGTH =		0.25;         	/* TECH LENGTH */
	 	Mhz =				600e6;			/* defined by Wattch */
	 	GEN_POWER_SCALE = 	GEN_POWER_FACTOR;
	 }

	 /* HotLeakage definition */

	 /* TECH_POINT180nm */

	 else if (!Sim->MISC->mystricmp(TECH_FACTOR, "TECH_180"))
	 {
	 	 CSCALE =		19.7172;			/* wire capacitance scaling factor */
	 	 RSCALE	=		20.0000;			/* wire resistance scaling factor */
	 	 LSCALE	=		0.2250;				/* length (feature) scaling factor */
	 	 ASCALE	=		(LSCALE*LSCALE);	/* area scaling factor */
	 	 VSCALE	=		0.4;				/* voltage scaling factor */
	 	 VTSCALE =		0.5046;				/* threshold voltage scaling factor */
	 	 SSCALE	=		0.85;				/* sense voltage scaling factor */
	 	 TECH_LENGTH =	0.18;         		/* TECH LENGTH */
	 	 Mhz =		    1000e6;
	 	 GEN_POWER_SCALE = 1;
	 }

	 /* TECH_POINT130nm */

	 else if (!Sim->MISC->mystricmp(TECH_FACTOR, "TECH_130"))
	   {
	  CSCALE	=	40.00;	/* wire capacitance scaling factor */
	  RSCALE	=	40.0000;	/* wire resistance scaling factor */
	  LSCALE	=	0.1440;		/* length (feature) scaling factor */
	  ASCALE	=	(LSCALE*LSCALE);	/* area scaling factor */
	  VSCALE	=	0.3;		/* voltage scaling factor */
	  VTSCALE	=	0.4980	;	/* threshold voltage scaling factor */
	  SSCALE	=	0.825	;	/* sense voltage scaling factor */
	  Mhz      =       1700e6;
	  TECH_LENGTH =    0.13;           /* TECH LENGTH */
	  GEN_POWER_SCALE= 1;

	   }

	 /* TECH_POINT100nm */

	 else if (!Sim->MISC->mystricmp(TECH_FACTOR, "TECH_100"))
	        {
	  CSCALE	=	84.2172;	/* wire capacitance scaling factor */
	 			/* linear: 51.7172, predicted: 84.2172 */
	  RSCALE	=	80.0000;	/* wire resistance scaling factor */
	  LSCALE	=	0.1250;		/* length (feature) scaling factor */
	  ASCALE	=	LSCALE*LSCALE;	/* area scaling factor */
	  // VSCALE		0.38		/* voltage scaling factor Sim-wattch definition */
	  VSCALE	=	0.24;		/* voltage scaling factor HotLeakage definition */
	  VTSCALE	=	0.49	;	/* threshold voltage scaling factor */
	  SSCALE	=	0.80;		/* sense voltage scaling factor */
	  TECH_LENGTH    = 0.10 ;           /* TECH LENGTH */
	  Mhz            = 3000e6;
	  GEN_POWER_SCALE = (1/GEN_POWER_FACTOR);
	 }

	 /* TECH_POINT70nm */

	 else if (!Sim->MISC->mystricmp(TECH_FACTOR, "TECH_070"))
	   {
	  CSCALE	=	160.2172;	/* wire capacitance scaling factor */
	 			/* linear: 51.7172, predicted: 84.2172 */
	  RSCALE	=	(160.0000);	/* wire resistance scaling factor */
	  LSCALE	=	0.0800	;	/* length (feature) scaling factor */
	  ASCALE	=	(LSCALE*LSCALE);	/* area scaling factor */
	  VSCALE	=	0.20;		/* voltage scaling factor */
	  VTSCALE	=	0.45;		/* threshold voltage scaling factor */
	  SSCALE	=	0.775;		/* sense voltage scaling factor */
	  TECH_LENGTH  =   0.07;            /* TECH LENGTH */
	  Mhz          =   5600e6;
	  GEN_POWER_SCALE = (1/GEN_POWER_FACTOR);

	 }

	 fprintf(stderr,"POWERINIT:TECH_FACTOR: %s\n",TECH_FACTOR);
	 fprintf(stderr,"POWERINIT:TECH_LENGTH: %g\n",TECH_LENGTH);

	 /*
	  * CMOS 0.8um model parameters
	  *   - from Appendix II of Cacti tech report
	  */

	 /* corresponds to 8um of m3 @ 225ff/um */
	  Cwordmetal  = (1.8e-15 * (CSCALE * ASCALE)) ;

	 /* corresponds to 16um of m2 @ 275ff/um */
	  Cbitmetal   = (4.4e-15 * (CSCALE * ASCALE)) ;

	  /* corresponds to 1um of m2 @ 275ff/um */
	  Cmetal  =      Cbitmetal/16 ;

 	  CM3metal  =      Cbitmetal/16 ;
 	  CM2metal   =     Cbitmetal/16 ;

 	 /*  Cmetal 1.222e-15 */

	 /* fF/um2 at 1.5V */
	  Cndiffarea  =  0.137e-15;		/* FIXME: ??? */

	 /* fF/um2 at 1.5V */
	  Cpdiffarea  =  0.343e-15;		/* FIXME: ??? */

	 /* fF/um at 1.5V */
	  Cndiffside   = 0.275e-15;		/* in general this does not scale */

	 /* fF/um at 1.5V */
	  Cpdiffside   = 0.275e-15;		/* in general this does not scale */

	 /* fF/um at 1.5V */
	  Cndiffovlp  =  0.138e-15;		/* FIXME: by depth??? */

	 /* fF/um at 1.5V */
	  Cpdiffovlp   = 0.138e-15;		/* FIXME: by depth??? */

	 /* fF/um assuming 25% Miller effect */
	  Cnoxideovlp =  0.263e-15;		/* FIXME: by depth??? */

	 /* fF/um assuming 25% Miller effect */
	  Cpoxideovlp =  0.338e-15;		/* FIXME: by depth??? */

	 /* um */
	  Leff        = (0.8 * LSCALE) ;

	 /* fF/um2 */
	  Cgate     =    1.95e-15;		/* FIXME: ??? */

	 /* fF/um2 */
	  Cgatepass    = 1.45e-15;		/* FIXME: ??? */

	 /* note that the value of Cgatepass will be different depending on
	    whether or not the source and drain are at different potentials or
	    the same potential.  The two values were averaged */

	 /* fF/um */
	  Cpolywire	= (0.25e-15 * CSCALE * LSCALE) ;

	 /* ohms*um of channel width */
	  Rnchannelstatic	= (25800 * LSCALE) ;

	 /* ohms*um of channel width */
	  Rpchannelstatic =	(61200 * LSCALE) ;

	  Rnchannelon =	(9723 * LSCALE) ;

	  Rpchannelon =	(22400 * LSCALE) ;

	 /* corresponds to 16um of m2 @ 48mO/sq */
	  Rbitmetal =	(0.320 * (RSCALE * ASCALE)) ;

	 /* corresponds to  8um of m3 @ 24mO/sq */
	  Rwordmetal =	(0.080 * (RSCALE * ASCALE)) ;

	  Vdd	=	(5 * VSCALE) ;

	 /* other stuff (from tech report, appendix 1) */

	  Period        = (1/Mhz) ;
	  krise	=	(0.4e-9 * LSCALE) ;
	  tsensedata	= (5.8e-10 * LSCALE) ;
	  tsensetag =	(2.6e-10 * LSCALE) ;
	  tfalldata =	(7e-10 * LSCALE) ;
	  tfalltag =	(7e-10 * LSCALE) ;
	  Vbitpre  =		(3.3 * SSCALE) ;
	  Vt	 =	(1.09 * VTSCALE) ;
	  Vbitsense =	(0.10 * SSCALE) ;
	  Sense2Powerfactor= (Mhz)*(2*.3+.1*Vdd);
	  LowSwingPowerfactor= (Mhz)*.2*.2;

	  MSCALE= (LSCALE * .624 / .2250);

	  Powerfactor =  (Mhz)*Vdd*Vdd;

	  SensePowerfactor3 =  (Mhz)*(Vbitsense)*(Vbitsense);
	  SensePowerfactor2 = (Mhz)*(Vbitpre-Vbitsense)*(Vbitpre-Vbitsense);
	  SensePowerfactor =  (Mhz)*(Vdd/2)*(Vdd/2);

	  AF   =   0.5;
	  POPCOUNT_AF =  (23.9/64.0) ;

	 /* Threshold voltages (as a proportion of Vdd)
	    If you don't know them, set all values to 0.5 */
	  VSINV        =  0.456 ;
	  VTHINV100x60  =  0.438;   /* inverter with p=100,n=60 */
	  VTHNAND60x90 =  0.561 ;  /* nand with p=60 and three n=90 */
	  VTHNOR12x4x1  =  0.503 ;  /* nor with p=12, n=4, 1 input */
	  VTHNOR12x4x2 =  0.452  ; /* nor with p=12, n=4, 2 inputs */
	  VTHNOR12x4x3 =  0.417  ; /* nor with p=12, n=4, 3 inputs */
	  VTHNOR12x4x4 =  0.390  ; /* nor with p=12, n=4, 4 inputs */
	  VTHOUTDRINV  =   0.437;
	  VTHOUTDRNOR  =  0.431;
	  VTHOUTDRNAND =  0.441;
	  VTHOUTDRIVE  =  0.425;
	  VTHCOMPINV   =  0.437;
	  VTHMUXDRV1  =   0.437;
	  VTHMUXDRV2   =  0.486;
	  VTHMUXDRV3  =   0.437;
	  VTHEVALINV   =  0.267;
	  VTHSENSEEXTDRV =   0.437;
	 /* transistor widths in um (as described in tech report, appendix 1) */
	  Wdecdrivep =	(57.0 * LSCALE) ;
	  Wdecdriven =	(40.0 * LSCALE) ;
	  Wdec3to8n = 	(14.4 * LSCALE) ;
	  Wdec3to8p =	(14.4 * LSCALE) ;
	  WdecNORn = 	(5.4 * LSCALE) ;
	  WdecNORp =	(30.5 * LSCALE) ;
	  Wdecinvn =	(5.0 * LSCALE) ;
	  Wdecinvp	= (10.0  * LSCALE) ;

	  Wworddrivemax =	(100.0 * LSCALE) ;
	  Wmemcella =	(2.4 * LSCALE) ;
	  Wmemcellr =	(4.0 * LSCALE) ;
	  Wmemcellw =	(2.1 * LSCALE) ;
	  Wmemcellbscale =	2;		/* means 2x bigger than Wmemcella */
	  Wbitpreequ =	(10.0 * LSCALE) ;
	  Wbitmuxn =	(10.0 * LSCALE) ;
	  WsenseQ1to4 = 	(4.0 * LSCALE) ;
	  Wcompinvp1  =	(10.0 * LSCALE) ;
	  Wcompinvn1  =	(6.0 * LSCALE) ;
	  Wcompinvp2  =	(20.0 * LSCALE) ;
	  Wcompinvn2  =	(12.0 * LSCALE) ;
	  Wcompinvp3  =	(40.0 * LSCALE) ;
	  Wcompinvn3  =	(24.0 * LSCALE) ;
	  Wevalinvp  =	(20.0 * LSCALE) ;
	  Wevalinvn  =	(80.0 * LSCALE) ;

	  Wcompn	  =	(20.0 * LSCALE) ;
	  Wcompp	  =	(30.0 * LSCALE) ;
	  Wcomppreequ   = (40.0 * LSCALE) ;
	  Wmuxdrv12n  =	(30.0 * LSCALE) ;
	  Wmuxdrv12p  =	(50.0 * LSCALE) ;
	  WmuxdrvNANDn  = (20.0 * LSCALE) ;
	  WmuxdrvNANDp  = (80.0 * LSCALE) ;
	  WmuxdrvNORn  =	(60.0 * LSCALE) ;
	  WmuxdrvNORp =	(80.0 * LSCALE) ;
	  Wmuxdrv3n   = (200.0 * LSCALE) ;
	  Wmuxdrv3p   = (480.0 * LSCALE) ;
	  Woutdrvseln   = (12.0 * LSCALE) ;
	  Woutdrvselp   = (20.0 * LSCALE) ;
	  Woutdrvnandn   = (24.0 * LSCALE) ;
	  Woutdrvnandp   = (10.0 * LSCALE) ;
	  Woutdrvnorn   = (6.0 * LSCALE) ;
	  Woutdrvnorp   = (40.0 * LSCALE) ;
	  Woutdrivern   = (48.0 * LSCALE) ;
	  Woutdriverp   = (80.0 * LSCALE) ;
	  Wcompcellpd2  = (2.4 * LSCALE) ;
	  Wcompdrivern  = (400.0 * LSCALE) ;
	  Wcompdriverp  = (800.0 * LSCALE) ;
	  Wcomparen2    = (40.0 * LSCALE) ;
	  Wcomparen1    = (20.0 * LSCALE) ;
	  Wmatchpchg    = (10.0 * LSCALE) ;
	  Wmatchinvn    = (10.0 * LSCALE) ;
	  Wmatchinvp    = (20.0 * LSCALE) ;
	  Wmatchnandn   = (20.0 * LSCALE) ;
	  Wmatchnandp   = (10.0 * LSCALE) ;
	  Wmatchnorn   = (20.0 * LSCALE) ;
	  Wmatchnorp   = (10.0 * LSCALE) ;

	  WSelORn       = (10.0 * LSCALE) ;
	  WSelORprequ   = (40.0 * LSCALE) ;
	  WSelPn        = (10.0 * LSCALE) ;
	  WSelPp        = (15.0 * LSCALE) ;
	  WSelEnn       = (5.0 * LSCALE) ;
	  WSelEnp       = (10.0 * LSCALE) ;
	  Wsenseextdrv1p =  (40.0*LSCALE) ;
	  Wsenseextdrv1n  = (24.0*LSCALE) ;
	  Wsenseextdrv2p  = (200.0*LSCALE) ;
	  Wsenseextdrv2n  = (120.0*LSCALE) ;

	 /* bit width of RAM cell in um */
	  BitWidth   = (16.0 * LSCALE) ;

	 /* bit height of RAM cell in um */
	  BitHeight   = (16.0 * LSCALE) ;

	  Cout	   = (0.5e-12 * LSCALE) ;

	 /* Sizing of cells and spacings */
	  RatCellHeight  = (40.0 * LSCALE) ;
	  RatCellWidth   = (70.0 * LSCALE) ;
	  RatShiftRegWidth = (120.0 * LSCALE) ;
	  RatNumShift    =  4;
	  BitlineSpacing = (6.0 * LSCALE) ;
	  WordlineSpacing = (6.0 * LSCALE) ;

	  RegCellHeight  = (16.0 * LSCALE) ;
	  RegCellWidth   = (8.0  * LSCALE) ;

	  CamCellHeight  = (40.0 * LSCALE) ;
	  CamCellWidth   = (25.0 * LSCALE) ;
	  MatchlineSpacing =  (6.0 * LSCALE) ;
	  TaglineSpacing = (6.0 * LSCALE) ;

	 /*===================================================================*/

	 /* ALU POWER NUMBERS for .18um 733Mhz */
	 /* normalize to cap from W */
	  NORMALIZE_SCALE  = (1.0/(733.0e6*1.45*1.45)) ;
	 /* normalize .18um cap to other gen's cap, then xPowerfactor */
	  POWER_SCALE  = (GEN_POWER_SCALE * NORMALIZE_SCALE * Powerfactor) ;
	  I_ADD        = ((.37 - .091) * POWER_SCALE) ;
	  I_ADD32      = (((.37 - .091)/2)  *POWER_SCALE) ;
	  I_MULT16     = ((.31-.095)*POWER_SCALE) ;
	  I_SHIFT      = ((.21-.089)*POWER_SCALE) ;
	  I_LOGIC      = ((.04-.015)*POWER_SCALE) ;
	  F_ADD        = ((1.307-.452)*POWER_SCALE) ;
	  F_MULT       = ((1.307-.452)*POWER_SCALE) ;

	  I_ADD_CLOCK  = (.091*POWER_SCALE) ;
	  I_MULT_CLOCK = (.095*POWER_SCALE) ;
	  I_SHIFT_CLOCK = (.089*POWER_SCALE) ;
	  I_LOGIC_CLOCK = (.015*POWER_SCALE) ;
	  F_ADD_CLOCK  = (.452*POWER_SCALE) ;
	  F_MULT_CLOCK = (.452*POWER_SCALE) ;

/* CACTI 4.1 */


}



/*----------------------------------------------------------------------*/
/* funcoes externas, porem internas                                     */

int pow2(int x) {
  return((int)pow(2.0,(double)x));
}

double logfour(double x)
{
  if (x<=0) fprintf(stderr,"%e\n",x);
  return( (double) (log(x)/log(4.0)) );
}
/*-----------------------------------------------------------------------*/


/* safer pop count to validate the fast algorithm */
int power::pop_count_slow(quad_t bits)
{
  int count = 0;
  quad_t tmpbits = bits;
  while (tmpbits) {
    if (tmpbits & 1) ++count;
    tmpbits >>= 1;
  }
  return count;
}

/* fast pop count */
int power::pop_count(quad_t bits)
{
#define T unsigned long long
#define ONES ((T)(-1))
#define TWO(k) ((T)1 << (k))
#define CYCL(k) (ONES/(1 + (TWO(TWO(k)))))
#define BSUM(x,k) ((x)+=(x) >> TWO(k), (x) &= CYCL(k))
  quad_t x = bits;
  x = (x & CYCL(0)) + ((x>>TWO(0)) & CYCL(0));
  x = (x & CYCL(1)) + ((x>>TWO(1)) & CYCL(1));
  BSUM(x,2);
  BSUM(x,3);
  BSUM(x,4);
  BSUM(x,5);
  return x;
}




//extern int ruu_decode_width;
//extern int ruu_issue_width;
//extern int ruu_commit_width;
//extern int RUU_size;
//extern int LSQ_size;
//extern int data_width;
//extern int res_ialu;
//extern int res_fpalu;
//extern int res_memport;





//extern int bimod_config[];

//extern struct cache_t *cache_dl1;
//extern struct cache_t *cache_il1;
//extern struct cache_t *cache_dl2;

//extern struct cache_t *dtlb;
//extern struct cache_t *itlb;

/* 2-level predictor config (<l1size> <l2size> <hist_size> <xor>) */
//extern int twolev_config[];

/* combining predictor config (<meta_table_size> */
//extern int comb_config[];

/* return address stack (RAS) size */
//extern int ras_size;

/* BTB predictor config (<num_sets> <associativity>) */
//extern int btb_config[];



/*
extern counter_t rename_access;
extern counter_t bpred_access;
extern counter_t window_access;
extern counter_t lsq_access;
extern counter_t regfile_access;
extern counter_t icache_access;
extern counter_t dcache_access;
extern counter_t dcache2_access;
extern counter_t alu_access;
extern counter_t ialu_access;
extern counter_t falu_access;
extern counter_t resultbus_access;

extern counter_t window_selection_access;
extern counter_t window_wakeup_access;
extern counter_t window_preg_access;
extern counter_t lsq_preg_access;
extern counter_t lsq_wakeup_access;
extern counter_t lsq_store_data_access;
extern counter_t lsq_load_data_access;

extern counter_t window_total_pop_count_cycle;
extern counter_t window_num_pop_count_cycle;
extern counter_t lsq_total_pop_count_cycle;
extern counter_t lsq_num_pop_count_cycle;
extern counter_t regfile_total_pop_count_cycle;
extern counter_t regfile_num_pop_count_cycle;
extern counter_t resultbus_total_pop_count_cycle;
extern counter_t resultbus_num_pop_count_cycle;
*/


void power::clear_access_stats()
{
  Sim->rename_access=0;
  Sim->bpred_access=0;
  Sim->window_access=0;
  Sim->lsq_access=0;
  Sim->regfile_access=0;
  Sim->icache_access=0;
  Sim->dcache_access=0;
  Sim->dcache2_access=0;
  Sim->alu_access=0;
  Sim->ialu_access=0;
  Sim->falu_access=0;
  Sim->resultbus_access=0;

  Sim->window_preg_access=0;
  Sim->window_selection_access=0;
  Sim->window_wakeup_access=0;
  Sim->lsq_store_data_access=0;
  Sim->lsq_load_data_access=0;
  Sim->lsq_wakeup_access=0;
  Sim->lsq_preg_access=0;

  Sim->window_total_pop_count_cycle=0;
  Sim->window_num_pop_count_cycle=0;
  Sim->lsq_total_pop_count_cycle=0;
  Sim->lsq_num_pop_count_cycle=0;
  Sim->regfile_total_pop_count_cycle=0;
  Sim->regfile_num_pop_count_cycle=0;
  Sim->resultbus_total_pop_count_cycle=0;
  Sim->resultbus_num_pop_count_cycle=0;
}

/* compute bitline activity factors which we use to scale bitline power
   Here it is very important whether we assume 0's or 1's are
   responsible for dissipating power in pre-charged stuctures. (since
   most of the bits are 0's, we assume the design is power-efficient
   enough to allow 0's to _not_ discharge
*/
double power::compute_af(counter_t num_pop_count_cycle,counter_t total_pop_count_cycle,int pop_width) {
  double avg_pop_count;
  double af,af_b;

  if(num_pop_count_cycle)
    avg_pop_count = (double)total_pop_count_cycle / (double)num_pop_count_cycle;
  else
    avg_pop_count = 0;

  af = avg_pop_count / (double)pop_width;

  af_b = 1.0 - af;

  /*  printf("af == %f%%, af_b == %f%%, total_pop == %d, num_pop == %d\n",100*af,100*af_b,total_pop_count_cycle,num_pop_count_cycle); */

  return(af_b);
}

/* compute power statistics on each cycle, for each conditional clocking style.  Obviously
most of the speed penalty comes here, so if you don't want per-cycle power estimates
you could post-process

See README.wattch for details on the various clock gating styles.

*/
void power::update_power_stats()
{
  double window_af_b, lsq_af_b, regfile_af_b, resultbus_af_b;

#ifdef DYNAMIC_AF
  window_af_b = compute_af(Sim->window_num_pop_count_cycle,Sim->window_total_pop_count_cycle,Sim->data_width);
  lsq_af_b = compute_af(Sim->lsq_num_pop_count_cycle,Sim->lsq_total_pop_count_cycle,Sim->data_width);
  regfile_af_b = compute_af(Sim->regfile_num_pop_count_cycle,Sim->regfile_total_pop_count_cycle,Sim->data_width);
  resultbus_af_b = compute_af(Sim->resultbus_num_pop_count_cycle,Sim->resultbus_total_pop_count_cycle,Sim->data_width);
#endif

//  printf("rename_power: %.4f  dcache_power: %.4f%\n",  rename_power, dcache_power);

  rename_power+=power0->rename_power;
  bpred_power+=power0->bpred_power;
  window_power+=power0->window_power;
  lsq_power+=power0->lsq_power;
  regfile_power+=power0->regfile_power;
  icache_power+=power0->icache_power+power0->itlb;
  dcache_power+=power0->dcache_power+power0->dtlb;
  dcache2_power+=power0->dcache2_power;
  alu_power+=power0->ialu_power + power0->falu_power;
  falu_power+=power0->falu_power;
  resultbus_power+=power0->resultbus;
  clock_power+=power0->clock_power;

  total_rename_access+=Sim->rename_access;
  total_bpred_access+=Sim->bpred_access;
  total_window_access+=Sim->window_access;
  total_lsq_access+=Sim->lsq_access;
  total_regfile_access+=Sim->regfile_access;
  total_icache_access+=Sim->icache_access;
  total_dcache_access+=Sim->dcache_access;
  total_dcache2_access+=Sim->dcache2_access;
  total_alu_access+=Sim->alu_access;
  total_resultbus_access+=Sim->resultbus_access;

  max_rename_access=MAX(Sim->rename_access,max_rename_access);
  max_bpred_access=MAX(Sim->bpred_access,max_bpred_access);
  max_window_access=MAX(Sim->window_access,max_window_access);
  max_lsq_access=MAX(Sim->lsq_access,max_lsq_access);
  max_regfile_access=MAX(Sim->regfile_access,max_regfile_access);
  max_icache_access=MAX(Sim->icache_access,max_icache_access);
  max_dcache_access=MAX(Sim->dcache_access,max_dcache_access);
  max_dcache2_access=MAX(Sim->dcache2_access,max_dcache2_access);
  max_alu_access=MAX(Sim->alu_access,max_alu_access);
  max_resultbus_access=MAX(Sim->resultbus_access,max_resultbus_access);

  if(Sim->rename_access) {
    rename_power_cc1+=power0->rename_power;
    rename_power_cc2+=((double)Sim->rename_access/(double)Sim->ruu_decode_width)*power0->rename_power;
    rename_power_cc3+=((double)Sim->rename_access/(double)Sim->ruu_decode_width)*power0->rename_power;
  }
  else
    rename_power_cc3+=turnoff_factor*power0->rename_power;

  if(Sim->bpred_access) {
    if(Sim->bpred_access <= 2)
      bpred_power_cc1+=power0->bpred_power;
    else
      bpred_power_cc1+=((double)Sim->bpred_access/2.0) * power0->bpred_power;
    bpred_power_cc2+=((double)Sim->bpred_access/2.0) * power0->bpred_power;
    bpred_power_cc3+=((double)Sim->bpred_access/2.0) * power0->bpred_power;
  }
  else
    bpred_power_cc3+=turnoff_factor*power0->bpred_power;

#ifdef STATIC_AF
  if(Sim->window_preg_access) {
    if(Sim->window_preg_access <= 3*Sim->ruu_issue_width)
      window_power_cc1+=power0->rs_power;
    else
      window_power_cc1+=((double)Sim->window_preg_access/(3.0*(double)Sim->ruu_issue_width))*power0->rs_power;
    window_power_cc2+=((double)Sim->window_preg_access/(3.0*(double)Sim->ruu_issue_width))*power0->rs_power;
    window_power_cc3+=((double)Sim->window_preg_access/(3.0*(double)Sim->ruu_issue_width))*power0->rs_power;
  }
  else
    window_power_cc3+=turnoff_factor*power0->rs_power;
#elif defined(DYNAMIC_AF)
  if(Sim->window_preg_access) {
    if(Sim->window_preg_access <= 3*Sim->ruu_issue_width)
      window_power_cc1+=power0->rs_power_nobit + window_af_b*power0->rs_bitline;
    else
      window_power_cc1+=((double)Sim->window_preg_access/(3.0*(double)Sim->ruu_issue_width))*(power0->rs_power_nobit + window_af_b*power0->rs_bitline);
    window_power_cc2+=((double)Sim->window_preg_access/(3.0*(double)Sim->ruu_issue_width))*(power0->rs_power_nobit + window_af_b*power0->rs_bitline);
    window_power_cc3+=((double)Sim->window_preg_access/(3.0*(double)Sim->ruu_issue_width))*(power0->rs_power_nobit + window_af_b*power0->rs_bitline);
  }
  else
    window_power_cc3+=turnoff_factor*power0->rs_power;
#else
  panic("no AF-style defined\n");
#endif

  if(Sim->window_selection_access) {
    if(Sim->window_selection_access <= Sim->ruu_issue_width)
      window_power_cc1+=power0->selection;
    else
      window_power_cc1+=((double)Sim->window_selection_access/((double)Sim->ruu_issue_width))*power0->selection;
    window_power_cc2+=((double)Sim->window_selection_access/((double)Sim->ruu_issue_width))*power0->selection;
    window_power_cc3+=((double)Sim->window_selection_access/((double)Sim->ruu_issue_width))*power0->selection;
  }
  else
    window_power_cc3+=turnoff_factor*power0->selection;

  if(Sim->window_wakeup_access) {
    if(Sim->window_wakeup_access <= Sim->ruu_issue_width)
      window_power_cc1+=power0->wakeup_power;
    else
      window_power_cc1+=((double)Sim->window_wakeup_access/((double)Sim->ruu_issue_width))*power0->wakeup_power;
    window_power_cc2+=((double)Sim->window_wakeup_access/((double)Sim->ruu_issue_width))*power0->wakeup_power;
    window_power_cc3+=((double)Sim->window_wakeup_access/((double)Sim->ruu_issue_width))*power0->wakeup_power;
  }
  else
    window_power_cc3+=turnoff_factor*power0->wakeup_power;

  if(Sim->lsq_wakeup_access) {
    if(Sim->lsq_wakeup_access <= Sim->res_memport)
      lsq_power_cc1+=power0->lsq_wakeup_power;
    else
      lsq_power_cc1+=((double)Sim->lsq_wakeup_access/((double)Sim->res_memport))*power0->lsq_wakeup_power;
    lsq_power_cc2+=((double)Sim->lsq_wakeup_access/((double)Sim->res_memport))*power0->lsq_wakeup_power;
    lsq_power_cc3+=((double)Sim->lsq_wakeup_access/((double)Sim->res_memport))*power0->lsq_wakeup_power;
  }
  else
    lsq_power_cc3+=turnoff_factor*power0->lsq_wakeup_power;

#ifdef STATIC_AF
  if(Sim->lsq_preg_access) {
    if(Sim->lsq_preg_access <= Sim->res_memport)
      lsq_power_cc1+=power0->lsq_rs_power;
    else
      lsq_power_cc1+=((double)Sim->lsq_preg_access/((double)Sim->res_memport))*power0->lsq_rs_power;
    lsq_power_cc2+=((double)Sim->lsq_preg_access/((double)Sim->res_memport))*power0->lsq_rs_power;
    lsq_power_cc3+=((double)Sim->lsq_preg_access/((double)Sim->res_memport))*power0->lsq_rs_power;
  }
  else
    lsq_power_cc3+=turnoff_factor*power0->lsq_rs_power;
#else
  if(Sim->lsq_preg_access) {
    if(Sim->lsq_preg_access <= Sim->res_memport)
      lsq_power_cc1+=power0->lsq_rs_power_nobit + lsq_af_b*power0->lsq_rs_bitline;
    else
      lsq_power_cc1+=((double)Sim->lsq_preg_access/((double)Sim->res_memport))*(power0->lsq_rs_power_nobit + lsq_af_b*power0->lsq_rs_bitline);
    lsq_power_cc2+=((double)Sim->lsq_preg_access/((double)Sim->res_memport))*(power0->lsq_rs_power_nobit + lsq_af_b*power0->lsq_rs_bitline);
    lsq_power_cc3+=((double)Sim->lsq_preg_access/((double)Sim->res_memport))*(power0->lsq_rs_power_nobit + lsq_af_b*power0->lsq_rs_bitline);
  }
  else
    lsq_power_cc3+=turnoff_factor*power0->lsq_rs_power;
#endif

#ifdef STATIC_AF
  if(Sim->regfile_access) {
    if(Sim->regfile_access <= (3.0*Sim->ruu_commit_width))
      regfile_power_cc1+=power0->regfile_power;
    else
      regfile_power_cc1+=((double)Sim->regfile_access/(3.0*(double)Sim->ruu_commit_width))*power0->regfile_power;
    regfile_power_cc2+=((double)Sim->regfile_access/(3.0*(double)Sim->ruu_commit_width))*power0->regfile_power;
    regfile_power_cc3+=((double)Sim->regfile_access/(3.0*(double)Sim->ruu_commit_width))*power0->regfile_power;
  }
  else
    regfile_power_cc3+=turnoff_factor*power0->regfile_power;
#else
  if(Sim->regfile_access) {
    if(Sim->regfile_access <= (3.0*Sim->ruu_commit_width))
      regfile_power_cc1+=power0->regfile_power_nobit + regfile_af_b*power0->regfile_bitline;
    else
      regfile_power_cc1+=((double)Sim->regfile_access/(3.0*(double)Sim->ruu_commit_width))*(power0->regfile_power_nobit + regfile_af_b*power0->regfile_bitline);
    regfile_power_cc2+=((double)Sim->regfile_access/(3.0*(double)Sim->ruu_commit_width))*(power0->regfile_power_nobit + regfile_af_b*power0->regfile_bitline);
    regfile_power_cc3+=((double)Sim->regfile_access/(3.0*(double)Sim->ruu_commit_width))*(power0->regfile_power_nobit + regfile_af_b*power0->regfile_bitline);
  }
  else
    regfile_power_cc3+=turnoff_factor*power0->regfile_power;
#endif

  if(Sim->icache_access) {
    /* don't scale icache because we assume 1 line is fetched, unless fetch stalls */
    icache_power_cc1+=power0->icache_power+power0->itlb;
    icache_power_cc2+=power0->icache_power+power0->itlb;
    icache_power_cc3+=power0->icache_power+power0->itlb;
  }
  else
    icache_power_cc3+=turnoff_factor*(power0->icache_power+power0->itlb);

  if(Sim->dcache_access) {
    if(Sim->dcache_access <= Sim->res_memport)
      dcache_power_cc1+=power0->dcache_power+power0->dtlb;
    else
      dcache_power_cc1+=((double)Sim->dcache_access/(double)Sim->res_memport)*(power0->dcache_power +
						     power0->dtlb);
    dcache_power_cc2+=((double)Sim->dcache_access/(double)Sim->res_memport)*(power0->dcache_power +
						   power0->dtlb);
    dcache_power_cc3+=((double)Sim->dcache_access/(double)Sim->res_memport)*(power0->dcache_power +
						   power0->dtlb);
  }
  else
    dcache_power_cc3+=turnoff_factor*(power0->dcache_power+power0->dtlb);

  if(Sim->dcache2_access) {
    if(Sim->dcache2_access <= Sim->res_memport)
      dcache2_power_cc1+=power0->dcache2_power;
    else
      dcache2_power_cc1+=((double)Sim->dcache2_access/(double)Sim->res_memport)*power0->dcache2_power;
    dcache2_power_cc2+=((double)Sim->dcache2_access/(double)Sim->res_memport)*power0->dcache2_power;
    dcache2_power_cc3+=((double)Sim->dcache2_access/(double)Sim->res_memport)*power0->dcache2_power;
  }
  else
    dcache2_power_cc3+=turnoff_factor*power0->dcache2_power;

  if(Sim->alu_access) {
    if(Sim->ialu_access)
      alu_power_cc1+=power0->ialu_power;
    else
      alu_power_cc3+=turnoff_factor*power0->ialu_power;
    if(Sim->falu_access)
      alu_power_cc1+=power0->falu_power;
    else
      alu_power_cc3+=turnoff_factor*power0->falu_power;

    alu_power_cc2+=((double)Sim->ialu_access/(double)Sim->res_ialu)*power0->ialu_power +
      ((double)Sim->falu_access/(double)Sim->res_fpalu)*power0->falu_power;
    alu_power_cc3+=((double)Sim->ialu_access/(double)Sim->res_ialu)*power0->ialu_power +
      ((double)Sim->falu_access/(double)Sim->res_fpalu)*power0->falu_power;
  }
  else
    alu_power_cc3+=turnoff_factor*(power0->ialu_power + power0->falu_power);

#ifdef STATIC_AF
  if(Sim->resultbus_access) {
    assert(Sim->ruu_issue_width != 0);
    if(Sim->resultbus_access <= Sim->ruu_issue_width) {
      resultbus_power_cc1+=power0->resultbus;
    }
    else {
      resultbus_power_cc1+=((double)Sim->resultbus_access/(double)Sim->ruu_issue_width)*power0->resultbus;
    }
    resultbus_power_cc2+=((double)Sim->resultbus_access/(double)Sim->ruu_issue_width)*power0->resultbus;
    resultbus_power_cc3+=((double)Sim->resultbus_access/(double)Sim->ruu_issue_width)*power0->resultbus;
  }
  else
    resultbus_power_cc3+=turnoff_factor*power0->resultbus;
#else
  if(Sim->resultbus_access) {
    assert(Sim->ruu_issue_width != 0);
    if(Sim->resultbus_access <= Sim->ruu_issue_width) {
      resultbus_power_cc1+=resultbus_af_b*power0->resultbus;
    }
    else {
      resultbus_power_cc1+=((double)Sim->resultbus_access/(double)Sim->ruu_issue_width)*resultbus_af_b*power0->resultbus;
    }
    resultbus_power_cc2+=((double)Sim->resultbus_access/(double)Sim->ruu_issue_width)*resultbus_af_b*power0->resultbus;
    resultbus_power_cc3+=((double)Sim->resultbus_access/(double)Sim->ruu_issue_width)*resultbus_af_b*power0->resultbus;
  }
  else
    resultbus_power_cc3+=turnoff_factor*power0->resultbus;
#endif

  total_cycle_power = rename_power + bpred_power + window_power +
    lsq_power + regfile_power + icache_power + dcache_power +
    alu_power + resultbus_power;

  total_cycle_power_cc1 = rename_power_cc1 + bpred_power_cc1 +
    window_power_cc1 + lsq_power_cc1 + regfile_power_cc1 +
    icache_power_cc1 + dcache_power_cc1 + alu_power_cc1 +
    resultbus_power_cc1;

  total_cycle_power_cc2 = rename_power_cc2 + bpred_power_cc2 +
    window_power_cc2 + lsq_power_cc2 + regfile_power_cc2 +
    icache_power_cc2 + dcache_power_cc2 + alu_power_cc2 +
    resultbus_power_cc2;

  total_cycle_power_cc3 = rename_power_cc3 + bpred_power_cc3 +
    window_power_cc3 + lsq_power_cc3 + regfile_power_cc3 +
    icache_power_cc3 + dcache_power_cc3 + alu_power_cc3 +
    resultbus_power_cc3;

  clock_power_cc1+=power0->clock_power*(total_cycle_power_cc1/total_cycle_power);
  clock_power_cc2+=power0->clock_power*(total_cycle_power_cc2/total_cycle_power);
  clock_power_cc3+=power0->clock_power*(total_cycle_power_cc3/total_cycle_power);

  total_cycle_power_cc1 += clock_power_cc1;
  total_cycle_power_cc2 += clock_power_cc2;
  total_cycle_power_cc3 += clock_power_cc3;

  current_total_cycle_power_cc1 = total_cycle_power_cc1
    -last_single_total_cycle_power_cc1;
  current_total_cycle_power_cc2 = total_cycle_power_cc2
    -last_single_total_cycle_power_cc2;
  current_total_cycle_power_cc3 = total_cycle_power_cc3
    -last_single_total_cycle_power_cc3;

  max_cycle_power_cc1 = MAX(max_cycle_power_cc1,current_total_cycle_power_cc1);
  max_cycle_power_cc2 = MAX(max_cycle_power_cc2,current_total_cycle_power_cc2);
  max_cycle_power_cc3 = MAX(max_cycle_power_cc3,current_total_cycle_power_cc3);

  last_single_total_cycle_power_cc1 = total_cycle_power_cc1;
  last_single_total_cycle_power_cc2 = total_cycle_power_cc2;
  last_single_total_cycle_power_cc3 = total_cycle_power_cc3;

}

void
power::power_reg_stats(struct stat_sdb_t *sdb)	/* stats database */
{
  Sim->STATS->stat_reg_double(sdb, "rename_power", "total power usage of rename unit", &rename_power, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "bpred_power", "total power usage of bpred unit", &bpred_power, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "window_power", "total power usage of instruction window", &window_power, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "lsq_power", "total power usage of load/store queue", &lsq_power, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "regfile_power", "total power usage of arch. regfile", &regfile_power, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "icache_power", "total power usage of icache", &icache_power, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "dcache_power", "total power usage of dcache", &dcache_power, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "dcache2_power", "total power usage of dcache2", &dcache2_power, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "alu_power", "total power usage of alu", &alu_power, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "falu_power", "total power usage of falu", &falu_power, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "resultbus_power", "total power usage of resultbus", &resultbus_power, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "clock_power", "total power usage of clock", &clock_power, 0, NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_rename_power", "avg power usage of rename unit", "rename_power/sim_cycle", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_bpred_power", "avg power usage of bpred unit", "bpred_power/sim_cycle", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_window_power", "avg power usage of instruction window", "window_power/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_lsq_power", "avg power usage of lsq", "lsq_power/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_regfile_power", "avg power usage of arch. regfile", "regfile_power/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_icache_power", "avg power usage of icache", "icache_power/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_dcache_power", "avg power usage of dcache", "dcache_power/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_dcache2_power", "avg power usage of dcache2", "dcache2_power/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_alu_power", "avg power usage of alu", "alu_power/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_falu_power", "avg power usage of falu", "falu_power/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_resultbus_power", "avg power usage of resultbus", "resultbus_power/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_clock_power", "avg power usage of clock", "clock_power/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "fetch_stage_power", "total power usage of fetch stage", "icache_power + bpred_power", NULL);

  Sim->STATS->stat_reg_formula(sdb, "dispatch_stage_power", "total power usage of dispatch stage", "rename_power", NULL);

  Sim->STATS->stat_reg_formula(sdb, "issue_stage_power", "total power usage of issue stage", "resultbus_power + alu_power + dcache_power + dcache2_power + window_power + lsq_power", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_fetch_power", "average power of fetch unit per cycle", "(icache_power + bpred_power)/ sim_cycle", /* format */NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_dispatch_power", "average power of dispatch unit per cycle", "(rename_power)/ sim_cycle", /* format */NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_issue_power", "average power of issue unit per cycle", "(resultbus_power + alu_power + dcache_power + dcache2_power + window_power + lsq_power)/ sim_cycle", /* format */NULL);

  Sim->STATS->stat_reg_formula(sdb, "total_power", "total power per cycle","(rename_power + bpred_power + window_power + lsq_power + regfile_power + icache_power  + resultbus_power + clock_power + alu_power + dcache_power + dcache2_power)", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_total_power_cycle", "average total power per cycle","(rename_power + bpred_power + window_power + lsq_power + regfile_power + icache_power + resultbus_power + clock_power + alu_power + dcache_power + dcache2_power)/sim_cycle", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_total_power_cycle_nofp_nod2", "average total power per cycle","(rename_power + bpred_power + window_power + lsq_power + regfile_power + icache_power + resultbus_power + clock_power + alu_power + dcache_power - falu_power )/sim_cycle", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_total_power_insn", "average total power per insn","(rename_power + bpred_power + window_power + lsq_power + regfile_power + icache_power + resultbus_power + clock_power + alu_power + dcache_power + dcache2_power)/sim_total_insn", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_total_power_insn_nofp_nod2", "average total power per insn","(rename_power + bpred_power + window_power + lsq_power + regfile_power + icache_power + resultbus_power + clock_power + alu_power + dcache_power - falu_power )/sim_total_insn", NULL);

  Sim->STATS->stat_reg_double(sdb, "rename_power_cc1", "total power usage of rename unit_cc1", &rename_power_cc1, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "bpred_power_cc1", "total power usage of bpred unit_cc1", &bpred_power_cc1, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "window_power_cc1", "total power usage of instruction window_cc1", &window_power_cc1, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "lsq_power_cc1", "total power usage of lsq_cc1", &lsq_power_cc1, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "regfile_power_cc1", "total power usage of arch. regfile_cc1", &regfile_power_cc1, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "icache_power_cc1", "total power usage of icache_cc1", &icache_power_cc1, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "dcache_power_cc1", "total power usage of dcache_cc1", &dcache_power_cc1, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "dcache2_power_cc1", "total power usage of dcache2_cc1", &dcache2_power_cc1, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "alu_power_cc1", "total power usage of alu_cc1", &alu_power_cc1, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "resultbus_power_cc1", "total power usage of resultbus_cc1", &resultbus_power_cc1, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "clock_power_cc1", "total power usage of clock_cc1", &clock_power_cc1, 0, NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_rename_power_cc1", "avg power usage of rename unit_cc1", "rename_power_cc1/sim_cycle", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_bpred_power_cc1", "avg power usage of bpred unit_cc1", "bpred_power_cc1/sim_cycle", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_window_power_cc1", "avg power usage of instruction window_cc1", "window_power_cc1/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_lsq_power_cc1", "avg power usage of lsq_cc1", "lsq_power_cc1/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_regfile_power_cc1", "avg power usage of arch. regfile_cc1", "regfile_power_cc1/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_icache_power_cc1", "avg power usage of icache_cc1", "icache_power_cc1/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_dcache_power_cc1", "avg power usage of dcache_cc1", "dcache_power_cc1/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_dcache2_power_cc1", "avg power usage of dcache2_cc1", "dcache2_power_cc1/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_alu_power_cc1", "avg power usage of alu_cc1", "alu_power_cc1/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_resultbus_power_cc1", "avg power usage of resultbus_cc1", "resultbus_power_cc1/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_clock_power_cc1", "avg power usage of clock_cc1", "clock_power_cc1/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "fetch_stage_power_cc1", "total power usage of fetch stage_cc1", "icache_power_cc1 + bpred_power_cc1", NULL);

  Sim->STATS->stat_reg_formula(sdb, "dispatch_stage_power_cc1", "total power usage of dispatch stage_cc1", "rename_power_cc1", NULL);

  Sim->STATS->stat_reg_formula(sdb, "issue_stage_power_cc1", "total power usage of issue stage_cc1", "resultbus_power_cc1 + alu_power_cc1 + dcache_power_cc1 + dcache2_power_cc1 + lsq_power_cc1 + window_power_cc1", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_fetch_power_cc1", "average power of fetch unit per cycle_cc1", "(icache_power_cc1 + bpred_power_cc1)/ sim_cycle", /* format */NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_dispatch_power_cc1", "average power of dispatch unit per cycle_cc1", "(rename_power_cc1)/ sim_cycle", /* format */NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_issue_power_cc1", "average power of issue unit per cycle_cc1", "(resultbus_power_cc1 + alu_power_cc1 + dcache_power_cc1 + dcache2_power_cc1 + lsq_power_cc1 + window_power_cc1)/ sim_cycle", /* format */NULL);

  Sim->STATS->stat_reg_formula(sdb, "total_power_cycle_cc1", "total power per cycle_cc1","(rename_power_cc1 + bpred_power_cc1 + lsq_power_cc1 + window_power_cc1 + regfile_power_cc1 + icache_power_cc1 + resultbus_power_cc1 + clock_power_cc1 + alu_power_cc1 + dcache_power_cc1 + dcache2_power_cc1)", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_total_power_cycle_cc1", "average total power per cycle_cc1","(rename_power_cc1 + bpred_power_cc1 + lsq_power_cc1 + window_power_cc1 + regfile_power_cc1 + icache_power_cc1 + resultbus_power_cc1 + clock_power_cc1 + alu_power_cc1 + dcache_power_cc1 +dcache2_power_cc1)/sim_cycle", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_total_power_insn_cc1", "average total power per insn_cc1","(rename_power_cc1 + bpred_power_cc1 + lsq_power_cc1 + window_power_cc1 + regfile_power_cc1 + icache_power_cc1 + resultbus_power_cc1 + clock_power_cc1 +  alu_power_cc1 + dcache_power_cc1 + dcache2_power_cc1)/sim_total_insn", NULL);

  Sim->STATS->stat_reg_double(sdb, "rename_power_cc2", "total power usage of rename unit_cc2", &rename_power_cc2, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "bpred_power_cc2", "total power usage of bpred unit_cc2", &bpred_power_cc2, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "window_power_cc2", "total power usage of instruction window_cc2", &window_power_cc2, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "lsq_power_cc2", "total power usage of lsq_cc2", &lsq_power_cc2, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "regfile_power_cc2", "total power usage of arch. regfile_cc2", &regfile_power_cc2, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "icache_power_cc2", "total power usage of icache_cc2", &icache_power_cc2, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "dcache_power_cc2", "total power usage of dcache_cc2", &dcache_power_cc2, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "dcache2_power_cc2", "total power usage of dcache2_cc2", &dcache2_power_cc2, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "alu_power_cc2", "total power usage of alu_cc2", &alu_power_cc2, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "resultbus_power_cc2", "total power usage of resultbus_cc2", &resultbus_power_cc2, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "clock_power_cc2", "total power usage of clock_cc2", &clock_power_cc2, 0, NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_rename_power_cc2", "avg power usage of rename unit_cc2", "rename_power_cc2/sim_cycle", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_bpred_power_cc2", "avg power usage of bpred unit_cc2", "bpred_power_cc2/sim_cycle", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_window_power_cc2", "avg power usage of instruction window_cc2", "window_power_cc2/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_lsq_power_cc2", "avg power usage of instruction lsq_cc2", "lsq_power_cc2/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_regfile_power_cc2", "avg power usage of arch. regfile_cc2", "regfile_power_cc2/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_icache_power_cc2", "avg power usage of icache_cc2", "icache_power_cc2/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_dcache_power_cc2", "avg power usage of dcache_cc2", "dcache_power_cc2/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_dcache2_power_cc2", "avg power usage of dcache2_cc2", "dcache2_power_cc2/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_alu_power_cc2", "avg power usage of alu_cc2", "alu_power_cc2/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_resultbus_power_cc2", "avg power usage of resultbus_cc2", "resultbus_power_cc2/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_clock_power_cc2", "avg power usage of clock_cc2", "clock_power_cc2/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "fetch_stage_power_cc2", "total power usage of fetch stage_cc2", "icache_power_cc2 + bpred_power_cc2", NULL);

  Sim->STATS->stat_reg_formula(sdb, "dispatch_stage_power_cc2", "total power usage of dispatch stage_cc2", "rename_power_cc2", NULL);

  Sim->STATS->stat_reg_formula(sdb, "issue_stage_power_cc2", "total power usage of issue stage_cc2", "resultbus_power_cc2 + alu_power_cc2 + dcache_power_cc2 + dcache2_power_cc2 + lsq_power_cc2 + window_power_cc2", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_fetch_power_cc2", "average power of fetch unit per cycle_cc2", "(icache_power_cc2 + bpred_power_cc2)/ sim_cycle", /* format */NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_dispatch_power_cc2", "average power of dispatch unit per cycle_cc2", "(rename_power_cc2)/ sim_cycle", /* format */NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_issue_power_cc2", "average power of issue unit per cycle_cc2", "(resultbus_power_cc2 + alu_power_cc2 + dcache_power_cc2 + dcache2_power_cc2 + lsq_power_cc2 + window_power_cc2)/ sim_cycle", /* format */NULL);

  Sim->STATS->stat_reg_formula(sdb, "total_power_cycle_cc2", "total power per cycle_cc2","(rename_power_cc2 + bpred_power_cc2 + lsq_power_cc2 + window_power_cc2 + regfile_power_cc2 + icache_power_cc2 + resultbus_power_cc2 + clock_power_cc2 + alu_power_cc2 + dcache_power_cc2 + dcache2_power_cc2)", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_total_power_cycle_cc2", "average total power per cycle_cc2","(rename_power_cc2 + bpred_power_cc2 + lsq_power_cc2 + window_power_cc2 + regfile_power_cc2 + icache_power_cc2 + resultbus_power_cc2 + clock_power_cc2 + alu_power_cc2 + dcache_power_cc2 + dcache2_power_cc2)/sim_cycle", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_total_power_insn_cc2", "average total power per insn_cc2","(rename_power_cc2 + bpred_power_cc2 + lsq_power_cc2 + window_power_cc2 + regfile_power_cc2 + icache_power_cc2 + resultbus_power_cc2 + clock_power_cc2 + alu_power_cc2 + dcache_power_cc2 + dcache2_power_cc2)/sim_total_insn", NULL);

  Sim->STATS->stat_reg_double(sdb, "rename_power_cc3", "total power usage of rename unit_cc3", &rename_power_cc3, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "bpred_power_cc3", "total power usage of bpred unit_cc3", &bpred_power_cc3, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "window_power_cc3", "total power usage of instruction window_cc3", &window_power_cc3, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "lsq_power_cc3", "total power usage of lsq_cc3", &lsq_power_cc3, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "regfile_power_cc3", "total power usage of arch. regfile_cc3", &regfile_power_cc3, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "icache_power_cc3", "total power usage of icache_cc3", &icache_power_cc3, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "dcache_power_cc3", "total power usage of dcache_cc3", &dcache_power_cc3, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "dcache2_power_cc3", "total power usage of dcache2_cc3", &dcache2_power_cc3, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "alu_power_cc3", "total power usage of alu_cc3", &alu_power_cc3, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "resultbus_power_cc3", "total power usage of resultbus_cc3", &resultbus_power_cc3, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "clock_power_cc3", "total power usage of clock_cc3", &clock_power_cc3, 0, NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_rename_power_cc3", "avg power usage of rename unit_cc3", "rename_power_cc3/sim_cycle", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_bpred_power_cc3", "avg power usage of bpred unit_cc3", "bpred_power_cc3/sim_cycle", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_window_power_cc3", "avg power usage of instruction window_cc3", "window_power_cc3/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_lsq_power_cc3", "avg power usage of instruction lsq_cc3", "lsq_power_cc3/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_regfile_power_cc3", "avg power usage of arch. regfile_cc3", "regfile_power_cc3/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_icache_power_cc3", "avg power usage of icache_cc3", "icache_power_cc3/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_dcache_power_cc3", "avg power usage of dcache_cc3", "dcache_power_cc3/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_dcache2_power_cc3", "avg power usage of dcache2_cc3", "dcache2_power_cc3/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_alu_power_cc3", "avg power usage of alu_cc3", "alu_power_cc3/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_resultbus_power_cc3", "avg power usage of resultbus_cc3", "resultbus_power_cc3/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_clock_power_cc3", "avg power usage of clock_cc3", "clock_power_cc3/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "fetch_stage_power_cc3", "total power usage of fetch stage_cc3", "icache_power_cc3 + bpred_power_cc3", NULL);

  Sim->STATS->stat_reg_formula(sdb, "dispatch_stage_power_cc3", "total power usage of dispatch stage_cc3", "rename_power_cc3", NULL);

  Sim->STATS->stat_reg_formula(sdb, "issue_stage_power_cc3", "total power usage of issue stage_cc3", "resultbus_power_cc3 + alu_power_cc3 + dcache_power_cc3 + dcache2_power_cc3 + lsq_power_cc3 + window_power_cc3", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_fetch_power_cc3", "average power of fetch unit per cycle_cc3", "(icache_power_cc3 + bpred_power_cc3)/ sim_cycle", /* format */NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_dispatch_power_cc3", "average power of dispatch unit per cycle_cc3", "(rename_power_cc3)/ sim_cycle", /* format */NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_issue_power_cc3", "average power of issue unit per cycle_cc3", "(resultbus_power_cc3 + alu_power_cc3 + dcache_power_cc3 + dcache2_power_cc3 + lsq_power_cc3 + window_power_cc3)/ sim_cycle", /* format */NULL);

  Sim->STATS->stat_reg_formula(sdb, "total_power_cycle_cc3", "total power per cycle_cc3","(rename_power_cc3 + bpred_power_cc3 + lsq_power_cc3 + window_power_cc3 + regfile_power_cc3 + icache_power_cc3 + resultbus_power_cc3 + clock_power_cc3 + alu_power_cc3 + dcache_power_cc3 + dcache2_power_cc3)", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_total_power_cycle_cc3", "average total power per cycle_cc3","(rename_power_cc3 + bpred_power_cc3 + lsq_power_cc3 + window_power_cc3 + regfile_power_cc3 + icache_power_cc3 + resultbus_power_cc3 + clock_power_cc3 + alu_power_cc3 + dcache_power_cc3 + dcache2_power_cc3)/sim_cycle", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_total_power_insn_cc3", "average total power per insn_cc3","(rename_power_cc3 + bpred_power_cc3 + lsq_power_cc3 + window_power_cc3 + regfile_power_cc3 + icache_power_cc3 + resultbus_power_cc3 + clock_power_cc3 + alu_power_cc3 + dcache_power_cc3 + dcache2_power_cc3)/sim_total_insn", NULL);

  Sim->stat_reg_counter(sdb, "total_rename_access", "total number accesses of rename unit", &total_rename_access, 0, NULL);

  Sim->stat_reg_counter(sdb, "total_bpred_access", "total number accesses of bpred unit", &total_bpred_access, 0, NULL);

  Sim->stat_reg_counter(sdb, "total_window_access", "total number accesses of instruction window", &total_window_access, 0, NULL);

  Sim->stat_reg_counter(sdb, "total_lsq_access", "total number accesses of load/store queue", &total_lsq_access, 0, NULL);

  Sim->stat_reg_counter(sdb, "total_regfile_access", "total number accesses of arch. regfile", &total_regfile_access, 0, NULL);

  Sim->stat_reg_counter(sdb, "total_icache_access", "total number accesses of icache", &total_icache_access, 0, NULL);

  Sim->stat_reg_counter(sdb, "total_dcache_access", "total number accesses of dcache", &total_dcache_access, 0, NULL);

  Sim->stat_reg_counter(sdb, "total_dcache2_access", "total number accesses of dcache2", &total_dcache2_access, 0, NULL);

  Sim->stat_reg_counter(sdb, "total_alu_access", "total number accesses of alu", &total_alu_access, 0, NULL);

  Sim->stat_reg_counter(sdb, "total_resultbus_access", "total number accesses of resultbus", &total_resultbus_access, 0, NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_rename_access", "avg number accesses of rename unit", "total_rename_access/sim_cycle", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_bpred_access", "avg number accesses of bpred unit", "total_bpred_access/sim_cycle", NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_window_access", "avg number accesses of instruction window", "total_window_access/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_lsq_access", "avg number accesses of lsq", "total_lsq_access/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_regfile_access", "avg number accesses of arch. regfile", "total_regfile_access/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_icache_access", "avg number accesses of icache", "total_icache_access/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_dcache_access", "avg number accesses of dcache", "total_dcache_access/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_dcache2_access", "avg number accesses of dcache2", "total_dcache2_access/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_alu_access", "avg number accesses of alu", "total_alu_access/sim_cycle",  NULL);

  Sim->STATS->stat_reg_formula(sdb, "avg_resultbus_access", "avg number accesses of resultbus", "total_resultbus_access/sim_cycle",  NULL);

  Sim->stat_reg_counter(sdb, "max_rename_access", "max number accesses of rename unit", &max_rename_access, 0, NULL);

  Sim->stat_reg_counter(sdb, "max_bpred_access", "max number accesses of bpred unit", &max_bpred_access, 0, NULL);

  Sim->stat_reg_counter(sdb, "max_window_access", "max number accesses of instruction window", &max_window_access, 0, NULL);

  Sim->stat_reg_counter(sdb, "max_lsq_access", "max number accesses of load/store queue", &max_lsq_access, 0, NULL);

  Sim->stat_reg_counter(sdb, "max_regfile_access", "max number accesses of arch. regfile", &max_regfile_access, 0, NULL);

  Sim->stat_reg_counter(sdb, "max_icache_access", "max number accesses of icache", &max_icache_access, 0, NULL);

  Sim->stat_reg_counter(sdb, "max_dcache_access", "max number accesses of dcache", &max_dcache_access, 0, NULL);

  Sim->stat_reg_counter(sdb, "max_dcache2_access", "max number accesses of dcache2", &max_dcache2_access, 0, NULL);

  Sim->stat_reg_counter(sdb, "max_alu_access", "max number accesses of alu", &max_alu_access, 0, NULL);

  Sim->stat_reg_counter(sdb, "max_resultbus_access", "max number accesses of resultbus", &max_resultbus_access, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "max_cycle_power_cc1", "maximum cycle power usage of cc1", &max_cycle_power_cc1, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "max_cycle_power_cc2", "maximum cycle power usage of cc2", &max_cycle_power_cc2, 0, NULL);

  Sim->STATS->stat_reg_double(sdb, "max_cycle_power_cc3", "maximum cycle power usage of cc3", &max_cycle_power_cc3, 0, NULL);

}


/* this routine takes the number of rows and cols of an array structure
   and attemps to make it make it more of a reasonable circuit structure
   by trying to make the number of rows and cols as close as possible.
   (scaling both by factors of 2 in opposite directions).  it returns
   a scale factor which is the amount that the rows should be divided
   by and the columns should be multiplied by.
*/
int power::squarify(int rows, int cols)
{
  int scale_factor = 1;

  if(rows == cols)
    return 1;

  /*
  printf("init rows == %d\n",rows);
  printf("init cols == %d\n",cols);
  */

  while(rows > cols) {
    rows = rows/2;
    cols = cols*2;

    /*
    printf("rows == %d\n",rows);
    printf("cols == %d\n",cols);
    printf("scale_factor == %d (2^ == %d)\n\n",scale_factor,(int)pow(2.0,(double)scale_factor));
    */

    if (rows/2 <= cols)
      return((int)pow(2.0,(double)scale_factor));
    scale_factor++;
  }

  return 1;
}

/* could improve squarify to work when rows < cols */

double power::squarify_new(int rows, int cols)
{
  double scale_factor = 0.0;

  if(rows==cols)
    return(pow(2.0,scale_factor));

  while(rows > cols) {
    rows = rows/2;
    cols = cols*2;
    if (rows <= cols)
      return(pow(2.0,scale_factor));
    scale_factor++;
  }

  while(cols > rows) {
    rows = rows*2;
    cols = cols/2;
    if (cols <= rows)
      return(pow(2.0,scale_factor));
    scale_factor--;
  }

  return 1;

}


void power::dump_power_stats()
{
  double total_power;
  double bpred_power;
  double rename_power;
  double rat_power;
  double dcl_power;
  double lsq_power;
  double window_power;
  double wakeup_power;
  double rs_power;
  double lsq_wakeup_power;
  double lsq_rs_power;
  double regfile_power;
  double reorder_power;
  double icache_power;
  double dcache_power;
  double dcache2_power;
  double dtlb_power;
  double itlb_power;
  double ambient_power = 2.0;

  icache_power = power0->icache_power;

  dcache_power = power0->dcache_power;

  dcache2_power = power0->dcache2_power;

  itlb_power = power0->itlb;
  dtlb_power = power0->dtlb;

  bpred_power = power0->btb + power0->local_predict + power0->global_predict +
    power0->chooser + power0->ras;

  rat_power = power0->rat_decoder +
    power0->rat_wordline + power0->rat_bitline + power0->rat_senseamp;

  dcl_power = power0->dcl_compare + power0->dcl_pencode;

  rename_power = power0->rat_power + power0->dcl_power + power0->inst_decoder_power;

  wakeup_power = power0->wakeup_tagdrive + power0->wakeup_tagmatch +
    power0->wakeup_ormatch;

  rs_power = power0->rs_decoder +
    power0->rs_wordline + power0->rs_bitline + power0->rs_senseamp;

  window_power = wakeup_power + rs_power + power0->selection;

  lsq_rs_power = power0->lsq_rs_decoder +
    power0->lsq_rs_wordline + power0->lsq_rs_bitline + power0->lsq_rs_senseamp;

  lsq_wakeup_power = power0->lsq_wakeup_tagdrive +
    power0->lsq_wakeup_tagmatch + power0->lsq_wakeup_ormatch;

  lsq_power = lsq_wakeup_power + lsq_rs_power;

  reorder_power = power0->reorder_decoder +
    power0->reorder_wordline + power0->reorder_bitline +
    power0->reorder_senseamp;

  regfile_power = power0->regfile_decoder +
    power0->regfile_wordline + power0->regfile_bitline +
    power0->regfile_senseamp;

  total_power = bpred_power + rename_power + window_power + regfile_power +
    power0->resultbus + lsq_power +
    icache_power + dcache_power + dcache2_power +
    dtlb_power + itlb_power + power0->clock_power + power0->ialu_power +
    power0->falu_power;

  fprintf(stderr,"\nProcessor Parameters:\n");
  fprintf(stderr,"Issue Width: %d\n",Sim->ruu_issue_width);
  fprintf(stderr,"Window Size: %d\n",Sim->RUU_size);
  fprintf(stderr,"Number of Virtual Registers: %d\n",MD_NUM_IREGS);
  fprintf(stderr,"Number of Physical Registers: %d\n",Sim->RUU_size);
  fprintf(stderr,"Datapath Width: %d\n",Sim->data_width);

  fprintf(stderr,"Total Power Consumption: %g\n",total_power+ambient_power);
  fprintf(stderr,"Branch Predictor Power Consumption: %g  (%.3g%%)\n",bpred_power,100*bpred_power/total_power);
  fprintf(stderr," branch target buffer power (W): %g\n",power0->btb);
  fprintf(stderr," local predict power (W): %g\n",power0->local_predict);
  fprintf(stderr," global predict power (W): %g\n",power0->global_predict);
  fprintf(stderr," chooser power (W): %g\n",power0->chooser);
  fprintf(stderr," RAS power (W): %g\n",power0->ras);
  fprintf(stderr,"Rename Logic Power Consumption: %g  (%.3g%%)\n",rename_power,100*rename_power/total_power);
  fprintf(stderr," Instruction Decode Power (W): %g\n",power0->inst_decoder_power);
  fprintf(stderr," RAT decode_power (W): %g\n",power0->rat_decoder);
  fprintf(stderr," RAT wordline_power (W): %g\n",power0->rat_wordline);
  fprintf(stderr," RAT bitline_power (W): %g\n",power0->rat_bitline);
  fprintf(stderr," DCL Comparators (W): %g\n",power0->dcl_compare);
  fprintf(stderr,"Instruction Window Power Consumption: %g  (%.3g%%)\n",window_power,100*window_power/total_power);
  fprintf(stderr," tagdrive (W): %g\n",power0->wakeup_tagdrive);
  fprintf(stderr," tagmatch (W): %g\n",power0->wakeup_tagmatch);
  fprintf(stderr," Selection Logic (W): %g\n",power0->selection);
  fprintf(stderr," decode_power (W): %g\n",power0->rs_decoder);
  fprintf(stderr," wordline_power (W): %g\n",power0->rs_wordline);
  fprintf(stderr," bitline_power (W): %g\n",power0->rs_bitline);
  fprintf(stderr,"Load/Store Queue Power Consumption: %g  (%.3g%%)\n",lsq_power,100*lsq_power/total_power);
  fprintf(stderr," tagdrive (W): %g\n",power0->lsq_wakeup_tagdrive);
  fprintf(stderr," tagmatch (W): %g\n",power0->lsq_wakeup_tagmatch);
  fprintf(stderr," decode_power (W): %g\n",power0->lsq_rs_decoder);
  fprintf(stderr," wordline_power (W): %g\n",power0->lsq_rs_wordline);
  fprintf(stderr," bitline_power (W): %g\n",power0->lsq_rs_bitline);
  fprintf(stderr,"Arch. Register File Power Consumption: %g  (%.3g%%)\n",regfile_power,100*regfile_power/total_power);
  fprintf(stderr," decode_power (W): %g\n",power0->regfile_decoder);
  fprintf(stderr," wordline_power (W): %g\n",power0->regfile_wordline);
  fprintf(stderr," bitline_power (W): %g\n",power0->regfile_bitline);
  fprintf(stderr,"Result Bus Power Consumption: %g  (%.3g%%)\n",power0->resultbus,100*power0->resultbus/total_power);
  fprintf(stderr,"Total Clock Power: %g  (%.3g%%)\n",power0->clock_power,100*power0->clock_power/total_power);
  fprintf(stderr,"Int ALU Power: %g  (%.3g%%)\n",power0->ialu_power,100*power0->ialu_power/total_power);
  fprintf(stderr,"FP ALU Power: %g  (%.3g%%)\n",power0->falu_power,100*power0->falu_power/total_power);
  fprintf(stderr,"Instruction Cache Power Consumption: %g  (%.3g%%)\n",icache_power,100*icache_power/total_power);
  fprintf(stderr," decode_power (W): %g\n",power0->icache_decoder);
  fprintf(stderr," wordline_power (W): %g\n",power0->icache_wordline);
  fprintf(stderr," bitline_power (W): %g\n",power0->icache_bitline);
  fprintf(stderr," senseamp_power (W): %g\n",power0->icache_senseamp);
  fprintf(stderr," tagarray_power (W): %g\n",power0->icache_tagarray);
  fprintf(stderr,"Itlb_power (W): %g (%.3g%%)\n",power0->itlb,100*power0->itlb/total_power);
  fprintf(stderr,"Data Cache Power Consumption: %g  (%.3g%%)\n",dcache_power,100*dcache_power/total_power);
  fprintf(stderr," decode_power (W): %g\n",power0->dcache_decoder);
  fprintf(stderr," wordline_power (W): %g\n",power0->dcache_wordline);
  fprintf(stderr," bitline_power (W): %g\n",power0->dcache_bitline);
  fprintf(stderr," senseamp_power (W): %g\n",power0->dcache_senseamp);
  fprintf(stderr," tagarray_power (W): %g\n",power0->dcache_tagarray);
  fprintf(stderr,"Dtlb_power (W): %g (%.3g%%)\n",power0->dtlb,100*power0->dtlb/total_power);
  fprintf(stderr,"Level 2 Cache Power Consumption: %g (%.3g%%)\n",dcache2_power,100*dcache2_power/total_power);
  fprintf(stderr," decode_power (W): %g\n",power0->dcache2_decoder);
  fprintf(stderr," wordline_power (W): %g\n",power0->dcache2_wordline);
  fprintf(stderr," bitline_power (W): %g\n",power0->dcache2_bitline);
  fprintf(stderr," senseamp_power (W): %g\n",power0->dcache2_senseamp);
  fprintf(stderr," tagarray_power (W): %g\n",power0->dcache2_tagarray);
}

/*======================================================================*/



/*
 * This part of the code contains routines for each section as
 * described in the tech report.  See the tech report for more details
 * and explanations */

/*----------------------------------------------------------------------*/

double power::driver_size(double driving_cap, double desiredrisetime) {
  double nsize, psize;
  double Rpdrive;

  Rpdrive = desiredrisetime/(driving_cap*log(VSINV)*-1.0);
  psize = restowidth(Rpdrive,PCH);
  nsize = restowidth(Rpdrive,NCH);
  if (psize > Wworddrivemax) {
    psize = Wworddrivemax;
  }
  if (psize < 4.0 * LSCALE)
    psize = 4.0 * LSCALE;

  return (psize);

}


/* Decoder delay:  (see section 6.1 of tech report) */

double power::array_decoder_power(int rows,int cols,double predeclength,int rports, int wports,int cache)
{
  double Ctotal=0;
  double Ceq=0;
  int numstack;
  int decode_bits=0;
  int ports;
  double rowsb;

  /* read and write ports are the same here */
  ports = rports + wports;

  rowsb = (double)rows;

  /* number of input bits to be decoded */
  decode_bits= (int) ceil((logtwo(rowsb)));

  /* First stage: driving the decoders */

  /* This is the capacitance for driving one bit (and its complement).
     -There are #rowsb 3->8 decoders contributing gatecap.
     - 2.0 factor from 2 identical sets of drivers in parallel
  */

  Ceq = 2.0*(draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1)) +

  gatecap(Wdec3to8n+Wdec3to8p,10.0)*rowsb;

  /* There are ports * #decode_bits total */
  Ctotal+=ports*decode_bits*Ceq;

  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"Decoder -- Driving decoders            == %g\n",.3*Ctotal*Powerfactor);

  /* second stage: driving a bunch of nor gates with a nand
     numstack is the size of the nor gates -- ie. a 7-128 decoder has
     3-input NAND followed by 3-input NOR  */

  numstack = (int) ceil((1.0/3.0)*logtwo(rows));

  if (numstack<=0) numstack = 1;
  if (numstack>5) numstack = 5;

  /* There are #rowsb NOR gates being driven*/
  Ceq = (3.0*draincap(Wdec3to8p,PCH,1) +draincap(Wdec3to8n,NCH,3) +
	 gatecap(WdecNORn+WdecNORp,((numstack*40)+20.0)))*rowsb;

  Ctotal+=ports*Ceq;

  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"Decoder -- Driving nor w/ nand         == %g\n",.3*ports*Ceq*Powerfactor);

  /* Final stage: driving an inverter with the nor
     (inverter preceding wordline driver) -- wordline driver is in the next section*/

  Ceq = (gatecap(Wdecinvn+Wdecinvp,20.0)+
	 numstack*draincap(WdecNORn,NCH,1)+
         draincap(WdecNORp,PCH,numstack));

  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"Decoder -- Driving inverter w/ nor     == %g\n",.3*ports*Ceq*Powerfactor);

  Ctotal+=ports*Ceq;

  /* assume Activity Factor == .3  */
  return(.3*Ctotal*Powerfactor);
}

double power::simple_array_decoder_power(int rows, int cols, int rports, int wports, int cache)
{
  double predeclength=0.0;
  return(array_decoder_power(rows,cols,predeclength,rports,wports,cache));
}


double power::array_wordline_power(int rows,int cols,double wordlinelength,int rports, int wports, int cache)
{
  double Ctotal=0;
  double Ceq=0;
  double Cline=0;
  double Cliner, Clinew=0;
  double desiredrisetime,psize,nsize;
  int ports;
  double colsb;

  ports = rports+wports;

  colsb = (double)cols;

  /* Calculate size of wordline drivers assuming rise time == Period / 8
     - estimate cap on line
     - compute min resistance to achieve this with RC
     - compute width needed to achieve this resistance */

  desiredrisetime = Period/16;
  Cline = (gatecappass(Wmemcellr,1.0))*colsb + wordlinelength*CM3metal;
  psize = driver_size(Cline,desiredrisetime);

  /* how do we want to do p-n ratioing? -- here we just assume the same ratio
     from an inverter pair  */
  nsize = psize * Wdecinvn/Wdecinvp;

  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"Wordline Driver Sizes -- nsize == %f, psize == %f\n",nsize,psize);

  Ceq = draincap(Wdecinvn,NCH,1) + draincap(Wdecinvp,PCH,1) +
    gatecap(nsize+psize,20.0);

  Ctotal+=ports*Ceq;

  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"Wordline -- Inverter -> Driver         == %g\n",ports*Ceq*Powerfactor);

  /* Compute caps of read wordline and write wordlines
     - wordline driver caps, given computed width from above
     - read wordlines have 1 nmos access tx, size ~4
     - write wordlines have 2 nmos access tx, size ~2
     - metal line cap
  */

  Cliner = (gatecappass(Wmemcellr,(BitWidth-2*Wmemcellr)/2.0))*colsb+
    wordlinelength*CM3metal+
    2.0*(draincap(nsize,NCH,1) + draincap(psize,PCH,1));
  Clinew = (2.0*gatecappass(Wmemcellw,(BitWidth-2*Wmemcellw)/2.0))*colsb+
    wordlinelength*CM3metal+
    2.0*(draincap(nsize,NCH,1) + draincap(psize,PCH,1));

  if(Sim->GLOBAL->verbose) {
    fprintf(stderr,"Wordline -- Line                       == %g\n",1e12*Cline);
    fprintf(stderr,"Wordline -- Line -- access -- gatecap  == %g\n",1e12*colsb*2*gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0));
    fprintf(stderr,"Wordline -- Line -- driver -- draincap == %g\n",1e12*draincap(nsize,NCH,1) + draincap(psize,PCH,1));
    fprintf(stderr,"Wordline -- Line -- metal              == %g\n",1e12*wordlinelength*CM3metal);
  }
  Ctotal+=rports*Cliner+wports*Clinew;

  /* AF == 1 assuming a different wordline is charged each cycle, but only
     1 wordline (per port) is actually used */

  return(Ctotal*Powerfactor);
}

double power::simple_array_wordline_power(int rows,int cols,int rports,int wports,int cache)
{
  double wordlinelength;
  int ports = rports + wports;
  wordlinelength = cols *  (RegCellWidth + 2 * ports * BitlineSpacing);
  return(array_wordline_power(rows,cols,wordlinelength,rports,wports,cache));
}


double power::array_bitline_power(int rows,int cols,double bitlinelength, int rports,int wports, int cache)
{
  double Ctotal=0;
  double Ccolmux=0;
  double Cbitrowr=0;
  double Cbitroww=0;
  double Cprerow=0;
  double Cwritebitdrive=0;
  double Cpregate=0;
  double Cliner=0;
  double Clinew=0;
  int ports;
  double rowsb;
  double colsb;

  double desiredrisetime, Cline, psize, nsize;

  ports = rports + wports;

  rowsb = (double)rows;
  colsb = (double)cols;

  /* Draincaps of access tx's */

  Cbitrowr = draincap(Wmemcellr,NCH,1);
  Cbitroww = draincap(Wmemcellw,NCH,1);

  /* Cprerow -- precharge cap on the bitline
     -simple scheme to estimate size of pre-charge tx's in a similar fashion
      to wordline driver size estimation.
     -FIXME: it would be better to use precharge/keeper pairs, i've omitted this
      from this version because it couldn't autosize as easily.
  */

  desiredrisetime = Period/8;

  Cline = rowsb*Cbitrowr+CM2metal*bitlinelength;
  psize = driver_size(Cline,desiredrisetime);

  /* compensate for not having an nmos pre-charging */
  psize = psize + psize * Wdecinvn/Wdecinvp;

  if(Sim->GLOBAL->verbose)
    printf("Cprerow auto   == %g (psize == %g)\n",draincap(psize,PCH,1),psize);

  Cprerow = draincap(psize,PCH,1);

  /* Cpregate -- cap due to gatecap of precharge transistors -- tack this
     onto bitline cap, again this could have a keeper */
  Cpregate = 4.0*gatecap(psize,10.0);
  global_clockcap+=rports*cols*2.0*Cpregate;

  /* Cwritebitdrive -- write bitline drivers are used instead of the precharge
     stuff for write bitlines
     - 2 inverter drivers within each driver pair */

  Cline = rowsb*Cbitroww+CM2metal*bitlinelength;

  psize = driver_size(Cline,desiredrisetime);
  nsize = psize * Wdecinvn/Wdecinvp;

  Cwritebitdrive = 2.0*(draincap(psize,PCH,1)+draincap(nsize,NCH,1));

  /*
     reg files (cache==0)
     => single ended bitlines (1 bitline/col)
     => AFs from pop_count
     caches (cache ==1)
     => double-ended bitlines (2 bitlines/col)
     => AFs = .5 (since one of the two bitlines is always charging/discharging)
  */

#ifdef STATIC_AF
  if (cache == 0) {
    /* compute the total line cap for read/write bitlines */
    Cliner = rowsb*Cbitrowr+CM2metal*bitlinelength+Cprerow;
    Clinew = rowsb*Cbitroww+CM2metal*bitlinelength+Cwritebitdrive;

    /* Bitline inverters at the end of the bitlines (replaced w/ sense amps
       in cache styles) */
    Ccolmux = gatecap(MSCALE*(29.9+7.8),0.0)+gatecap(MSCALE*(47.0+12.0),0.0);
    Ctotal+=(1.0-POPCOUNT_AF)*rports*cols*(Cliner+Ccolmux+2.0*Cpregate);
    Ctotal+=.3*wports*cols*(Clinew+Cwritebitdrive);
  }
  else {
    Cliner = rowsb*Cbitrowr+CM2metal*bitlinelength+Cprerow + draincap(Wbitmuxn,NCH,1);
    Clinew = rowsb*Cbitroww+CM2metal*bitlinelength+Cwritebitdrive;
    Ccolmux = (draincap(Wbitmuxn,NCH,1))+2.0*gatecap(WsenseQ1to4,10.0);
    Ctotal+=.5*rports*2.0*cols*(Cliner+Ccolmux+2.0*Cpregate);
    Ctotal+=.5*wports*2.0*cols*(Clinew+Cwritebitdrive);
  }
#else
  if (cache == 0) {
    /* compute the total line cap for read/write bitlines */
    Cliner = rowsb*Cbitrowr+CM2metal*bitlinelength+Cprerow;
    Clinew = rowsb*Cbitroww+CM2metal*bitlinelength+Cwritebitdrive;

    /* Bitline inverters at the end of the bitlines (replaced w/ sense amps
       in cache styles) */
    Ccolmux = gatecap(MSCALE*(29.9+7.8),0.0)+gatecap(MSCALE*(47.0+12.0),0.0);
    Ctotal += rports*cols*(Cliner+Ccolmux+2.0*Cpregate);
    Ctotal += .3*wports*cols*(Clinew+Cwritebitdrive);
  }
  else {
    Cliner = rowsb*Cbitrowr+CM2metal*bitlinelength+Cprerow + draincap(Wbitmuxn,NCH,1);
    Clinew = rowsb*Cbitroww+CM2metal*bitlinelength+Cwritebitdrive;
    Ccolmux = (draincap(Wbitmuxn,NCH,1))+2.0*gatecap(WsenseQ1to4,10.0);
    Ctotal+=.5*rports*2.0*cols*(Cliner+Ccolmux+2.0*Cpregate);
    Ctotal+=.5*wports*2.0*cols*(Clinew+Cwritebitdrive);
  }
#endif

  if(Sim->GLOBAL->verbose) {
    fprintf(stderr,"Bitline -- Precharge                   == %g\n",1e12*Cpregate);
    fprintf(stderr,"Bitline -- Line                        == %g\n",1e12*(Cliner+Clinew));
    fprintf(stderr,"Bitline -- Line -- access draincap     == %g\n",1e12*rowsb*Cbitrowr);
    fprintf(stderr,"Bitline -- Line -- precharge draincap  == %g\n",1e12*Cprerow);
    fprintf(stderr,"Bitline -- Line -- metal               == %g\n",1e12*bitlinelength*CM2metal);
    fprintf(stderr,"Bitline -- Colmux                      == %g\n",1e12*Ccolmux);

    fprintf(stderr,"\n");
  }


  if(cache==0)
    return(Ctotal*Powerfactor);
  else
    return(Ctotal*SensePowerfactor*.4);

}


double power::simple_array_bitline_power(int rows,int cols,int rports,int wports,int cache)
{
  double bitlinelength;

  int ports = rports + wports;

  bitlinelength = rows * (RegCellHeight + ports * WordlineSpacing);

  return (array_bitline_power(rows,cols,bitlinelength,rports,wports,cache));

}

/* estimate senseamp power dissipation in cache structures (Zyuban's method) */
double power::senseamp_power(int cols)
{
	return((double)cols * Vdd/8 * .5e-3);
}

/* estimate comparator power consumption (this comparator is similar
   to the tag-match structure in a CAM */
double power::compare_cap(int compare_bits)
{
  double c1, c2;
  /* bottom part of comparator */
  c2 = (compare_bits)*(draincap(Wcompn,NCH,1)+draincap(Wcompn,NCH,2))+
    draincap(Wevalinvp,PCH,1) + draincap(Wevalinvn,NCH,1);

  /* top part of comparator */
  c1 = (compare_bits)*(draincap(Wcompn,NCH,1)+draincap(Wcompn,NCH,2)+
		       draincap(Wcomppreequ,NCH,1)) +
    gatecap(WdecNORn,1.0)+
    gatecap(WdecNORp,3.0);

  return(c1 + c2);
}

/* power of depency check logic */
double power::dcl_compare_power(int compare_bits)
{
  double Ctotal;
  int num_comparators;

  num_comparators = (Sim->ruu_decode_width - 1) * (Sim->ruu_decode_width);

  Ctotal = num_comparators * compare_cap(compare_bits);

  return(Ctotal*Powerfactor*AF);
}

double power::simple_array_power(int rows,int cols,int rports,int wports,int cache)
{
  if(cache==0)
    return( simple_array_decoder_power(rows,cols,rports,wports,cache)+
	    simple_array_wordline_power(rows,cols,rports,wports,cache)+
	    simple_array_bitline_power(rows,cols,rports,wports,cache));
  else
    return( simple_array_decoder_power(rows,cols,rports,wports,cache)+
	    simple_array_wordline_power(rows,cols,rports,wports,cache)+
	    simple_array_bitline_power(rows,cols,rports,wports,cache)+
	    senseamp_power(cols));
}


double power::cam_tagdrive(int rows,int cols,int rports,int wports)
{
  double Ctotal, Ctlcap, Cblcap, Cwlcap;
  double taglinelength;
  double wordlinelength;
  double nsize, psize;
  int ports;
  Ctotal=0;

  ports = rports + wports;

  taglinelength = rows *
    (CamCellHeight + ports * MatchlineSpacing);

  wordlinelength = cols *
    (CamCellWidth + ports * TaglineSpacing);

  /* Compute tagline cap */
  Ctlcap = Cmetal * taglinelength +
    rows * gatecappass(Wcomparen2,2.0) +
    draincap(Wcompdrivern,NCH,1)+draincap(Wcompdriverp,PCH,1);

  /* Compute bitline cap (for writing new tags) */
  Cblcap = Cmetal * taglinelength +
    rows * draincap(Wmemcellr,NCH,2);

  /* autosize wordline driver */
  psize = driver_size(Cmetal * wordlinelength + 2 * cols * gatecap(Wmemcellr,2.0),Period/8);
  nsize = psize * Wdecinvn/Wdecinvp;

  /* Compute wordline cap (for writing new tags) */
  Cwlcap = Cmetal * wordlinelength +
    draincap(nsize,NCH,1)+draincap(psize,PCH,1) +
    2 * cols * gatecap(Wmemcellr,2.0);

  Ctotal += (rports * cols * 2 * Ctlcap) +
    (wports * ((cols * 2 * Cblcap) + (rows * Cwlcap)));

  return(Ctotal*Powerfactor*AF);
}

double power::cam_tagmatch(int rows,int cols,int rports,int wports)
{
  double Ctotal, Cmlcap;
  double matchlinelength;
  int ports;
  Ctotal=0;

  ports = rports + wports;

  matchlinelength = cols *
    (CamCellWidth + ports * TaglineSpacing);

  Cmlcap = 2 * cols * draincap(Wcomparen1,NCH,2) +
    Cmetal * matchlinelength + draincap(Wmatchpchg,NCH,1) +
    gatecap(Wmatchinvn+Wmatchinvp,10.0) +
    gatecap(Wmatchnandn+Wmatchnandp,10.0);

  Ctotal += rports * rows * Cmlcap;

  global_clockcap += rports * rows * gatecap(Wmatchpchg,5.0);

  /* noring the nanded match lines */
  if(Sim->ruu_issue_width >= 8)
    Ctotal += 2 * gatecap(Wmatchnorn+Wmatchnorp,10.0);

  return(Ctotal*Powerfactor*AF);
}

double power::cam_array(int rows,int cols,int rports,int wports)
{
  return(cam_tagdrive(rows,cols,rports,wports) +
	 cam_tagmatch(rows,cols,rports,wports));
}


double power::selection_power(int win_entries)
{
  double Ctotal, Cor, Cpencode;
  int num_arbiter=1;

  Ctotal=0;

  while(win_entries > 4)
    {
      win_entries = (int)ceil((double)win_entries / 4.0);
      num_arbiter += win_entries;
    }

  Cor = 4 * draincap(WSelORn,NCH,1) + draincap(WSelORprequ,PCH,1);

  Cpencode = draincap(WSelPn,NCH,1) + draincap(WSelPp,PCH,1) +
    2*draincap(WSelPn,NCH,1) + draincap(WSelPp,PCH,2) +
    3*draincap(WSelPn,NCH,1) + draincap(WSelPp,PCH,3) +
    4*draincap(WSelPn,NCH,1) + draincap(WSelPp,PCH,4) +
    4*gatecap(WSelEnn+WSelEnp,20.0) +
    4*draincap(WSelEnn,NCH,1) + 4*draincap(WSelEnp,PCH,1);

  Ctotal += Sim->ruu_issue_width * num_arbiter*(Cor+Cpencode);

  return(Ctotal*Powerfactor*AF);
}

/* very rough clock power estimates */
double power::total_clockpower(double die_length)
{

  double clocklinelength;
  double Cline,Cline2,Ctotal;
  double pipereg_clockcap=0;
  double global_buffercap = 0;
  double Clockpower;

  double num_piperegs;

  int npreg_width = (int)ceil(logtwo((double)Sim->RUU_size));

  /* Assume say 8 stages (kinda low now).
     FIXME: this could be a lot better; user could input
     number of pipestages, etc  */

  /* assume 8 pipe stages and try to estimate bits per pipe stage */
  /* pipe stage 0/1 */
  num_piperegs = Sim->ruu_issue_width*inst_length + Sim->data_width;
  /* pipe stage 1/2 */
  num_piperegs += Sim->ruu_issue_width*(inst_length + 3 * Sim->RUU_size);
  /* pipe stage 2/3 */
  num_piperegs += Sim->ruu_issue_width*(inst_length + 3 * Sim->RUU_size);
  /* pipe stage 3/4 */
  num_piperegs += Sim->ruu_issue_width*(3 * npreg_width + pow2(opcode_length));
  /* pipe stage 4/5 */
  num_piperegs += Sim->ruu_issue_width*(2*Sim->data_width + pow2(opcode_length));
  /* pipe stage 5/6 */
  num_piperegs += Sim->ruu_issue_width*(Sim->data_width + pow2(opcode_length));
  /* pipe stage 6/7 */
  num_piperegs += Sim->ruu_issue_width*(Sim->data_width + pow2(opcode_length));
  /* pipe stage 7/8 */
  num_piperegs += Sim->ruu_issue_width*(Sim->data_width + pow2(opcode_length));

  /* assume 50% extra in control signals (rule of thumb) */
  num_piperegs = num_piperegs * 1.5;

  pipereg_clockcap = num_piperegs * 4*gatecap(10.0,0);

  /* estimate based on 3% of die being in clock metal */
  Cline2 = Cmetal * (.03 * die_length * die_length/BitlineSpacing) * 1e6 * 1e6;

  /* another estimate */
  clocklinelength = die_length*(.5 + 4 * (.25 + 2*(.25) + 4 * (.125)));
  Cline = 20 * Cmetal * (clocklinelength) * 1e6;
  global_buffercap = 12*gatecap(1000.0,10.0)+16*gatecap(200,10.0)+16*8*2*gatecap(100.0,10.00) + 2*gatecap(.29*1e6,10.0);
  /* global_clockcap is computed within each array structure for pre-charge tx's*/
  Ctotal = Cline+global_clockcap+pipereg_clockcap+global_buffercap;

  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"num_piperegs == %f\n",num_piperegs);

  /* add I_ADD Clockcap and F_ADD Clockcap */
  Clockpower = Ctotal*Powerfactor + Sim->res_ialu*I_ADD_CLOCK + Sim->res_fpalu*F_ADD_CLOCK;

  if(Sim->GLOBAL->verbose) {
    fprintf(stderr,"Global Clock Power: %g\n",Clockpower);
    fprintf(stderr," Global Metal Lines   (W): %g\n",Cline*Powerfactor);
    fprintf(stderr," Global Metal Lines (3%%) (W): %g\n",Cline2*Powerfactor);
    fprintf(stderr," Global Clock Buffers (W): %g\n",global_buffercap*Powerfactor);
    fprintf(stderr," Global Clock Cap (Explicit) (W): %g\n",global_clockcap*Powerfactor+I_ADD_CLOCK+F_ADD_CLOCK);
    fprintf(stderr," Global Clock Cap (Implicit) (W): %g\n",pipereg_clockcap*Powerfactor);
  }
  return(Clockpower);

}

/* very rough global clock power estimates */
double power::global_clockpower(double die_length)
{

  double clocklinelength;
  double Cline,Cline2,Ctotal;
  double global_buffercap = 0;

  Cline2 = Cmetal * (.03 * die_length * die_length/BitlineSpacing) * 1e6 * 1e6;

  clocklinelength = die_length*(.5 + 4 * (.25 + 2*(.25) + 4 * (.125)));
  Cline = 20 * Cmetal * (clocklinelength) * 1e6;
  global_buffercap = 12*gatecap(1000.0,10.0)+16*gatecap(200,10.0)+16*8*2*gatecap(100.0,10.00) + 2*gatecap(.29*1e6,10.0);
  Ctotal = Cline+global_buffercap;

  if(Sim->GLOBAL->verbose) {
    fprintf(stderr,"Global Clock Power: %g\n",Ctotal*Powerfactor);
    fprintf(stderr," Global Metal Lines   (W): %g\n",Cline*Powerfactor);
    fprintf(stderr," Global Metal Lines (3%%) (W): %g\n",Cline2*Powerfactor);
    fprintf(stderr," Global Clock Buffers (W): %g\n",global_buffercap*Powerfactor);
  }

  return(Ctotal*Powerfactor);

}


double power::compute_resultbus_power()
{
  double Ctotal, Cline;

  double regfile_height;

  /* compute size of result bus tags */
  int npreg_width = (int)ceil(logtwo((double)Sim->RUU_size));

  Ctotal=0;

  regfile_height = Sim->RUU_size * (RegCellHeight +
			       WordlineSpacing * 3 * Sim->ruu_issue_width);

  /* assume num alu's == ialu  (FIXME: generate a more detailed result bus network model*/
  Cline = Cmetal * (regfile_height + .5 * Sim->res_ialu * 3200.0 * LSCALE);

  /* or use result bus length measured from 21264 die photo */
  /*  Cline = Cmetal * 3.3*1000;*/

  /* Assume Sim->ruu_issue_width result busses -- power can be scaled linearly
     for number of result busses (scale by writeback_access) */
  Ctotal += 2.0 * (Sim->data_width + npreg_width) * (Sim->ruu_issue_width)* Cline;

#ifdef STATIC_AF
  return(Ctotal*Powerfactor*AF);
#else
  return(Ctotal*Powerfactor);
#endif

}

void power::calculate_power()
{
  double clockpower;
  double predeclength, wordlinelength, bitlinelength;

  /* CACTI 4.1 */
  	// int ndwl, ndbl, nspd, ntwl, ntbl, ntspd, c, b, a, cache, rowsb, colsb;
  	int ndwl, ndbl, ntwl, ntbl, ntspd, c, b, a, cache, rowsb, colsb;
  	double nspd;
  /* CACTI 4.1 */

  int trowsb, tcolsb, tagsize;
  int va_size = 48;

  int npreg_width = (int)ceil(logtwo((double)Sim->RUU_size));

  /* CACTI 4.1 */
  /* these variables are needed to use Cacti to auto-size cache arrays
     (for optimal delay) */

	  //time_result_type time_result;
	  //time_parameter_type time_parameters;
  total_result_type result_total;
  /* CACTI 4.1 */


  /* used to autosize other structures, like bpred tables */
  int scale_factor;

  global_clockcap = 0;

  cache=0;

  /* MSG: CACTI 4.1*/
  /* A chamada foi inserida antes para atualizar todas as variáveis */

  c = (Sim->btb_config[0] * (Sim->data_width/8)* Sim->btb_config[1]);
  b = (Sim->data_width/8);
  a = Sim->btb_config[1];

  result_total = cacti_interface(  /*C*/ c,
  			 				  	   /*B*/ b,
  			 				  	   /*A*/ a,
  			 				  	   /*RWP*/ 1,
  			 				  	   /*ERP*/ 0,
  			 				  	   /*EWP*/ 0,
  			 				  	   /*NSER*/ 0 ,
  			 				  	   /*Nbanks*/ 1,
  			 				  	   /*TECH*/ TECH_LENGTH,
  			 				  	   /*OUTPUTWIDTH*/ 64,
  			 				  	   /*CUSTOMTAG*/ 0,
  			 				  	   /*TAGWIDTH*/ 0,
  			 				  	   /*ACCESSMODE*/ 0,
  			 				  	   /*PURESRAM*/ 0);

  /* MSG: CACTI 4.1*/

  /* FIXME: ALU power is a simple constant, it would be better
     to include bit AFs and have different numbers for different
     types of operations */
  power0->ialu_power = Sim->res_ialu * I_ADD;
  power0->falu_power = Sim->res_fpalu * F_ADD;

  nvreg_width = (int)ceil(logtwo((double)MD_NUM_IREGS));
  npreg_width = (int)ceil(logtwo((double)Sim->RUU_size));


  /* RAT has shadow bits stored in each cell, this makes the
     cell size larger than normal array structures, so we must
     compute it here */

  predeclength = MD_NUM_IREGS *
    (RatCellHeight + 3 * Sim->ruu_decode_width * WordlineSpacing);

  wordlinelength = npreg_width *
    (RatCellWidth +
     6 * Sim->ruu_decode_width * BitlineSpacing +
     RatShiftRegWidth*RatNumShift);

  bitlinelength = MD_NUM_IREGS * (RatCellHeight + 3 * Sim->ruu_decode_width * WordlineSpacing);

  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"rat power stats\n");
  power0->rat_decoder = array_decoder_power(MD_NUM_IREGS,npreg_width,predeclength,2*Sim->ruu_decode_width,Sim->ruu_decode_width,cache);
  power0->rat_wordline = array_wordline_power(MD_NUM_IREGS,npreg_width,wordlinelength,2*Sim->ruu_decode_width,Sim->ruu_decode_width,cache);
  power0->rat_bitline = array_bitline_power(MD_NUM_IREGS,npreg_width,bitlinelength,2*Sim->ruu_decode_width,Sim->ruu_decode_width,cache);
  power0->rat_senseamp = 0;

  power0->dcl_compare = dcl_compare_power(nvreg_width);
  power0->dcl_pencode = 0;
  power0->inst_decoder_power = Sim->ruu_decode_width * simple_array_decoder_power(opcode_length,1,1,1,cache);
  power0->wakeup_tagdrive =cam_tagdrive(Sim->RUU_size,npreg_width,Sim->ruu_issue_width,Sim->ruu_issue_width);
  power0->wakeup_tagmatch =cam_tagmatch(Sim->RUU_size,npreg_width,Sim->ruu_issue_width,Sim->ruu_issue_width);
  power0->wakeup_ormatch =0;

  power0->selection = selection_power(Sim->RUU_size);


  predeclength = MD_NUM_IREGS * (RegCellHeight + 3 * Sim->ruu_issue_width * WordlineSpacing);

  wordlinelength = Sim->data_width *
    (RegCellWidth +
     6 * Sim->ruu_issue_width * BitlineSpacing);

  bitlinelength = MD_NUM_IREGS * (RegCellHeight + 3 * Sim->ruu_issue_width * WordlineSpacing);

  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"regfile power stats\n");

  power0->regfile_decoder = array_decoder_power(MD_NUM_IREGS,Sim->data_width,predeclength,2*Sim->ruu_issue_width,Sim->ruu_issue_width,cache);
  power0->regfile_wordline = array_wordline_power(MD_NUM_IREGS,Sim->data_width,wordlinelength,2*Sim->ruu_issue_width,Sim->ruu_issue_width,cache);
  power0->regfile_bitline = array_bitline_power(MD_NUM_IREGS,Sim->data_width,bitlinelength,2*Sim->ruu_issue_width,Sim->ruu_issue_width,cache);
  power0->regfile_senseamp =0;

  predeclength = Sim->RUU_size * (RegCellHeight + 3 * Sim->ruu_issue_width * WordlineSpacing);

  wordlinelength = Sim->data_width *
    (RegCellWidth +
     6 * Sim->ruu_issue_width * BitlineSpacing);

  bitlinelength = Sim->RUU_size * (RegCellHeight + 3 * Sim->ruu_issue_width * WordlineSpacing);

  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"res station power stats\n");
  power0->rs_decoder = array_decoder_power(Sim->RUU_size,Sim->data_width,predeclength,2*Sim->ruu_issue_width,Sim->ruu_issue_width,cache);
  power0->rs_wordline = array_wordline_power(Sim->RUU_size,Sim->data_width,wordlinelength,2*Sim->ruu_issue_width,Sim->ruu_issue_width,cache);
  power0->rs_bitline = array_bitline_power(Sim->RUU_size,Sim->data_width,bitlinelength,2*Sim->ruu_issue_width,Sim->ruu_issue_width,cache);
  /* no senseamps in reg file structures (only caches) */
  power0->rs_senseamp =0;

  /* addresses go into lsq tag's */
  power0->lsq_wakeup_tagdrive =cam_tagdrive(Sim->LSQ_size,Sim->data_width,Sim->res_memport,Sim->res_memport);
  power0->lsq_wakeup_tagmatch =cam_tagmatch(Sim->LSQ_size,Sim->data_width,Sim->res_memport,Sim->res_memport);
  power0->lsq_wakeup_ormatch =0;

  wordlinelength = Sim->data_width *
    (RegCellWidth +
     4 * Sim->res_memport * BitlineSpacing);

  bitlinelength = Sim->RUU_size * (RegCellHeight + 4 * Sim->res_memport * WordlineSpacing);

  /* rs's hold data */
  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"lsq station power stats\n");
  power0->lsq_rs_decoder = array_decoder_power(Sim->LSQ_size,Sim->data_width,predeclength,Sim->res_memport,Sim->res_memport,cache);
  power0->lsq_rs_wordline = array_wordline_power(Sim->LSQ_size,Sim->data_width,wordlinelength,Sim->res_memport,Sim->res_memport,cache);
  power0->lsq_rs_bitline = array_bitline_power(Sim->LSQ_size,Sim->data_width,bitlinelength,Sim->res_memport,Sim->res_memport,cache);
  power0->lsq_rs_senseamp =0;

  power0->resultbus = compute_resultbus_power();

  /* CACTI 4.1 */
  /* Load cache values into what cacti is expecting */
  //time_parameters.cache_size = Sim->btb_config[0] * (Sim->data_width/8) * Sim->btb_config[1]; /* C */
  //time_parameters.block_size = (Sim->data_width/8); /* B */
  //time_parameters.associativity = Sim->btb_config[1]; /* A */
  //time_parameters.number_of_sets = Sim->btb_config[0]; /* C/(B*A) */

  fprintf(stderr,"\n*** Calculating btb consumption parameters\n");

  /* have Cacti compute optimal cache config */
  //calculate_time(&time_result,&time_parameters);
  //output_data(&time_result,&time_parameters);


  /* MSG*/
  /* a chamada da cacti_interface que estava aki foi colocada no comeco deste metodo */

  output_data(&result_total.result, &result_total.area, &result_total.params);

	/* extract Cacti results */
	//	ndwl=time_result.best_Ndwl;
	//	ndbl=time_result.best_Ndbl;
	//	nspd=time_result.best_Nspd;
	//	ntwl=time_result.best_Ntwl;
	//	ntbl=time_result.best_Ntbl;
	//	ntspd=time_result.best_Ntspd;
	//	c = time_parameters.cache_size;
	//	b = time_parameters.block_size;
	//	a = time_parameters.associativity;

	ndwl=result_total.result.best_Ndwl;
	ndbl=result_total.result.best_Ndbl;
	nspd=result_total.result.best_Nspd;
	ntwl=result_total.result.best_Ntwl;
	ntbl=result_total.result.best_Ntbl;
	ntspd=result_total.result.best_Ntspd;

	/* CACTI 4.1 */

  cache=1;

  /* Figure out how many rows/cols there are now */
  rowsb = c/(b*a*ndbl*nspd);
  colsb = 8*b*a*nspd/ndwl;

  if(Sim->GLOBAL->verbose) {
    fprintf(stderr,"%d KB %d-way btb (%d-byte block size):\n",c,a,b);
    fprintf(stderr,"ndwl == %d, ndbl == %d, nspd == %d\n",ndwl,ndbl,nspd);
    fprintf(stderr,"%d sets of %d rows x %d cols\n",ndwl*ndbl,rowsb,colsb);
  }

  predeclength = rowsb * (RegCellHeight + WordlineSpacing);
  wordlinelength = colsb *  (RegCellWidth + BitlineSpacing);
  bitlinelength = rowsb * (RegCellHeight + WordlineSpacing);

  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"btb power stats\n");
  power0->btb = ndwl*ndbl*(array_decoder_power(rowsb,colsb,predeclength,1,1,cache) + array_wordline_power(rowsb,colsb,wordlinelength,1,1,cache) + array_bitline_power(rowsb,colsb,bitlinelength,1,1,cache) + senseamp_power(colsb));

  cache=1;

  scale_factor = squarify(Sim->twolev_config[0],Sim->twolev_config[2]);
  predeclength = (Sim->twolev_config[0] / scale_factor)* (RegCellHeight + WordlineSpacing);
  wordlinelength = Sim->twolev_config[2] * scale_factor *  (RegCellWidth + BitlineSpacing);
  bitlinelength = (Sim->twolev_config[0] / scale_factor) * (RegCellHeight + WordlineSpacing);

  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"local predict power stats\n");

  power0->local_predict = array_decoder_power(Sim->twolev_config[0]/scale_factor,Sim->twolev_config[2]*scale_factor,predeclength,1,1,cache) + array_wordline_power(Sim->twolev_config[0]/scale_factor,Sim->twolev_config[2]*scale_factor,wordlinelength,1,1,cache) + array_bitline_power(Sim->twolev_config[0]/scale_factor,Sim->twolev_config[2]*scale_factor,bitlinelength,1,1,cache) + senseamp_power(Sim->twolev_config[2]*scale_factor);

  scale_factor = squarify(Sim->twolev_config[1],3);

  predeclength = (Sim->twolev_config[1] / scale_factor)* (RegCellHeight + WordlineSpacing);
  wordlinelength = 3 * scale_factor *  (RegCellWidth + BitlineSpacing);
  bitlinelength = (Sim->twolev_config[1] / scale_factor) * (RegCellHeight + WordlineSpacing);


  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"local predict power stats\n");
  power0->local_predict += array_decoder_power(Sim->twolev_config[1]/scale_factor,3*scale_factor,predeclength,1,1,cache) + array_wordline_power(Sim->twolev_config[1]/scale_factor,3*scale_factor,wordlinelength,1,1,cache) + array_bitline_power(Sim->twolev_config[1]/scale_factor,3*scale_factor,bitlinelength,1,1,cache) + senseamp_power(3*scale_factor);

  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"bimod_config[0] == %d\n",Sim->bimod_config[0]);

  scale_factor = squarify(Sim->bimod_config[0],2);

  predeclength = Sim->bimod_config[0]/scale_factor * (RegCellHeight + WordlineSpacing);
  wordlinelength = 2*scale_factor *  (RegCellWidth + BitlineSpacing);
  bitlinelength = Sim->bimod_config[0]/scale_factor * (RegCellHeight + WordlineSpacing);


  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"global predict power stats\n");
  power0->global_predict = array_decoder_power(Sim->bimod_config[0]/scale_factor,2*scale_factor,predeclength,1,1,cache) + array_wordline_power(Sim->bimod_config[0]/scale_factor,2*scale_factor,wordlinelength,1,1,cache) + array_bitline_power(Sim->bimod_config[0]/scale_factor,2*scale_factor,bitlinelength,1,1,cache) + senseamp_power(2*scale_factor);

  scale_factor = squarify(Sim->comb_config[0],2);

  predeclength = Sim->comb_config[0]/scale_factor * (RegCellHeight + WordlineSpacing);
  wordlinelength = 2*scale_factor *  (RegCellWidth + BitlineSpacing);
  bitlinelength = Sim->comb_config[0]/scale_factor * (RegCellHeight + WordlineSpacing);

  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"chooser predict power stats\n");
  power0->chooser = array_decoder_power(Sim->comb_config[0]/scale_factor,2*scale_factor,predeclength,1,1,cache) + array_wordline_power(Sim->comb_config[0]/scale_factor,2*scale_factor,wordlinelength,1,1,cache) + array_bitline_power(Sim->comb_config[0]/scale_factor,2*scale_factor,bitlinelength,1,1,cache) + senseamp_power(2*scale_factor);

  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"RAS predict power stats\n");
  power0->ras = simple_array_power(Sim->ras_size,Sim->data_width,1,1,0);

  tagsize = va_size - ((int)logtwo(Sim->cache_dl1->nsets) + (int)logtwo(Sim->cache_dl1->bsize));

  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"dtlb predict power stats\n");
  power0->dtlb = Sim->res_memport*(cam_array(Sim->dtlb->nsets, va_size - (int)logtwo((double)Sim->dtlb->bsize),1,1) + simple_array_power(Sim->dtlb->nsets,tagsize,1,1,cache));

  tagsize = va_size - ((int)logtwo(Sim->cache_il1->nsets) + (int)logtwo(Sim->cache_il1->bsize));

  predeclength = Sim->itlb->nsets * (RegCellHeight + WordlineSpacing);
  wordlinelength = logtwo((double)Sim->itlb->bsize) * (RegCellWidth + BitlineSpacing);
  bitlinelength = Sim->itlb->nsets * (RegCellHeight + WordlineSpacing);

  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"itlb predict power stats\n");
  power0->itlb = cam_array(Sim->itlb->nsets, va_size - (int)logtwo((double)Sim->itlb->bsize),1,1) + simple_array_power(Sim->itlb->nsets,tagsize,1,1,cache);


  cache=1;

  /* CACTI 4.1 */

  //time_parameters.cache_size = Sim->cache_il1->nsets * Sim->cache_il1->bsize * Sim->cache_il1->assoc; /* C */
  //time_parameters.block_size = Sim->cache_il1->bsize; /* B */
  //time_parameters.associativity = Sim->cache_il1->assoc; /* A */
  //time_parameters.number_of_sets = Sim->cache_il1->nsets; /* C/(B*A) */

  fprintf(stderr,"\n*** Calculating il1 consumption parameters\n");

  //calculate_time(&time_result,&time_parameters);
  //output_data(&time_result,&time_parameters);

  c = Sim->cache_il1->nsets * Sim->cache_il1->bsize* Sim->cache_il1->assoc; /* C */
  b = Sim->cache_il1->bsize; /* B */
  a = Sim->cache_il1->assoc; /* A */

  result_total = cacti_interface(/*C*/ c,
								 /*B*/ b,
								 /*A*/ a,
								 /*RWP*/ 1,
								 /*ERP*/ 0,
								 /*EWP*/ 0,
								 /*NSER*/ 0 ,
								 /*Nbanks*/ 1,
								 /*TECH*/ TECH_LENGTH ,
								 /*OUTPUTWIDTH*/ 64,
								 /*CUSTOMTAG*/ 0,
								 /*TAGWIDTH*/ 0,
								 /*ACCESSMODE*/ 0,
								 /*PURESRAM*/ 0);

  output_data(&result_total.result, &result_total.area, &result_total.params);

    // ndwl=time_result.best_Ndwl;
  	// ndbl=time_result.best_Ndbl;
  	// nspd=time_result.best_Nspd;
  	// ntwl=time_result.best_Ntwl;
  	// ntbl=time_result.best_Ntbl;
  	// ntspd=time_result.best_Ntspd;

  	// c = time_parameters.cache_size;
  	// b = time_parameters.block_size;
  	// a = time_parameters.associativity;


  	ndwl=result_total.result.best_Ndwl;
  	ndbl=result_total.result.best_Ndbl;
  	nspd=result_total.result.best_Nspd;
  	ntwl=result_total.result.best_Ntwl;
  	ntbl=result_total.result.best_Ntbl;
  	ntspd=result_total.result.best_Ntspd;

  	/* CACTI 4.1*/

  rowsb = c/(b*a*ndbl*nspd);
  colsb = 8*b*a*nspd/ndwl;

  tagsize = va_size - ((int)logtwo(Sim->cache_il1->nsets) + (int)logtwo(Sim->cache_il1->bsize));
  trowsb = c/(b*a*ntbl*ntspd);
  tcolsb = a * (tagsize + 1 + 6) * ntspd/ntwl;

  if(Sim->GLOBAL->verbose) {
    fprintf(stderr,"%d KB %d-way cache (%d-byte block size):\n",c,a,b);
    fprintf(stderr,"ndwl == %d, ndbl == %d, nspd == %d\n",ndwl,ndbl,nspd);
    fprintf(stderr,"%d sets of %d rows x %d cols\n",ndwl*ndbl,rowsb,colsb);
    fprintf(stderr,"tagsize == %d\n",tagsize);
  }

  predeclength = rowsb * (RegCellHeight + WordlineSpacing);
  wordlinelength = colsb *  (RegCellWidth + BitlineSpacing);
  bitlinelength = rowsb * (RegCellHeight + WordlineSpacing);

  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"icache power stats\n");
  power0->icache_decoder = ndwl*ndbl*array_decoder_power(rowsb,colsb,predeclength,1,1,cache);
  power0->icache_wordline = ndwl*ndbl*array_wordline_power(rowsb,colsb,wordlinelength,1,1,cache);
  power0->icache_bitline = ndwl*ndbl*array_bitline_power(rowsb,colsb,bitlinelength,1,1,cache);
  power0->icache_senseamp = ndwl*ndbl*senseamp_power(colsb);
  power0->icache_tagarray = ntwl*ntbl*(simple_array_power(trowsb,tcolsb,1,1,cache));

  power0->icache_power = power0->icache_decoder + power0->icache_wordline + power0->icache_bitline + power0->icache_senseamp + power0->icache_tagarray;

	/* CACTI 4.1 */

    // time_parameters.cache_size = cache_dl1->nsets * cache_dl1->bsize* cache_dl1->assoc; /* C */
  	// time_parameters.block_size = cache_dl1->bsize; /* B */
  	// time_parameters.associativity = cache_dl1->assoc; /* A */
  	// time_parameters.number_of_sets = cache_dl1->nsets; /* C/(B*A) */

  	c = Sim->cache_dl1->nsets * Sim->cache_dl1->bsize* Sim->cache_dl1->assoc; /* C */
  	b = Sim->cache_dl1->bsize; /* B */
  	a = Sim->cache_dl1->assoc; /* A */

  	fprintf(stderr,"\n*** Calculating dl1 consumption parameters\n");

  	// calculate_time(	 &time_result, &time_parameters);
  	// output_data(&time_result, &time_parameters);

  	result_total = cacti_interface(	 /*C*/ c,
									 /*B*/ b,
									 /*A*/ a,
									 /*RWP*/ 1,
									 /*ERP*/ 0,
									 /*EWP*/ 0,
									 /*NSER*/ 0 ,
									 /*Nbanks*/ 1,
									 /*TECH*/ TECH_LENGTH ,
									 /*OUTPUTWIDTH*/ 64,
									 /*CUSTOMTAG*/ 0,
									 /*TAGWIDTH*/ 0,
									 /*ACCESSMODE*/ 0,
									 /*PURESRAM*/ 0);

  	output_data(&result_total.result, &result_total.area, &result_total.params);


  	/*ndwl=time_result.best_Ndwl;
  	ndbl=time_result.best_Ndbl;
  	nspd=time_result.best_Nspd;
  	ntwl=time_result.best_Ntwl;
  	ntbl=time_result.best_Ntbl;
  	ntspd=time_result.best_Ntspd;
  	c = time_parameters.cache_size;
  	b = time_parameters.block_size;
  	a = time_parameters.associativity;*/

  	ndwl=result_total.result.best_Ndwl;
  	ndbl=result_total.result.best_Ndbl;
  	nspd=result_total.result.best_Nspd;
  	ntwl=result_total.result.best_Ntwl;
  	ntbl=result_total.result.best_Ntbl;
  	ntspd=result_total.result.best_Ntspd;

  	/* CACTI 4.1 */

  cache=1;

  rowsb = c/(b*a*ndbl*nspd);
  colsb = 8*b*a*nspd/ndwl;

  tagsize = va_size - ((int)logtwo(Sim->cache_dl1->nsets) + (int)logtwo(Sim->cache_dl1->bsize));
  trowsb = c/(b*a*ntbl*ntspd);
  tcolsb = a * (tagsize + 1 + 6) * ntspd/ntwl;

  if(Sim->GLOBAL->verbose) {
    fprintf(stderr,"%d KB %d-way cache (%d-byte block size):\n",c,a,b);
    fprintf(stderr,"ndwl == %d, ndbl == %d, nspd == %d\n",ndwl,ndbl,nspd);
    fprintf(stderr,"%d sets of %d rows x %d cols\n",ndwl*ndbl,rowsb,colsb);
    fprintf(stderr,"tagsize == %d\n",tagsize);

    fprintf(stderr,"\nntwl == %d, ntbl == %d, ntspd == %d\n",ntwl,ntbl,ntspd);
    fprintf(stderr,"%d sets of %d rows x %d cols\n",ntwl*ntbl,trowsb,tcolsb);
  }

  predeclength = rowsb * (RegCellHeight + WordlineSpacing);
  wordlinelength = colsb *  (RegCellWidth + BitlineSpacing);
  bitlinelength = rowsb * (RegCellHeight + WordlineSpacing);

  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"dcache power stats\n");
  power0->dcache_decoder = Sim->res_memport*ndwl*ndbl*array_decoder_power(rowsb,colsb,predeclength,1,1,cache);
  power0->dcache_wordline = Sim->res_memport*ndwl*ndbl*array_wordline_power(rowsb,colsb,wordlinelength,1,1,cache);
  power0->dcache_bitline = Sim->res_memport*ndwl*ndbl*array_bitline_power(rowsb,colsb,bitlinelength,1,1,cache);
  power0->dcache_senseamp = Sim->res_memport*ndwl*ndbl*senseamp_power(colsb);
  power0->dcache_tagarray = Sim->res_memport*ntwl*ntbl*(simple_array_power(trowsb,tcolsb,1,1,cache));

  power0->dcache_power = power0->dcache_decoder + power0->dcache_wordline + power0->dcache_bitline + power0->dcache_senseamp + power0->dcache_tagarray;

  clockpower = total_clockpower(.018);
  power0->clock_power = clockpower;
  if(Sim->GLOBAL->verbose) {
    fprintf(stderr,"result bus power == %f\n",power0->resultbus);
    fprintf(stderr,"global clock power == %f\n",clockpower);
  }

   /* DL2 consumption */

  	// time_parameters.cache_size = cache_dl2->nsets * cache_dl2->bsize* cache_dl2->assoc; /* C */
  	// time_parameters.block_size = cache_dl2->bsize; /* B */
  	// time_parameters.associativity = cache_dl2->assoc; /* A */
  	// time_parameters.number_of_sets = cache_dl2->nsets; /* C/(B*A) */

  	c = Sim->cache_dl2->nsets * Sim->cache_dl2->bsize* Sim->cache_dl2->assoc; /* C */
  	b = Sim->cache_dl2->bsize; /* B */
  	a = Sim->cache_dl2->assoc; /* A */

  	fprintf(stderr,"Calculating dl2 consumption parameters\n");

  	// calculate_time(	 &time_result, &time_parameters);
  	// output_data(&time_result, &time_parameters);

  	result_total = cacti_interface(/*C*/c,
  						 		 /*B*/ b,
  						 		 /*A*/ a,
  								 /*RWP*/ 1,
  								 /*ERP*/ 0,
  								 /*EWP*/ 0,
  								 /*NSER*/ 0 ,
  								 /*Nbanks*/ 1,
  								 /*TECH*/ TECH_LENGTH ,
  								 /*OUTPUTWIDTH*/ 64,
  								 /*CUSTOMTAG*/ 0,
  								 /*TAGWIDTH*/ 0,
  								 /*ACCESSMODE*/ 0,
  								 /*PURESRAM*/ 0);

  	output_data(&result_total.result, &result_total.area, &result_total.params);


  	/*ndwl=time_result.best_Ndwl;
  	ndbl=time_result.best_Ndbl;
  	nspd=time_result.best_Nspd;
  	ntwl=time_result.best_Ntwl;
  	ntbl=time_result.best_Ntbl;
  	ntspd=time_result.best_Ntspd;
  	c = time_parameters.cache_size;
  	b = time_parameters.block_size;
  	a = time_parameters.associativity;*/

  	ndwl=result_total.result.best_Ndwl;
  	ndbl=result_total.result.best_Ndbl;
  	nspd=result_total.result.best_Nspd;
  	ntwl=result_total.result.best_Ntwl;
  	ntbl=result_total.result.best_Ntbl;
  	ntspd=result_total.result.best_Ntspd;

  	/* CACTI 4.1 */

  rowsb = c/(b*a*ndbl*nspd);
  colsb = 8*b*a*nspd/ndwl;

  tagsize = va_size - ((int)logtwo(Sim->cache_dl2->nsets) + (int)logtwo(Sim->cache_dl2->bsize));
  trowsb = c/(b*a*ntbl*ntspd);
  tcolsb = a * (tagsize + 1 + 6) * ntspd/ntwl;

  if(Sim->GLOBAL->verbose) {
    fprintf(stderr,"%d KB %d-way cache (%d-byte block size):\n",c,a,b);
    fprintf(stderr,"ndwl == %d, ndbl == %d, nspd == %d\n",ndwl,ndbl,nspd);
    fprintf(stderr,"%d sets of %d rows x %d cols\n",ndwl*ndbl,rowsb,colsb);
    fprintf(stderr,"tagsize == %d\n",tagsize);
  }

  predeclength = rowsb * (RegCellHeight + WordlineSpacing);
  wordlinelength = colsb *  (RegCellWidth + BitlineSpacing);
  bitlinelength = rowsb * (RegCellHeight + WordlineSpacing);

  if(Sim->GLOBAL->verbose)
    fprintf(stderr,"dcache2 power stats\n");
  power0->dcache2_decoder = array_decoder_power(rowsb,colsb,predeclength,1,1,cache);
  power0->dcache2_wordline = array_wordline_power(rowsb,colsb,wordlinelength,1,1,cache);
  power0->dcache2_bitline = array_bitline_power(rowsb,colsb,bitlinelength,1,1,cache);
  power0->dcache2_senseamp = senseamp_power(colsb);
  power0->dcache2_tagarray = simple_array_power(trowsb,tcolsb,1,1,cache);

  power0->dcache2_power = power0->dcache2_decoder + power0->dcache2_wordline + power0->dcache2_bitline + power0->dcache2_senseamp + power0->dcache2_tagarray;

  power0->rat_decoder *= crossover_scaling;
  power0->rat_wordline *= crossover_scaling;
  power0->rat_bitline *= crossover_scaling;

  power0->dcl_compare *= crossover_scaling;
  power0->dcl_pencode *= crossover_scaling;
  power0->inst_decoder_power *= crossover_scaling;
  power0->wakeup_tagdrive *= crossover_scaling;
  power0->wakeup_tagmatch *= crossover_scaling;
  power0->wakeup_ormatch *= crossover_scaling;

  power0->selection *= crossover_scaling;

  power0->regfile_decoder *= crossover_scaling;
  power0->regfile_wordline *= crossover_scaling;
  power0->regfile_bitline *= crossover_scaling;
  power0->regfile_senseamp *= crossover_scaling;

  power0->rs_decoder *= crossover_scaling;
  power0->rs_wordline *= crossover_scaling;
  power0->rs_bitline *= crossover_scaling;
  power0->rs_senseamp *= crossover_scaling;

  power0->lsq_wakeup_tagdrive *= crossover_scaling;
  power0->lsq_wakeup_tagmatch *= crossover_scaling;

  power0->lsq_rs_decoder *= crossover_scaling;
  power0->lsq_rs_wordline *= crossover_scaling;
  power0->lsq_rs_bitline *= crossover_scaling;
  power0->lsq_rs_senseamp *= crossover_scaling;

  power0->resultbus *= crossover_scaling;

  power0->btb *= crossover_scaling;
  power0->local_predict *= crossover_scaling;
  power0->global_predict *= crossover_scaling;
  power0->chooser *= crossover_scaling;

  power0->dtlb *= crossover_scaling;

  power0->itlb *= crossover_scaling;

  power0->icache_decoder *= crossover_scaling;
  power0->icache_wordline*= crossover_scaling;
  power0->icache_bitline *= crossover_scaling;
  power0->icache_senseamp*= crossover_scaling;
  power0->icache_tagarray*= crossover_scaling;

  power0->icache_power *= crossover_scaling;

  power0->dcache_decoder *= crossover_scaling;
  power0->dcache_wordline *= crossover_scaling;
  power0->dcache_bitline *= crossover_scaling;
  power0->dcache_senseamp *= crossover_scaling;
  power0->dcache_tagarray *= crossover_scaling;

  power0->dcache_power *= crossover_scaling;

  power0->clock_power *= crossover_scaling;

  power0->dcache2_decoder *= crossover_scaling;
  power0->dcache2_wordline *= crossover_scaling;
  power0->dcache2_bitline *= crossover_scaling;
  power0->dcache2_senseamp *= crossover_scaling;
  power0->dcache2_tagarray *= crossover_scaling;

  power0->dcache2_power *= crossover_scaling;

  power0->total_power = power0->local_predict + power0->global_predict +
    power0->chooser + power0->btb +
    power0->rat_decoder + power0->rat_wordline +
    power0->rat_bitline + power0->rat_senseamp +
    power0->dcl_compare + power0->dcl_pencode +
    power0->inst_decoder_power +
    power0->wakeup_tagdrive + power0->wakeup_tagmatch +
    power0->selection +
    power0->regfile_decoder + power0->regfile_wordline +
    power0->regfile_bitline + power0->regfile_senseamp +
    power0->rs_decoder + power0->rs_wordline +
    power0->rs_bitline + power0->rs_senseamp +
    power0->lsq_wakeup_tagdrive + power0->lsq_wakeup_tagmatch +
    power0->lsq_rs_decoder + power0->lsq_rs_wordline +
    power0->lsq_rs_bitline + power0->lsq_rs_senseamp +
    power0->resultbus +
    power0->clock_power +
    power0->icache_power +
    power0->itlb +
    power0->dcache_power +
    power0->dtlb +
    power0->dcache2_power;

  power0->total_power_nodcache2 =power0->local_predict + power0->global_predict +
    power0->chooser + power0->btb +
    power0->rat_decoder + power0->rat_wordline +
    power0->rat_bitline + power0->rat_senseamp +
    power0->dcl_compare + power0->dcl_pencode +
    power0->inst_decoder_power +
    power0->wakeup_tagdrive + power0->wakeup_tagmatch +
    power0->selection +
    power0->regfile_decoder + power0->regfile_wordline +
    power0->regfile_bitline + power0->regfile_senseamp +
    power0->rs_decoder + power0->rs_wordline +
    power0->rs_bitline + power0->rs_senseamp +
    power0->lsq_wakeup_tagdrive + power0->lsq_wakeup_tagmatch +
    power0->lsq_rs_decoder + power0->lsq_rs_wordline +
    power0->lsq_rs_bitline + power0->lsq_rs_senseamp +
    power0->resultbus +
    power0->clock_power +
    power0->icache_power +
    power0->itlb +
    power0->dcache_power +
    power0->dtlb +
    power0->dcache2_power;

  power0->bpred_power = power0->btb + power0->local_predict + power0->global_predict + power0->chooser + power0->ras;

  power0->rat_power = power0->rat_decoder +
    power0->rat_wordline + power0->rat_bitline + power0->rat_senseamp;

  power0->dcl_power = power0->dcl_compare + power0->dcl_pencode;

  power0->rename_power = power0->rat_power +
    power0->dcl_power +
    power0->inst_decoder_power;

  power0->wakeup_power = power0->wakeup_tagdrive + power0->wakeup_tagmatch +
    power0->wakeup_ormatch;

  power0->rs_power = power0->rs_decoder +
    power0->rs_wordline + power0->rs_bitline + power0->rs_senseamp;

  power0->rs_power_nobit = power0->rs_decoder +
    power0->rs_wordline + power0->rs_senseamp;

  power0->window_power = power0->wakeup_power + power0->rs_power +
    power0->selection;

  power0->lsq_rs_power = power0->lsq_rs_decoder +
    power0->lsq_rs_wordline + power0->lsq_rs_bitline +
    power0->lsq_rs_senseamp;

  power0->lsq_rs_power_nobit = power0->lsq_rs_decoder +
    power0->lsq_rs_wordline + power0->lsq_rs_senseamp;

  power0->lsq_wakeup_power = power0->lsq_wakeup_tagdrive + power0->lsq_wakeup_tagmatch;

  power0->lsq_power = power0->lsq_wakeup_power + power0->lsq_rs_power;

  power0->regfile_power = power0->regfile_decoder +
    power0->regfile_wordline + power0->regfile_bitline +
    power0->regfile_senseamp;

  power0->regfile_power_nobit = power0->regfile_decoder +
    power0->regfile_wordline + power0->regfile_senseamp;

  dump_power_stats();
}

