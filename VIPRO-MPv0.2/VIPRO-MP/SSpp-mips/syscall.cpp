
#include "syscall.hpp"
#include "simplescalar.hpp"
#include "endian.hpp"
#include "sim.hpp"
#include "misc.hpp"

/* syscall proxy handler, architect registers and memory are assumed to be
   precise when this function is called, register and memory are updated with
   the results of the system call */

/*
syscalle::syscalle()
{

}


syscalle::syscalle(FILE *p)
{
	sim_eio_fd = p;
}
 */
syscalle::syscalle(sim *S)
{
	Sim = S;
}

void
syscalle::sys_syscall(struct regs_t *regs,	/* registers to access */
		/*mem_access_fn mem_fn,*/	/* generic memory accessor */
		/*memory *m,*/
		struct mem_t *mem,		/* memory space to access */
		md_inst_t inst,		/* system call inst */
		int traceable		/* traceable system call? */
)
{
	word_t syscode = regs->regs_R[2];

	/* first, check if an EIO trace is being consumed... */
	if (traceable && Sim->GLOBAL->sim_eio_fd != NULL)
	{
		Sim->EIOobj->eio_read_trace(Sim->GLOBAL->sim_eio_fd, Sim->GLOBAL->sim_num_insn, regs, /*mem_fn, */mem, inst);

		/* fini... */
		return;
	}

	/* no, OK execute the live system call... */
	switch (syscode)
	{
	case SS_SYS_exit:
		/* exit jumps to the target set in main() */
		longjmp(Sim->GLOBAL->sim_exit_buf, /* exitcode + fudge */regs->regs_R[4]+1);
		break;

#if 0
	case SS_SYS_fork:
		/* FIXME: this is broken... */
		regs->regs_R[2] = fork();
		if (regs->regs_R[2] != -1)
		{
			regs->regs_R[7] = 0;
			/* parent process */
			if (regs->regs_R[2] != 0)
				regs->regs_R[3] = 0;
		}
		else
			fatal("SYS_fork failed");
		break;
#endif

#if 0
	case SS_SYS_vfork:
		/* FIXME: this is broken... */
		int r31_parent = regs->regs_R[31];
		/* pid */regs->regs_R[2] = vfork();
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
			fatal("vfork() in SYS_vfork failed");
		if (regs->regs_R[2] != 0)
		{
			regs->regs_R[3] = 0;
			regs->regs_R[7] = 0;
			regs->regs_R[31] = r31_parent;
		}
		break;
#endif

	case SS_SYS_read:
	{
		char *buf;

		/* allocate same-sized input buffer in host memory */
		if (!(buf = (char *)calloc(/*nbytes*/regs->regs_R[6], sizeof(char))))
			Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of memory in SYS_read");

		/* read data from file */
		/*nread*/regs->regs_R[2] =
			read(/*fd*/regs->regs_R[4], buf, /*nbytes*/regs->regs_R[6]);

		/* check for error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}

		/* copy results back into host memory */
		Sim->MEMORY->mem_bcopy(/*mem_fn*//*MEMORY*/ mem,
				Write, /*buf*/regs->regs_R[5],
				buf, /*nread*/regs->regs_R[2]);

		/* done with input buffer */
		free(buf);
	}
	break;

	case SS_SYS_write:
	{
		char *buf;

		/* allocate same-sized output buffer in host memory */
		if (!(buf = (char *)calloc(/*nbytes*/regs->regs_R[6], sizeof(char))))
			Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of memory in SYS_write");

		/* copy inputs into host memory */
		Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem,
				Read, /*buf*/regs->regs_R[5],
				buf, /*nbytes*/regs->regs_R[6]);

		/* write data to file */
		if (Sim->GLOBAL->sim_progfd && MD_OUTPUT_SYSCALL(regs))
		{
			/* redirect program output to file *
			/*nwritten*/regs->regs_R[2] =
				fwrite(buf, 1, /*nbytes*/regs->regs_R[6], Sim->GLOBAL->sim_progfd);
		}
		else
		{
			/*nwritten*/regs->regs_R[2] =
				write(/*fd*/regs->regs_R[4],
						buf, /*nbytes*/regs->regs_R[6]);
		}

		/* check for an error condition */
		if (regs->regs_R[2] == regs->regs_R[6])
			/*result*/regs->regs_R[7] = 0;
		else
		{
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}

		/* done with output buffer */
		free(buf);
	}
	break;

	case SS_SYS_open:
	{
		char buf[MAXBUFSIZE];
		unsigned int i;
		int ss_flags = regs->regs_R[5], local_flags = 0;

		/* translate open(2) flags */
		for (i=0; i<SS_NFLAGS; i++)
		{
			if (ss_flags & ss_flag_table[i].ss_flag)
			{
				ss_flags &= ~ss_flag_table[i].ss_flag;
				local_flags |= ss_flag_table[i].local_flag;
			}
		}
		/* any target flags left? */
		if (ss_flags != 0)
			Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "syscall: open: cannot decode flags: 0x%08x", ss_flags);

		/* copy filename to host memory */
		Sim->MEMORY->mem_strcpy(/*mem_fn*/ mem, Read, /*fname*/regs->regs_R[4], buf);

		/* open the file */
#ifdef __CYGWIN32__
		/*fd*/regs->regs_R[2] =
			open(buf, local_flags|O_BINARY, /*mode*/regs->regs_R[6]);
#else /* !__CYGWIN32__ */
		/*fd*/regs->regs_R[2] =
			open(buf, local_flags, /*mode*/regs->regs_R[6]);
#endif /* __CYGWIN32__ */

		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}
	}
	break;

	case SS_SYS_close:
		/* don't close stdin, stdout, or stderr as this messes up sim logs */
		if (/*fd*/regs->regs_R[4] == 0
				|| /*fd*/regs->regs_R[4] == 1
				|| /*fd*/regs->regs_R[4] == 2)
		{
			regs->regs_R[7] = 0;
			break;
		}

		/* close the file */
		regs->regs_R[2] = close(/*fd*/regs->regs_R[4]);

		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}
		break;

	case SS_SYS_creat:
	{
		char buf[MAXBUFSIZE];

		/* copy filename to host memory */
		Sim->MEMORY->mem_strcpy(/*mem_fn*/ mem, Read, /*fname*/regs->regs_R[4], buf);

		/* create the file */
		/*fd*/regs->regs_R[2] = creat(buf, /*mode*/regs->regs_R[5]);

		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}
	}
	break;

	case SS_SYS_unlink:
	{
		char buf[MAXBUFSIZE];

		/* copy filename to host memory */
		Sim->MEMORY->mem_strcpy(/*mem_fn*/ mem, Read, /*fname*/regs->regs_R[4], buf);

		/* delete the file */
		/*result*/regs->regs_R[2] = unlink(buf);

		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}
	}
	break;

	case SS_SYS_chdir:
	{
		char buf[MAXBUFSIZE];

		/* copy filename to host memory */
		Sim->MEMORY->mem_strcpy(/*mem_fn*/ mem, Read, /*fname*/regs->regs_R[4], buf);

		/* change the working directory */
		/*result*/regs->regs_R[2] = chdir(buf);

		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}
	}
	break;

	case SS_SYS_chmod:
	{
		char buf[MAXBUFSIZE];

		/* copy filename to host memory */
		Sim->MEMORY->mem_strcpy(/*mem_fn*/ mem, Read, /*fname*/regs->regs_R[4], buf);

		/* chmod the file */
		/*result*/regs->regs_R[2] = chmod(buf, /*mode*/regs->regs_R[5]);

		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}
	}
	break;

	case SS_SYS_chown:
