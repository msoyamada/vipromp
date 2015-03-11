#ifndef POWER_H
#define POWER_H

#include <math.h>
#include <assert.h>
#include "stats.hpp"


/*  The following are things you might want to change
 *  when compiling
 */

/*
 * The output can be in 'long' format, which shows everything, or
 * 'short' format, which is just what a program like 'grap' would
 * want to see
 */

#define LONG 1
#define SHORT 2

#define OUTPUTTYPE LONG

/* Do we want static AFs (STATIC_AF) or Dynamic AFs (DYNAMIC_AF) */
/* #define DYNAMIC_AF */
#define DYNAMIC_AF

/*
 * Address bits in a word, and number of output bits from the cache
 */

#define ADDRESS_BITS 64
#define BITOUT 64

/* limits on the various N parameters */

#define MAXN 8            /* Maximum for Ndwl,Ntwl,Ndbl,Ntbl */
#define MAXSUBARRAYS 8    /* Maximum subarrays for data and tag arrays */
#define MAXSPD 8          /* Maximum for Nspd, Ntspd */


/*===================================================================*/

/*
 * The following are things you probably wouldn't want to change.
 */


#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define BIGNUM 1e30
#define DIVIDE(a,b) ((b)==0)? 0:(a)/(b)

/* Used to communicate with the horowitz model */

#define RISE 1
#define FALL 0
#define NCH  1
#define PCH  0



/*
 * The following scale factor can be used to scale between technologies.
 * To convert from 0.8um to 0.5um, make FUDGEFACTOR = 1.6
 */

#define FUDGEFACTOR 1.0


#if 0
/*===================================================================*/

/*
 * Cache layout parameters and process parameters
 * Thanks to Glenn Reinman for the technology scaling factors
 */

#define GEN_POWER_FACTOR 1.31

#define TECH_POINT35
#if defined(TECH_POINT10)
#define CSCALE		(84.2172)	/* wire capacitance scaling factor */
			/* linear: 51.7172, predicted: 84.2172 */
#define RSCALE		(80.0000)	/* wire resistance scaling factor */
#define LSCALE		0.1250		/* length (feature) scaling factor */
#define ASCALE		(LSCALE*LSCALE)	/* area scaling factor */
#define VSCALE		0.38		/* voltage scaling factor */
#define VTSCALE		0.49		/* threshold voltage scaling factor */
#define SSCALE		0.80		/* sense voltage scaling factor */
#define GEN_POWER_SCALE (1/GEN_POWER_FACTOR)
#elif defined(TECH_POINT18)
#define CSCALE		(19.7172)	/* wire capacitance scaling factor */
#define RSCALE		(20.0000)	/* wire resistance scaling factor */
#define LSCALE		0.2250		/* length (feature) scaling factor */
#define ASCALE		(LSCALE*LSCALE)	/* area scaling factor */
#define VSCALE		0.4		/* voltage scaling factor */
#define VTSCALE		0.5046		/* threshold voltage scaling factor */
#define SSCALE		0.85		/* sense voltage scaling factor */
#define GEN_POWER_SCALE 1
#elif defined(TECH_POINT25)
#define CSCALE		(10.2197)	/* wire capacitance scaling factor */
#define RSCALE		(10.2571)	/* wire resistance scaling factor */
#define LSCALE		0.3571		/* length (feature) scaling factor */
#define ASCALE		(LSCALE*LSCALE)	/* area scaling factor */
#define VSCALE		0.45		/* voltage scaling factor */
#define VTSCALE		0.5596		/* threshold voltage scaling factor */
#define SSCALE		0.90		/* sense voltage scaling factor */
#define GEN_POWER_SCALE GEN_POWER_FACTOR
#elif defined(TECH_POINT35a)
#define CSCALE		(5.2197)	/* wire capacitance scaling factor */
#define RSCALE		(5.2571)	/* wire resistance scaling factor */
#define LSCALE		0.4375		/* length (feature) scaling factor */
#define ASCALE		(LSCALE*LSCALE)	/* area scaling factor */
#define VSCALE		0.5		/* voltage scaling factor */
#define VTSCALE		0.6147		/* threshold voltage scaling factor */
#define SSCALE		0.95		/* sense voltage scaling factor */
#define GEN_POWER_SCALE (GEN_POWER_FACTOR*GEN_POWER_FACTOR)
#elif defined(TECH_POINT35)
#define CSCALE		(5.2197)	/* wire capacitance scaling factor */
#define RSCALE		(5.2571)	/* wire resistance scaling factor */
#define LSCALE		0.4375		/* length (feature) scaling factor */
#define ASCALE		(LSCALE*LSCALE)	/* area scaling factor */
#define VSCALE		0.5		/* voltage scaling factor */
#define VTSCALE		0.6147		/* threshold voltage scaling factor */
#define SSCALE		0.95		/* sense voltage scaling factor */
#define TECH_LENGTH 0.35		/* MSG for CACTI 4.1 */
#define GEN_POWER_SCALE (GEN_POWER_FACTOR*GEN_POWER_FACTOR)
#elif defined(TECH_POINT40)
#define CSCALE		1.0		/* wire capacitance scaling factor */
#define RSCALE		1.0		/* wire resistance scaling factor */
#define LSCALE		0.5		/* length (feature) scaling factor */
#define ASCALE		(LSCALE*LSCALE)	/* area scaling factor */
#define VSCALE		1.0		/* voltage scaling factor */
#define VTSCALE		1.0		/* threshold voltage scaling factor */
#define SSCALE		1.0		/* sense voltage scaling factor */
#define GEN_POWER_SCALE (GEN_POWER_FACTOR*GEN_POWER_FACTOR*GEN_POWER_FACTOR)
#else /* TECH_POINT80 */
/* scaling factors */
#define CSCALE		1.0		/* wire capacitance scaling factor */
#define RSCALE		1.0		/* wire resistance scaling factor */
#define LSCALE		1.0		/* length (feature) scaling factor */
#define ASCALE		(LSCALE*LSCALE)	/* area scaling factor */
#define VSCALE		1.0		/* voltage scaling factor */
#define VTSCALE		1.0		/* threshold voltage scaling factor */
#define SSCALE		1.0		/* sense voltage scaling factor */
#define GEN_POWER_SCALE (GEN_POWER_FACTOR*GEN_POWER_FACTOR*GEN_POWER_FACTOR*GEN_POWER_FACTOR)
#endif

