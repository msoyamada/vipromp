#ifndef SYSCALL_H
#define SYSCALL_H

#include <sys/types.h>
#ifdef _MSC_VER
#include <time.h>
#else
#include <sys/time.h>
#endif


#include "host.hpp"
#include "misc.hpp"
#include "machine.hpp"
#include "eio.hpp"
#include "global.hpp"


//#include "regs.hpp"
//#include "loader.hpp"
//#include "endian.hpp"


/*
 * This module implements the system call portion of the SimpleScalar
 * instruction set architecture.  The system call definitions are borrowed
 * from Ultrix.  All system calls are executed by the simulator (the host) on
 * behalf of the simulated program (the target). The basic procedure for
 * implementing a system call is as follows:
 *
 *	1) decode the system call (this is the enum in "syscode")
 *	2) copy system call inputs in target (simulated program) memory
 *	   to host memory (simulator memory), note: the location and
 *	   amount of memory to copy is system call specific
 *	3) the simulator performs the system call on behalf of the target prog
 *	4) copy system call results in host memory to target memory
 *	5) set result register to indicate the error status of the system call
 *
 * That's it...  If you encounter an unimplemented system call and would like
 * to add support for it, first locate the syscode and arguments for the system
 * call when it occurs (do this in the debugger) and then implement a proxy
 * procedure in syscall.c.
 *
 */
 
 
#include <stdio.h>
#include <stdlib.h>
#ifdef _MSC_VER
#include <io.h>
#else /* !_MSC_VER */
#include <unistd.h>
#endif
#include <fcntl.h>
#include <sys/types.h>
#ifndef _MSC_VER
#include <sys/param.h>
#endif
#include <errno.h>
#include <time.h>
#ifndef _MSC_VER
#include <sys/time.h>
#endif
#ifndef _MSC_VER
#include <sys/resource.h>
#endif
#include <signal.h>

/* #include <sys/file.h> */

#include <sys/stat.h>
#ifndef _MSC_VER
#include <sys/uio.h>
#endif
#include <setjmp.h>
#ifndef _MSC_VER
#include <sys/times.h>
#endif
#include <limits.h>
#ifndef _MSC_VER
#include <sys/ioctl.h>
#endif
#if !defined(linux) && !defined(sparc) && !defined(hpux) && !defined(__hpux) && !defined(__CYGWIN32__) && !defined(ultrix)
#ifndef _MSC_VER
#include <sys/select.h>
#endif
#endif
#ifdef linux
#include <utime.h>
//#include <bsd/sgtty.h>
#endif /* linux */

#if defined(hpux) || defined(__hpux)
#include <sgtty.h>
#endif

#if defined(sparc) && defined(__unix__)
#if defined(__svr4__) || defined(__USLC__)
#include <dirent.h>
#else
#include <sys/dir.h>
#endif

/* dorks */
#undef NL0
#undef NL1
#undef CR0
#undef CR1
#undef CR2
#undef CR3
#undef TAB0
#undef TAB1
#undef TAB2
#undef XTABS
#undef BS0
#undef BS1
#undef FF0
#undef FF1
#undef ECHO
#undef NOFLSH
#undef TOSTOP
#undef FLUSHO
#undef PENDIN
#endif

#if defined(hpux) || defined(__hpux)
#undef CR0
#endif

#ifdef __FreeBSD__
#include <sys/ioctl_compat.h>
#else /* !__FreeBSD__ */
#ifndef _MSC_VER
#include <termio.h>
#endif
#endif

#if defined(hpux) || defined(__hpux)
/* et tu, dorks! */
#undef HUPCL
#undef ECHO
#undef B50
#undef B75
#undef B110
#undef B134
#undef B150
#undef B200
#undef B300
#undef B600
#undef B1200
#undef B1800
#undef B2400
#undef B4800
#undef B9600
#undef B19200
#undef B38400
#undef NL0
#undef NL1
#undef CR0
#undef CR1
#undef CR2
#undef CR3
#undef TAB0
#undef TAB1
#undef BS0
#undef BS1
#undef FF0
#undef FF1
#undef EXTA
#undef EXTB
#undef B900
#undef B3600
#undef B7200
#undef XTABS
#include <sgtty.h>
#include <utime.h>
#endif



