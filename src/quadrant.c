/***********************************************************
	quadrant -- 象限
***********************************************************/
#include <math.h> /* fpclassify(), atan(), atan2() */
#define PI 3.14159265358979323846

double
quadrant(double x, double y)
{
	double atanxy = 0.0;
	int kind = 0;  // 0:= arg, 1:= quadrant, 2:= radian
	int quadrant = 0;

	switch (fpclassify(y)) {
	case FP_NAN:
		return NAN;
		break;
	case FP_INFINITE:
		// switch radian
		kind = 2;
		break;
	case FP_ZERO:
	case FP_SUBNORMAL:
		// switch argument of complex
		kind = 0;
		break;
	default:
		// switch quadrant
		kind = 1;
		break;
	}
	switch (fpclassify(x)) {
	case FP_NAN:
		return NAN;
		break;
	case FP_INFINITE:
		switch (kind) {
		case 0:
			kind = 0;  // re-correction to kind is complex argument
			quadrant = -1;  // mark zero
			break;
		case 1:
			kind = 2;  // re-correction to kind is radian
			quadrant = x < 0 ? 7 : 5;
			break;
		case 2:
			quadrant = y >= 0 ? x >= 0 ? 1 : 2 : x < 0 ? 3 : 4;
			break;
		default:
			break;
		}
		break;
	case FP_ZERO:
	case FP_SUBNORMAL:
		switch (kind) {
		case 0:
			quadrant = -3;
			break;
		case 1:
		case 2:
			kind = 0;  // re-correction to kind is arg
			quadrant = -2;  // calculate complex-arg
			break;
		default:
			break;
		}
		break;
	default:
		switch (kind) {
		case 0:
			quadrant = -1;  // calculate complex-absolute
			break;
		case 1:
			quadrant = y >= 0 ? x >= 0 ? 1 : 2 : x < 0 ? 3 : 4;
			x = fabs(x); y = fabs(y);
			break;
		case 2: // y is infinite and x is finite
			quadrant = y < 0 ? 8 : 6;
			break;
		default:
			break;
		}
		break;
	}
	switch (kind) {
	case 0:  // arg
		switch (quadrant) {
		case -1:  // Absolute
			return x < 0 ? PI : 0;  // $\pi$
			break;
		case -2:  // Arg
			return y < 0 ? -0.5 * PI : 0.5 * PI;  // $\frac{1}{2}\pi$
			break;
		case -3:
			return 0.0; // arg(0+0i) is originally the undefined but returns 0.0
			break;
		default:
			break;
		}
		break;
	case 1:  // quadrant
		atanxy = atan(y / x);
		switch (quadrant) {
		case 1:
			return atanxy;
			break;
		case 2:
			return PI - atanxy;
			break;
		case 3:
			return atanxy - PI;
			break;
		case 4:
			return -atanxy;
			break;
		default:
			break;
		}
		break;
	case 2:  // radian
		switch (quadrant) {
		case 1: // 1st: $45^{\circ}$
			return 0.25 * PI;
			break;
		case 2: // 2nd: $-45^{\circ}$
			return 3.0/4.0 * PI;
			break;
		case 3: // 3rd: $-135^{\circ}$
			return -3.0/4.0 * PI;
			break;
		case 4: // 4th: $135^{\circ}$
			return -0.25 * PI;
			break;
		case 5: // $\rm{just} 0^{\circ}$
			return y < 0 ? -0.0 : 0.0; // 0-deg is originally the undefined but return 0
			break;
		case 6: // $\rm{just} 90^{\circ}$
			return 0.5 * PI; // $\frac{1}{2}\pi$
			break;
		case 7: // $\rm{just} 180^{\circ}$
			return y < 0 ? -PI : PI; // $\frac{1}{2}\pi$
			break;
		case 8: // $\rm{just} 270^{\circ}$
			return -0.5 * PI; // $\neg\frac{1}{2}\pi$
			break;
		default:
			break;
		}
		break;
	default:
		return NAN;
		break;
	}
	return NAN;
}


/******************************************************************************/
// 以下はテスト
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#define A_SIZE 6
#define B_SIZE 4
#define C_SIZE 12