/*
 * CMOS 0.8um model parameters
 *   - from Appendix II of Cacti tech report
 */


/* corresponds to 8um of m3 @ 225ff/um */
#define Cwordmetal    (1.8e-15 * (CSCALE * ASCALE))

/* corresponds to 16um of m2 @ 275ff/um */
#define Cbitmetal     (4.4e-15 * (CSCALE * ASCALE))

/* corresponds to 1um of m2 @ 275ff/um */
#define Cmetal        Cbitmetal/16

#define CM3metal        Cbitmetal/16
#define CM2metal        Cbitmetal/16

/* #define Cmetal 1.222e-15 */

/* fF/um2 at 1.5V */
#define Cndiffarea    0.137e-15		/* FIXME: ??? */

/* fF/um2 at 1.5V */
#define Cpdiffarea    0.343e-15		/* FIXME: ??? */

/* fF/um at 1.5V */
#define Cndiffside    0.275e-15		/* in general this does not scale */

/* fF/um at 1.5V */
#define Cpdiffside    0.275e-15		/* in general this does not scale */

/* fF/um at 1.5V */
#define Cndiffovlp    0.138e-15		/* FIXME: by depth??? */

/* fF/um at 1.5V */
#define Cpdiffovlp    0.138e-15		/* FIXME: by depth??? */

/* fF/um assuming 25% Miller effect */
#define Cnoxideovlp   0.263e-15		/* FIXME: by depth??? */

/* fF/um assuming 25% Miller effect */
#define Cpoxideovlp   0.338e-15		/* FIXME: by depth??? */

/* um */
#define Leff          (0.8 * LSCALE)

/* fF/um2 */
#define Cgate         1.95e-15		/* FIXME: ??? */

/* fF/um2 */
#define Cgatepass     1.45e-15		/* FIXME: ??? */

/* note that the value of Cgatepass will be different depending on
   whether or not the source and drain are at different potentials or
   the same potential.  The two values were averaged */

/* fF/um */
#define Cpolywire	(0.25e-15 * CSCALE * LSCALE)

/* ohms*um of channel width */
#define Rnchannelstatic	(25800 * LSCALE)

/* ohms*um of channel width */
#define Rpchannelstatic	(61200 * LSCALE)

#define Rnchannelon	(9723 * LSCALE)

#define Rpchannelon	(22400 * LSCALE)

/* corresponds to 16um of m2 @ 48mO/sq */
#define Rbitmetal	(0.320 * (RSCALE * ASCALE))

/* corresponds to  8um of m3 @ 24mO/sq */
#define Rwordmetal	(0.080 * (RSCALE * ASCALE))

#define Vdd		(5 * VSCALE)

/* other stuff (from tech report, appendix 1) */

#define Mhz             600e6
#define Period          (1/Mhz)

#define krise		(0.4e-9 * LSCALE)
#define tsensedata	(5.8e-10 * LSCALE)
#define tsensetag	(2.6e-10 * LSCALE)
#define tfalldata	(7e-10 * LSCALE)
#define tfalltag	(7e-10 * LSCALE)
#define Vbitpre		(3.3 * SSCALE)
#define Vt		(1.09 * VTSCALE)
#define Vbitsense	(0.10 * SSCALE)

#define Powerfactor (Mhz)*Vdd*Vdd

#define SensePowerfactor3 (Mhz)*(Vbitsense)*(Vbitsense)
#define SensePowerfactor2 (Mhz)*(Vbitpre-Vbitsense)*(Vbitpre-Vbitsense)
#define SensePowerfactor (Mhz)*(Vdd/2)*(Vdd/2)

#define AF    .5
#define POPCOUNT_AF  (23.9/64.0)

/* Threshold voltages (as a proportion of Vdd)
   If you don't know them, set all values to 0.5 */

