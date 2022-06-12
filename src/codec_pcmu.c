/*******************************************************************************
	codec_pcmu.c -- パルス符合変調
********************************************************************************/
#include <stdint.h>

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


/* Codec's Singleton Functions
Declare of Callback function of codecs
typedef void (* codec_ptr[CODEC_PATT][CODEC_TYPE])(pio_broker_t *);
CODEC_PATT .. 0 is Decode, 1 is Encode. Use constant DECODE / ENCODE
CODEC_TYPE .. Each these format of type
 */

/* Wave format information is crowds into main function as a parameter setting */

//0x0007  WAVE_FORMAT_PCMU
void dec_pcmu(pio_broker_t*);
void enc_pcmu(pio_broker_t*);

typedef void (* pcmucodec_tab[2][1])(pio_broker_t *);
const pcmucodec_tab pcmu_codec =
{
	{ dec_pcmu },
	{ enc_pcmu }
};

static const short PCMU_LEVEL[8] =
{
	0x00FF, 0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF
};
static const short (*const level)[8] = &PCMU_LEVEL;

/*  */
static inline double
formula_dec_pcmu(register uint8_t c)
{
	register double s; /* 16bit sound data */
//	unsigned char c; /* 8bit compressed data */
	register unsigned char sign, exponent, mantissa;
	register int magnitude;
	
	c = ~c;
	
	sign = c & 0x80;
	exponent = (c >> 4) & 0x07;
	mantissa = c & 0x0F;
	
	magnitude = ((((int)mantissa << 3) + 0x84) << exponent) - 0x84;
	
	if (sign == 0x80)
		s = -(double)((short)magnitude);
	else
		s = (double)((short)magnitude);
	
    return s / 32768.0; /* Normalize sound data to a range of -1 or more and less than 1. */
}

static inline unsigned char
formula_enc_pcmu(register double s)
{
	register double x;
//	register short s; /* 16bit sound data */
	register unsigned char c; /* 8bit compressed data */
	register unsigned char sign, exponent, mantissa;
	register int magnitude;
	
	x = sounddata_normalize(s, 65536.0);
	x = s_clamp(x, 0.0, 65535.0); /* clipping */
	x = ((x + 0.5) - 32768); /* Rounding and adjusting offsets */
	
	if (x < 0)
	{
		magnitude = -(int)x;
		sign = 0x80;
	}
	else
	{
		magnitude = (int)x;
		sign = 0x00;
	}
	
	magnitude += 0x84;
	if (magnitude > 0x7FFF)
		magnitude = 0x7FFF;
	
	for (exponent = 0; exponent < 8; exponent++)
		if (magnitude <= (*level)[exponent])
			break;
	
	mantissa = (magnitude >> (exponent + 3)) & 0x0F;
	
	c = ~(sign | (exponent << 4) | mantissa);
	
	return c; /* Export compressed data */

}

/*
 * Entity of encode/decode that mu-law
 */

void
dec_pcmu(pio_broker_t *bro)
{
	bro->s = formula_dec_pcmu(bro->b1);
}

void
enc_pcmu(pio_broker_t *bro)
{
	bro->b1 = formula_enc_pcmu(bro->s);
}

//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double GetRandom(void);

int
main(void)
{
	pio_broker_t bro;
	int i;
	
	srand(time(NULL));
	
	bro.s = 0.0;
	bro.b1 = bro.b2 = bro.b3 = bro.b4 = 0;
	
#define info  (printf("PCMU: (Sound data: %f)\n", bro.s))
#define print_enc  (printf("Encode: %02X\n", bro.b1))
#define print_dec  (printf("Decode: %f\n", bro.s))
#define show_codec {           \
  info;                        \
  pcmu_codec[ENCODE][0](&bro); \
  print_enc;                   \
  pcmu_codec[DECODE][0](&bro); \
  print_dec;                   \
  puts("");                    \
}

	printf("PCM mu-law Encoder/Decoder Test \n");
	puts("");
	
	// random value
	printf("Test for random value.\n");

	for (i =0; i < 10; i++)
	{
		bro.s = GetRandom();
		show_codec;
	}
	
	return 0;
}

double GetRandom(void)
{
	return 1.0 - (rand() / (double)RAND_MAX) * 2.0;
}
