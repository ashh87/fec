/* Determine CPU support for SIMD
 * Copyright 2004 Phil Karn, KA9Q
 */
#include <stdio.h>
#include "fec.h"

/* Various SIMD instruction set names */
char *Cpu_modes[] = {"Unknown","Portable C","x86 Multi Media Extensions (MMX)",
		   "x86 Streaming SIMD Extensions (SSE)",
		   "x86 Streaming SIMD Extensions 2 (SSE2)",
		   "PowerPC G4/G5 Altivec/Velocity Engine",
		   "x86 Streaming SIMD Extensions 3 (SSE3)",
		   "x86 Supplemental Streaming SIMD Extensions 3 (SSSE3)",
		   "x86 Streaming SIMD Extensions 4.1 (SSE4_1)",
		   "x86 Streaming SIMD Extensions 4.2 (SSE4_2)",
		   "x86 Advanced Vector Extensions (AVX)",
		   "x86 Advanced Vector Extensions 2 (AVX2)",
		   "x86 Advanced Vector Extensions 512 bit Foundation (AVX-512F)"
};
/* Ignoring the rest of AVX-512 for now */

enum cpu_mode Cpu_mode;

void find_cpu_mode(void){

  unsigned int ebx, ecx, edx, dummy, ebx_extended = 0, ecx_extended = 0;
  int max_leaf;
  if(Cpu_mode != UNKNOWN)
    return;

  /* NB: I think all x64 should have a minimum of SSE2 */

  /* Figure out what kind of CPU we have */
  /* eax = cpu_features_64(1, &ebx, &ecx, &edx); */

  /* highest cpuid leaf from eax */
  max_leaf = cpu_features_64(0, &dummy, &dummy, &dummy);

  cpu_features_64(1, &ebx, &ecx, &edx);

  /* details about AVX2 and above are in leaf 7, subleaf 0,
   * which is only readable if IA32_MISC_ENABLES.BOOT_NT4 ==0
   * which we will assume on Linux
   */
  if (max_leaf >= 7)
    cpu_features_extended_64(7, &ebx_extended, &ecx_extended, &dummy);

  if(ebx_extended & (1<<16)) { /* AVX-512F is present */
	Cpu_mode = AVX_512F;
  }else if(ebx_extended & (1<<5)) { /* AVX2 is present */
	Cpu_mode = AVX2;
  } else if(ecx & (1<<28)) { /* AVX is present */
	Cpu_mode = AVX;
  } else if(ecx & (1<<20)) { /* SSE4_2 is present */
	Cpu_mode = SSE4_2;
  } else if(ecx & (1<<19)) { /* SSE4_1 is present */
	Cpu_mode = SSE4_1;
  } else if(ecx & (1<<9)) { /* SSSE3 is present */
	Cpu_mode = SSSE3;
  } else if(ecx & 1) { /* SSE3 is present */
	Cpu_mode = SSE3;
  } else if(edx & (1<<26)){ /* SSE2 is present */
    Cpu_mode = SSE2;
  } else if(edx & (1<<25)){ /* SSE is present */
    Cpu_mode = SSE;
  } else if(edx & (1<<23)){ /* MMX is present */
    Cpu_mode = MMX;
  } else { /* No SIMD at all */
    Cpu_mode = PORT;
  }
  fprintf(stderr,"SIMD CPU detect: %s\n",Cpu_modes[Cpu_mode]);
}
