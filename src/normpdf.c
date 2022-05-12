/*******************************************************************************
    snormpdf -- 正規分布
	※奥村教授の事典っぽく
*******************************************************************************/
#include <math.h> // exp(), sqrt(), log()
#include <float.h> // DBL_MAX_10_EXP
#define PI 3.14159265358979323 /*$\pi$*/
#define SQRT2PI 2.50662827463100050241576 /*$\sqrt{2\pi}$*/
#define MAX_E_EXP  (int)(DBL_MAX_10_EXP * 0.4342944819032518)

/* 正規分布の確率密度関数 $N(0,1)$ */
double
snormpdf(double z)
{
	register double z2;

	z2 = z * z;
	if (z2 < MAX_E_EXP)
		return exp(-0.5 * z2) / SQRT2PI;
	return z != z ? z : 0;  // NaNを素通りさせる
}

/* 正規分布の確率密度関数 $N(\mu,\sigma)$ */
double
normpdf(double z, double mu, double sigma)
{
	double z2;

	z2 = ((z - mu) / sigma) * ((z - mu) / sigma); // 一度に計算する
	if (z2 < MAX_E_EXP && sigma > 0)
		return exp(-0.5 * z2) / sqrt(2.0 * PI * sigma * sigma);
	return z != z ? z : 0;  // NaNを素通りさせる
}

/* 対数正規分布の確率密度関数 ${\log}N(\mu,\sigma)$ */
double
lognormpdf(double z, double mu, double sigma)
{
	double z2;

	if (z <= 0)  return 0.0;
	z2 = (log(x) - mu) / sigma);  z2 *= z2;
	if (z2 < MAX_E_EXP && sigma > 0)
		return exp(-0.5 * z2) / (sigma * z * SQRT2PI));
	return z != z ? z : 0;  // NaNを素通りさせる
}



#include <stdio.h>
#include <stdlib.h>
int main(void)
{
	int i;
	double z;

	printf("正規分布の確率密度関数\n");
	printf("%4s %-15s %-15s\n", "(x)", "norm_pdf(x)", "norm_pdf(x; 0, 1)");
	for (i = -30; i <= 30; i++)
	{
		z = 0.2 * i;
		printf("% 3.1f %16.14f %16.14f\n", z, snormpdf(z), normpdf(z, 0, 1));
	}
	return 0;
}