#ifdef _MSC_VER
#define access		_access
#define chmod		_chmod
#define chdir		_chdir
#define unlink		_unlink
#define open		_open
#define creat		_creat
#define pipe		_pipe
#define dup		_dup
#define dup2		_dup2
#define stat		_stat
#define fstat		_fstat
#define lseek		_lseek
#define read		_read
#define write		_write
#define close		_close
#define getpid		_getpid
#define utime		_utime
#include <sys/utime.h>
#endif /* _MSC_VER */

/* SimpleScalar SStrix (a derivative of Ultrix) system call codes, note these
   codes reside in register $r2 at the point a `syscall' inst is executed,
   not all of these codes are implemented, see the main switch statement in
   syscall.c for a list of implemented system calls */

#define SS_SYS_syscall		0
/* SS_SYS_exit was moved to pisa.h */
#define	SS_SYS_fork		2
#define	SS_SYS_read		3
/* SS_SYS_write was moved to pisa.h */
#define	SS_SYS_open		5
#define	SS_SYS_close		6
						/*  7 is old: wait */
#define	SS_SYS_creat		8
#define	SS_SYS_link		9
#define	SS_SYS_unlink		10
#define	SS_SYS_execv		11
#define	SS_SYS_chdir		12
						/* 13 is old: time */
#define	SS_SYS_mknod		14
#define	SS_SYS_chmod		15
#define	SS_SYS_chown		16
#define	SS_SYS_brk		17		/* 17 is old: sbreak */
						/* 18 is old: stat */
#define	SS_SYS_lseek		19
#define	SS_SYS_getpid		20
#define	SS_SYS_mount		21
#define	SS_SYS_umount		22
						/* 23 is old: setuid */
#define	SS_SYS_getuid		24
						/* 25 is old: stime */
#define	SS_SYS_ptrace		26
						/* 27 is old: alarm */
						/* 28 is old: fstat */
						/* 29 is old: pause */
						/* 30 is old: utime */
						/* 31 is old: stty */
						/* 32 is old: gtty */
#define	SS_SYS_access		33
						/* 34 is old: nice */
						/* 35 is old: ftime */
#define	SS_SYS_sync		36
#define	SS_SYS_kill		37
#define	SS_SYS_stat		38
						/* 39 is old: setpgrp */
#define	SS_SYS_lstat		40
#define	SS_SYS_dup		41
#define	SS_SYS_pipe		42
						/* 43 is old: times */
#define	SS_SYS_profil		44
						/* 45 is unused */
						/* 46 is old: setgid */
#define	SS_SYS_getgid		47
						/* 48 is old: sigsys */
						/* 49 is unused */
						/* 50 is unused */
#define	SS_SYS_acct		51
						/* 52 is old: phys */
						/* 53 is old: syslock */
#define	SS_SYS_ioctl		54
#define	SS_SYS_reboot		55
						/* 56 is old: mpxchan */
#define	SS_SYS_symlink		57
#define	SS_SYS_readlink		58
#define	SS_SYS_execve		59
#define	SS_SYS_umask		60
#define	SS_SYS_chroot		61
#define	SS_SYS_fstat		62
						/* 63 is unused */
#define	SS_SYS_getpagesize 	64
#define	SS_SYS_mremap		65
#define SS_SYS_vfork		66		/* 66 is old: vfork */
						/* 67 is old: vread */
						/* 68 is old: vwrite */
#define	SS_SYS_sbrk		69
#define	SS_SYS_sstk		70
#define	SS_SYS_mmap		71
#define SS_SYS_vadvise		72		/* 72 is old: vadvise */
#define	SS_SYS_munmap		73
#define	SS_SYS_mprotect		74
#define	SS_SYS_madvise		75
#define	SS_SYS_vhangup		76
						/* 77 is old: vlimit */
