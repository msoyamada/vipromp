/*
 * cjpeg.c
 *
 * Copyright (C) 1991-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains a command-line user interface for the JPEG compressor.
 * It should work on any system with Unix- or MS-DOS-style command lines.
 *
 * Two different command line styles are permitted, depending on the
 * compile-time switch TWO_FILE_COMMANDLINE:
 *	cjpeg [options]  inputfile outputfile
 *	cjpeg [options]  [inputfile]
 * In the second style, output is always to standard output, which you'd
 * normally redirect to a file or pipe to some other program.  Input is
 * either from a named file or from standard input (typically redirected).
 * The second style is convenient on Unix but is unhelpful on systems that
 * don't support pipes.  Also, you MUST use the first style if your system
 * doesn't do binary I/O to stdin/stdout.
 * To simplify script writing, the "-outfile" switch is provided.  The syntax
 *	cjpeg [options]  -outfile outputfile  inputfile
 * works regardless of which command line style is used.
 */

#include "cdjpeg.h"		/* Common decls for cjpeg/djpeg applications */
#include "jversion.h"		/* for version message */

#ifdef USE_CCOMMAND		/* command-line reader for Macintosh */
#ifdef __MWERKS__
#include <SIOUX.h>              /* Metrowerks needs this */
#include <console.h>		/* ... and this */
#endif
#ifdef THINK_C
#include <console.h>		/* Think declares it here */
#endif
#endif


/* Create the add-on message string table. */

#define JMESSAGE(code,string)	string ,

static const char * const cdjpeg_message_table[] = {
#include "cderror.h"
  NULL
};


/*
 * This routine determines what format the input file is,
 * and selects the appropriate input-reading module.
 *
 * To determine which family of input formats the file belongs to,
 * we may look only at the first byte of the file, since C does not
 * guarantee that more than one character can be pushed back with ungetc.
 * Looking at additional bytes would require one of these approaches:
 *     1) assume we can fseek() the input file (fails for piped input);
 *     2) assume we can push back more than one character (works in
 *        some C implementations, but unportable);
 *     3) provide our own buffering (breaks input readers that want to use
 *        stdio directly, such as the RLE library);
 * or  4) don't put back the data, and modify the input_init methods to assume
 *        they start reading after the start of file (also breaks RLE library).
 * #1 is attractive for MS-DOS but is untenable on Unix.
 *
 * The most portable solution for file types that can't be identified by their
 * first byte is to make the user tell us what they are.  This is also the
 * only app	roach for "raw" file types that contain only arbitrary values.
 * We presently apply this method for Targa files.  Most of the time Targa
 * files start with 0x00, so we recognize that case.  Potentially, however,
 * a Targa file could start with any byte value (byte 0 is the length of the
 * seldom-used ID field), so we provide a switch to force Targa input mode.
 */

static boolean is_targa;	/* records user -targa switch */

/* MSO: PID defines which processor I'm running. Starts in 1 */
int PID = 6;
/* MSG: flags in shared memory */
char *flags  =0x80000000;
/* MSG: number of processors instantiated */
int *num_processors=0x80000030;

// MSO: this variable is defined in the rdppm.c
extern int image_pos;

/* MSK: pointer for IRQ_Routine */
unsigned char *pIRQ;
/* MSK: interrup cont */
volatile int int_cont = 0;
/* MSK: Proc ID*/
int ID = 7;
/* MSK: the buffer size for jpeg_write*/
int buffer_size = 128;//from 1 to 128

/*MSK: the global interrup function */
void __it_handle(){
 	int_cont++;
	//printf("IT_HANDLE_7!!! ");
	fflush(stdout);

}

// Copy the IRQ_Routine to the IRQ address = 0x00300000
void init_IT() { 
   unsigned char *p=0x00300000;
 
   asm(" la  $27, _IRQ_Routine" );
   asm(" sw  $27, pIRQ " );

   memcpy(p, pIRQ, 1024); 

}

