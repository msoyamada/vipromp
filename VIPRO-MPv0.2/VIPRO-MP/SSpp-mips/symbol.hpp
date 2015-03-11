#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdio.h>
#include <stdlib.h>

#ifdef BFD_LOADER
#include <bfd.h>
#else /* !BFD_LOADER */
#include "target-pisa/ecoff.h"
#endif /* BFD_LOADER */

#include "loader.hpp"
#include "host.hpp"
#include "misc.hpp"
#include "machine.hpp"

#define RELEVANT_SCOPE(SYM)						\
(/* global symbol */							\
 ((SYM)->flags & BSF_GLOBAL)						\
 || (/* local symbol */							\
     (((SYM)->flags & (BSF_LOCAL|BSF_DEBUGGING)) == BSF_LOCAL)		\
     && (SYM)->name[0] != '$')						\
 || (/* compiler local */						\
     load_locals							\
     && ((/* basic block idents */					\
	  ((SYM)->flags&(BSF_LOCAL|BSF_DEBUGGING))==(BSF_LOCAL|BSF_DEBUGGING)\
	  && (SYM)->name[0] == '$')					\
	 || (/* local constant idents */				\
	     ((SYM)->flags & (BSF_LOCAL|BSF_DEBUGGING)) == (BSF_LOCAL)	\
	     && (SYM)->name[0] == '$'))))


enum sym_seg_t {
  ss_data,			/* data segment symbol */
  ss_text,			/* text segment symbol */
  ss_NUM
};

/* internal program symbol format */
struct sym_sym_t {
  char *name;			/* symbol name */
  enum sym_seg_t seg;		/* symbol segment */
  int initialized;		/* initialized? (if data segment) */
  int pub;			/* externally visible? */
  int local;			/* compiler local symbol? */
  md_addr_t addr;		/* symbol address value */
  int size;			/* bytes to next symbol */
};

/* symbol databases available */
enum sym_db_t {
  sdb_any,			/* search all symbols */
  sdb_text,			/* search text symbols */
  sdb_data,			/* search data symbols */
  sdb_NUM
};

class sim;

class symbol
{
	private:

		sim *Sim;

		/* symbols loaded? */
		/*static*/ int syms_loaded;
		
		char *			/* symbol flags string */
		flags2str(unsigned int flags);	/* bfd symbol flags */
/*	
		int
		acmp(struct sym_sym_t **sym1, struct sym_sym_t **sym2);

		int
		ncmp(struct sym_sym_t **sym1, struct sym_sym_t **sym2);
*/
	public:
		
		symbol(sim*);
		
		/* symbol database in no particular order */
		struct sym_sym_t *sym_db;

		/* all symbol sorted by address */
		int sym_nsyms;
		struct sym_sym_t **sym_syms;

		/* all symbols sorted by name */
		struct sym_sym_t **sym_syms_by_name;

		/* text symbols sorted by address */
		int sym_ntextsyms;
		struct sym_sym_t **sym_textsyms;

		/* text symbols sorted by name */
		struct sym_sym_t **sym_textsyms_by_name;

		/* data symbols sorted by address */
		int sym_ndatasyms;
		struct sym_sym_t **sym_datasyms;

		/* data symbols sorted by name */
		struct sym_sym_t **sym_datasyms_by_name;

		/* load symbols out of FNAME */
		void
		sym_loadsyms(char *fname,	/* file name containing symbols */
			     int load_locals);	/* load local symbols */
			     
		/* dump symbol SYM to output stream FD */
		void
		sym_dumpsym(struct sym_sym_t *sym,	/* symbol to display */
		    	FILE *fd);			/* output stream */

		void
		sym_dumpsyms(FILE *fd);			/* output stream */


		/* dump all symbol state to output stream FD */
		void
		sym_dumpstate(FILE *fd);			/* output stream */

		/* bind address ADDR to a symbol in symbol database DB, the address must
		   match exactly if EXACT is non-zero, the index of the symbol in the
		   requested symbol database is returned in *PINDEX if the pointer is
		   non-NULL */
		struct sym_sym_t *			/* symbol found, or NULL */
		sym_bind_addr(md_addr_t addr,		/* address of symbol to locate */
			      int *pindex,		/* ptr to index result var */
			      int exact,		/* require exact address match? */
			      enum sym_db_t db);		/* symbol database to search */
			      
		/* bind name NAME to a symbol in symbol database DB, the index of the symbol
		   in the requested symbol database is returned in *PINDEX if the pointer is
		   non-NULL */
		struct sym_sym_t *				/* symbol found, or NULL */
		sym_bind_name(char *name,			/* symbol name to locate */
			      int *pindex,			/* ptr to index result var */
			      enum sym_db_t db);		/* symbol database to search */

};

#endif