#ifdef _MSC_VER
		Sim->MISC->_warn(__FILE__, __FUNCTION__, __LINE__, "syscall chown() not yet implemented for MSC...");
		regs->regs_R[7] = 0;
#else /* !_MSC_VER */
		{
			char buf[MAXBUFSIZE];

			/* copy filename to host memory */
			Sim->MEMORY->mem_strcpy(/*mem_fn*/ mem, Read, /*fname*/regs->regs_R[4], buf);

			/* chown the file */
			/*result*/regs->regs_R[2] = chown(buf, /*owner*/regs->regs_R[5],
					/*group*/regs->regs_R[6]);

			/* check for an error condition */
			if (regs->regs_R[2] != -1)
				regs->regs_R[7] = 0;
			else
			{
				/* got an error, return details */
				regs->regs_R[2] = errno;
				regs->regs_R[7] = 1;
			}
		}
#endif /* _MSC_VER */
		break;

	case SS_SYS_brk:
	{
		md_addr_t addr;

		/* round the new heap pointer to the its page boundary */
		addr = ROUND_UP(/*base*/regs->regs_R[4], MD_PAGE_SIZE);

		/* check whether heap area has merged with stack area */
		if (addr >= Sim->LOADER->ld_brk_point && addr < (md_addr_t)regs->regs_R[29])
		{
			regs->regs_R[2] = 0;
			regs->regs_R[7] = 0;
			Sim->LOADER->ld_brk_point = addr;
		}
		else
		{
			/* out of address space, indicate error */
			regs->regs_R[2] = ENOMEM;
			regs->regs_R[7] = 1;
		}
	}
	break;

	case SS_SYS_lseek:
		/* seek into file */
		regs->regs_R[2] =
			lseek(/*fd*/regs->regs_R[4],
					/*off*/regs->regs_R[5], /*dir*/regs->regs_R[6]);

		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}
		break;

	case SS_SYS_getpid:
		/* get the simulator process id */
		/*result*/regs->regs_R[2] = getpid();

		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}
		break;

	case SS_SYS_getuid:
#ifdef _MSC_VER
		Sim->MISC->_warn(__FILE__, __FUNCTION__, __LINE__, "syscall getuid() not yet implemented for MSC...");
		regs->regs_R[7] = 0;
#else /* !_MSC_VER */
		/* get current user id */
		/*first result*/regs->regs_R[2] = getuid();

		/* get effective user id */
		/*second result*/regs->regs_R[3] = geteuid();

		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}