#define VSINV         0.456
#define VTHINV100x60  0.438   /* inverter with p=100,n=60 */
#define VTHNAND60x90  0.561   /* nand with p=60 and three n=90 */
#define VTHNOR12x4x1  0.503   /* nor with p=12, n=4, 1 input */
#define VTHNOR12x4x2  0.452   /* nor with p=12, n=4, 2 inputs */
#define VTHNOR12x4x3  0.417   /* nor with p=12, n=4, 3 inputs */
#define VTHNOR12x4x4  0.390   /* nor with p=12, n=4, 4 inputs */
#define VTHOUTDRINV    0.437
#define VTHOUTDRNOR   0.431
#define VTHOUTDRNAND  0.441
#define VTHOUTDRIVE   0.425
#define VTHCOMPINV    0.437
#define VTHMUXDRV1    0.437
#define VTHMUXDRV2    0.486
#define VTHMUXDRV3    0.437
#define VTHEVALINV    0.267
#define VTHSENSEEXTDRV  0.437

/* transistor widths in um (as described in tech report, appendix 1) */
#define Wdecdrivep	(57.0 * LSCALE)
#define Wdecdriven	(40.0 * LSCALE)
#define Wdec3to8n	(14.4 * LSCALE)
#define Wdec3to8p	(14.4 * LSCALE)
#define WdecNORn	(5.4 * LSCALE)
#define WdecNORp	(30.5 * LSCALE)
#define Wdecinvn	(5.0 * LSCALE)
#define Wdecinvp	(10.0  * LSCALE)

#define Wworddrivemax	(100.0 * LSCALE)
#define Wmemcella	(2.4 * LSCALE)
#define Wmemcellr	(4.0 * LSCALE)
#define Wmemcellw	(2.1 * LSCALE)
#define Wmemcellbscale	2		/* means 2x bigger than Wmemcella */
#define Wbitpreequ	(10.0 * LSCALE)

#define Wbitmuxn	(10.0 * LSCALE)
#define WsenseQ1to4	(4.0 * LSCALE)
#define Wcompinvp1	(10.0 * LSCALE)
#define Wcompinvn1	(6.0 * LSCALE)
#define Wcompinvp2	(20.0 * LSCALE)
#define Wcompinvn2	(12.0 * LSCALE)
#define Wcompinvp3	(40.0 * LSCALE)
#define Wcompinvn3	(24.0 * LSCALE)
#define Wevalinvp	(20.0 * LSCALE)
#define Wevalinvn	(80.0 * LSCALE)

#define Wcompn		(20.0 * LSCALE)
#define Wcompp		(30.0 * LSCALE)
#define Wcomppreequ     (40.0 * LSCALE)
#define Wmuxdrv12n	(30.0 * LSCALE)
#define Wmuxdrv12p	(50.0 * LSCALE)
#define WmuxdrvNANDn    (20.0 * LSCALE)
#define WmuxdrvNANDp    (80.0 * LSCALE)
#define WmuxdrvNORn	(60.0 * LSCALE)
#define WmuxdrvNORp	(80.0 * LSCALE)
#define Wmuxdrv3n	(200.0 * LSCALE)
#define Wmuxdrv3p	(480.0 * LSCALE)
#define Woutdrvseln	(12.0 * LSCALE)
#define Woutdrvselp	(20.0 * LSCALE)
#define Woutdrvnandn	(24.0 * LSCALE)
#define Woutdrvnandp	(10.0 * LSCALE)
#define Woutdrvnorn	(6.0 * LSCALE)
#define Woutdrvnorp	(40.0 * LSCALE)
#define Woutdrivern	(48.0 * LSCALE)
#define Woutdriverp	(80.0 * LSCALE)

#define Wcompcellpd2    (2.4 * LSCALE)
#define Wcompdrivern    (400.0 * LSCALE)
#define Wcompdriverp    (800.0 * LSCALE)
#define Wcomparen2      (40.0 * LSCALE)
#define Wcomparen1      (20.0 * LSCALE)
#define Wmatchpchg      (10.0 * LSCALE)
#define Wmatchinvn      (10.0 * LSCALE)
#define Wmatchinvp      (20.0 * LSCALE)
#define Wmatchnandn     (20.0 * LSCALE)
#define Wmatchnandp     (10.0 * LSCALE)
#define Wmatchnorn     (20.0 * LSCALE)
#define Wmatchnorp     (10.0 * LSCALE)

#define WSelORn         (10.0 * LSCALE)
#define WSelORprequ     (40.0 * LSCALE)
#define WSelPn          (10.0 * LSCALE)
#define WSelPp          (15.0 * LSCALE)
#define WSelEnn         (5.0 * LSCALE)
#define WSelEnp         (10.0 * LSCALE)

#define Wsenseextdrv1p  (40.0*LSCALE)
#define Wsenseextdrv1n  (24.0*LSCALE)
#define Wsenseextdrv2p  (200.0*LSCALE)
#define Wsenseextdrv2n  (120.0*LSCALE)


/* bit width of RAM cell in um */
#define BitWidth	(16.0 * LSCALE)

/* bit height of RAM cell in um */
#define BitHeight	(16.0 * LSCALE)

#define Cout		(0.5e-12 * LSCALE)

/* Sizing of cells and spacings */
#define RatCellHeight    (40.0 * LSCALE)
#define RatCellWidth     (70.0 * LSCALE)
#define RatShiftRegWidth (120.0 * LSCALE)
#define RatNumShift      4
#define BitlineSpacing   (6.0 * LSCALE)
#define WordlineSpacing  (6.0 * LSCALE)

#define RegCellHeight    (16.0 * LSCALE)
#define RegCellWidth     (8.0  * LSCALE)

