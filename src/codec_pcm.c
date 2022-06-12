/***********************************************************************
	codec_pcm.c -- パルス符合変調
***********************************************************************/

/* pio_p.h */

#include <stdint.h>
#include <math.h> /* floor(), HUGE_VAL */

typedef volatile double io_snddata_t;
typedef volatile uint8_t io_bindata_t;

typedef struct {
	io_snddata_t s;
	io_bindata_t b1;
	io_bindata_t b2;
	io_bindata_t b3;
	io_bindata_t b4;
} pio_broker_t;

#define DECODE 0
#define ENCODE 1

/* Codec's Singleton Functions
Declare of Callback function of codecs
typedef void (* codec_ptr[CODEC_PATT][CODEC_TYPE])(pio_broker_t *);
CODEC_PATT .. 0 is Decode, 1 is Encode. Use constant DECODE / ENCODE
CODEC_TYPE .. Each these format of type
 */

/* Wave format information is crowds into main function as a parameter setting */

//0x0001  WAVE_FORMAT_PCM
void dec_pcm8bit(pio_broker_t*);
void dec_pcm16bit(pio_broker_t*);
void dec_pcm24bit(pio_broker_t*);
void dec_pcm32bit(pio_broker_t*);
void enc_pcm8bit(pio_broker_t*);
void enc_pcm16bit(pio_broker_t*);
void enc_pcm24bit(pio_broker_t*);
void enc_pcm32bit(pio_broker_t*);

typedef void (* lpcmcodec_tab[2][4])(pio_broker_t *);
const lpcmcodec_tab lpcm_codec =
	{
		{ dec_pcm8bit, dec_pcm16bit, dec_pcm24bit, dec_pcm32bit },
		{ enc_pcm8bit, enc_pcm16bit, enc_pcm24bit, enc_pcm32bit }
	};


/* end */

/* x0001lpcm.c */


/*
 * Variables & Accessory functions.
 */

#define LINEAR_PCM8   0
#define LINEAR_PCM16  1
#define LINEAR_PCM24  2
#define LINEAR_PCM32  3

#define ZERO_FLO             0.0
#define DWIDTH_X1          256.0
#define DWIDTH_X2        65536.0
#define DWIDTH_X3     16777216.0
#define DWIDTH_X4   4294967296.0
#define SDWIDTH_X1         128.0
#define SDWIDTH_X2       32768.0
#define SDWIDTH_X3     8388608.0
#define SDWIDTH_X4  2147483648.0
#define DWIDTH_X1M         255.0
#define DWIDTH_X2M       65535.0
#define DWIDTH_X3M    16777215.0
#define DWIDTH_X4M  4294967295.0

/*
 * Sound-datum AD/DA normalizer.
 * e.g. (16bit) -1.0 to 1.0 -> 0.0 to 65536.0
 */
static inline double
sounddata_normalize(register double s, register double bs)
{
	return (s + 1.0) / 2.0 * bs;
}

/*
 * Sound data clamper.
 * e.g. low=0.0 to high=255.0 then sound data is clipped in it.
 */
static inline double
s_clamp(register double x, register double low, register double high)
{
	return ((x > high) ? high : ((x < low) ? low : x));
}

/*
 * Cast unsigned char to signable double float.
 * 8-bit of argument must be have MSB.
 * 0x00 .. 0x7F -> Floating point as it is
 * 0x80 .. 0xFF -> Signed floating point
 */
static double
uchar2sgndbl(register uint8_t s)
{
	return (s >= 128 ? -(DWIDTH_X1-s) : (double)s);
}

/*
 * On digitize of sign to unsign.
 * This is referenced from Ruby's Math.gamma (by CoreCommitter nobu)
 * Because the data width is confident, the HUGE_VAL is returned as
 * a return value when a value of 4 or more is passed to byte_length
 * temporary argument.
 */
#define numberof(arr)  (sizeof (arr) / sizeof ((arr)[0]))
static double
digit_sgn2usgn(register double data, register uint8_t byte_length)
{
	static const double bytesize_table[] = {
		/*  8bit(0) */ DWIDTH_X1,
		/* 16bit(1) */ DWIDTH_X2,
		/* 24bit(2) */ DWIDTH_X3,
		/* 32bit(3) */ DWIDTH_X4
    };
	enum {NBYTESIZE_TABLE = numberof(bytesize_table)};
	if (byte_length >= NBYTESIZE_TABLE)  return HUGE_VAL;
	return floor(data < ZERO_FLO ? -(bytesize_table[byte_length]-data) : data);
}