#endif /* _MSC_VER */
		break;

	case SS_SYS_access:
	{
		char buf[MAXBUFSIZE];

		/* copy filename to host memory */
		Sim->MEMORY->mem_strcpy(/*mem_fn*/ mem, Read, /*fName*/regs->regs_R[4], buf);

		/* check access on the file */
		/*result*/regs->regs_R[2] = access(buf, /*mode*/regs->regs_R[5]);

		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}
	}
	break;

	case SS_SYS_stat:
	case SS_SYS_lstat:
	{
		char buf[MAXBUFSIZE];
		struct ss_statbuf ss_sbuf;
#ifdef _MSC_VER
		struct _stat sbuf;
#else /* !_MSC_VER */
		struct stat sbuf;
#endif /* _MSC_VER */

		/* copy filename to host memory */
		Sim->MEMORY->mem_strcpy(/*mem_fn*/ mem, Read, /*fName*/regs->regs_R[4], buf);

		/* stat() the file */
		if (syscode == SS_SYS_stat)
			/*result*/regs->regs_R[2] = stat(buf, &sbuf);
		else /* syscode == SS_SYS_lstat */
		{
#ifdef _MSC_VER
			Sim->MISC->_warn(__FILE__, __FUNCTION__, __LINE__, "syscall lstat() not yet implemented for MSC...");
			regs->regs_R[7] = 0;
			break;
#else /* !_MSC_VER */
			/*result*/regs->regs_R[2] = lstat(buf, &sbuf);
#endif /* _MSC_VER */
		}

		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}

		/* translate from host stat structure to target format */
		ss_sbuf.ss_st_dev = SWAP_HALF(sbuf.st_dev);
		ss_sbuf.ss_pad = 0;
		ss_sbuf.ss_st_ino = SWAP_WORD(sbuf.st_ino);
		ss_sbuf.ss_st_mode = SWAP_HALF(sbuf.st_mode);
		ss_sbuf.ss_st_nlink = SWAP_HALF(sbuf.st_nlink);
		ss_sbuf.ss_st_uid = SWAP_HALF(sbuf.st_uid);
		ss_sbuf.ss_st_gid = SWAP_HALF(sbuf.st_gid);
		ss_sbuf.ss_st_rdev = SWAP_HALF(sbuf.st_rdev);
		ss_sbuf.ss_pad1 = 0;
		ss_sbuf.ss_st_size = SWAP_WORD(sbuf.st_size);
		ss_sbuf.ss_st_atime = SWAP_WORD(sbuf.st_atime);
		ss_sbuf.ss_st_spare1 = 0;
		ss_sbuf.ss_st_mtime = SWAP_WORD(sbuf.st_mtime);
		ss_sbuf.ss_st_spare2 = 0;
		ss_sbuf.ss_st_ctime = SWAP_WORD(sbuf.st_ctime);
		ss_sbuf.ss_st_spare3 = 0;
#ifndef _MSC_VER
		ss_sbuf.ss_st_blksize = SWAP_WORD(sbuf.st_blksize);
		ss_sbuf.ss_st_blocks = SWAP_WORD(sbuf.st_blocks);
#endif /* !_MSC_VER */
		ss_sbuf.ss_st_gennum = 0;
		ss_sbuf.ss_st_spare4 = 0;

		/* copy stat() results to simulator memory */
		Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Write, /*sbuf*/regs->regs_R[5],
				&ss_sbuf, sizeof(struct ss_statbuf));
	}
	break;

	case SS_SYS_dup:
		/* dup() the file descriptor */
		/*fd*/regs->regs_R[2] = dup(/*fd*/regs->regs_R[4]);

		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}
		break;

#ifndef _MSC_VER
	case SS_SYS_pipe:
	{
		int fd[2];

		/* copy pipe descriptors to host memory */;
		Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Read, /*fd's*/regs->regs_R[4], fd, sizeof(fd));

		/* create a pipe */
		/*result*/regs->regs_R[7] = pipe(fd);

		/* copy descriptor results to result registers */
		/*pipe1*/regs->regs_R[2] = fd[0];
		/*pipe 2*/regs->regs_R[3] = fd[1];

		/* check for an error condition */
		if (regs->regs_R[7] == -1)
		{
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}
	}
	break;
#endif

	case SS_SYS_getgid:
#ifdef _MSC_VER
		Sim->MISC->_warn(__FILE__, __FUNCTION__, __LINE__, "syscall getgid() not yet implemented for MSC...");
		regs->regs_R[7] = 0;
#else /* !_MSC_VER */
		/* get current group id */
		/*first result*/regs->regs_R[2] = getgid();

		/* get current effective group id */
		/*second result*/regs->regs_R[3] = getegid();

		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}
