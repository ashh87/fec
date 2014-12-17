 //SIMD SSE2 dot product
 //Equivalent to the following C code:
 long dotprod_mmx_assist_64(signed short *a,signed short *b,int cnt)
 {
	long sum = 0; 
	cnt *= 8; 
	while(cnt--)
		sum += *a++ + *b++;
	return sum;
 }


