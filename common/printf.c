/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  printf function in the OS kernel                                  */
/*  These are drived from FreeBSD sys/kern/subr_prf.c                 */
/**********************************************************************/

/*-
 * Copyright (c) 1986, 1988, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)subr_prf.c	8.3 (Berkeley) 1/21/94
 */


#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#include "kern/kernel.h"

#define NULLFMT   "(fmt null)"  /*< 書式指定文字列にNULLを指定された  */
#define MIN_RADIX          (2)  /*< 底の最小値                        */
#define MAX_RADIX         (36)  /*< 底の最大値                        */
#define DEFAULT_RADIX     (10)  /*< 底のデフォルト値                  */

char const hex2ascii_data[] = "0123456789abcdefghijklmnopqrstuvwxyz";  /*< 16進数を文字に変換するテーブル  */

/** バッファに文字を記録する
    @param[in]      c 記録する文字
    @param[in] outbuf 記録先バッファへのポインタ
    @param[in] retval 返り値を格納した変数
 */
#define PUTCHAR(c, outbuf, retval) do{		\
		*(outbuf) = (c);			\
		++(outbuf);				\
		++(retval);				\
	}while(0)


typedef struct _fmt_cmd{
	unsigned dot:1;
	unsigned neg:1;
	unsigned sign:1;
	unsigned ladjust:1;
	unsigned upper:1;
	unsigned cflag:1;
	unsigned hflag:1;
	unsigned jflag:1;
	unsigned lflag:1;
	unsigned qflag:1;
	unsigned sharpflag:1;
	unsigned tflag:1;
	unsigned zflag:1;
}fmt_cmd_t;

/** 16進数を文字列にする
 */
#define hex2ascii(hex)  (hex2ascii_data[hex])
#define ktoupper(c)      ((c) - 0x20 * (((c) >= 'a') && ((c) <= 'z')))


/** 書式指示をリセットする
    @param[in] cmdp 書式指定指示構造体へのポインタ
 */
#define RESET_FMT(cmdp) do{				\
		(cmdp)->dot = 0;			\
		(cmdp)->neg = 0;			\
		(cmdp)->sign = 0;			\
		(cmdp)->ladjust = 0;			\
		(cmdp)->upper = 0;			\
		(cmdp)->cflag = 0;			\
		(cmdp)->hflag = 0;			\
		(cmdp)->jflag = 0;			\
		(cmdp)->lflag = 0;			\
		(cmdp)->qflag = 0;			\
		(cmdp)->sharpflag = 0;			\
		(cmdp)->tflag = 0;			\
		(cmdp)->zflag = 0;			\
	} while (0)

/** 書式指定コマンド初期化子
 */
#define FMTCMD_INITILIZER    {			\
		.dot = 0,			\
		.neg = 0,			\
		.sign = 0,			\
		.ladjust = 0,			\
		.upper = 0,			\
		.cflag = 0,			\
		.hflag = 0,			\
		.jflag = 0,			\
		.lflag = 0,			\
		.qflag = 0,			\
		.sharpflag = 0,			\
		.tflag = 0,			\
		.zflag = 0			\
	}

/*
 * Put a NUL-terminated ASCII number (base <= 36) in a buffer in reverse
 * order; return an optional length and a pointer to the last character
 * written in the buffer (i.e., the first character of the string).
 * The buffer pointed to by `nbuf' must have length >= MAXNBUF.
 */
static char *
ksprintn(char *nbuf, uintmax_t num, int base, int *lenp, int upper)
{
	char *p, c;

	p = nbuf;
	*p = '\0';
	do {
		c = hex2ascii(num % base);
		*++p = upper ? ktoupper(c) : c;
	} while (num /= base);

	if (lenp != NULL)
		*lenp = p - nbuf;

	return (p);
}

/** 書式指定に従って文字列を生成する
    @param[in] len   文字列出力先バッファの長さ
    @param[in] buf   文字列出力先バッファ
    @param[in] fmt   書式指定文字列
    @param[in] radix 数値出力の底
    @param[in] ap    可変引数リスト
    @retval 出力文字列長
 */