#endif /* _MSC_VER */
		break;

	case SS_SYS_ioctl:
	{
		char buf[NUM_IOCTL_BYTES];
		int local_req = 0;

		/* convert target ioctl() request to host ioctl() request values */
		switch (/*req*/regs->regs_R[5]) {
#ifdef TIOCGETP
		case SS_IOCTL_TIOCGETP:
			local_req = TIOCGETP;
			break;
#endif
#ifdef TIOCSETP
		case SS_IOCTL_TIOCSETP:
			local_req = TIOCSETP;
			break;
#endif
#ifdef TIOCGETP
		case SS_IOCTL_TCGETP:
			local_req = TIOCGETP;
			break;
#endif
#ifdef TCGETA
		case SS_IOCTL_TCGETA:
			local_req = TCGETA;
			break;
#endif
#ifdef TIOCGLTC
		case SS_IOCTL_TIOCGLTC:
			local_req = TIOCGLTC;
			break;
#endif
#ifdef TIOCSLTC
		case SS_IOCTL_TIOCSLTC:
			local_req = TIOCSLTC;
			break;
#endif
#ifdef TIOCGWINSZ
		case SS_IOCTL_TIOCGWINSZ:
			local_req = TIOCGWINSZ;
			break;
#endif
#ifdef TCSETAW
		case SS_IOCTL_TCSETAW:
			local_req = TCSETAW;
			break;
#endif
#ifdef TIOCGETC
		case SS_IOCTL_TIOCGETC:
			local_req = TIOCGETC;
			break;
#endif
#ifdef TIOCSETC
		case SS_IOCTL_TIOCSETC:
			local_req = TIOCSETC;
			break;
#endif
#ifdef TIOCLBIC
		case SS_IOCTL_TIOCLBIC:
			local_req = TIOCLBIC;
			break;
#endif
#ifdef TIOCLBIS
		case SS_IOCTL_TIOCLBIS:
			local_req = TIOCLBIS;
			break;
#endif
#ifdef TIOCLGET
		case SS_IOCTL_TIOCLGET:
			local_req = TIOCLGET;
			break;
#endif
#ifdef TIOCLSET
		case SS_IOCTL_TIOCLSET:
			local_req = TIOCLSET;
			break;
#endif
		}
		if (!local_req)
		{
			/* FIXME: could not translate the ioctl() request, just warn user
	       and ignore the request */
			Sim->MISC->_warn(__FILE__, __FUNCTION__, __LINE__, "syscall: ioctl: ioctl code not supported d=%d, req=%d",
					regs->regs_R[4], regs->regs_R[5]);
			regs->regs_R[2] = 0;
			regs->regs_R[7] = 0;
		}
		else
		{
#ifdef _MSC_VER
			Sim->MISC->_warn(__FILE__, __FUNCTION__, __LINE__, "syscall getgid() not yet implemented for MSC...");
			regs->regs_R[7] = 0;
			break;
#else /* !_MSC_VER */

#if 0 /* FIXME: needed? */
#ifdef TIOCGETP
			if (local_req == TIOCGETP && sim_progfd)
			{
				/* program I/O has been redirected to file, make
		   termios() calls fail... */

				/* got an error, return details */
				regs->regs_R[2] = ENOTTY;
				regs->regs_R[7] = 1;
				break;
			}
#endif
#endif
			/* ioctl() code was successfully translated to a host code */

			/* if arg ptr exists, copy NUM_IOCTL_BYTES bytes to host mem */
			if (/*argp*/regs->regs_R[6] != 0)
				Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem,
						Read, /*argp*/regs->regs_R[6], buf, NUM_IOCTL_BYTES);

			/* perform the ioctl() call */
			/*result*/regs->regs_R[2] =
				ioctl(/*fd*/regs->regs_R[4], local_req, buf);

			/* if arg ptr exists, copy NUM_IOCTL_BYTES bytes from host mem */
			if (/*argp*/regs->regs_R[6] != 0)
				Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Write, regs->regs_R[6],
						buf, NUM_IOCTL_BYTES);

			/* check for an error condition */
			if (regs->regs_R[2] != -1)
				regs->regs_R[7] = 0;
			else
			{
				/* got an error, return details */
				regs->regs_R[2] = errno;
				regs->regs_R[7] = 1;
			}
#endif /* _MSC_VER */
		}
	}
	break;

	case SS_SYS_fstat:
	{
		struct ss_statbuf ss_sbuf;
#ifdef _MSC_VER
		struct _stat sbuf;
#else /* !_MSC_VER */
		struct stat sbuf;
#endif /* _MSC_VER */

		/* fstat() the file */
		/*result*/regs->regs_R[2] = fstat(/*fd*/regs->regs_R[4], &sbuf);

		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}

		/* translate the stat structure to host format */
		ss_sbuf.ss_st_dev = SWAP_HALF(sbuf.st_dev);
		ss_sbuf.ss_pad = 0;
		ss_sbuf.ss_st_ino = SWAP_WORD(sbuf.st_ino);
		ss_sbuf.ss_st_mode = SWAP_HALF(sbuf.st_mode);
		ss_sbuf.ss_st_nlink = SWAP_HALF(sbuf.st_nlink);
		ss_sbuf.ss_st_uid = SWAP_HALF(sbuf.st_uid);
		ss_sbuf.ss_st_gid = SWAP_HALF(sbuf.st_gid);
		ss_sbuf.ss_st_rdev = SWAP_HALF(sbuf.st_rdev);
		ss_sbuf.ss_pad1 = 0;
		ss_sbuf.ss_st_size = SWAP_WORD(sbuf.st_size);
		ss_sbuf.ss_st_atime = SWAP_WORD(sbuf.st_atime);
		ss_sbuf.ss_st_spare1 = 0;
		ss_sbuf.ss_st_mtime = SWAP_WORD(sbuf.st_mtime);
		ss_sbuf.ss_st_spare2 = 0;
		ss_sbuf.ss_st_ctime = SWAP_WORD(sbuf.st_ctime);
		ss_sbuf.ss_st_spare3 = 0;
#ifndef _MSC_VER
		ss_sbuf.ss_st_blksize = SWAP_WORD(sbuf.st_blksize);
		ss_sbuf.ss_st_blocks = SWAP_WORD(sbuf.st_blocks);
#endif /* !_MSC_VER */
		ss_sbuf.ss_st_gennum = 0;
		ss_sbuf.ss_st_spare4 = 0;

		/* copy fstat() results to simulator memory */
		Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Write, /*sbuf*/regs->regs_R[5],
				&ss_sbuf, sizeof(struct ss_statbuf));
	}
	break;

	case SS_SYS_getpagesize:
		/* get target pagesize */
		regs->regs_R[2] = /* was: getpagesize() */MD_PAGE_SIZE;

		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}
		break;

	case SS_SYS_setitimer:
		/* FIXME: the sigvec system call is ignored */
		regs->regs_R[2] = regs->regs_R[7] = 0;
		Sim->MISC->_warn(__FILE__, __FUNCTION__, __LINE__, "syscall: setitimer ignored");
		break;

	case SS_SYS_getdtablesize:
#if defined(_AIX)
		/* get descriptor table size */
		regs->regs_R[2] = getdtablesize();

		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}
#elif defined(__CYGWIN32__) || defined(ultrix) || defined(_MSC_VER)
		{
			/* no comparable system call found, try some reasonable defaults */
			Sim->MISC->_warn(__FILE__, __FUNCTION__, __LINE__, "syscall: called getdtablesize()\n");
			regs->regs_R[2] = 16;
			regs->regs_R[7] = 0;
		}
