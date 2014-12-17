/* Reed-Solomon encoder
 * Copyright 2004, Phil Karn, KA9Q
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 */
#include <string.h>
#include "fixed.h"
#ifdef __VEC__

#if HAVE_SYS_SYSCTL_H && HAVE_SYSCTL && defined(CTL_HW) && defined(HW_VECTORUNIT)
#include <sys/sysctl.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <linux/auxvec.h>
#include <asm/cputable.h>
#endif

#endif


static enum {UNKNOWN=0,MMX,SSE,SSE2,ALTIVEC,PORT} cpu_mode;

static void encode_rs_8_c(data_t *data, data_t *parity,int pad);
#if __vec__
static void encode_rs_8_av(data_t *data, data_t *parity,int pad);
#endif
#if __i386__
int cpu_features(void);
#endif

void encode_rs_8(data_t *data, data_t *parity,int pad){
  if(cpu_mode == UNKNOWN){
#ifdef __i386__
    int f;
    /* Figure out what kind of CPU we have */
    f = cpu_features();
    if(f & (1<<26)){ /* SSE2 is present */
      cpu_mode = SSE2;
    } else if(f & (1<<25)){ /* SSE is present */
      cpu_mode = SSE;
    } else if(f & (1<<23)){ /* MMX is present */
      cpu_mode = MMX;
    } else { /* No SIMD at all */
      cpu_mode = PORT;
    }
#elif __VEC__

#if HAVE_SYS_SYSCTL_H && HAVE_SYSCTL && defined(CTL_HW) && defined(HW_VECTORUNIT)
    /* Ask the OS if we have Altivec support */
    int selectors[2] = { CTL_HW, HW_VECTORUNIT };
    int hasVectorUnit = 0;
    size_t length = sizeof(hasVectorUnit);
    int error = sysctl(selectors, 2, &hasVectorUnit, &length, NULL, 0);
    if(0 == error && hasVectorUnit)
      cpu_mode = ALTIVEC;
    else
      cpu_mode = PORT;
#else
    int result = 0;
    unsigned long buf[64];
    ssize_t count;
    int fd, i;

    fd = open("/proc/self/auxv", O_RDONLY);
    if (fd < 0) {
	cpu_mode = PORT;
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
    cpu_mode = result ? ALTIVEC : PORT;
#endif
#else
    cpu_mode = PORT;
#endif
  }
  switch(cpu_mode){
#if __vec__
  case ALTIVEC:
    encode_rs_8_av(data,parity,pad);
    return;
#endif
#if __i386__
  case MMX:
  case SSE:
  case SSE2:
#endif
  default:
    encode_rs_8_c(data,parity,pad);
    return;
  }
}

#if __vec__ /* PowerPC G4/G5 Altivec instructions are available */

static vector unsigned char reverse = (vector unsigned char)(0,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1);
static vector unsigned char shift_right = (vector unsigned char)(15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30);

/* Lookup table for feedback multiplications
 * These are the low half of the coefficients. Since the generator polynomial is
 * palindromic, we form the other half by reversing this one
 */
extern static union { vector unsigned char v; unsigned char c[16]; } table[256];

static void encode_rs_8_av(data_t *data, data_t *parity,int pad){
  union { vector unsigned char v[2]; unsigned char c[32]; } shift_register;
  int i;

  shift_register.v[0] = (vector unsigned char)(0);
  shift_register.v[1] = (vector unsigned char)(0);
  
  for(i=0;i<NN-NROOTS-pad;i++){
    vector unsigned char feedback0,feedback1;
    unsigned char f;

    f = data[i] ^ shift_register.c[31];
    feedback1 = table[f].v;
    feedback0 = vec_perm(feedback1,feedback1,reverse);

    /* Shift right one byte */
    shift_register.v[1] = vec_perm(shift_register.v[0],shift_register.v[1],shift_right) ^ feedback1;
    shift_register.v[0] = vec_sro(shift_register.v[0],(vector unsigned char)(8)) ^ feedback0;
    shift_register.c[0] = f;
  }
  for(i=0;i<NROOTS;i++)
    parity[NROOTS-i-1] = shift_register.c[i];
}
#endif

/* Portable C version */
static void encode_rs_8_c(data_t *data, data_t *parity,int pad){

#include "encode_rs.h"

}