LOCAL(cjpeg_source_ptr)
select_file_type (j_compress_ptr cinfo, FILE * infile)
{
  int c;

  if (is_targa) {
#ifdef TARGA_SUPPORTED
    return jinit_read_targa(cinfo);
#else
    ERREXIT(cinfo, JERR_TGA_NOTCOMP);
#endif
  }

  if ((c = getc(infile)) == EOF)
    ERREXIT(cinfo, JERR_INPUT_EMPTY);
  if (ungetc(c, infile) == EOF)
    ERREXIT(cinfo, JERR_UNGETC_FAILED);

  switch (c) {
#ifdef BMP_SUPPORTED
  case 'B':
    return jinit_read_bmp(cinfo);
#endif
#ifdef GIF_SUPPORTED
  case 'G':
    return jinit_read_gif(cinfo);
#endif
#ifdef PPM_SUPPORTED
  case 'P':
    return jinit_read_ppm(cinfo);
#endif
#ifdef RLE_SUPPORTED
  case 'R':
    return jinit_read_rle(cinfo);
#endif
#ifdef TARGA_SUPPORTED
  case 0x00:
    return jinit_read_targa(cinfo);
#endif
  default:
    ERREXIT(cinfo, JERR_UNKNOWN_FORMAT);
    break;
  }

  return NULL;			/* suppress compiler warnings */
}


/*
 * Argument-parsing code.
 * The switch parser is designed to be useful with DOS-style command line
 * syntax, ie, intermixed switches and file names, where only the switches
 * to the left of a given file name affect processing of that file.
 * The main program in this file doesn't actually use this capability...
 */


static const char * progname;	/* program name for error messages */
static char * outfilename;	/* for -outfile switch */


LOCAL(void)
usage (void)
/* complain about bad command line */
{
  fprintf(stderr, "usage: %s [switches] ", progname);
#ifdef TWO_FILE_COMMANDLINE
  fprintf(stderr, "inputfile outputfile\n");
#else
  fprintf(stderr, "[inputfile]\n");
#endif

  fprintf(stderr, "Switches (names may be abbreviated):\n");
  fprintf(stderr, "  -quality N     Compression quality (0..100; 5-95 is useful range)\n");
  fprintf(stderr, "  -grayscale     Create monochrome JPEG file\n");
#ifdef ENTROPY_OPT_SUPPORTED
  fprintf(stderr, "  -optimize      Optimize Huffman table (smaller file, but slow compression)\n");
#endif
#ifdef C_PROGRESSIVE_SUPPORTED
  fprintf(stderr, "  -progressive   Create progressive JPEG file\n");
#endif
#ifdef TARGA_SUPPORTED
  fprintf(stderr, "  -targa         Input file is Targa format (usually not needed)\n");
#endif
  fprintf(stderr, "Switches for advanced users:\n");
#ifdef DCT_ISLOW_SUPPORTED
  fprintf(stderr, "  -dct int       Use integer DCT method%s\n",
	  (JDCT_DEFAULT == JDCT_ISLOW ? " (default)" : ""));
#endif
#ifdef DCT_IFAST_SUPPORTED
  fprintf(stderr, "  -dct fast      Use fast integer DCT (less accurate)%s\n",
	  (JDCT_DEFAULT == JDCT_IFAST ? " (default)" : ""));
#endif
#ifdef DCT_FLOAT_SUPPORTED
  fprintf(stderr, "  -dct float     Use floating-point DCT method%s\n",
	  (JDCT_DEFAULT == JDCT_FLOAT ? " (default)" : ""));
#endif
  fprintf(stderr, "  -restart N     Set restart interval in rows, or in blocks with B\n");
#ifdef INPUT_SMOOTHING_SUPPORTED
  fprintf(stderr, "  -smooth N      Smooth dithered input (N=1..100 is strength)\n");
#endif
  fprintf(stderr, "  -maxmemory N   Maximum memory to use (in kbytes)\n");
  fprintf(stderr, "  -outfile name  Specify name for output file\n");
  fprintf(stderr, "  -verbose  or  -debug   Emit debug output\n");
  fprintf(stderr, "Switches for wizards:\n");
#ifdef C_ARITH_CODING_SUPPORTED
  fprintf(stderr, "  -arithmetic    Use arithmetic coding\n");
#endif
  fprintf(stderr, "  -baseline      Force baseline output\n");
  fprintf(stderr, "  -qtables file  Use quantization tables given in file\n");
  fprintf(stderr, "  -qslots N[,...]    Set component quantization tables\n");
  fprintf(stderr, "  -sample HxV[,...]  Set component sampling factors\n");
#ifdef C_MULTISCAN_FILES_SUPPORTED
  fprintf(stderr, "  -scans file    Create multi-scan JPEG per script file\n");
#endif
  exit(EXIT_FAILURE);
}