#else
		{
			struct rlimit rl;

			/* get descriptor table size in rlimit structure */
			if (getrlimit(RLIMIT_NOFILE, &rl) != -1)
			{
				regs->regs_R[2] = rl.rlim_cur;
				regs->regs_R[7] = 0;
			}
			else
			{
				/* got an error, return details */
				regs->regs_R[2] = errno;
				regs->regs_R[7] = 1;
			}
		}
#endif
		break;

	case SS_SYS_dup2:
		/* dup2() the file descriptor */
		regs->regs_R[2] =
			dup2(/* fd1 */regs->regs_R[4], /* fd2 */regs->regs_R[5]);

		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}
		break;

	case SS_SYS_fcntl:
#ifdef _MSC_VER
		Sim->MISC->_warn(__FILE__, __FUNCTION__, __LINE__, "syscall fcntl() not yet implemented for MSC...");
		regs->regs_R[7] = 0;
#else /* !_MSC_VER */
		/* get fcntl() information on the file */
		regs->regs_R[2] =
			fcntl(/*fd*/regs->regs_R[4], /*cmd*/regs->regs_R[5],
					/*arg*/regs->regs_R[6]);

		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, return details */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}
#endif /* _MSC_VER */
		break;

	case SS_SYS_select:
#ifdef _MSC_VER
		Sim->MISC->_warn(__FILE__, __FUNCTION__, __LINE__, "syscall select() not yet implemented for MSC...");
		regs->regs_R[7] = 0;
#else /* !_MSC_VER */
		{
			fd_set readfd, writefd, exceptfd;
			fd_set *readfdp, *writefdp, *exceptfdp;
			struct timeval timeout, *timeoutp;
			word_t param5;

			/* FIXME: swap words? */

			/* read the 5th parameter (timeout) from the stack */
			Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem,
					Read, regs->regs_R[29]+16, &param5, sizeof(word_t));

			/* copy read file descriptor set into host memory */
			if (/*readfd*/regs->regs_R[5] != 0)
			{
				Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Read, /*readfd*/regs->regs_R[5],
						&readfd, sizeof(fd_set));
				readfdp = &readfd;
			}
			else
				readfdp = NULL;

			/* copy write file descriptor set into host memory */
			if (/*writefd*/regs->regs_R[6] != 0)
			{
				Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Read, /*writefd*/regs->regs_R[6],
						&writefd, sizeof(fd_set));
				writefdp = &writefd;
			}
			else
				writefdp = NULL;

			/* copy exception file descriptor set into host memory */
			if (/*exceptfd*/regs->regs_R[7] != 0)
			{
				Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Read, /*exceptfd*/regs->regs_R[7],
						&exceptfd, sizeof(fd_set));
				exceptfdp = &exceptfd;
			}
			else
				exceptfdp = NULL;

			/* copy timeout value into host memory */
			if (/*timeout*/param5 != 0)
			{
				Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Read, /*timeout*/param5,
						&timeout, sizeof(struct timeval));
				timeoutp = &timeout;
			}
			else
				timeoutp = NULL;

#if defined(hpux) || defined(__hpux)
			/* select() on the specified file descriptors */
			/*result*/regs->regs_R[2] =
				select(/*nfd*/regs->regs_R[4],
						(int *)readfdp, (int *)writefdp, (int *)exceptfdp, timeoutp);
#else
			/* select() on the specified file descriptors */
			/*result*/regs->regs_R[2] =
				select(/*nfd*/regs->regs_R[4],
						readfdp, writefdp, exceptfdp, timeoutp);
#endif

			/* check for an error condition */
			if (regs->regs_R[2] != -1)
				regs->regs_R[7] = 0;
			else
			{
				/* got an error, return details */
				regs->regs_R[2] = errno;
				regs->regs_R[7] = 1;
			}

			/* copy read file descriptor set to target memory */
			if (/*readfd*/regs->regs_R[5] != 0)
				Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Write, /*readfd*/regs->regs_R[5],
						&readfd, sizeof(fd_set));

			/* copy write file descriptor set to target memory */
			if (/*writefd*/regs->regs_R[6] != 0)
				Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Write, /*writefd*/regs->regs_R[6],
						&writefd, sizeof(fd_set));

			/* copy exception file descriptor set to target memory */
			if (/*exceptfd*/regs->regs_R[7] != 0)
				Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Write, /*exceptfd*/regs->regs_R[7],
						&exceptfd, sizeof(fd_set));

			/* copy timeout value result to target memory */
			if (/* timeout */param5 != 0)
				Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Write, /*timeout*/param5,
						&timeout, sizeof(struct timeval));
		}
#endif
		break;

	case SS_SYS_sigvec:
		/* FIXME: the sigvec system call is ignored */
		regs->regs_R[2] = regs->regs_R[7] = 0;
		Sim->MISC->_warn(__FILE__, __FUNCTION__, __LINE__, "syscall: sigvec ignored");
		break;

	case SS_SYS_sigblock:
		/* FIXME: the sigblock system call is ignored */
		regs->regs_R[2] = regs->regs_R[7] = 0;
		Sim->MISC->_warn(__FILE__, __FUNCTION__, __LINE__, "syscall: sigblock ignored");
		break;

	case SS_SYS_sigsetmask:
		/* FIXME: the sigsetmask system call is ignored */
		regs->regs_R[2] = regs->regs_R[7] = 0;
		Sim->MISC->_warn(__FILE__, __FUNCTION__, __LINE__, "syscall: sigsetmask ignored");
		break;

