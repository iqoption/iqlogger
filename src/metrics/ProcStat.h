//
// ProcStat.h
// ~~~~~~~~~~~~~~~~
//
// Copyright (C) 2018 IQOption Software, Inc.
//
//

#pragma once

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

#include <atomic>
#include <string>

namespace iqlogger::metrics::procstat {

    // http://man7.org/linux/man-pages/man5/proc.5.html

    typedef struct procstat {

        int                     pid;                    /**  (1) %d   The process ID. **/
        char                    comm[_POSIX_PATH_MAX];  /**  (2) %s   The filename of the executable, in parentheses. **/
        char                    state;                  /**  (3) %c   Process state **/
        int                     ppid;                   /**  (4) %d   The PID of the parent of this process. **/
        int                     pgrp;                   /**  (5) %d   The process group ID of the process. **/
        int                     session;                /**  (6) %d   The session ID of the process. **/
        int                     tty_nr;                 /**  (7) %d   The controlling terminal of the process. **/
        int                     tpgid;                  /**  (8) %d   The ID of the foreground process group of the con‐trolling terminal of the process. **/
        unsigned int            flags;                  /**  (9) %u   The kernel flags word of the process. **/
        unsigned long int	    minflt;                 /** (10) %lu  The number of minor faults **/
        unsigned long int       cminflt;                /** (11) %lu  The number of minor faults with childs **/
        unsigned long int       majflt;                 /** (12) %lu  The number of major faults **/
        unsigned long int       cmajflt;                /** (13) %lu  The number of major faults with childs **/
        unsigned long int       utime;                  /** (14) %lu  Amount of time that this process has been scheduled in user mode, measured in clock ticks **/
        unsigned long int       stime;                  /** (15) %lu  Amount of time that this process has been scheduled in kernel mode, measured in clock ticks **/
        long int                cutime;                 /** (16) %ld  Amount of time that this process's waited-for children have been scheduled in user mode **/
        long int                cstime;                 /** (17) %ld  Amount of time that this process's waited-for children have been scheduled in kernel mode **/
        long int                priority;               /** (18) %ld  Negated scheduling priority **/
        long int                nice;                   /** (19) %ld  The nice value **/
        long int                num_threads;            /** (20) %ld  Number of threads in this process **/
        long int                itrealvalue;            /** (21) %ld  The time in jiffies before the next SIGALRM is sent **/
        unsigned long long int  starttime;              /** (22) %llu The time the process started after system boot. **/
        unsigned long int       vsize;                  /** (23) %lu  Virtual memory size in bytes. **/
        long int                rss;                    /** (24) %ld  Resident Set Size: number of pages the process has in real memory. **/
        unsigned long int       rsslim;                 /** (25) %lu  Current soft limit in bytes on the rss of the process. **/
        unsigned long int       startcode;              /** (26) %lu  The address above which program text can run. **/
        unsigned long int       endcode;                /** (27) %lu  The address below which program text can run. **/
        unsigned long int       startstack;             /** (28) %lu  The address of the start (i.e., bottom) of the stack. **/
        unsigned long int       kstkesp;                /** (29) %lu  The current value of ESP (stack pointer), as found in the kernel stack page for the process. **/
        unsigned long int       kstkeip;                /** (30) %lu  The current EIP (instruction pointer). **/
        unsigned long int       signal;                 /** (31) %lu  The bitmap of pending signals, displayed as a decimal number. **/
        unsigned long int       blocked;                /** (32) %lu  The bitmap of blocked signals, displayed as a decimal number. **/
        unsigned long int       sigignore;              /** (33) %lu  The bitmap of ignored signals, displayed as a decimal number. **/
        unsigned long int       sigcatch;               /** (34) %lu  The bitmap of caught signals, displayed as a decimal number. **/
        unsigned long int       wchan;                  /** (35) %lu  This is the "channel" in which the process is waiting. **/
        unsigned long int       nswap;                  /** (36) %lu  Number of pages swapped (not maintained). **/
        unsigned long int       cnswap;                 /** (37) %lu  Cumulative nswap for child processes (not maintained). **/
        int                     exit_signal;            /** (38) %d   Signal to be sent to parent when we die. **/
        int                     processor;              /** (39) %d   CPU number last executed on. **/
        unsigned int            rt_priority;            /** (40) %u   Real-time scheduling priority. **/
        unsigned int            policy;                 /** (41) %u   Scheduling policy (see sched_setscheduler(2)). **/
        unsigned long long int  delayacct_blkio_ticks;  /** (42) %llu Aggregated block I/O delays, measured in clock ticks (centiseconds). **/
        unsigned long int       guest_time;             /** (43) %lu  Guest time of the process (time spent running a virtual CPU for a guest operating system) **/
        long int                cguest_time;            /** (44) %ld  Guest time of the process's children, measured in clock ticks. **/
        unsigned long int       start_data;             /** (45) %lu  Address above which program initialized and uninitialized (BSS) data are placed. **/
        unsigned long int       end_data;               /** (46) %lu  Address below which program initialized and uninitialized (BSS) data are placed. **/
        unsigned long int       start_brk;              /** (47) %lu  Address above which program heap can be expanded with brk(2). **/
        unsigned long int       arg_start;              /** (48) %lu  Address above which program command-line arguments (argv) are placed. **/
        unsigned long int       arg_end;                /** (49) %lu  Address below program command-line arguments (argv) are placed. **/
        unsigned long int       env_start;              /** (50) %lu  Address above which program environment is placed. **/
        unsigned long int       env_end;                /** (51) %lu  Address below which program environment is placed. **/
        int                     exit_code;              /** (52) %d   The thread's exit status in the form reported by waitpid(2). **/
    } procstat;

