/* Determine CPU support for SIMD on Power PC
 * Copyright 2004 Phil Karn, KA9Q
 */
#include <stdio.h>
#include "fec.h"

#if HAVE_SYS_SYSCTL_H && HAVE_SYSCTL && defined(CTL_HW) && defined(HW_VECTORUNIT)
#include <sys/sysctl.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <linux/auxvec.h>
#include <asm/cputable.h>
#endif


/* Various SIMD instruction set names */
char *Cpu_modes[] = {"Unknown","Portable C","x86 Multi Media Extensions (MMX)",
		   "x86 Streaming SIMD Extensions (SSE)",
		   "x86 Streaming SIMD Extensions 2 (SSE2)",
		   "PowerPC G4/G5 Altivec/Velocity Engine"};

enum cpu_mode Cpu_mode;

void find_cpu_mode(void){

  if(Cpu_mode != UNKNOWN)
    return;

#if HAVE_SYS_SYSCTL_H && HAVE_SYSCTL && defined(CTL_HW) && defined(HW_VECTORUNIT)
  {
  /* Ask the OS if we have Altivec support */
  int selectors[2] = { CTL_HW, HW_VECTORUNIT };
  int hasVectorUnit = 0;
  size_t length = sizeof(hasVectorUnit);
  int error = sysctl(selectors, 2, &hasVectorUnit, &length, NULL, 0);
  if(0 == error && hasVectorUnit)
    Cpu_mode = ALTIVEC;
  else
    Cpu_mode = PORT;
  }
#else
    int result = 0;
    unsigned long buf[64];
    ssize_t count;
    int fd, i;

    fd = open("/proc/self/auxv", O_RDONLY);
    if (fd < 0) {
	Cpu_mode = PORT;
        return;
    }
    // loop on reading
    do {
        count = read(fd, buf, sizeof(buf));
        if (count < 0)
            break;
        for (i=0; i < (count / sizeof(unsigned long)); i += 2) {
            if (buf[i] == AT_HWCAP) {
                result = !!(buf[i+1] & PPC_FEATURE_HAS_ALTIVEC);
                goto out_close;
            } else if (buf[i] == AT_NULL)
                goto out_close;
        }
    } while (count == sizeof(buf));
out_close:
    close(fd);
    Cpu_mode = result ? ALTIVEC : PORT;
#endif

  fprintf(stderr,"SIMD CPU detect: %s\n",Cpu_modes[Cpu_mode]);
}