#if 0
	case SS_SYS_sigstack:
		/* FIXME: this is broken... */
		/* do not make the system call; instead, modify (the stack
	 portion of) the simulator's main memory, ignore the 1st
	 argument (regs->regs_R[4]), as it relates to signal handling */
		if (regs->regs_R[5] != 0)
		{
			(*maf)(Read, regs->regs_R[29]+28, (unsigned char *)&temp, 4);
			(*maf)(Write, regs->regs_R[5], (unsigned char *)&temp, 4);
		}
		regs->regs_R[2] = regs->regs_R[7] = 0;
		break;
#endif

	case SS_SYS_gettimeofday:
#ifdef _MSC_VER
		Sim->MISC->_warn(__FILE__, __FUNCTION__, __LINE__, "syscall gettimeofday() not yet implemented for MSC...");
		regs->regs_R[7] = 0;
#else /* _MSC_VER */
		{
			struct ss_timeval ss_tv;
			struct timeval tv, *tvp;
			struct ss_timezone ss_tz;
			struct timezone tz, *tzp;

			if (/*timeval*/regs->regs_R[4] != 0)
			{
				/* copy timeval into host memory */
				Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Read, /*timeval*/regs->regs_R[4],
						&ss_tv, sizeof(struct ss_timeval));

				/* convert target timeval structure to host format */
				tv.tv_sec = SWAP_WORD(ss_tv.ss_tv_sec);
				tv.tv_usec = SWAP_WORD(ss_tv.ss_tv_usec);
				tvp = &tv;
			}
			else
				tvp = NULL;

			if (/*timezone*/regs->regs_R[5] != 0)
			{
				/* copy timezone into host memory */
				Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Read, /*timezone*/regs->regs_R[5],
						&ss_tz, sizeof(struct ss_timezone));

				/* convert target timezone structure to host format */
				tz.tz_minuteswest = SWAP_WORD(ss_tz.ss_tz_minuteswest);
				tz.tz_dsttime = SWAP_WORD(ss_tz.ss_tz_dsttime);
				tzp = &tz;
			}
			else
				tzp = NULL;

			/* get time of day */
			/*result*/regs->regs_R[2] = gettimeofday(tvp, tzp);

			/* check for an error condition */
			if (regs->regs_R[2] != -1)
				regs->regs_R[7] = 0;
			else
			{
				/* got an error, indicate result */
				regs->regs_R[2] = errno;
				regs->regs_R[7] = 1;
			}

			if (/*timeval*/regs->regs_R[4] != 0)
			{
				/* convert host timeval structure to target format */
				ss_tv.ss_tv_sec = SWAP_WORD(tv.tv_sec);
				ss_tv.ss_tv_usec = SWAP_WORD(tv.tv_usec);

				/* copy timeval to target memory */
				Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Write, /*timeval*/regs->regs_R[4],
						&ss_tv, sizeof(struct ss_timeval));
			}

			if (/*timezone*/regs->regs_R[5] != 0)
			{
				/* convert host timezone structure to target format */
				ss_tz.ss_tz_minuteswest = SWAP_WORD(tz.tz_minuteswest);
				ss_tz.ss_tz_dsttime = SWAP_WORD(tz.tz_dsttime);

				/* copy timezone to target memory */
				Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Write, /*timezone*/regs->regs_R[5],
						&ss_tz, sizeof(struct ss_timezone));
			}
		}
#endif /* !_MSC_VER */
		break;

	case SS_SYS_getrusage:
#if defined(__svr4__) || defined(__USLC__) || defined(hpux) || defined(__hpux) || defined(_AIX)
	{
		struct tms tms_buf;
		struct ss_rusage rusage;

		/* get user and system times */
		if (times(&tms_buf) != -1)
		{
			/* no error */
			regs->regs_R[2] = 0;
			regs->regs_R[7] = 0;
		}
		else
		{
			/* got an error, indicate result */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}

		/* initialize target rusage result structure */
#if defined(__svr4__)
		memset(&rusage, '\0', sizeof(struct ss_rusage));
#else /* !defined(__svr4__) */
		bzero(&rusage, sizeof(struct ss_rusage));
#endif

		/* convert from host rusage structure to target format */
		rusage.ss_ru_utime.ss_tv_sec = tms_buf.tms_utime/CLK_TCK;
		rusage.ss_ru_utime.ss_tv_sec = SWAP_WORD(rusage.ss_ru_utime.ss_tv_sec);
		rusage.ss_ru_utime.ss_tv_usec = 0;
		rusage.ss_ru_stime.ss_tv_sec = tms_buf.tms_stime/CLK_TCK;
		rusage.ss_ru_stime.ss_tv_sec = SWAP_WORD(rusage.ss_ru_stime.ss_tv_sec);
		rusage.ss_ru_stime.ss_tv_usec = 0;

		/* copy rusage results into target memory */
		Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Write, /*rusage*/regs->regs_R[5],
				&rusage, sizeof(struct ss_rusage));
	}