/*
 * Linear PCM Formulas.
 * AD/DA is calculated with a double float value, which follows a polynomial approximation.
 * (Taylor series)
 * e.g. 16bit of -1(=\xFF\xFF) == (MSB)0xFFsigned=-1x(coef:x1=)256.0=-256.0 + (LSB)255.0
 * e.g. 24bit of -1(=\xFF\xFF\xFF) == (MSB)0xFFsigned=-1x(coef:x2=)65536.0=-65536.0 + (HSB)255.0x(coef:x1)256.0=65280.0 + (LSB)255.0
 * In addition: This way seems to be called 'Galois Field'.
 */
static inline double
formula_dec_pcm8bit(register uint8_t b1)
{
	return ((double)b1 - SDWIDTH_X1) / SDWIDTH_X1;
}

static inline double
formula_dec_pcm16bit(register uint8_t b1, register uint8_t b2)
{
	register double data = (uchar2sgndbl(b2)*DWIDTH_X1 + (double)b1);
	return data / SDWIDTH_X2;
}

static inline double
formula_dec_pcm24bit(register uint8_t b1, register uint8_t b2, register uint8_t b3)
{
	register double data = (uchar2sgndbl(b3)*DWIDTH_X2 + (double)b2*DWIDTH_X1 + (double)b1);
	return data / SDWIDTH_X3;
}

static inline double
formula_dec_pcm32bit(register uint8_t b1, register uint8_t b2, register uint8_t b3, register uint8_t b4)
{
	register double data = (uchar2sgndbl(b4)*DWIDTH_X3 + (double)b3*DWIDTH_X2 + (double)b2*DWIDTH_X1 + (double)b1);
	return data / SDWIDTH_X4;
}

static inline double
formula_enc_pcm8bit(register double s)
{
	return sounddata_normalize(s, DWIDTH_X1);
}

static inline double
formula_enc_pcm16bit(register double s)
{
	return sounddata_normalize(s, DWIDTH_X2);
}

static inline double
formula_enc_pcm24bit(register double s)
{
	return sounddata_normalize(s, DWIDTH_X3);
}

static inline double
formula_enc_pcm32bit(register double s)
{
	return sounddata_normalize(s, DWIDTH_X4);
}

/*
 * Entity of encode/decode that Linear PCM
 * 
 * b1->b4: Little endian(for RIFF), b4->b1: Big endian(for AIFF)
 */

void
dec_pcm8bit(pio_broker_t *bro)
{
	bro->s = formula_dec_pcm8bit(bro->b1);
}

void
dec_pcm16bit(pio_broker_t *bro)
{
	bro->s = formula_dec_pcm16bit(bro->b1, bro->b2);
}

void
dec_pcm24bit(pio_broker_t *bro)
{
	bro->s = formula_dec_pcm24bit(bro->b1, bro->b2, bro->b3);
}

void
dec_pcm32bit(pio_broker_t *bro)
{
	bro->s = formula_dec_pcm32bit(bro->b1, bro->b2, bro->b3, bro->b4);
}

void
enc_pcm8bit(pio_broker_t *bro)
{
	/* clipping */
	double data = s_clamp(formula_enc_pcm8bit(bro->s), ZERO_FLO, DWIDTH_X1M);
	/* rounding & digitize & writing */
	bro->b1 = (uint8_t)((int)(data + 0.5));
}

void
enc_pcm16bit(pio_broker_t *bro)
{
	/* clipping */
	double data = s_clamp(formula_enc_pcm16bit(bro->s), ZERO_FLO, DWIDTH_X2M);
	/* rounding & digitize */
	unsigned short digitize = (unsigned short)digit_sgn2usgn(data + 0.5 - SDWIDTH_X2, LINEAR_PCM16);
	/* writing */
	bro->b1 = (uint8_t)(digitize & 0xFF);
	bro->b2 = (uint8_t)((digitize >> 8) & 0xFF);
}

