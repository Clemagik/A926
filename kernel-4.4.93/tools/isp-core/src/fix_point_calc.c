#include <linux/kernel.h>
#include <linux/math64.h>
#include "isp-debug.h"
#include "fix_point_calc.h"

uint64_t fix_point_add(uint32_t pointpos, uint64_t a, uint64_t b)
{
	uint64_t x;
	x = a+b;
	return x;
}

uint64_t fix_point_sub(uint32_t pointpos, uint64_t a, uint64_t b)
{
	uint64_t x;
	if (a<b) {
		ISP_ERROR("error: do not support negative number\n");
	}
	x = a-b;
	return x;
}

uint64_t fix_point_mult2(uint32_t pointpos, uint64_t a, uint64_t b)
{
	uint64_t x1,x2,x;
	uint64_t a1,a2,b1,b2;
	uint64_t mask = (((uint64_t)(~0x0))>>(64-pointpos));

	if(pointpos == 0)
		mask = 0;
	a1 = a>>pointpos;
	a2 = a&mask;
	b1 = b>>pointpos;
	b2 = b&mask;
	x1 = (a1*b1)<<pointpos;
	x1 += ((a1*b2)+(a2*b1));
	x2 = (a2*b2)>>pointpos;
	x = (x1)+x2;

	return x;
}

uint64_t fix_point_mult3(uint32_t pointpos, uint64_t a, uint64_t b, uint64_t c)
{
	uint64_t x = 0;
	x = fix_point_mult2(pointpos, a,b);
	x = fix_point_mult2(pointpos, x,c);
	return x;
}

uint64_t fix_point_div(uint32_t pointpos, uint64_t a, uint64_t b)
{
	uint64_t x1,x2,x3,x;
	int32_t i;
	uint64_t a1,b1;
	a1 = a;
	b1 = b;
	//do_div(a1,b1);
	a1 = div64_u64(a1, b1);
	x1 = a1;
	x2 = a-(x1*b);
	x3 = b;
	x = 0;
	//ISP_ERROR("a=0x%llx,b=0x%llx\n",a,b);
	//ISP_ERROR("a1=%llx,b1=%llx,x1=%llx,x2=%llx,x3=%llx\n",a1,b1,x1,x2,x3);
	for (i = 0; i< pointpos; i++) {
		x2=x2<<1;
		x=x<<1;
		if (x2>x3) {
			x=x|0x1;
			x2=x2-x3;
		} else if (x2 == x3) {
			x=x|0x1;
			x=x<<(pointpos-i-1);
			break;
		}
	}
	x=(x1<<pointpos)|x;
	return x;
}


uint64_t fix_point_add_64(uint32_t pointpos, uint64_t a, uint64_t b)
{
	uint64_t x;
	x = a+b;
	return x;
}

uint64_t fix_point_sub_64(uint32_t pointpos, uint64_t a, uint64_t b)
{
	uint64_t x;
	if (a<b) {
		ISP_ERROR("error: do not support negative number\n");
	}
	x = a-b;
	return x;
}

uint64_t fix_point_mult2_64(uint32_t pointpos, uint64_t a, uint64_t b)
{
	uint64_t x1,x2,x;
	uint64_t a1,a2,b1,b2;
	uint64_t mask = (((uint64_t)(~0x0))>>(64-pointpos));
	a1 = a>>pointpos;
	a2 = a&mask;
	b1 = b>>pointpos;
	b2 = b&mask;
	x1 = (a1*b1)<<pointpos;
	x1 += ((a1*b2)+(a2*b1));
	x2 = (a2*b2)>>pointpos;
	x = (x1)+x2;
	return x;
}

uint64_t fix_point_mult3_64(uint32_t pointpos, uint64_t a, uint64_t b, uint64_t c)
{
	uint64_t x = 0;
	x = fix_point_mult2_64(pointpos, a,b);
	x = fix_point_mult2_64(pointpos, x,c);
	return x;
}

uint64_t fix_point_div_64(uint32_t pointpos, uint64_t a, uint64_t b)
{
	uint64_t x1,x2,x3,x;
	int32_t i;
	uint64_t a1,b1;
	a1 = a;
	b1 = b;
	a1 = div64_u64(a1, b1);
	x1 = a1;
	x2 = a-(x1*b);
	x3 = b;
	x = 0;
	for (i = 0; i< pointpos; i++) {
		x2=x2<<1;
		x=x<<1;
		if (x2>x3) {
			x=x|0x1;
			x2=x2-x3;
		} else if (x2 == x3) {
			x=x|0x1;
			x=x<<(pointpos-i-1);
			break;
		}
	}
	x=(x1<<pointpos)|x;
	return x;
}

uint32_t fix_point_add_32(uint32_t pointpos, uint32_t a, uint32_t b)
{
	uint32_t x;
	x = a+b;
	return x;
}

uint32_t fix_point_sub_32(uint32_t pointpos, uint32_t a, uint32_t b)
{
	uint64_t x;
	if (a<b) {
		ISP_ERROR("error: do not support negative number\n");
	}
	x = a-b;
	return x;
}