LOCAL(int)
parse_switches (j_compress_ptr cinfo, int argc, char **argv,
		int last_file_arg_seen, boolean for_real)
/* Parse optional switches.
 * Returns argv[] index of first file-name argument (== argc if none).
 * Any file names with indexes <= last_file_arg_seen are ignored;
 * they have presumably been processed in a previous iteration.
 * (Pass 0 for last_file_arg_seen on the first or only iteration.)
 * for_real is FALSE on the first (dummy) pass; we may skip any expensive
 * processing.
 */
{
  int argn;
  char * arg;
  int quality;			/* -quality parameter */
  int q_scale_factor;		/* scaling percentage for -qtables */
  boolean force_baseline;
  boolean simple_progressive;
  char * qtablefile = NULL;	/* saves -qtables filename if any */
  char * qslotsarg = NULL;	/* saves -qslots parm if any */
  char * samplearg = NULL;	/* saves -sample parm if any */
  char * scansarg = NULL;	/* saves -scans parm if any */

  /* Set up default JPEG parameters. */
  /* Note that default -quality level need not, and does not,
   * match the default scaling for an explicit -qtables argument.
   */
  quality = 75;			/* default -quality value */
  q_scale_factor = 100;		/* default to no scaling for -qtables */
  force_baseline = FALSE;	/* by default, allow 16-bit quantizers */
  simple_progressive = FALSE;
  is_targa = FALSE;
  outfilename = NULL;
  cinfo->err->trace_level = 0;

  /* Scan command line options, adjust parameters */

  for (argn = 1; argn < argc; argn++) {
    arg = argv[argn];
    if (*arg != '-') {
      /* Not a switch, must be a file name argument */
      if (argn <= last_file_arg_seen) {
	outfilename = NULL;	/* -outfile applies to just one input file */
	continue;		/* ignore this name if previously processed */
      }
      break;			/* else done parsing switches */
    }
    arg++;			/* advance past switch marker character */

    if (keymatch(arg, "arithmetic", 1)) {
      /* Use arithmetic coding. */
#ifdef C_ARITH_CODING_SUPPORTED
      cinfo->arith_code = TRUE;
#else
      fprintf(stderr, "%s: sorry, arithmetic coding not supported\n",
	      progname);
      exit(EXIT_FAILURE);
#endif

    } else if (keymatch(arg, "baseline", 1)) {
      /* Force baseline output (8-bit quantizer values). */
      force_baseline = TRUE;

    } else if (keymatch(arg, "dct", 2)) {
      /* Select DCT algorithm. */
      if (++argn >= argc)	/* advance to next argument */
	usage();
      if (keymatch(argv[argn], "int", 1)) {
	cinfo->dct_method = JDCT_ISLOW;
      } else if (keymatch(argv[argn], "fast", 2)) {
	cinfo->dct_method = JDCT_IFAST;
      } else if (keymatch(argv[argn], "float", 2)) {
	cinfo->dct_method = JDCT_FLOAT;
      } else
	usage();

    } else if (keymatch(arg, "debug", 1) || keymatch(arg, "verbose", 1)) {
      /* Enable debug printouts. */
      /* On first -d, print version identification */
      static boolean printed_version = FALSE;

      if (! printed_version) {
	fprintf(stderr, "Independent JPEG Group's CJPEG, version %s\n%s\n",
		JVERSION, JCOPYRIGHT);
	printed_version = TRUE;
      }
      cinfo->err->trace_level++;

    } else if (keymatch(arg, "grayscale", 2) || keymatch(arg, "greyscale",2)) {
      /* Force a monochrome JPEG file to be generated. */
      jpeg_set_colorspace(cinfo, JCS_GRAYSCALE);

    } else if (keymatch(arg, "maxmemory", 3)) {
      /* Maximum memory in Kb (or Mb with 'm'). */
      long lval;
      char ch = 'x';

      if (++argn >= argc)	/* advance to next argument */
	usage();
      if (sscanf(argv[argn], "%ld%c", &lval, &ch) < 1)
	usage();
      if (ch == 'm' || ch == 'M')
	lval *= 1000L;
      cinfo->mem->max_memory_to_use = lval * 1000L;

    } else if (keymatch(arg, "optimize", 1) || keymatch(arg, "optimise", 1)) {
      /* Enable entropy parm optimization. */
#ifdef ENTROPY_OPT_SUPPORTED
      cinfo->optimize_coding = TRUE;
#else
      fprintf(stderr, "%s: sorry, entropy optimization was not compiled\n",
	      progname);
      exit(EXIT_FAILURE);
#endif

    } else if (keymatch(arg, "outfile", 4)) {
      /* Set output file name. */
      if (++argn >= argc)	/* advance to next argument */
	usage();
      outfilename = argv[argn];	/* save it away for later use */

    } else if (keymatch(arg, "progressive", 1)) {
      /* Select simple progressive mode. */
#ifdef C_PROGRESSIVE_SUPPORTED
      simple_progressive = TRUE;
      /* We must postpone execution until num_components is known. */
#else
      fprintf(stderr, "%s: sorry, progressive output was not compiled\n",
	      progname);
      exit(EXIT_FAILURE);
#endif

    } else if (keymatch(arg, "quality", 1)) {
      /* Quality factor (quantization table scaling factor). */
      if (++argn >= argc)	/* advance to next argument */
	usage();
      if (sscanf(argv[argn], "%d", &quality) != 1)
	usage();
      /* Change scale factor in case -qtables is present. */
      q_scale_factor = jpeg_quality_scaling(quality);

    } else if (keymatch(arg, "qslots", 2)) {
      /* Quantization table slot numbers. */
      if (++argn >= argc)	/* advance to next argument */
	usage();
      qslotsarg = argv[argn];
      /* Must delay setting qslots until after we have processed any
       * colorspace-determining switches, since jpeg_set_colorspace sets
       * default quant table numbers.
       */

    } else if (keymatch(arg, "qtables", 2)) {
      /* Quantization tables fetched from file. */
      if (++argn >= argc)	/* advance to next argument */
	usage();
      qtablefile = argv[argn];
      /* We postpone actually reading the file in case -quality comes later. */

    } else if (keymatch(arg, "restart", 1)) {
      /* Restart interval in MCU rows (or in MCUs with 'b'). */
      long lval;
      char ch = 'x';

      if (++argn >= argc)	/* advance to next argument */
	usage();
      if (sscanf(argv[argn], "%ld%c", &lval, &ch) < 1)
	usage();
      if (lval < 0 || lval > 65535L)
	usage();
      if (ch == 'b' || ch == 'B') {
	cinfo->restart_interval = (unsigned int) lval;
	cinfo->restart_in_rows = 0; /* else prior '-restart n' overrides me */
      } else {
	cinfo->restart_in_rows = (int) lval;
	/* restart_interval will be computed during startup */
      }

    } else if (keymatch(arg, "sample", 2)) {
      /* Set sampling factors. */
      if (++argn >= argc)	/* advance to next argument */
	usage();
      samplearg = argv[argn];
      /* Must delay setting sample factors until after we have processed any
       * colorspace-determining switches, since jpeg_set_colorspace sets
       * default sampling factors.
       */

    } else if (keymatch(arg, "scans", 2)) {
      /* Set scan script. */
#ifdef C_MULTISCAN_FILES_SUPPORTED
      if (++argn >= argc)	/* advance to next argument */
	usage();
      scansarg = argv[argn];
      /* We must postpone reading the file in case -progressive appears. */
#else
      fprintf(stderr, "%s: sorry, multi-scan output was not compiled\n",
	      progname);
      exit(EXIT_FAILURE);
#endif

    } else if (keymatch(arg, "smooth", 2)) {
      /* Set input smoothing factor. */
      int val;

      if (++argn >= argc)	/* advance to next argument */
	usage();
      if (sscanf(argv[argn], "%d", &val) != 1)
	usage();
      if (val < 0 || val > 100)
	usage();
      cinfo->smoothing_factor = val;

    } else if (keymatch(arg, "targa", 1)) {
      /* Input file is Targa format. */
      is_targa = TRUE;

    } else {
      usage();			/* bogus switch */
    }
  }

  /* Post-switch-scanning cleanup */

  if (for_real) {

    /* Set quantization tables for selected quality. */
    /* Some or all may be overridden if -qtables is present. */
    jpeg_set_quality(cinfo, quality, force_baseline);

    if (qtablefile != NULL)	/* process -qtables if it was present */
      if (! read_quant_tables(cinfo, qtablefile,
			      q_scale_factor, force_baseline))
	usage();

    if (qslotsarg != NULL)	/* process -qslots if it was present */
      if (! set_quant_slots(cinfo, qslotsarg))
	usage();

    if (samplearg != NULL)	/* process -sample if it was present */
      if (! set_sample_factors(cinfo, samplearg))
	usage();

#ifdef C_PROGRESSIVE_SUPPORTED
    if (simple_progressive)	/* process -progressive; -scans can override */
      jpeg_simple_progression(cinfo);
#endif

#ifdef C_MULTISCAN_FILES_SUPPORTED
    if (scansarg != NULL)	/* process -scans if it was present */
      if (! read_scan_script(cinfo, scansarg))
	usage();
#endif
  }

  return argn;			/* return index of next arg (file name) */
}