void
enc_pcm24bit(pio_broker_t *bro)
{
	/* clipping */
	double data = s_clamp(formula_enc_pcm24bit(bro->s), ZERO_FLO, DWIDTH_X3M);
	/* rounding & digitize */
	unsigned long digitize = (unsigned long)digit_sgn2usgn(data + 0.5 - SDWIDTH_X3, LINEAR_PCM24);
	/* writing */
	bro->b1 = (uint8_t)(digitize & 0xFF);
	bro->b2 = (uint8_t)((digitize >> 8) & 0xFF);
	bro->b3 = (uint8_t)((digitize >> 16) & 0xFF);
}

void
enc_pcm32bit(pio_broker_t *bro)
{
	/* clipping */
	double data = s_clamp(formula_enc_pcm32bit(bro->s), ZERO_FLO, DWIDTH_X4M);
	/* rounding & digitize */
	unsigned long digitize = (unsigned long)digit_sgn2usgn(data + 0.5 - SDWIDTH_X4, LINEAR_PCM32);
	/* writing */
	bro->b1 = (uint8_t)(digitize & 0xFF);
	bro->b2 = (uint8_t)((digitize >> 8) & 0xFF);
	bro->b3 = (uint8_t)((digitize >> 16) & 0xFF);
	bro->b4 = (uint8_t)((digitize >> 24) & 0xFF);
}


//----------

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double GetRandom(void);

int main(void)
{
	pio_broker_t bro;
	uint8_t rsvbits;
	
	srand(time(NULL));
	
	bro.s = 0.0;
	bro.b1 = bro.b2 = bro.b3 = bro.b4 = 0;
	
#define info  (printf("PCM %2dBIT: (Sound data: %f)\n", 8*(rsvbits+1), bro.s))
#define print_enc { \
  switch(rsvbits){\
  case LINEAR_PCM8:\
	printf("Encode: %02X\n", bro.b1);\
	break;\
  case LINEAR_PCM16:\
	printf("Encode: %02X %02X\n", bro.b1, bro.b2);\
	break;\
  case LINEAR_PCM24:\
    printf("Encode: %02X %02X %02X\n", bro.b1, bro.b2, bro.b3);\
    break;\
  case LINEAR_PCM32:\
    printf("Encode: %02X %02X %02X %02X\n", bro.b1, bro.b2, bro.b3, bro.b4);\
    break;\
  default:\
    break;\
  }\
}
#define print_dec  (printf("Decode: %f\n", bro.s))
#define show_codec {                 \
  info;                              \
  lpcm_codec[ENCODE][rsvbits](&bro); \
  print_enc;                         \
  lpcm_codec[DECODE][rsvbits](&bro); \
  print_dec;                         \
  puts("");                          \
}

	printf("Linear PCM Encoder/Decoder Test \n");
	puts("");
	// value: -1.0
	printf("Test for value -1.0.\n");
	
	rsvbits = LINEAR_PCM8;
	bro.s = -1.0;
	show_codec;
	
	rsvbits = LINEAR_PCM16;
	show_codec;
	
	rsvbits = LINEAR_PCM24;
	show_codec;
	
	rsvbits = LINEAR_PCM32;
	show_codec;
	
	// value: 1.0
	printf("Test for value 1.0.\n");
	
	rsvbits = LINEAR_PCM8;
	bro.s = 1.0;
	show_codec;
	
	rsvbits = LINEAR_PCM16;
	bro.s = 1.0;
	show_codec;
	
	rsvbits = LINEAR_PCM24;
	bro.s = 1.0;
	show_codec;
	
	rsvbits = LINEAR_PCM32;
	bro.s = 1.0;
	show_codec;
	
	// random value
	printf("Test for random value.\n");
	
	rsvbits = LINEAR_PCM8;
	bro.s = GetRandom();
	show_codec;
	
	rsvbits = LINEAR_PCM16;
	bro.s = GetRandom();
	show_codec;
	
	rsvbits = LINEAR_PCM24;
	bro.s = GetRandom();
	show_codec;
	
	rsvbits = LINEAR_PCM32;
	bro.s = GetRandom();
	show_codec;
	
	return 0;
}

double
GetRandom(void)
{
	return 1.0 - (rand() / (double)RAND_MAX) * 2.0;
}