#define CamCellHeight    (40.0 * LSCALE)
#define CamCellWidth     (25.0 * LSCALE)
#define MatchlineSpacing (6.0 * LSCALE)
#define TaglineSpacing   (6.0 * LSCALE)

/*===================================================================*/

/* ALU POWER NUMBERS for .18um 733Mhz */
/* normalize to cap from W */
#define NORMALIZE_SCALE (1.0/(733.0e6*1.45*1.45))
/* normalize .18um cap to other gen's cap, then xPowerfactor */
#define POWER_SCALE    (GEN_POWER_SCALE * NORMALIZE_SCALE * Powerfactor)
#define I_ADD          ((.37 - .091) * POWER_SCALE)
#define I_ADD32        (((.37 - .091)/2)*POWER_SCALE)
#define I_MULT16       ((.31-.095)*POWER_SCALE)
#define I_SHIFT        ((.21-.089)*POWER_SCALE)
#define I_LOGIC        ((.04-.015)*POWER_SCALE)
#define F_ADD          ((1.307-.452)*POWER_SCALE)
#define F_MULT         ((1.307-.452)*POWER_SCALE)

#define I_ADD_CLOCK    (.091*POWER_SCALE)
#define I_MULT_CLOCK   (.095*POWER_SCALE)
#define I_SHIFT_CLOCK  (.089*POWER_SCALE)
#define I_LOGIC_CLOCK  (.015*POWER_SCALE)
#define F_ADD_CLOCK    (.452*POWER_SCALE)
#define F_MULT_CLOCK   (.452*POWER_SCALE)

// define do power.cpp

#define MSCALE (LSCALE * .624 / .2250)

#endif



/* Used to pass values around the program */

/* typedef struct {
   int tech;
   int iw;
   int winsize;
   int nvreg;
   int npreg;
   int nvreg_width;
   int npreg_width;
   int data_width;
} parameter_type;
*/
typedef struct {
  double btb;
  double local_predict;
  double global_predict;
  double chooser;
  double ras;
  double rat_driver;
  double rat_decoder;
  double rat_wordline;
  double rat_bitline;
  double rat_senseamp;
  double dcl_compare;
  double dcl_pencode;
  double inst_decoder_power;
  double wakeup_tagdrive;
  double wakeup_tagmatch;
  double wakeup_ormatch;
  double lsq_wakeup_tagdrive;
  double lsq_wakeup_tagmatch;
  double lsq_wakeup_ormatch;
  double selection;
  double regfile_driver;
  double regfile_decoder;
  double regfile_wordline;
  double regfile_bitline;
  double regfile_senseamp;
  double reorder_driver;
  double reorder_decoder;
  double reorder_wordline;
  double reorder_bitline;
  double reorder_senseamp;
  double rs_driver;
  double rs_decoder;
  double rs_wordline;
  double rs_bitline;
  double rs_senseamp;
  double lsq_rs_driver;
  double lsq_rs_decoder;
  double lsq_rs_wordline;
  double lsq_rs_bitline;
  double lsq_rs_senseamp;
  double resultbus;

  double icache_decoder;
  double icache_wordline;
  double icache_bitline;
  double icache_senseamp;
  double icache_tagarray;

  double icache;

  double dcache_decoder;
  double dcache_wordline;
  double dcache_bitline;
  double dcache_senseamp;
  double dcache_tagarray;

  double dtlb;
  double itlb;

  double dcache2_decoder;
  double dcache2_wordline;
  double dcache2_bitline;
  double dcache2_senseamp;
  double dcache2_tagarray;

  double total_power;
  double total_power_nodcache2;
  double ialu_power;
  double falu_power;
  double bpred_power;
  double rename_power;
  double rat_power;
  double dcl_power;
  double window_power;
  double lsq_power;
  double wakeup_power;
  double lsq_wakeup_power;
  double rs_power;
  double rs_power_nobit;
  double lsq_rs_power;
  double lsq_rs_power_nobit;
  double selection_power;
  double regfile_power;
  double regfile_power_nobit;
  double result_power;
  double icache_power;
  double dcache_power;
  double dcache2_power;

  double clock_power;

} power_result_type;

/* Used to pass values around the program */

/* CACTI 4.1 */

/*typedef struct {
   int cache_size;
   int number_of_sets;
   int associativity;
   int block_size;
} time_parameter_type;

typedef struct {
   double access_time,cycle_time;
   int best_Ndwl,best_Ndbl;
   int best_Nspd;
   int best_Ntwl,best_Ntbl;
   int best_Ntspd;
   double decoder_delay_data,decoder_delay_tag;
   double dec_data_driver,dec_data_3to8,dec_data_inv;
   double dec_tag_driver,dec_tag_3to8,dec_tag_inv;
   double wordline_delay_data,wordline_delay_tag;
   double bitline_delay_data,bitline_delay_tag;
  double sense_amp_delay_data,sense_amp_delay_tag;
  double senseext_driver_delay_data;
   double compare_part_delay;
   double drive_mux_delay;
   double selb_delay;
   double data_output_delay;
   double drive_valid_delay;
   double precharge_delay;

} time_result_type;*/

typedef struct {
	double dynamic;
	double leakage;
} powerComponents;

typedef struct {
	powerComponents readOp;
	powerComponents writeOp;
} powerDef;


struct cache_params_t
{
  unsigned int nsets;
  unsigned int assoc;
  unsigned int dbits;
  unsigned int tbits;

