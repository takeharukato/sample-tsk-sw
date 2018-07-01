/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  string operations                                                 */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

/** メモリを指定された値で埋める
    @param[in] b   書き込み対象メモリの先頭アドレス
    @param[in] c   書き込む値
    @param[in] len 書き込み対象領域のサイズ
    @return 書き込み対象メモリの先頭アドレス
 */
void *
memset(void *b, int c, size_t len) {
	size_t i;
	caddr_t m = b;

	for(i = 0; i < len; ++i) {
		*m++ = (char )c;
	}
	
	return b;
}

/** メモリの内容をコピーする
    @param[in] dst   コピー先メモリの先頭アドレス
    @param[in] src   コピー元メモリの先頭アドレス
    @param[in] len   コピーするサイズ
    @return コピー先メモリの先頭アドレス
    @note srcとdstの領域に重なりがある場合はコピーが正しく行われない
 */
void *
memcpy(void *dst, const void *src, size_t len){
	caddr_t end  = (caddr_t)(src + len);
	caddr_t dstp = (caddr_t)dst, srcp = (caddr_t)src;

	do {
		*dstp++ = *srcp++;
	}while (srcp != end);

	return dst;
}

/** メモリの内容をコピーする
    @param[in] dst   コピー先メモリの先頭アドレス
    @param[in] src   コピー元メモリの先頭アドレス
    @param[in] len   コピーするサイズ
    @return コピー先メモリの先頭アドレス
 */
void *
memmove(void *dst, const void *src, size_t len){
	caddr_t end;
	caddr_t dstp = (caddr_t)dst, srcp = (caddr_t)src;

	if (dst < src) {
		end = (caddr_t)( src + len );

		do {
			*dstp++ = *srcp++;
		}while (srcp != end);

	} else {
		srcp = (caddr_t)(src + len);
		dstp = (caddr_t)(dst + len);
		end = (caddr_t)src;

		do{
			*--dstp = *--srcp;
		}while (srcp != end);
	}

	return dst;
}

/** 文字列長を返却する
    @param[in] str 文字列
    @retval strの長さ
 */
size_t
strlen(const char *str) {
	const char *cp = str;
	size_t len;

	for(len = 0; *cp != '\0'; ++cp, ++len);

	return len;
}