    static inline int get_proc_info(pid_t pid, procstat * pinfo)
    {
        char szFileName[_POSIX_PATH_MAX], szStatStr[2048];
        FILE *fp;

        if (NULL == pinfo)
        {
            errno = EINVAL;
            return -1;
        }

        sprintf(szFileName, "/proc/%u/stat", (unsigned)pid);

        if (-1 == access(szFileName, R_OK))
        {
            return (pinfo->pid = -1);
        }

        if ((fp = fopen(szFileName, "r")) == NULL)
        {
            return (pinfo->pid = -1);
        }

        if (fgets(szStatStr, 2048, fp) == NULL)
        {
            fclose(fp);
            return (pinfo->pid = -1);
        }

        sscanf(szStatStr,
                "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d %u %u %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %d",
//                1  2  3  4  5  6  7  8  9  10  11  12  13  14  15  16  17  18  19  20  21  22   23  24  25  26  27  28  29  30  31  32  33  34  35  36  37 38 39 40 41   42  43  44  45  46  47  48  49  50  51 52
               &(pinfo->pid),
               pinfo->comm,
               &(pinfo->state),
               &(pinfo->ppid),
               &(pinfo->pgrp),
               &(pinfo->session),
               &(pinfo->tty_nr),
               &(pinfo->tpgid),
               &(pinfo->flags),
               &(pinfo->minflt),
               &(pinfo->cminflt),
               &(pinfo->majflt),
               &(pinfo->cmajflt),
               &(pinfo->utime),
               &(pinfo->stime),
               &(pinfo->cutime),
               &(pinfo->cstime),
               &(pinfo->priority),
               &(pinfo->nice),
               &(pinfo->num_threads),
               &(pinfo->itrealvalue),
               &(pinfo->starttime),
               &(pinfo->vsize),
               &(pinfo->rss),
               &(pinfo->rsslim),
               &(pinfo->startcode),
               &(pinfo->endcode),
               &(pinfo->startstack),
               &(pinfo->kstkesp),
               &(pinfo->kstkeip),
               &(pinfo->signal),
               &(pinfo->blocked),
               &(pinfo->sigignore),
               &(pinfo->sigcatch),
               &(pinfo->wchan),
               &(pinfo->nswap),
               &(pinfo->cnswap),
               &(pinfo->exit_signal),
               &(pinfo->processor),
               &(pinfo->rt_priority),
               &(pinfo->policy),
               &(pinfo->delayacct_blkio_ticks),
               &(pinfo->guest_time),
               &(pinfo->cguest_time),
               &(pinfo->start_data),
               &(pinfo->end_data),
               &(pinfo->start_brk),
               &(pinfo->arg_start),
               &(pinfo->arg_end),
               &(pinfo->env_start),
               &(pinfo->env_end),
               &(pinfo->exit_code)
        );

        fclose(fp);
        return 0;

    }
}