  unsigned int nbanks;
  unsigned int rport;
  unsigned int wport;
  unsigned int rwport;
  unsigned int serport; // single-ended bitline read ports

  unsigned int obits;
  unsigned int abits;

  double dweight;
  double pweight;
  double aweight;
};

struct subarray_params_t
{
  unsigned int Ndwl;
  unsigned int Ndbl;
  unsigned int Nspd;
  unsigned int Ntwl;
  unsigned int Ntbl;
  unsigned int Ntspd;
  unsigned int muxover;
};


struct tech_params_t
{
  double tech_size;
  double crossover;
  double standby;
  double VddPow;
  double scaling_factor;
};


typedef struct {
	double height;
	double width;
	double scaled_area;
}area_type;

typedef struct {
        area_type dataarray_area,datapredecode_area;
        area_type datacolmuxpredecode_area,datacolmuxpostdecode_area;
		area_type datawritesig_area;
        area_type tagarray_area,tagpredecode_area;
        area_type tagcolmuxpredecode_area,tagcolmuxpostdecode_area;
        area_type tagoutdrvdecode_area;
        area_type tagoutdrvsig_area;
        double totalarea, subbankarea;
		double total_dataarea;
        double total_tagarea;
		double max_efficiency, efficiency;
		double max_aspect_ratio_total, aspect_ratio_total;
		double perc_data, perc_tag, perc_cont, sub_eff, total_eff;
}arearesult_type;


typedef struct {
   int cache_size;
   int number_of_sets;
   //int associativity;
   int tag_associativity, data_associativity;
   int block_size;
   int num_write_ports;
   int num_readwrite_ports;
   int num_read_ports;
   int num_single_ended_read_ports;
  char fully_assoc;
  double fudgefactor;
  double tech_size;
  double VddPow;
  int sequential_access;
  int fast_access;
  int force_tag;
  int tag_size;
  int nr_bits_out;
  int pure_sram;
} parameter_type;

typedef struct {
	int subbanks;
   double access_time,cycle_time;
  double senseext_scale;
  powerDef total_power;
   int best_Ndwl,best_Ndbl;
  double max_power, max_access_time;
   double best_Nspd;
   int best_Ntwl,best_Ntbl;
   int best_Ntspd;
   int best_muxover;
   powerDef total_routing_power;
   powerDef total_power_without_routing, total_power_allbanks;
   double subbank_address_routing_delay;
   powerDef subbank_address_routing_power;
   double decoder_delay_data,decoder_delay_tag;
   powerDef decoder_power_data,decoder_power_tag;
   double dec_data_driver,dec_data_3to8,dec_data_inv;
   double dec_tag_driver,dec_tag_3to8,dec_tag_inv;
   double wordline_delay_data,wordline_delay_tag;
   powerDef wordline_power_data,wordline_power_tag;
   double bitline_delay_data,bitline_delay_tag;
   powerDef bitline_power_data,bitline_power_tag;
  double sense_amp_delay_data,sense_amp_delay_tag;
  powerDef sense_amp_power_data,sense_amp_power_tag;
  double total_out_driver_delay_data;
  powerDef total_out_driver_power_data;
   double compare_part_delay;
   double drive_mux_delay;
   double selb_delay;
   powerDef compare_part_power, drive_mux_power, selb_power;
   double data_output_delay;
   powerDef data_output_power;
   double drive_valid_delay;
   powerDef drive_valid_power;
   double precharge_delay;
  int data_nor_inputs;
  int tag_nor_inputs;
} result_type;

typedef struct{
	result_type result;
	arearesult_type area;
	parameter_type params;
}total_result_type;

/* CACTI 4.0 */