#ifdef HAVE_UNSIGNED_CHAR
typedef unsigned char U_CHAR;
#define UCH(x)	((int) (x))
#else // !HAVE_UNSIGNED_CHAR 
#ifdef CHAR_IS_UNSIGNED
typedef char U_CHAR;
#define UCH(x)	((int) (x))
#else
typedef char U_CHAR;
#define UCH(x)	((int) (x) & 0xFF)
#endif
#endif // HAVE_UNSIGNED_CHAR 


typedef struct {
  struct cjpeg_source_struct pub; /* public fields */
  U_CHAR *iobuffer;		/* non-FAR pointer to I/O buffer */
  JSAMPROW pixrow;		/* FAR pointer to same */
  size_t buffer_width;		/* width of I/O buffer */
  JSAMPLE *rescale;		/* => maxval-remapping array, or NULL */
} ppm_source_struct;

typedef ppm_source_struct * ppm_source_ptr;


/*
 * The main program.
 */

int
main (int argc, char **argv)
{
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
#ifdef PROGRESS_REPORT
  struct cdjpeg_progress_mgr progress;
#endif
  int file_index;
  cjpeg_source_ptr src_mgr;
  FILE * input_file;
  FILE * output_file;
  JDIMENSION num_scanlines;
  // MSK
  int image_buffer_width;
  int image_size;	
  U_CHAR* image;
  U_CHAR* v_image_iobuffer1;
  U_CHAR* v_image_iobuffer2;
  int *dma_location;
  int cont_interrupt;
  ppm_source_ptr source;
  // MSO
  int i, cont, aux, cont_aux, ping_pong;

/*
  char *arg0= "cjpeg";
  char *arg1= "-dct";
  char *arg2= "int";
//  char *arg3= "-opt";
  char *arg3= "-outfile";
  char *arg4= "output_small.jpeg";
  char *arg5= "input_small.ppm";
*/
  
  /* On Mac, fetch a command line. */
#ifdef USE_CCOMMAND
  argc = ccommand(&argv);
#endif
   /*MSO: putting the parameters by hand
    -dct int -progressive -opt -outfile output_small_encode.jpeg input_small.ppm
  */
/*
  argc= 6;
  argv[0]= arg0;
  argv[1]= arg1;
  argv[2]= arg2;
  argv[3]= arg3;
  argv[4]= arg4;
  argv[5]= arg5;
*/
    progname = argv[0];
  if (progname == NULL || progname[0] == 0)
    progname = "cjpeg";		/* in case C library doesn't provide it */

  /* Initialize the JPEG compression object with default error handling. */
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  /* Add some application-specific error messages (from cderror.h) */
  jerr.addon_message_table = cdjpeg_message_table;
  jerr.first_addon_message = JMSG_FIRSTADDONCODE;
  jerr.last_addon_message = JMSG_LASTADDONCODE;

  /* Now safe to enable signal catcher. */
#ifdef NEED_SIGNAL_CATCHER
  enable_signal_catcher((j_common_ptr) &cinfo);
#endif

  /* Initialize JPEG parameters.
   * Much of this may be overridden later.
   * In particular, we don't yet know the input file's color space,
   * but we need to provide some value for jpeg_set_defaults() to work.
   */

  cinfo.in_color_space = JCS_RGB; /* arbitrary guess */
  jpeg_set_defaults(&cinfo);

  /* Scan command line to find file names.
   * It is convenient to use just one switch-parsing routine, but the switch
   * values read here are ignored; we will rescan the switches after opening
   * the input file.
   */

  file_index = parse_switches(&cinfo, argc, argv, 0, FALSE);

#ifdef TWO_FILE_COMMANDLINE
  /* Must have either -outfile switch or explicit output file name */
  if (outfilename == NULL) {
    if (file_index != argc-2) {
      fprintf(stderr, "%s: must name one input and one output file\n",
	      progname);
      usage();
    }
    outfilename = argv[file_index+1];
  } else {
    if (file_index != argc-1) {
      fprintf(stderr, "%s: must name one input and one output file\n",
	      progname);
      usage();
    }
  }
#else
  /* Unix style: expect zero or one file name */
  if (file_index < argc-1) {
    fprintf(stderr, "%s: only one input file\n", progname);
    usage();
  }
#endif /* TWO_FILE_COMMANDLINE */

  /* Open the input file. */
  if (file_index < argc) {
    if ((input_file = fopen(argv[file_index], READ_BINARY)) == NULL) {
      fprintf(stderr, "%s: can't open %s\n", progname, argv[file_index]);
      exit(EXIT_FAILURE);
    }
  } else {
    /* default input file is stdin */
    input_file = read_stdin();
  }

  /* Open the output file. */
  if (outfilename != NULL) {
    if ((output_file = fopen(outfilename, WRITE_BINARY)) == NULL) {
      fprintf(stderr, "%s: can't open %s\n", progname, outfilename);
      exit(EXIT_FAILURE);
    }
  } else {
    /* default output file is stdout */
    output_file = write_stdout();
  }

#ifdef PROGRESS_REPORT
  start_progress_monitor((j_common_ptr) &cinfo, &progress);
#endif

  /* Figure out the input file format, and set up to read it. */
  src_mgr = select_file_type(&cinfo, input_file);
  src_mgr->input_file = input_file;

  /* Read the input file header to obtain file size & colorspace. */
  (*src_mgr->start_input) (&cinfo, src_mgr);

  /* Now that we know input colorspace, fix colorspace-dependent defaults */
  jpeg_default_colorspace(&cinfo);

  /* Adjust default compression parameters by re-parsing the options */
  file_index = parse_switches(&cinfo, argc, argv, 0, TRUE);

  /* Specify data destination for compression */
  jpeg_stdio_dest(&cinfo, output_file);

  /* Start compressor */
  jpeg_start_compress(&cinfo, TRUE);

  /* Process data */
  /* MSO: Process just a half of image.
     Pay attention, it need to be a multiple of MCU size! So, we have to rewrite it!! */
  /* Starts in the half end of the image */

  printf("P6: wainting for load image in shared memory\n");
  fflush(stdout);

  while(flags[0] == 0)
	  {
//	  printf("input_done P2: %d", *input_done);
	  fflush(stdout);
	  }

  printf("P6: started encode\n");
  fflush(stdout);

  /* MSK: link the data image in shared memory*/
  image_buffer_width = get_buffer_width(src_mgr);
  image = get_local_image();
  image_size = get_image_size();
  aux = 0;

  init_IT();

  switch(*num_processors)
  {
  case 8:
	  /*cinfo.next_scanline = (cinfo.image_height*3)/4;
	  image_pos = cinfo.image_width* cinfo.num_components*cinfo.next_scanline;
	  while (cinfo.next_scanline < (cinfo.image_height*7)/8) {
		  num_scanlines = (*src_mgr->get_pixel_rows) (&cinfo, src_mgr);
		  (void) jpeg_write_scanlines(&cinfo, src_mgr->buffer, num_scanlines);
	  }*/
	  cinfo.next_scanline = (cinfo.image_height*3)/4;
	  aux = cinfo.image_width* cinfo.num_components*cinfo.next_scanline;
	  ping_pong = 0;
	  source = (ppm_source_ptr) src_mgr;
	  v_image_iobuffer1 = (U_CHAR *)calloc(sizeof(U_CHAR*)*source->buffer_width*buffer_size,1);
	  v_image_iobuffer2 = (U_CHAR *)calloc(sizeof(U_CHAR*)*source->buffer_width*buffer_size,1);
	  for(i =0;i<source->buffer_width*buffer_size;i++){
	  	v_image_iobuffer1[i]=0;
		v_image_iobuffer2[i]=0;
	  }
	  while (cinfo.next_scanline < (cinfo.image_height*7)/8) {
		// MSK: call dma
		if(aux < (image_size*7)/8){
			cont_interrupt = int_cont;
			dma_location = (int *)malloc(sizeof(int *));
			dma_location = 0x90000000+(ID-1)*28;
			dma_location[0] = (&image[0])+aux;
			dma_location[1] = sizeof(unsigned char);
			dma_location[2] = (source->buffer_width * buffer_size);
			dma_location[3] = 0; 		// 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
			dma_location[4] = ID;
			if(ping_pong == 0){
				dma_location[5] = &v_image_iobuffer1[0];//&source->iobuffer[0];
			}else{
				dma_location[5] = &v_image_iobuffer2[0];//&source->iobuffer[0];
			}
			dma_location[6] = 1; 			// 1 dma start 2 dma work 0 dma stop
			dma_location = (int *)malloc(sizeof(int *));
			dma_location =0x90000100;
			dma_location[0] = ID;			//indica qual é o channel solicitante
			if(aux == 0){
				aux = source->buffer_width * buffer_size;
			}else{
				aux = (source->buffer_width * buffer_size) + aux;
			}
		}
		fflush(stdout);
		if(int_cont == 0){
			while(int_cont == 0);
			ping_pong = 1;
		}else{	
			num_scanlines = (JDIMENSION)1;
			for(cont_aux = 0; cont_aux < buffer_size; cont_aux++){
				if(ping_pong == 1){
					source->iobuffer = (U_CHAR*)&v_image_iobuffer1[cont_aux*source->buffer_width];
					source->pixrow = (JSAMPROW) source->iobuffer;
					(void) jpeg_write_scanlines(&cinfo, src_mgr->buffer, num_scanlines);
				}else{
					source->iobuffer = (U_CHAR*)&v_image_iobuffer2[cont_aux*source->buffer_width];
					source->pixrow = (JSAMPROW) source->iobuffer;
					(void) jpeg_write_scanlines(&cinfo, src_mgr->buffer, num_scanlines);
				}
			}
			while(int_cont <= cont_interrupt);	
			if(ping_pong == 0){
				ping_pong = 1;
			}else{
				ping_pong = 0;
			}
		}
	  }
	  break;
  }

  // Finished, my work is done!
  flags[6] = 1;

  return 0;			/* suppress no-return-value warnings */
}