#elif defined(__unix__) || defined(unix)
	{
		struct rusage local_rusage;
		struct ss_rusage rusage;

		/* get rusage information */
		/*result*/regs->regs_R[2] =
			getrusage(/*who*/regs->regs_R[4], &local_rusage);

		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, indicate result */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}

		/* convert from host rusage structure to target format */
		rusage.ss_ru_utime.ss_tv_sec = local_rusage.ru_utime.tv_sec;
		rusage.ss_ru_utime.ss_tv_usec = local_rusage.ru_utime.tv_usec;
		rusage.ss_ru_utime.ss_tv_sec = SWAP_WORD(local_rusage.ru_utime.tv_sec);
		rusage.ss_ru_utime.ss_tv_usec =
			SWAP_WORD(local_rusage.ru_utime.tv_usec);
		rusage.ss_ru_stime.ss_tv_sec = local_rusage.ru_stime.tv_sec;
		rusage.ss_ru_stime.ss_tv_usec = local_rusage.ru_stime.tv_usec;
		rusage.ss_ru_stime.ss_tv_sec =
			SWAP_WORD(local_rusage.ru_stime.tv_sec);
		rusage.ss_ru_stime.ss_tv_usec =
			SWAP_WORD(local_rusage.ru_stime.tv_usec);
		rusage.ss_ru_maxrss = SWAP_WORD(local_rusage.ru_maxrss);
		rusage.ss_ru_ixrss = SWAP_WORD(local_rusage.ru_ixrss);
		rusage.ss_ru_idrss = SWAP_WORD(local_rusage.ru_idrss);
		rusage.ss_ru_isrss = SWAP_WORD(local_rusage.ru_isrss);
		rusage.ss_ru_minflt = SWAP_WORD(local_rusage.ru_minflt);
		rusage.ss_ru_majflt = SWAP_WORD(local_rusage.ru_majflt);
		rusage.ss_ru_nswap = SWAP_WORD(local_rusage.ru_nswap);
		rusage.ss_ru_inblock = SWAP_WORD(local_rusage.ru_inblock);
		rusage.ss_ru_oublock = SWAP_WORD(local_rusage.ru_oublock);
		rusage.ss_ru_msgsnd = SWAP_WORD(local_rusage.ru_msgsnd);
		rusage.ss_ru_msgrcv = SWAP_WORD(local_rusage.ru_msgrcv);
		rusage.ss_ru_nsignals = SWAP_WORD(local_rusage.ru_nsignals);
		rusage.ss_ru_nvcsw = SWAP_WORD(local_rusage.ru_nvcsw);
		rusage.ss_ru_nivcsw = SWAP_WORD(local_rusage.ru_nivcsw);

		/* copy rusage results into target memory */
		Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Write, /*rusage*/regs->regs_R[5],
				&rusage, sizeof(struct ss_rusage));
	}
#elif defined(__CYGWIN32__) || defined(_MSC_VER)
	Sim->MISC->_warn(__FILE__, __FUNCTION__, __LINE__, "syscall: called getrusage()\n");
	regs->regs_R[7] = 0;
#else
#err No getrusage() implementation!
#endif
	break;

	case SS_SYS_writev:
#ifdef _MSC_VER
		Sim->MISC->_warn(__FILE__, __FUNCTION__, __LINE__, "syscall writev() not yet implemented for MSC...");
		regs->regs_R[7] = 0;
#else /* !_MSC_VER */
		{
			int i;
			char *buf;
			struct iovec *iov;

			/* allocate host side I/O vectors */
			iov =
				(struct iovec *)malloc(/*iovcnt*/regs->regs_R[6]
				                                              * sizeof(struct iovec));
			if (!iov)
				Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of virtual memory in SYS_writev");

			/* copy target side pointer data into host side vector */
			Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Read, /*iov*/regs->regs_R[5],
					iov, /*iovcnt*/regs->regs_R[6] * sizeof(struct iovec));

			/* copy target side I/O vector buffers to host memory */
			for (i=0; i < /*iovcnt*/regs->regs_R[6]; i++)
			{
				iov[i].iov_base = (char *)SWAP_WORD((unsigned)iov[i].iov_base);
				iov[i].iov_len = SWAP_WORD(iov[i].iov_len);
				if (iov[i].iov_base != NULL)
				{
					buf = (char *)calloc(iov[i].iov_len, sizeof(char));
					if (!buf)
						Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of virtual memory in SYS_writev");
					Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Read, (md_addr_t)iov[i].iov_base,
							buf, iov[i].iov_len);
					iov[i].iov_base = buf;
				}
			}

			/* perform the vector'ed write */
			/*result*/regs->regs_R[2] =
				writev(/*fd*/regs->regs_R[4], iov, /*iovcnt*/regs->regs_R[6]);

			/* check for an error condition */
			if (regs->regs_R[2] != -1)
				regs->regs_R[7] = 0;
			else
			{
				/* got an error, indicate results */
				regs->regs_R[2] = errno;
				regs->regs_R[7] = 1;
			}

			/* free all the allocated memory */
			for (i=0; i < /*iovcnt*/regs->regs_R[6]; i++)
			{
				if (iov[i].iov_base)
				{
					free(iov[i].iov_base);
					iov[i].iov_base = NULL;
				}
			}
			free(iov);
		}