extern "C" {

	double restowidth(double res,int nchannel);
	double logtwo(double x);
	double gatecap(double width,double wirelength);
	double gatecappass(double width,double wirelength);
	double draincap(double width,int nchannel,int stack);
	//void calculate_time(time_result_type*, time_parameter_type*);
	//void output_data(time_result_type*, time_parameter_type*);

	/* CACTI 4.0 */

	total_result_type cacti_interface(
			int cache_size,
			int line_size,
			int associativity,
			int rw_ports,
			int excl_read_ports,
			int excl_write_ports,
			int single_ended_read_ports,
			int banks,
			double tech_node,
			int output_width,
			int specific_tag,
			int tag_width,
			int access_mode,
			int pure_sram);

	void output_data(result_type *result,arearesult_type *arearesult, parameter_type *parameters);

	/* CACTI 4.0 */

	/* MSG: macros converted for attr
	/* -----------------------------------------------*/

	/* Power.c */
	/* Options */

	 double Sense2Powerfactor;
	 double LowSwingPowerfactor;

	/* Technology Length */
	 char *TECH_FACTOR;
	 double  MSCALE;

	double  GEN_POWER_FACTOR ;
	double CSCALE;	/* wire capacitance scaling factor */
	double RSCALE	;/* wire resistance scaling factor */
	double LSCALE		;	/* length (feature) scaling factor */
	double ASCALE  ;	/* area scaling factor */
	double VSCALE		;	/* voltage scaling factor */
	double VTSCALE	;		/* threshold voltage scaling factor */
	double SSCALE	;	/* sense voltage scaling factor */
	double TECH_LENGTH    ;         /* TECH LENGTH */
	double Mhz  ;
	double GEN_POWER_SCALE ;

	/*
	  * CMOS 0.8um model parameters
	  *   - from Appendix II of Cacti tech report
	  */

	/* corresponds to 8um of m3 @ 225ff/um */
	double Cwordmetal      ;

	/*double Corresponds to 16um of m2 @ 275ff/um */
	double Cbitmetal    ;

	/* corresponds to 1um of m2 @ 275ff/um */
	double Cmetal   ;

	double CM3metal  ;
	double CM2metal  ;

	/*  Cmetal 1.222e-15 */

	/* fF/um2 at 1.5V */
	double Cndiffarea ;		/* FIXME: ??? */

	/* fF/um2 at 1.5V */
	double Cpdiffarea ;		/* FIXME: ??? */

	/* fF/um at 1.5V */
	double Cndiffside ;		/* in general this does not scale */

	/* fF/um at 1.5V */
	double Cpdiffside ;		/* in general this does not scale */

	/* fF/um at 1.5V */
	double Cndiffovlp   ;		/* FIXME: by depth??? */

	/* fF/um at 1.5V */
	double Cpdiffovlp   ;		/* FIXME: by depth??? */

	/* fF/um assuming 25% Miller effect */
	double Cnoxideovlp ;		/* FIXME: by depth??? */

	/* fF/um assuming 25% Miller effect */
	double Cpoxideovlp ;		/* FIXME: by depth??? */

	/* um */
	double Leff         ;

	/* fF/um2 */
	double Cgate        ;		/* FIXME: ??? */

	/* fF/um2 */
	double Cgatepass    ;		/* FIXME: ??? */

	/* note that the value ofdouble Cgatepass will be different depending on
	   whether or not the source and drain are at different potentials or
	   the same potential.  The two values were averaged */

	/* fF/um */
	double Cpolywire	 ;

	/* ohms*um of channel width */
	double Rnchannelstatic   ;

	/* ohms*um of channel width */
	double Rpchannelstatic ;

	double Rnchannelon ;

	double Rpchannelon;

	/* corresponds to 16um of m2 @ 48mO/sq */
	double Rbitmetal ;

	/* corresponds to  8um of m3 @ 24mO/sq */
	double Rwordmetal;

	double Vdd   ;

	/* other stuff (from tech report, appendix 1) */


	double Period   ;

	double krise ;
	double tsensedata	 ;
	double tsensetag  ;
	double tfalldata;
	double tfalltag;
	double Vbitpre  ;
	double Vt	 ;
	double Vbitsense  ;

	double Powerfactor ;

	double SensePowerfactor3   ;
	double SensePowerfactor2  ;
	double SensePowerfactor ;

	double AF   ;
	double POPCOUNT_AF ;

	/* Threshold voltages (as a proportion of Vdd)
	   If you don't know them, set all values to 0.5 */

	double VSINV          ;
	double VTHINV100x60  ;   /* inverter with p00,n0 */
	double VTHNAND60x90   ;  /* nand with p=60 and three n=90 */
	double VTHNOR12x4x1    ;  /* nor with p=12, n=4, 1 input */
	double VTHNOR12x4x2    ; /* nor with p=12, n=4, 2 inputs */
	double VTHNOR12x4x3    ; /* nor with p=12, n=4, 3 inputs */
	double VTHNOR12x4x4    ; /* nor with p=12, n=4, 4 inputs */
	double VTHOUTDRINV    ;
	double VTHOUTDRNOR   ;
	double VTHOUTDRNAND  ;
	double VTHOUTDRIVE   ;
	double VTHCOMPINV    ;
	double VTHMUXDRV1    ;
	double VTHMUXDRV2    ;
	double VTHMUXDRV3    ;
	double VTHEVALINV    ;
	double VTHSENSEEXTDRV   ;

	/* transistor widths in um (as described in tech report, appendix 1) */
	double Wdecdrivep 	 ;
	double Wdecdriven 	 ;
	double Wdec3to8n 	 ;
	double Wdec3to8p 	 ;
	double WdecNORn 	 ;
	double WdecNORp 	 ;
	double Wdecinvn 	 ;
	double Wdecinvp	 ;

	double Wworddrivemax 	 ;
	double Wmemcella 	 ;
	double Wmemcellr 	 ;
	double Wmemcellw 	 ;
	double Wmemcellbscale 	;		/* means 2x bigger than Wmemcella */
	double Wbitpreequ 	 ;
	double Wbitmuxn 	 ;
	double WsenseQ1to4 	 ;
	double Wcompinvp1  	 ;
	double Wcompinvn1  	 ;
	double Wcompinvp2  	 ;
	double Wcompinvn2  	 ;
	double Wcompinvp3  	 ;
	double Wcompinvn3  	 ;
	double Wevalinvp  	 ;
	double Wevalinvn  	 ;

	double Wcompn	  	 ;
	double Wcompp	  	 ;
	double Wcomppreequ    ;
	double Wmuxdrv12n  	 ;
	double Wmuxdrv12p  	 ;
	double WmuxdrvNANDn   ;
	double WmuxdrvNANDp   ;
	double WmuxdrvNORn  	 ;
	double WmuxdrvNORp 	 ;
	double Wmuxdrv3n    ;
	double Wmuxdrv3p    ;
	double Woutdrvseln    ;
	double Woutdrvselp    ;
	double Woutdrvnandn    ;
	double Woutdrvnandp    ;
	double Woutdrvnorn    ;
	double Woutdrvnorp    ;
	double Woutdrivern    ;
	double Woutdriverp     ;

	double Wcompcellpd2    ;
	double Wcompdrivern    ;
	double Wcompdriverp    ;
	double Wcomparen2      ;
	double Wcomparen1      ;
	double Wmatchpchg      ;
	double Wmatchinvn      ;
	double Wmatchinvp      ;
	double Wmatchnandn     ;
	double Wmatchnandp     ;
	double Wmatchnorn     ;
	double Wmatchnorp     ;
	double WSelORn         ;
	double WSelORprequ     ;
	double WSelPn          ;
	double WSelPp          ;
	double WSelEnn         ;
	double WSelEnp         ;
	double Wsenseextdrv1p    ;
	double Wsenseextdrv1n    ;
	double Wsenseextdrv2p    ;
	double Wsenseextdrv2n    ;


	/* bit width of RAM cell in um */
	double BitWidth     ;

	/* bit height of RAM cell in um */
	double BitHeight     ;

	double Cout	     ;

	/* Sizing of cells and spacings */
	double RatCellHeight    ;
	double RatCellWidth     ;
	double RatShiftRegWidth  ;
	double RatNumShift      ;
	double BitlineSpacing   ;
	double WordlineSpacing   ;

	double RegCellHeight    ;
	double RegCellWidth     ;

	double CamCellHeight    ;
	double CamCellWidth     ;
	double MatchlineSpacing  ;
	double TaglineSpacing   ;

	/*=================================================================*/

	/* ALU POWER NUMBERS for .18um 733Mhz */
	/* normalize to cap from W */
	double NORMALIZE_SCALE    ;
	/* normalize .18um cap to other gen's cap, then xPowerfactor */
	double  POWER_SCALE   ;
	double I_ADD         ;
	double I_ADD32      ;
	double I_MULT16      ;
	double I_SHIFT      ;
	double I_LOGIC     ;
	double F_ADD     ;
	double F_MULT    ;

	double I_ADD_CLOCK   ;
	double I_MULT_CLOCK   ;
	double I_SHIFT_CLOCK ;
	double I_LOGIC_CLOCK   ;
	double F_ADD_CLOCK    ;
	double F_MULT_CLOCK   ;


}