#define	SS_SYS_mincore		78
#define	SS_SYS_getgroups	79
#define	SS_SYS_setgroups	80
#define	SS_SYS_getpgrp		81
#define	SS_SYS_setpgrp		82
#define	SS_SYS_setitimer	83
#define	SS_SYS_wait3		84
#define	SS_SYS_wait		SYS_wait3
#define	SS_SYS_swapon		85
#define	SS_SYS_getitimer	86
#define	SS_SYS_gethostname	87
#define	SS_SYS_sethostname	88
#define	SS_SYS_getdtablesize	89
#define	SS_SYS_dup2		90
#define	SS_SYS_getdopt		91
#define	SS_SYS_fcntl		92
#define	SS_SYS_select		93
#define	SS_SYS_setdopt		94
#define	SS_SYS_fsync		95
#define	SS_SYS_setpriority	96
#define	SS_SYS_socket		97
#define	SS_SYS_connect		98
#define	SS_SYS_accept		99
#define	SS_SYS_getpriority	100
#define	SS_SYS_send		101
#define	SS_SYS_recv		102
#define SS_SYS_sigreturn	103		/* new sigreturn */
						/* 103 was socketaddr */
#define	SS_SYS_bind		104
#define	SS_SYS_setsockopt	105
#define	SS_SYS_listen		106
						/* 107 was vtimes */
#define	SS_SYS_sigvec		108
#define	SS_SYS_sigblock		109
#define	SS_SYS_sigsetmask	110
#define	SS_SYS_sigpause		111
#define	SS_SYS_sigstack		112
#define	SS_SYS_recvmsg		113
#define	SS_SYS_sendmsg		114
						/* 115 is old vtrace */
#define	SS_SYS_gettimeofday	116
#define	SS_SYS_getrusage	117
#define	SS_SYS_getsockopt	118
						/* 119 is old resuba */
#define	SS_SYS_readv		120
#define	SS_SYS_writev		121
#define	SS_SYS_settimeofday	122
#define	SS_SYS_fchown		123
#define	SS_SYS_fchmod		124
#define	SS_SYS_recvfrom		125
#define	SS_SYS_setreuid		126
#define	SS_SYS_setregid		127
#define	SS_SYS_rename		128
#define	SS_SYS_truncate		129
#define	SS_SYS_ftruncate	130
#define	SS_SYS_flock		131
						/* 132 is unused */
#define	SS_SYS_sendto		133
#define	SS_SYS_shutdown		134
#define	SS_SYS_socketpair	135
#define	SS_SYS_mkdir		136
#define	SS_SYS_rmdir		137
#define	SS_SYS_utimes		138
#define SS_SYS_sigcleanup  	139		/* From 4.2 longjmp */
                                                /* same as SYS_sigreturn */
#define	SS_SYS_adjtime		140
#define	SS_SYS_getpeername	141
#define	SS_SYS_gethostid	142
#define	SS_SYS_sethostid	143
#define	SS_SYS_getrlimit	144
#define	SS_SYS_setrlimit	145
#define	SS_SYS_killpg		146
						/* 147 is unused */
#define	SS_SYS_setquota		148
#define	SS_SYS_quota		149
#define	SS_SYS_getsockname	150

#define SS_SYS_sysmips     	151		/* floating point control */

/* formerly mips local system calls */

#define SS_SYS_cacheflush  	152
#define SS_SYS_cachectl    	153
#define SS_SYS_atomic_op   	155

/* nfs releated system calls */
#define SS_SYS_debug       	154

#define SS_SYS_statfs      	160
#define SS_SYS_fstatfs     	161
#define SS_SYS_unmount     	162

#define SS_SYS_quotactl    	168
/* #define SS_SYS_mount       170 */

#define SS_SYS_hdwconf     	171

/* try to keep binary compatibility with mips */

