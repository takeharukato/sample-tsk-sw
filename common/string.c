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
	char * m = b;

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

/** 文字列 s1 とs2の先頭n文字を比較する 
    @param[in] s1 比較対象のメモリ領域1
    @param[in] s2 比較対象のメモリ領域2
    @retval 負の整数 s1がs2よりも小さい(s1 < s2)
    @retval 0        s1とs2が等しい
    @retval 正の整数 s1がs2よりも大きいさい(s1 > s2)
 */
int 
strncmp(const char *s1, const char *s2, size_t n){
	signed char rc;

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