class sim;

class power
{
	private:

		 sim *Sim;

		 double rename_power;
		 double bpred_power;
		 double window_power;
		 double lsq_power;
		 double regfile_power;
		 double icache_power;
		 double dcache_power;
		 double dcache2_power;
		 double alu_power;
		 double falu_power;
		 double resultbus_power;
		 double clock_power;

		 double rename_power_cc1;
		 double bpred_power_cc1;
		 double window_power_cc1;
		 double lsq_power_cc1;
		 double regfile_power_cc1;
		 double icache_power_cc1;
		 double dcache_power_cc1;
		 double dcache2_power_cc1;
		 double alu_power_cc1;
		 double resultbus_power_cc1;
		 double clock_power_cc1;

		 double rename_power_cc2;
		 double bpred_power_cc2;
		 double window_power_cc2;
		 double lsq_power_cc2;
		 double regfile_power_cc2;
		 double icache_power_cc2;
		 double dcache_power_cc2;
		 double dcache2_power_cc2;
		 double alu_power_cc2;
		 double resultbus_power_cc2;
		 double clock_power_cc2;

		 double rename_power_cc3;
		 double bpred_power_cc3;
		 double window_power_cc3;
		 double lsq_power_cc3;
		 double regfile_power_cc3;
		 double icache_power_cc3;
		 double dcache_power_cc3;
		 double dcache2_power_cc3;
		 double alu_power_cc3;
		 double resultbus_power_cc3;
		 double clock_power_cc3;

		 double total_cycle_power;
		 double total_cycle_power_cc1;
		 double total_cycle_power_cc2;
		 double total_cycle_power_cc3;

		 double last_single_total_cycle_power_cc1;
		 double last_single_total_cycle_power_cc2;
		 double last_single_total_cycle_power_cc3;
		 double current_total_cycle_power_cc1;
		 double current_total_cycle_power_cc2;
		 double current_total_cycle_power_cc3;

		 double max_cycle_power_cc1;
		 double max_cycle_power_cc2;
		 double max_cycle_power_cc3;


		 counter_t total_rename_access;
		 counter_t total_bpred_access;
		 counter_t total_window_access;
		 counter_t total_lsq_access;
		 counter_t total_regfile_access;
		 counter_t total_icache_access;
		 counter_t total_dcache_access;
		 counter_t total_dcache2_access;
		 counter_t total_alu_access;
		 counter_t total_resultbus_access;

		 counter_t max_rename_access;
		 counter_t max_bpred_access;
		 counter_t max_window_access;
		 counter_t max_lsq_access;
		 counter_t max_regfile_access;
		 counter_t max_icache_access;
		 counter_t max_dcache_access;
		 counter_t max_dcache2_access;
		 counter_t max_alu_access;
		 counter_t max_resultbus_access;




		//------------ functions -------------
		double compute_af(counter_t num_pop_count_cycle,counter_t total_pop_count_cycle,int pop_width);
		int squarify(int rows, int cols);
		double driver_size(double driving_cap, double desiredrisetime);

