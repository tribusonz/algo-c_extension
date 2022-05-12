/*******************************************************************************
	nnint.c -- 床・天井
	※奥村教授の事典っぽく。第三版を出すというお話をいただいて
*******************************************************************************/
#include <math.h> /* frexp(), ldexp(), isfinite() */

/* 床関数の元 */
static inline double
floor_bisect(double x)
{
	int base_exp;
	double frac, eq, s, t;
	
	if (x < 2)
		return (double)(int)x;
	
	// use frexp
	frac = frexp(x, &base_exp);
	base_exp--; 
	frac = (frac-0.5)*2;
	
	s = 0.0; t = 1.0 / 2;

	for (volatile int i = 0; i < base_exp; i++)
	{
		if (frac >= (s + t))  s += t;
		if (s == frac)  break;
		t /= 2;
	}

	eq = ldexp(1.0, base_exp);

	return eq + eq * s;

}

/* 天井関数の元  */
static inline double
ceil_bisect(double x)
{
	int base_exp;
	double frac, eq, s, t;
	
	if (x < 2)
		return (int)x == x ? x : (double)(int)(x + 1);
	
	// use frexp
	frac = frexp(x, &base_exp);
	base_exp--; 
	frac = (frac-0.5)*2;
	
	s = 0.0; t = 1.0 / 2;

	for (volatile int i = 0; i < base_exp; i++)
	{
		if (frac >= (s + t))  s += t;
		if (s == frac)  break;
		t /= 2;
	}

	eq = ldexp(1.0, base_exp);

	return s == frac ? eq + eq * s : eq + eq * (s + 1.0 / eq);
}

/* 床関数 $\left\lfloor{x}\right\rfloor$ */
double
my_floor(double x)
{
	if (isfinite(x)) // 有限のみ入場
		return x < 0 ? -ceil_bisect(-x) : floor_bisect(x);
	return x; // 他は退場
}

/* 天井関数 $\left\lceil{x}\right\rceil$ */
double
my_ceil(double x)
{
	if (isfinite(x)) // 有限のみ入場
		return x < 0 ? -floor_bisect(-x) : ceil_bisect(x);
	return x; // 他は退場
}

/* trunc */
double
my_trunc(double)
{
	if (isfinite(x)) // 有限のみ入場
		return x < 0 ? -floor_bisect(-x) : floor_bisect(x);
	return x; // 他は退場
}


#include <stdio.h>
#include <stdlib.h>
int
main(void)
{
	double x;
	puts("test of floor, ceil");
	printf("x = ? "); scanf("%lf", &x);
	printf("floor(%f) = %g\n", x, my_floor(x));
	printf("ceil(%f) = %g\n", x, my_ceil(x));
	
	return 0;
}