#endif /* !_MSC_VER */
		break;

	case SS_SYS_utimes:
	{
		char buf[MAXBUFSIZE];

		/* copy filename to host memory */
		Sim->MEMORY->mem_strcpy(/*mem_fn*/ mem, Read, /*fname*/regs->regs_R[4], buf);

		if (/*timeval*/regs->regs_R[5] == 0)
		{
#if defined(hpux) || defined(__hpux) || defined(__i386__)
			/* no utimes() in hpux, use utime() instead */
			/*result*/regs->regs_R[2] = utime(buf, NULL);
#elif defined(_MSC_VER)
			/* no utimes() in MSC, use utime() instead */
			/*result*/regs->regs_R[2] = utime(buf, NULL);
#elif defined(__svr4__) || defined(__USLC__) || defined(unix) || defined(_AIX) || defined(__alpha)
			/*result*/regs->regs_R[2] = utimes(buf, NULL);
#elif defined(__CYGWIN32__)
			Sim->MISC->_warn(__FILE__, __FUNCTION__, __LINE__, "syscall: called utimes()\n");
#else
#error No utimes() implementation!
#endif
		}
		else
		{
			struct ss_timeval ss_tval[2];
#ifndef _MSC_VER
			struct timeval tval[2];
#endif /* !_MSC_VER */

			/* copy timeval structure to host memory */
			Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Read, /*timeout*/regs->regs_R[5],
					ss_tval, 2*sizeof(struct ss_timeval));

#ifndef _MSC_VER
			/* convert timeval structure to host format */
			tval[0].tv_sec = SWAP_WORD(ss_tval[0].ss_tv_sec);
			tval[0].tv_usec = SWAP_WORD(ss_tval[0].ss_tv_usec);
			tval[1].tv_sec = SWAP_WORD(ss_tval[1].ss_tv_sec);
			tval[1].tv_usec = SWAP_WORD(ss_tval[1].ss_tv_usec);
#endif /* !_MSC_VER */

#if defined(hpux) || defined(__hpux)
			/* no utimes() in hpux, use utime() instead */
			{
				struct utimbuf ubuf;

				ubuf.actime = tval[0].tv_sec;
				ubuf.modtime = tval[1].tv_sec;

				/* result */regs->regs_R[2] = utime(buf, &ubuf);
			}
#elif defined(_MSC_VER)
			/* no utimes() in MSC, use utime() instead */
			{
				struct _utimbuf ubuf;

				ubuf.actime = ss_tval[0].ss_tv_sec;
				ubuf.modtime = ss_tval[1].ss_tv_sec;

				/* result */regs->regs_R[2] = utime(buf, &ubuf);
			}
#elif defined(__svr4__) || defined(__USLC__) || defined(unix) || defined(_AIX) || defined(__alpha)
			/* result */regs->regs_R[2] = utimes(buf, tval);
#elif defined(__CYGWIN32__)
			Sim->MISC->_warn(__FILE__, __FUNCTION__, __LINE__, "syscall: called utimes()\n");
#else
#error No utimes() implementation!
#endif
		}

		/* check for an error condition */
		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			/* got an error, indicate results */
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}
	}
	break;

	case SS_SYS_getrlimit:
	case SS_SYS_setrlimit:
#ifdef _MSC_VER
		Sim->MISC->_warn(__FILE__, __FUNCTION__, __LINE__, "syscall get/setrlimit() not yet implemented for MSC...");
		regs->regs_R[7] = 0;
#elif defined(__CYGWIN32__)
		Sim->MISC->_warn(__FILE__, __FUNCTION__, __LINE__, "syscall: called get/setrlimit()\n");
		regs->regs_R[7] = 0;
#else
		{
			/* FIXME: check this..., was: struct rlimit ss_rl; */
			struct ss_rlimit ss_rl;
			struct rlimit rl;

			/* copy rlimit structure to host memory */
			Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Read, /*rlimit*/regs->regs_R[5],
					&ss_rl, sizeof(struct ss_rlimit));

			/* convert rlimit structure to host format */
			rl.rlim_cur = SWAP_WORD(ss_rl.ss_rlim_cur);
			rl.rlim_max = SWAP_WORD(ss_rl.ss_rlim_max);

			/* get rlimit information */
			if (syscode == SS_SYS_getrlimit)
				/*result*/regs->regs_R[2] = getrlimit(regs->regs_R[4], &rl);
			else /* syscode == SS_SYS_setrlimit */
				/*result*/regs->regs_R[2] = setrlimit(regs->regs_R[4], &rl);

			/* check for an error condition */
			if (regs->regs_R[2] != -1)
				regs->regs_R[7] = 0;
			else
			{
				/* got an error, indicate results */
				regs->regs_R[2] = errno;
				regs->regs_R[7] = 1;
			}

			/* convert rlimit structure to target format */
			ss_rl.ss_rlim_cur = SWAP_WORD(rl.rlim_cur);
			ss_rl.ss_rlim_max = SWAP_WORD(rl.rlim_max);

			/* copy rlimit structure to target memory */
			Sim->MEMORY->mem_bcopy(/*mem_fn*/ mem, Write, /*rlimit*/regs->regs_R[5],
					&ss_rl, sizeof(struct ss_rlimit));
		}
#endif
		break;

#if 0
	case SS_SYS_getdirentries:
		/* FIXME: this is currently broken due to incompatabilities in
	 disk directory formats */
	{
		unsigned int i;
		char *buf;
		int base;

		buf = (char *)calloc(/* nbytes */regs->regs_R[6] + 1, sizeof(char));
		if (!buf)
			fatal("out of memory in SYS_getdirentries");

		/* copy in */
		for (i=0; i</* nbytes */regs->regs_R[6]; i++)
			(*maf)(Read, /* buf */regs->regs_R[5]+i,
					(unsigned char *)&buf[i], 1);
		(*maf)(Read, /* basep */regs->regs_R[7], (unsigned char *)&base, 4);

		/*cc*/regs->regs_R[2] =
			getdirentries(/*fd*/regs->regs_R[4], buf,
					/*nbytes*/regs->regs_R[6], &base);

		if (regs->regs_R[2] != -1)
			regs->regs_R[7] = 0;
		else
		{
			regs->regs_R[2] = errno;
			regs->regs_R[7] = 1;
		}

		/* copy out */
		for (i=0; i</* nbytes */regs->regs_R[6]; i++)
			(*maf)(Write, /* buf */regs->regs_R[5]+i,
					(unsigned char *)&buf[i], 1);
		(*maf)(Write, /* basep */regs->regs_R[7], (unsigned char *)&base, 4);

		free(buf);
	}
	break;
#endif

	default:
		Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "invalid/unimplemented system call encountered, code %d", syscode);
	}
}
