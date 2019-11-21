/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  string operations                                                 */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

/**
   sで示されるメモリ領域の先頭からnバイトをcで埋める。
    @param[in] s 書き込み先メモリの先頭アドレス
    @param[in] c 書き込む値(charとして扱われる)
    @param[in] n 書き込むバイト数
    @note 8バイトまたは4バイト境界から始まるアドレスについては, 可能な限り
    8バイトまたは4バイト単位で埋め, 余りバイトはバイト単位で埋める。
    OS内の典型的なデータ構造は, 8バイトまたは4バイト境界にそろっているので
    典型的な場合に効果があり極力単純化した実装を採用している。
 */
void *
memset(void *s, int c, size_t n){
	uint64_t   val;
	uint8_t   *cp8;
	uint32_t *cp32;
	uint64_t *cp64;
	uint32_t   c32;
	uint64_t   c64;
	size_t     len;

	len = n;  /* コピー長を記憶する */

	/*  32bit, 64bit長のバイトcで埋められた領域を作成する  */
	val = c & 0xff;  /* バイトcを符号なし64bit整数に変換  */
	/*  32bitの領域をバイトcで埋める  */
	c32 = (uint32_t)((val << 24) | (val << 16) | (val << 8) | val);
	/*  64bitの領域をバイトcで埋める  */
	c64 = ( ( (uint64_t)c32 ) << 32 ) | (uint64_t)c32;

	if ( ( ( (uintptr_t)s & ( sizeof(uint64_t) - 1) ) == 0 ) 
	    && ( len >= sizeof(uint64_t) ) ) {
		
		/* 8バイト境界で始まり, 残転送量が8バイト以上ある場合は, 8バイト単位で
		 * 領域を埋める
		 */
		for( cp64 = s; len >= sizeof(uint64_t); len -= sizeof(uint64_t)) 
			*cp64++ = c64;

		cp8 = (uint8_t *)cp64;  /* 残りバイトの先頭を指す */
	} else if ( ( ( (uintptr_t)s & ( sizeof(uint32_t) - 1) ) == 0 ) 
	    && ( len >= sizeof(uint32_t) ) ) {

		/* 4バイト境界で始まり, 残転送量が4バイト以上ある場合は, 4バイト単位で
		 * 領域を埋める
		 */
		for( cp32 = s; len >= sizeof(uint32_t); len -= sizeof(uint32_t)) 
			*cp32++ = c32;
		
		cp8 = (uint8_t *)cp32; /* 残りバイトの先頭を指す */
	} else 
		cp8 = (uint8_t *)s; /* 8バイトでも4バイト境界でもなければバイト単位で埋める */

	/* 残りバイトをフィルする */
	while ( len-- > 0 ) 
		*cp8++ = (uint8_t)(c & 0xff);

	return s;
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
	char *end  = (char *)(src + len);
	char *dstp = (char *)dst;
	char *srcp = (char *)src;

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
	char *end;
	char *dstp = (char *)dst;
	char *srcp = (char *)src;

	if (dst < src) {
		end = (char *)( src + len );

		do {
			*dstp++ = *srcp++;
		}while (srcp != end);

	} else {
		srcp = (char *)(src + len);
		dstp = (char *)(dst + len);
		end = (char *)src;

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
	char *cp = (char *)str;
	size_t len;

	for(len = 0; *cp != '\0'; ++cp, ++len);

	return len;
}

/** 文字列 s1 とs2とを比較する 
    @param[in] s1 比較対象のメモリ領域1
    @param[in] s2 比較対象のメモリ領域2
    @retval 負の整数 s1がs2よりも小さい(s1 < s2)
    @retval 0        s1とs2が等しい
    @retval 正の整数 s1がs2よりも大きいさい(s1 > s2)
 */
int
strcmp(char const *s1, char const *s2){

	for(; ( *s1 != '\0' ) && ( *s1 == *s2 ) ; ++s1, ++s2);

	return (int)( *s1 - *s2 );
}

/** メモリ m1 とm2の先頭nバイトを比較する 
    @param[in] m1 比較対象のメモリ領域1
    @param[in] m2 比較対象のメモリ領域2
    @param[in] n  比較長
    @retval 負の整数 s1がs2よりも小さい(s1 < s2)
    @retval 0        s1とs2が等しい
    @retval 正の整数 s1がs2よりも大きいさい(s1 > s2)
 */
int 
memcmp(const void *m1, const void *m2, size_t n){
	const unsigned char *s1, *s2;
	signed char rc = 0;

	for(s1 = m1, s2 = m2; n > 0; ++s1, ++s2, --n) {
		rc = *s1 - *s2;
		if (rc != 0)
			break;
	}

	return (int)rc;
}

/** 文字列 s1 とs2の先頭n文字を比較する 
    @param[in] s1 比較対象のメモリ領域1
    @param[in] s2 比較対象のメモリ領域2
    @retval 負の整数 s1がs2よりも小さい(s1 < s2)
    @retval 0        s1とs2が等しい
    @retval 正の整数 s1がs2よりも大きいさい(s1 > s2)
 */
int 
strncmp(const char *s1, const char *s2, size_t n){
	signed char rc = 0;

	for(; *s1 != '\0'; ++s1, ++s2, --n) {

		rc = *s1 - *s2;
		if ( (rc) || (n == 0) )
			break;
		
	}

	return (int)rc;
}

/** 文字列 srcをdestにコピーする
    @param[in] dest コピー先のアドレス
    @param[in] src  コピー元のアドレス
    @retval 負の整数 s1がs2よりも小さい(s1 < s2)
    @retval 0        s1とs2が等しい
    @retval 正の整数 s1がs2よりも大きいさい(s1 > s2)
 */
char *
strcpy(char *dest, char const *src){
	char *tmp;

	for(tmp = dest; ; ++src, ++dest) {

		*dest = *src;
		if ( *src == '\0' )
			break;
	}
	return tmp;
}

/** 文字列 srcをdestに指定バイト数だけコピーする
    @param[in] dest コピー先のアドレス
    @param[in] src  コピー元のアドレス
    @param[in] count  コピーするバイト数
    @retval 負の整数 s1がs2よりも小さい(s1 < s2)
    @retval 0        s1とs2が等しい
    @retval 正の整数 s1がs2よりも大きいさい(s1 > s2)
 */
char *
strncpy(char *dest, char const *src, size_t count){
	char *tmp;

	if (count == 0)
		return dest;

	for(tmp = dest; ; ++src, ++dest, --count) {

		*dest = *src;
		if ( ( count == 0 ) || ( *src == '\0' ) )
			break;
	}

	return tmp;
}
