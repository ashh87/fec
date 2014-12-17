/* Test dot-product function */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include "config.h"
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#include "fec.h"

#include <time.h>

#if HAVE_GETOPT_LONG
struct option Options[] = {
  {"force-altivec",0,NULL,'a'},
  {"force-port",0,NULL,'p'},
  {"force-mmx",0,NULL,'m'},
  {"force-sse",0,NULL,'s'},
  {"force-sse2",0,NULL,'t'},
  {"trials",0,NULL,'n'},
  {NULL},
};
#endif

int main(int argc,char *argv[]){
  short coeffs[512];
  short input[2048];
  int trials=100,d,nt;
  int errors = 0;
  clock_t start1, start2, end1, end2;
  double t1, t2;
  double m1 = 0, m2 = 0;

#if HAVE_GETOPT_LONG
  while((d = getopt_long(argc,argv,"apmstn:",Options,NULL)) != EOF){
#else
  while((d = getopt(argc,argv,"apmstn:")) != EOF){
#endif
    switch(d){
    case 'a':
      Cpu_mode = ALTIVEC;
      break;
    case 'p':
      Cpu_mode = PORT;
      break;
    case 'm':
      Cpu_mode = MMX;
      break;
    case 's':
      Cpu_mode = SSE;
      break;
    case 't':
      Cpu_mode = SSE2;
      break;
    case 'n':
      trials = atoi(optarg);
      break;
    }
  }
  nt = trials;
  while(trials--){
    long port_result;
    long simd_result;
    int ntaps;
    int i;
    int csum = 0;
    int offset;
    void *dp_simd,*dp_port;

    /* Generate set of coefficients
     * limit sum of absolute values to 32767 to avoid overflow
     */
    memset(coeffs,0,sizeof(coeffs));
    for(i=0;i<512;i++){
      double gv;

      gv = normal_rand(0.,100.);
      if(csum + fabs(gv) > 32767)
	break;
      coeffs[i] = gv;
      csum += fabs(gv);
    }
    ntaps = i;

    /* Compare results to portable C version for a bunch of random data buffers and offsets */
    dp_simd = initdp(coeffs,ntaps);
    dp_port = initdp_port(coeffs,ntaps);
    
    for(i=0;i<2048;i++)
      input[i] = random();
    
    offset = random() & 511;
	start1 = clock();
    simd_result = dotprod(dp_simd,input+offset);
    end1 = clock();
	t1 =  ((double)(end1-start1));
	m1 += t1;

    start2 = clock();
    port_result = dotprod_port(dp_port,input+offset);
    end2 = clock();
	t2 =  ((double)(end2-start2));
	m2 += t2;

    if(simd_result != port_result){
      errors++;
    }
  }
  m1 /= nt;
  m2 /= nt;
  printf("dtest: %d errors\navg simd: %f, avg port: %f\n",errors, m1, m2);
  exit(0);
}