		void dump_power_stats();
		double array_decoder_power(int rows, int cols, double predeclength, int rports, int wports, int cache);
		double array_wordline_power(int rows,int cols, double wordlinelength,int rports,int wports, int cache);
		double array_bitline_power(int rows, int cols, double bitlinelength, int rports,int wports, int cache);
		double senseamp_power(int cols);
		double compare_cap(int compare_bits);
		double dcl_compare_power(int compare_bits);
		double cam_tagdrive(int rows,int cols,int rports,int wports);
		double cam_tagmatch(int rows,int cols,int rports,int wports);
		double cam_array   (int rows,int cols,int rports,int wports);
		double selection_power(int win_entries);
		double total_clockpower(double die_length);
		double global_clockpower(double die_length);
		double compute_resultbus_power();


	public:

		power(sim*);

		/* set scale for crossover (vdd->gnd) currents */
		double crossover_scaling;
		/* set non-ideal turnoff percentage */
		double turnoff_factor;

		/* static power model results */
		power_result_type *power0;

		int opcode_length;
		int inst_length;
		int nvreg_width;
		int npreg_width;
		double global_clockcap;

		//double logtwo(double x);
		//double gatecap(double width,double wirelength);
		//double gatecappass(double width,double wirelength);
		//double draincap(double width,int nchannel,int stack);
		double simple_array_power(int rows,int cols,int rports,int wports,int cache);
		double simple_array_decoder_power(int rows,int cols,int rports,int wports,int cache);
		double simple_array_bitline_power(int rows,int cols,int rports,int wports,int cache);
		double simple_array_wordline_power(int rows,int cols,int rports,int wports,int cache);
		double squarify_new(int rows,int cols);
		void clear_access_stats();
		void update_power_stats();
		/* register power stats */
		void power_reg_stats(struct stat_sdb_t *sdb);/* stats database */
		//void calculate_time(time_result_type*, time_parameter_type*);
		//void output_data(time_result_type*, time_parameter_type*);
		void calculate_power();
		int pop_count(quad_t bits);
		int pop_count_slow(quad_t bits);


#if 0
		double logtwo(double x);
		double gatecap(double width,double wirelength);
		double gatecappass(double width,double wirelength);
		double draincap(double width,int nchannel,int stack);
		double restowidth(double res,int nchannel);
		double simple_array_power(int rows,int cols,int rports,int wports,int cache);
		double simple_array_decoder_power(int rows,int cols,int rports,int wports,int cache);
		double simple_array_bitline_power(int rows,int cols,int rports,int wports,int cache);
		double simple_array_wordline_power(int rows,int cols,int rports,int wports,int cache);
		double squarify_new(int rows,int cols);
		void clear_access_stats();
		void update_power_stats();
		/* register power stats */
		void power_reg_stats(struct stat_sdb_t *sdb);/* stats database */
		void calculate_time(time_result_type*, time_parameter_type*);
		void output_time_components(int A,time_result_type* result);
		void output_data(time_result_type*, time_parameter_type*);
		void calculate_power();
		int pop_count(quad_t bits);
		int pop_count_slow(quad_t bits);

		double transresswitch(double width,int nchannel,int stack);
		double transreson(double width,int nchannel,int stack);
		double horowitz(double inputramptime,double tf,double vs1,double vs2,int rise);
		double decoder_delay(int C, int B, int A, int Ndwl, int  Ndbl, int Nspd, int Ntwl,int  Ntbl, int Ntspd, double *Tdecdrive,
             				double *Tdecoder1, double *Tdecoder2,double *outrisetime);
             	double decoder_tag_delay(int C, int B, int A, int Ndwl, int  Ndbl, int Nspd, int Ntwl,int  Ntbl, int Ntspd, double *Tdecdrive,
             				double *Tdecoder1, double *Tdecoder2,double *outrisetime);

		double wordline_delay(int B,int A,int Ndwl,int Nspd,double inrisetime,double *outrisetime);
		double wordline_tag_delay(int C,int A,int Ntspd,int Ntwl,double inrisetime,double *outrisetime);
		double bitline_delay(int C,int A,int B,int Ndwl,int Ndbl,int Nspd,double inrisetime,double *outrisetime);
		double bitline_tag_delay(int C,int A,int B,int Ndwl,int Ndbl,int Nspd,double inrisetime,double *outrisetime);
		double sense_amp_delay(double inrisetime,double *outrisetime);
		double sense_amp_tag_delay(double inrisetime,double *outrisetime);
		double mux_driver_delay(int C,int B,int A,int Ndbl,int Nspd,int Ndwl,int Ntbl,int Ntspd,double inputtime,double *outputtime);
		double valid_driver_delay(int C,int A,int Ntbl,int Ntspd,double inputtime);
		double dataoutput_delay(int C,int B,int A,int Ndbl,int Nspd,int Ndwl,
		       		       double inrisetime,double *outrisetime);
		double selb_delay_tag_path(double inrisetime,double *outrisetime);
		double precharge_delay(double worddata);
		int organizational_parameters_valid(int rows,int cols,int Ndwl,int Ndbl,int Nspd,int Ntwl,int Ntbl,int Ntspd);

		double compare_time(int C,int A,int Ntbl,int Ntspd,double inputtime,double *outputtime);
#endif

};


#endif