uint32_t fix_point_mult2_32(uint32_t pointpos, uint32_t a, uint32_t b)
{
	uint32_t x1,x2,x;
	uint32_t a1,a2,b1,b2;
	uint32_t mask = (((uint32_t)(~0x0))>>(32-pointpos));
	a1 = a>>pointpos;
	a2 = a&mask;
	b1 = b>>pointpos;
	b2 = b&mask;
	x1 = (a1*b1)<<pointpos;
	x1 += ((a1*b2)+(a2*b1));
	x2 = (a2*b2)>>pointpos;
	x = (x1)+x2;
	return x;
}

uint32_t fix_point_mult3_32(uint32_t pointpos, uint32_t a, uint32_t b, uint32_t c)
{
	uint32_t x = 0;
	x = fix_point_mult2_32(pointpos, a,b);
	x = fix_point_mult2_32(pointpos, x,c);
	return x;
}

uint32_t fix_point_div_32(uint32_t pointpos, uint32_t a, uint32_t b)
{
	uint32_t i;
	uint32_t x2,x3,x;
	uint64_t a1;
	a1 = a;
	do_div(a1,b);
	x2 = a-(a1*b);
	x3 = b;
	x = 0;
	for (i = 0; i< pointpos; i++) {
		x2=x2<<1;
		x=x<<1;
		if (x2>x3) {
			x=x|0x1;
			x2=x2-x3;
		} else if (x2 == x3) {
			x=x|0x1;
			x=x<<(pointpos-i-1);
			break;
		}
	}
	x=(a1<<pointpos)|x;
	return x;
}

//chazhi
uint32_t fix_point_intp(uint32_t ypointpos, uint32_t x1, uint32_t x2, uint32_t y1, uint32_t y2, uint32_t x)
{
	if (x2 < x1)
		ISP_ERROR("%s,%d: error\n", __func__, __LINE__);
	if (x1 == x2) {
		if (y1 == y2) {
			return y1;
		} else {
			ISP_ERROR("%s,%d: error\n", __func__, __LINE__);
		}
	} else {
		if (y1 == y2) {
			return y1;
		} else {
			uint8_t i = 0;
			uint32_t neg = 0;
			uint64_t y2y1 = 0;
			uint64_t x2x1 = 0;
			uint64_t xx1 = 0;
			uint64_t y = 0;
			uint64_t y1 = 0,y2 = 0;
			uint32_t r = 0;
			if (y2 > y1) {
				y2y1 = y2-y1;
			} else {
				neg = 1;
				y2y1 = y1-y2;
			}
			x2x1 = x2-x1;
			xx1 = x-x1;
			y = xx1*y2y1;

			y1 = div64_u64(y, x2x1);
			y2 = y-y1*x2x1;
			for (i = 0; i< ypointpos; i++) {
				y2=y2<<1;
				r=r<<1;
				if (y2>x2x1) {
					r=r|0x1;
					y2=y2-x2x1;
				} else if (y2==x2x1) {
					r=r|0x1;
					r=r<<(ypointpos-i-1);
					break;
				}
			}
			r=(y1<<ypointpos)|r;
			if (0 == neg) {
				r = y1+r;
			} else {
				r = y1-r;
			}
			return r;
		}
	}
	return 0;
}

uint32_t table_intp(uint32_t ypointpos, uint32_t *table, uint32_t rows, uint32_t x)
{
	uint8_t i = 0;
	uint32_t r = 0;
	if (x<=*(table+1)) {
		r = *table;
		return r;
	}
	for (i = 1; i < rows; i++) {
		if (x<=*(table+2*i+1)) {
			r = fix_point_intp(ypointpos, *(table+2*(i-1)+1), *(table+2*(i)+1), *(table+2*(i-1)), *(table+2*(i)), x);
			return r;
		}
	}
	r = *(table+2*(rows-1));
	return r;
}

uint32_t tisp_simple_intp(uint32_t x_int, uint32_t x_fra, uint32_t *y_array)
{
	//ISP_ERROR("%s,%d: y_array = %p\n", __func__, __LINE__, y_array);
	int sel_y_0, sel_y_1;
	int data_diff, sign_value;
	int intp_value;

	if (x_int >= 8) {
		intp_value = y_array[8];
	} else {
		sel_y_0 = y_array[x_int];
		sel_y_1 = y_array[x_int+1];

		if(sel_y_0 == sel_y_1)
			intp_value = sel_y_0;
		else {
			if(sel_y_0 < sel_y_1){
				data_diff = sel_y_1-sel_y_0;
				sign_value = 0;
			} else {
				data_diff = sel_y_0-sel_y_1;
				sign_value = 1;
			}

			data_diff = data_diff*x_fra;
			data_diff = (data_diff>>16)+((data_diff&0x8000) >> 15);

			if(sign_value == 1)
				intp_value = sel_y_0 - data_diff;
			else
				intp_value = sel_y_0 + data_diff;
		}
	}

	return intp_value;
}