static int
kvprintf(size_t len, void *buf, char const *fmt, int radix, va_list ap) {
	char     nbuf[PRFBUFLEN];
	const char            *p;
	char            *d = buf;
	int              n;
	int             ch;
	int             rc = 0;
	uintmax_t      num = 0;
	int           base = 0;
	int            tmp = 0;
	int          width = 0;
	int         dwidth = 0;
	char       padchar = ' ';
	int discard_format = 0;
	fmt_cmd_t      cmd = FMTCMD_INITILIZER, *cmdp = &cmd;
	char          *percent;

	if (fmt == NULL)
		fmt = NULLFMT;

	if (len == 0)
		return 0;

	if (radix < MIN_RADIX || radix > MAX_RADIX)
		radix = DEFAULT_RADIX;

	for (;;) {
		width = 0;
		padchar = ' ';
		
		/*
		 * 書式指定文字('%')に出会うまでバッファに文字を出力する
		 */
		while ((ch = (unsigned char)*fmt++) != '%' || discard_format) {
			if ( (ch == '\0') || ( rc >= (len - 1) ) )
				return ( rc >= (len - 1) ) ? (len - 1) : (rc);
			PUTCHAR(ch, d, rc);
		}
		percent = (char *)(fmt - 1);
		RESET_FMT(cmdp);

	reswitch:
		switch (ch = (unsigned char)*fmt++) {
		case '.':
			cmdp->dot = 1;
			goto reswitch;
		case '#':
			cmdp->sharpflag = 1;
			goto reswitch;
		case '+':
			cmdp->sign = 1;
			goto reswitch;
		case '-':
			cmdp->ladjust = 1;
			goto reswitch;
		case '%':
			PUTCHAR(ch, d, rc);
			break;
		case '*':
			if (!cmdp->dot) {
				width = va_arg(ap, int);
				if (width < 0) {
					cmdp->ladjust = !cmdp->ladjust;
					width = -width;
				}
			} else {
				dwidth = va_arg(ap, int);
			}
			goto reswitch;
		case '0':
			if (!cmdp->dot) {
				padchar = '0';
				goto reswitch;
			}
		case '1': 
		case '2': 
		case '3': 
		case '4':
		case '5': 
		case '6': 
		case '7': 
		case '8': 
		case '9':
			for (n = 0;; ++fmt) {
				n = n * 10 + ch - '0';
				ch = *fmt;
				if (ch < '0' || ch > '9')
					break;
			}
			if (cmdp->dot)
				dwidth = n;
			else
				width = n;
			goto reswitch;
		case 'c':
			PUTCHAR(va_arg(ap, int), d, rc);
			break;
		case 'd':
		case 'i':
			base = 10;
			cmdp->sign = 1;
			goto handle_sign;
		case 'h':
			if (cmdp->hflag) {
				cmdp->hflag = 0;
				cmdp->cflag = 1;
			} else
				cmdp->hflag = 1;
			goto reswitch;
		case 'l':
			if (cmdp->lflag) {
				cmdp->lflag = 0;
				cmdp->qflag = 1;
			} else
				cmdp->lflag = 1;
			goto reswitch;
		case 'o':
			base = 8;
			goto handle_nosign;
		case 'p':
			base = 16;
			cmdp->sharpflag = (width == 0);
			cmdp->sign = 0;
			num = (uintptr_t)va_arg(ap, void *);
			goto number;
		case 'q':
			cmdp->qflag = 1;
			goto reswitch;
		case 'r':
			base = radix;
			if (cmdp->sign)
				goto handle_sign;
			goto handle_nosign;
		case 's':
			p = va_arg(ap, char *);
			if (p == NULL)
				p = "(null)";
			if (!cmdp->dot)
				n = strlen (p);
			else
				for (n = 0; n < dwidth && p[n]; n++)
					continue;

			width -= n;

			if (!cmdp->ladjust && width > 0)
				while (width--)
					PUTCHAR(padchar, d, rc);
			while (n--)
				PUTCHAR(*p++, d, rc);
			if (cmdp->ladjust && width > 0)
				while (width--)
					PUTCHAR(padchar, d, rc);
			break;
		case 't':
			cmdp->tflag = 1;
			goto reswitch;
		case 'u':
			base = 10;
			goto handle_nosign;
		case 'X':
			cmdp->upper = 1;
		case 'x':
			base = 16;
			goto handle_nosign;
		case 'y':
			base = 16;
			cmdp->sign = 1;
			goto handle_sign;
		case 'z':
			cmdp->zflag = 1;
			goto reswitch;
handle_nosign:
			cmdp->sign = 0;
			if (cmdp->qflag)
				num = va_arg(ap, uint64_t);
			else if (cmdp->tflag)
				num = va_arg(ap, ptrdiff_t);
			else if (cmdp->lflag)
				num = va_arg(ap, unsigned long);
			else if (cmdp->zflag)
				num = va_arg(ap, size_t);
			else if (cmdp->hflag)
				num = (unsigned short)va_arg(ap, int);
			else if (cmdp->cflag)
				num = (unsigned char)va_arg(ap, int);
			else
				num = va_arg(ap, unsigned int);
			goto number;
handle_sign:
			if (cmdp->qflag)
				num = va_arg(ap, int64_t);
			else if (cmdp->tflag)
				num = va_arg(ap, ptrdiff_t);
			else if (cmdp->lflag)
				num = va_arg(ap, long);
			else if (cmdp->zflag)
				num = va_arg(ap, ssize_t);
			else if (cmdp->hflag)
				num = (short)va_arg(ap, int);
			else if (cmdp->cflag)
				num = (char)va_arg(ap, int);
			else
				num = va_arg(ap, int);
number:
			if (cmdp->sign && (intmax_t)num < 0) {
				cmdp->neg = 1;
				num = -(intmax_t)num;
			}
			p = ksprintn(nbuf, num, base, &tmp, cmdp->upper);
			if (cmdp->sharpflag && num != 0) {
				if (base == 8)
					tmp++;
				else if (base == 16)
					tmp += 2;
			}
			if (cmdp->neg)
				tmp++;

			if (!cmdp->ladjust && padchar != '0' && width
			    && (width -= tmp) > 0)
				while (width--)
					PUTCHAR(padchar, d, rc);
			if (cmdp->neg)
				PUTCHAR('-', d, rc);
			if (cmdp->sharpflag && num != 0) {
				if (base == 8) {
					PUTCHAR('0', d, rc);
				} else if (base == 16) {
					PUTCHAR('0', d, rc);
					PUTCHAR('x', d, rc);
				}
			}
			if (!cmdp->ladjust && width && (width -= tmp) > 0)
				while (width--)
					PUTCHAR(padchar, d, rc);

			while (*p)
				PUTCHAR(*p--, d, rc);

			if (cmdp->ladjust && width && (width -= tmp) > 0)
				while (width--)
					PUTCHAR(padchar, d, rc);
			break;
		default:
			while (percent < fmt)
				PUTCHAR(*percent++, d, rc);
			discard_format = 1;
			break;
		}
	}
}

/** 文字列表示
    @param[in] str 表示する文字列
 */
static void
kprint_string(const char *str) {
	
	while(*str !=  '\0')
		kputchar((int)*str++);
}

/** 書式付きカーネル内出力関数
    @param[in] fmt 書式指定文字列
    @param[in] ... 可変個引数
    @retval 出力した文字列長
    @note 文字列長は, PRFBUFLEN(127)文字までに制限される
 */
int
kprintf(const char *fmt, ...) {
	va_list ap;
	int     rc;
	char buff[PRFBUFLEN];

	va_start(ap, fmt);
	rc = kvprintf(PRFBUFLEN, buff, fmt, 10, ap);
	va_end(ap);
	buff[rc] = '\0';

	kprint_string(buff);

	return rc;
}