// List
static const char at[] = "complex argument switched by zero";
static const char bt[] = "quadrant switched by finite";
static const char ct[] = "radian switched by infinite";
static const double ax[A_SIZE]  = { 0.0, 1.0, 0.0, -1.0, 0.0, 0.0 };
static const double ay[A_SIZE]  = { 1.0, 0.0, -1.0, 0.0, HUGE_VAL, -HUGE_VAL };
static const double bx[B_SIZE]  = { 1.0, 1.0, -1.0, -1.0 };
static const double by[B_SIZE]  = { 1.0, -1.0, -1.0, 1.0 };
static const double cx[C_SIZE] =
{
	1.0, -1.0, HUGE_VAL, HUGE_VAL, HUGE_VAL, HUGE_VAL,
	1.0, -HUGE_VAL, -HUGE_VAL, -HUGE_VAL, -HUGE_VAL, -1.0
};
static const double cy[C_SIZE] =
{
	HUGE_VAL, HUGE_VAL, HUGE_VAL, 1.0, -1.0, -HUGE_VAL,
	-HUGE_VAL, HUGE_VAL, 1.0, -1.0, -HUGE_VAL, -HUGE_VAL
};
static const char cs[C_SIZE][9] =
{
	{' ', ' ', ' ', '0', 'd', 'e', 'g', ':', ' '},
	{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
	{' ', ' ', '4', '5', 'd', 'e', 'g', ':', ' '},
	{' ', ' ', '9', '0', 'd', 'e', 'g', ':', ' '},
	{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
	{' ', '1', '3', '5', 'd', 'e', 'g', ':', ' '},
	{' ', '1', '8', '0', 'd', 'e', 'g', ':', ' '},
	{' ', '-', '4', '5', 'd', 'e', 'g', ':', ' '},
	{' ', '-', '9', '0', 'd', 'e', 'g', ':', ' '},
	{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
	{'-', '1', '3', '5', 'd', 'e', 'g', ':', ' '},
	{'-', '1', '8', '0', 'd', 'e', 'g', ':', ' '}
};

// Prototypes
static char *eval_print(double, double);
static char *quadrant_print(double, double);
static char *atan2_print(double, double);
static void quadrant_list_print(void);

int
main(void)
{
	quadrant_list_print();
	return 0;
}

static char *
quadrant_print(double x, double y)
{
	static char s[0x100];

	snprintf(s, 0x100, "% *.*f\n", DBL_DIG, DBL_DIG, quadrant(x, y));

	return s;
}

static char *
atan2_print(double x, double y)
{
	static char s[0x100];

	snprintf(s, 0x100, "% *.*f\n", DBL_DIG, DBL_DIG, atan2(y, x));

	return s;
}

static char *
eval_print(const double x, const double y)
{
	static char s[0x100];
	
	snprintf(s, 0x100, "quadrant(% 1.1f, % 1.1f)", x, y);

	return s;
}

static void
quadrant_list_print(void)
{
	printf("%s\n", at);
	for (volatile int i = 0; i < A_SIZE; i++)
	{
		const double quadrant_x = ax[i];
		const double quadrant_y = ay[i];
		printf("  %20.22s = %*.*s | atan2: %*.*s\n", eval_print(quadrant_x, quadrant_y),
		       DBL_DIG + 2, DBL_DIG + 2, quadrant_print(quadrant_x, quadrant_y),
		       DBL_DIG + 2, DBL_DIG + 2, atan2_print(quadrant_x, quadrant_y));
	}
	puts("");
	printf("%s\n", bt);
	for (volatile int i = 0; i < B_SIZE; i++)
	{
		const double quadrant_x = bx[i];
		const double quadrant_y = by[i];
		printf("  %20.22s = %*.*s | atan2: %*.*s\n", eval_print(quadrant_x, quadrant_y),
		       DBL_DIG + 2, DBL_DIG + 2, quadrant_print(quadrant_x, quadrant_y),
		       DBL_DIG + 2, DBL_DIG + 2, atan2_print(quadrant_x, quadrant_y));
	}
	puts("");
	printf("%s\n", ct);
	for (volatile int i = 0; i < C_SIZE; i++)
	{
		const double quadrant_x = cx[i];
		const double quadrant_y = cy[i];
		printf("  ");
		for (volatile int j = 0; j < 9; j++)
			printf("%c", cs[i][j]);
		printf(" ");
		printf("%20.22s = %*.*s | atan2: %*.*s\n", eval_print(quadrant_x, quadrant_y),
		       DBL_DIG + 2, DBL_DIG + 2, quadrant_print(quadrant_x, quadrant_y),
		       DBL_DIG + 2, DBL_DIG + 2, atan2_print(quadrant_x, quadrant_y));
	}
}
