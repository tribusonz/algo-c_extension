/***********************************************************
	binomial.c -- 2項分布
***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#define N_MAX 1000
#define ITERATOR_AN_END 1

// インタフェース
typedef struct BinomParam {
	int k, n;
	double p, q, s, t;
	int status;
} binom_param_t ;

void binom_param_nu_set(binom_param_t *, int);
void binom_param_phi_set(binom_param_t *, double);
static double binom_logfact(double, double, int, int);
double binompmf(int, int, double);
void binom_enum_initialize(int, double, binom_param_t *);

void
error(char *s)
{
	printf("%s\n", s);
	exit(1);
}

/* ベルヌーイ試行をセットする関数 */
void
binom_param_nu_set(binom_param_t *param, int n)
{
	if (n <= 0 || n > N_MAX)
		error("nが小さいか大きすぎます");
	param->n = n;
}

/* 成功率をセットする関数 */
void
binom_param_phi_set(binom_param_t *param, double p)
{
	if (p < 0 || p > 1)
		error("pが百分率ではありません");
	param->p = p;
}

/* 対数階乗による確率質量関数ルーチン */
static double
binom_logfact(double p, double q, int m, int n)
{ 
	register double temp = lgamma(m + n + 1.0);
	temp -= lgamma(n + 1.0) + lgamma(m + 1.0);
	temp += m * log(p) + n * log(q);
	return exp(temp);
}

/* 確率質量関数ルーチン */
double
binompmf(int x, int n, double p)
{
	if (x < 0 || x > n)  return 0.0;
	if (p < 0.0 || p > 1.0)  return NAN;
	return binom_logfact(p, 1-p, x, n-x);
}

/* 二項分布のEnumerator */
void
binom_enum_initialize(int n, double p, binom_param_t *param)
{
	binom_param_nu_set(param, n);
	binom_param_phi_set(param, p);
	param->k = 0;
	param->q = 1 - p;
	param->s = param->t = pow(param->q, n);
	if (param->s == 0) param->s = param->t = binompmf(0, n, p);	
	if (param->s == 0) error("n か p が大きすぎます");
	param->status = 0;
}

/* yield */
void
binom_each_yield(binom_param_t *param)
{
	if (param->k < param->n)
	{
		param->t *= (param->n - param->k) * param->p / ((param->k + 1) * param->q);
		param->s += param->t;
		if (param->s >= 1)  param->s = 0.5+0.5-DBL_EPSILON;
		if (param->k == param->n - 1)  param->s = 1.0;
		param->k++;
	}
	else
		param->status = ITERATOR_AN_END;
}

int
main(void)
{
	binom_param_t param;
	int n;
	double p;

	printf("n, p? ");  scanf("%d%lf", &n, &p);
	binom_enum_initialize(n, p, &param);
	
	puts(" (n) CDF               PMF");
	while (param.status != ITERATOR_AN_END) {
		printf("%4d %16.15f %16.15f\n", param.k, param.s, param.t);
		binom_each_yield(&param);
	}
	return 0;
}