static const unsigned int __pow2_lut[33]={
	1073741824,1097253708,1121280436,1145833280,1170923762,1196563654,1222764986,1249540052,
	1276901417,1304861917,1333434672,1362633090,1392470869,1422962010,1454120821,1485961921,
	1518500250,1551751076,1585730000,1620452965,1655936265,1692196547,1729250827,1767116489,
	1805811301,1845353420,1885761398,1927054196,1969251188,2012372174,2056437387,2101467502,
	2147483648U};

uint32_t tisp_math_exp2(uint32_t val, const unsigned char shift_in, const unsigned char shift_out)
{
	unsigned int fract_part = (val & ((1<<shift_in)-1));
	unsigned int int_part = val >> shift_in;
	if (shift_in <= 5)
	{
		unsigned int lut_index = fract_part << (5-shift_in);
		return __pow2_lut[lut_index] >> (30 - shift_out - int_part);
	}
	else
	{
		unsigned int lut_index = fract_part >> (shift_in - 5);
		unsigned int lut_fract = fract_part & ((1<<(shift_in-5))-1);
		unsigned int a = __pow2_lut[lut_index];
		unsigned int b =  __pow2_lut[lut_index+1];
		unsigned int res = ((unsigned long long)(b - a)*lut_fract) >> (shift_in-5);
		res = (res + a) >> (30-shift_out-int_part);
		return res;
	}
}
EXPORT_SYMBOL(tisp_math_exp2);

static uint8_t tisp_leading_one_position(const uint32_t in)
{
	uint8_t pos = 0;
	uint32_t val = in;
	if (val >= 1<<16) { val >>= 16; pos += 16; }
	if (val >= 1<< 8) { val >>=  8; pos +=  8; }
	if (val >= 1<< 4) { val >>=  4; pos +=  4; }
	if (val >= 1<< 2) { val >>=  2; pos +=  2; }
	if (val >= 1<< 1) {             pos +=  1; }
	return pos;
}

static int tisp_leading_one_position_64(uint64_t val)
{
	int pos = 0;
	if (val >= (uint64_t)1<<32) { val >>= 32; pos += 32; }
	if (val >= 1<<16) { val >>= 16; pos += 16; }
	if (val >= 1<< 8) { val >>=  8; pos +=  8; }
	if (val >= 1<< 4) { val >>=  4; pos +=  4; }
	if (val >= 1<< 2) { val >>=  2; pos +=  2; }
	if (val >= 1<< 1) {             pos +=  1; }
	return pos;
}
//  y = log2(x)
//
//	input:  Integer: val
//	output: Fixed point x.y
//  y: out_precision
//
uint32_t tisp_log2_int_to_fixed(const uint32_t val, const uint8_t out_precision, const uint8_t shift_out)
{
	int i;
	int pos = 0;
	uint32_t a = 0;
	uint32_t b = 0;
	uint32_t in = val;
	uint32_t result = 0;
	const unsigned char precision = out_precision;

	if(0 == val)
	{
		return 0;
	}
	// integral part
	pos = tisp_leading_one_position(val);
	// fractional part
	a = (pos <= 15) ? (in << (15 - pos)) : (in>>(pos - 15));
	for(i = 0; i < precision; ++i)
	{
		b = a * a;
		if(b & (1<<31))
		{
			result = (result << 1) + 1;
			a = b >> 16;
		}
		else
		{
			result = (result << 1);
			a = b >> 15;
		}
	}
	return (((pos << precision) + result) << shift_out) | ((a & 0x7fff)>> (15-shift_out));
}

uint32_t tisp_log2_int_to_fixed_64(uint64_t val, uint8_t out_precision, uint8_t shift_out)
{
	int i;
	int pos = 0;
	uint64_t a = 0;
	uint64_t b = 0;
	uint64_t in = val;
	uint64_t result = 0;
	const unsigned char precision = out_precision;

	if(0 == val)
	{
		return 0;
	}
	// integral part
	pos = tisp_leading_one_position_64(val);
	// fractional part
	a = (pos <= 15) ? (in << (15 - pos)) : (in>>(pos - 15));
	for(i = 0; i < precision; ++i)
	{
		b = a * a;
		if(b & (1<<31))
		{
			result = (result << 1) + 1;
			a = b >> 16;
		}
		else
		{
			result = (result << 1);
			a = b >> 15;
		}
	}
	return (uint32_t)((((pos << precision) + result) << shift_out) | ((a & 0x7fff)>> (15-shift_out)));
}
//  y = log2(x)
//
//	input:  Fixed point: x1.y1
//	output: Fixed point: x2.y2
//  y1: in_fraction
//  y2: out_precision
//
uint32_t tisp_log2_fixed_to_fixed(const uint32_t val, const int in_fix_point, const uint8_t out_fix_point)
{
	return tisp_log2_int_to_fixed(val, out_fix_point, 0) - (in_fix_point << out_fix_point);
}
EXPORT_SYMBOL(tisp_log2_fixed_to_fixed);

uint32_t tisp_log2_fixed_to_fixed_64(uint64_t val, int32_t in_fix_point, uint8_t out_fix_point)
{
	return tisp_log2_int_to_fixed_64(val, out_fix_point, 0) - (in_fix_point << out_fix_point);
}