#define SS_SYS_nfs_svc		158
#define SS_SYS_nfssvc		158 /* cruft - delete when kernel fixed */
#define SS_SYS_nfs_biod		163
#define SS_SYS_async_daemon	163 /* cruft - delete when kernel fixed */
#define SS_SYS_nfs_getfh	164
#define SS_SYS_getfh		164 /* cruft - delete when kernel fixed */
#define SS_SYS_getdirentries	159
#define SS_SYS_getdomainname	165
#define SS_SYS_setdomainname	166
#define SS_SYS_exportfs		169

#define SS_SYS_msgctl		172
#define SS_SYS_msgget		173
#define SS_SYS_msgrcv		174
#define SS_SYS_msgsnd		175
#define SS_SYS_semctl		176
#define SS_SYS_semget		177
#define SS_SYS_semop		178
#define SS_SYS_uname		179
#define SS_SYS_shmsys		180
#define SS_SYS_plock		181
#define SS_SYS_lockf		182
#define SS_SYS_ustat		183
#define SS_SYS_getmnt		184
#define	SS_SYS_sigpending	187
#define	SS_SYS_setsid		188
#define	SS_SYS_waitpid		189

#define	SS_SYS_utc_gettime	233	 /* 233 -- same as OSF/1 */
#define SS_SYS_utc_adjtime	234	 /* 234 -- same as OSF/1 */
#define SS_SYS_audcntl		252
#define SS_SYS_audgen		253
#define SS_SYS_startcpu		254	 /* 254 -- Ultrix Private */
#define SS_SYS_stopcpu		255	 /* 255 -- Ultrix Private */
#define SS_SYS_getsysinfo	256	 /* 256 -- Ultrix Private */
#define SS_SYS_setsysinfo	257	 /* 257 -- Ultrix Private */

/* SStrix ioctl values */
#define SS_IOCTL_TIOCGETP	1074164744
#define SS_IOCTL_TIOCSETP	-2147060727
#define SS_IOCTL_TCGETP		1076130901
#define SS_IOCTL_TCGETA		1075082331
#define SS_IOCTL_TIOCGLTC	1074164852
#define SS_IOCTL_TIOCSLTC	-2147060619
#define SS_IOCTL_TIOCGWINSZ	1074295912
#define SS_IOCTL_TCSETAW	-2146143143
#define SS_IOCTL_TIOCGETC	1074164754
#define SS_IOCTL_TIOCSETC	-2147060719
#define SS_IOCTL_TIOCLBIC	-2147191682
#define SS_IOCTL_TIOCLBIS	-2147191681
#define SS_IOCTL_TIOCLGET	0x4004747c
#define SS_IOCTL_TIOCLSET	-2147191683

/* internal system call buffer size, used primarily for file name arguments,
   argument larger than this will be truncated */
#define MAXBUFSIZE 		1024

/* total bytes to copy from a valid pointer argument for ioctl() calls,
   syscall.c does not decode ioctl() calls to determine the size of the
   arguments that reside in memory, instead, the ioctl() proxy simply copies
   NUM_IOCTL_BYTES bytes from the pointer argument to host memory */
#define NUM_IOCTL_BYTES		128

/* target stat() buffer definition, the host stat buffer format is
   automagically mapped to/from this format in syscall.c */
struct ss_statbuf
{
  shalf_t ss_st_dev;
  shalf_t ss_pad;
  word_t ss_st_ino;
  half_t ss_st_mode;
  shalf_t ss_st_nlink;
  shalf_t ss_st_uid;
  shalf_t ss_st_gid;
  shalf_t ss_st_rdev;
  shalf_t ss_pad1;
  sword_t ss_st_size;
  sword_t ss_st_atime;
  sword_t ss_st_spare1;
  sword_t ss_st_mtime;
  sword_t ss_st_spare2;
  sword_t ss_st_ctime;
  sword_t ss_st_spare3;
  sword_t ss_st_blksize;
  sword_t ss_st_blocks;
  word_t ss_st_gennum;
  sword_t ss_st_spare4;
};

struct ss_timeval
{
  sword_t ss_tv_sec;		/* seconds */
  sword_t ss_tv_usec;		/* microseconds */
};

/* target getrusage() buffer definition, the host stat buffer format is
   automagically mapped to/from this format in syscall.c */
