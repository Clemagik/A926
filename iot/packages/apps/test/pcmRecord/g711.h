/*
 * This source code is a product of Sun Microsystems, Inc. and is provided
 * for unrestricted use.  Users may copy or modify this source code without
 * charge.
 *
 * SUN SOURCE CODE IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING
 * THE WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * See file LICENCE for terms and conditions of usage PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun source code is provided with no support and without any obligation on
 * convert unquantized 16 bit values.
 * Tables for direct u- to A-law and A- to u-law conversions have been
 * corrected.
 * Borge Lindberg, Center for PersonKommunikation, Aalborg University.
 * bli@cpk.auc.dk
 *
 */
/*
 * Downloaded from comp.speech site in Cambridge.
 *
 */
/*
 * g711.h
 *
 * u-law, A-law and linear PCM conversions.
 */

#ifndef _G711_H_
#define _G711_H_

extern "C" {

unsigned char   linear2alaw(short pcm_val);
short           alaw2linear(unsigned char a_val);
unsigned char   linear2ulaw(short pcm_val);
short           ulaw2linear(unsigned char u_val);
unsigned char   alaw2ulaw(unsigned char aval);
unsigned char   ulaw2alaw(unsigned char uval);

}

#endif /* _G711_H_ */