struct ss_rusage
{
  struct ss_timeval ss_ru_utime;
  struct ss_timeval ss_ru_stime;
  sword_t ss_ru_maxrss;
  sword_t ss_ru_ixrss;
  sword_t ss_ru_idrss;
  sword_t ss_ru_isrss;
  sword_t ss_ru_minflt;
  sword_t ss_ru_majflt;
  sword_t ss_ru_nswap;
  sword_t ss_ru_inblock;
  sword_t ss_ru_oublock;
  sword_t ss_ru_msgsnd;
  sword_t ss_ru_msgrcv;
  sword_t ss_ru_nsignals;
  sword_t ss_ru_nvcsw;
  sword_t ss_ru_nivcsw;
};

struct ss_timezone
{
  sword_t ss_tz_minuteswest;	/* minutes west of Greenwich */
  sword_t ss_tz_dsttime;	/* type of dst correction */
};

struct ss_rlimit
{
  int ss_rlim_cur;		/* current (soft) limit */
  int ss_rlim_max;		/* maximum value for rlim_cur */
};

/* open(2) flags for SimpleScalar target, syscall.c automagically maps
   between these codes to/from host open(2) flags */
#define SS_O_RDONLY		0
#define SS_O_WRONLY		1
#define SS_O_RDWR		2
#define SS_O_APPEND		0x0008
#define SS_O_CREAT		0x0200
#define SS_O_TRUNC		0x0400
#define SS_O_EXCL		0x0800
#define SS_O_NONBLOCK		0x4000
#define SS_O_NOCTTY		0x8000
#define SS_O_SYNC		0x2000

/* open(2) flags translation table for SimpleScalar target */
struct {
  int ss_flag;
  int local_flag;
} ss_flag_table[] = {
  /* target flag */	/* host flag */
#ifdef _MSC_VER
  { SS_O_RDONLY,	_O_RDONLY },
  { SS_O_WRONLY,	_O_WRONLY },
  { SS_O_RDWR,		_O_RDWR },
  { SS_O_APPEND,	_O_APPEND },
  { SS_O_CREAT,		_O_CREAT },
  { SS_O_TRUNC,		_O_TRUNC },
  { SS_O_EXCL,		_O_EXCL },
#ifdef _O_NONBLOCK
  { SS_O_NONBLOCK,	_O_NONBLOCK },
#endif
#ifdef _O_NOCTTY
  { SS_O_NOCTTY,	_O_NOCTTY },
#endif
#ifdef _O_SYNC
  { SS_O_SYNC,		_O_SYNC },
#endif
#else /* !_MSC_VER */
  { SS_O_RDONLY,	O_RDONLY },
  { SS_O_WRONLY,	O_WRONLY },
  { SS_O_RDWR,		O_RDWR },
  { SS_O_APPEND,	O_APPEND },
  { SS_O_CREAT,		O_CREAT },
  { SS_O_TRUNC,		O_TRUNC },
  { SS_O_EXCL,		O_EXCL },
  { SS_O_NONBLOCK,	O_NONBLOCK },
  { SS_O_NOCTTY,	O_NOCTTY },
#ifdef O_SYNC
  { SS_O_SYNC,		O_SYNC },
#endif
#endif /* _MSC_VER */
};
#define SS_NFLAGS	(sizeof(ss_flag_table)/sizeof(ss_flag_table[0]))

class sim;


class syscalle
{
	private:
		sim *Sim;

	public:
		syscalle(sim*);
		//syscalle(counter_t *c);
		//syscalle(FILE *p);

		/* syscall proxy handler, architect registers and memory are assumed to be
		   precise when this function is called, register and memory are updated with
		   the results of the sustem call */
		void
		sys_syscall(struct regs_t *regs,	/* registers to access */
			    /*mem_access_fn mem_fn,*/	/* generic memory accessor */
			    struct mem_t *mem,		/* memory space to access */
			    md_inst_t inst,		/* system call inst */
			    int traceable
				);

};

#endif

