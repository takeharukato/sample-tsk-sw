/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Bit wise operations                                               */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

/**  size_t型で与えられたサイズ中のMSB(1となっている最上位ビットの意味)の位置を算出する
     @param[in] x サイズ
     @retval 0 xが0だった
     @retval MSBの位置(再下位ビットを1として返却)
     @note xが0のケースを考慮して, 再下位ビットを1としてMSBを返却する
 */
int 
find_msr_bit_in_size(size_t x) {
	int i = 0;

	if (x == 0)
		goto out;

	for(i = (sizeof(x)*BIT_PER_BYTE - 1); i >= 0; --i) {
		/*  最上位ビットから順番にビットが立っているか検査する  */
		if (x & (1UL << i) ) /* 1になっているビットを検出  */
			return i + 1;
	}

out:
	return 0;  /* xが0である  */
}


/**  size_t型で与えられたサイズ中のLSB(1となっている最下位ビットの意味)の位置を算出する
     @param[in] x サイズ
     @retval 0 xが0だった
     @retval LSBの位置(再下位ビットを1として返却)
     @note xが0のケースを考慮して, 再下位ビットを1としてMSBを返却する
 */
int 
find_lsr_bit_in_size(size_t x) {
	int i = 0;

	if (x == 0)
		goto out;

	for(i = 0; i <= (sizeof(x)*BIT_PER_BYTE - 1); ++i) {
		/*  最下位ビットから順番にビットが立っているか検査する  */
		if (x & (1UL << i) ) /* 1になっているビットを検出  */
			return i + 1;
	}

out:
	return 0;  /* xが0である  */
}

/**  int型で与えられたサイズ中のMSB(1となっている最上位ビットの意味)の位置を算出する
     @param[in] x サイズ
     @retval 0 xが0だった
     @retval MSBの位置(再下位ビットを1として返却)
     @note xが0のケースを考慮して, 再下位ビットを1としてMSBを返却する
 */
int 
find_msr_bit(int x) {
	int i = 0;

	if (x == 0)
		goto out;

	for(i = (sizeof(x)*BIT_PER_BYTE - 1); i >= 0; --i) {
		/*  最上位ビットから順番にビットが立っているか検査する  */
		if (x & (1UL << i) ) /* 1になっているビットを検出  */
			return i + 1;
	}

out:
	return 0;  /* xが0である  */
}

/**  int型で与えられたサイズ中のLSB(1となっている最下位ビットの意味)の位置を算出する
     @param[in] x サイズ
     @retval 0 xが0だった
     @retval LSBの位置(再下位ビットを1として返却)
     @note xが0のケースを考慮して, 再下位ビットを1としてMSBを返却する
 */
int 
find_lsr_bit(int x) {
	int i = 0;

	if (x == 0)
		goto out;

	for(i = 0; i <= (sizeof(x)*BIT_PER_BYTE - 1); ++i) {
		/*  最下位ビットから順番にビットが立っているか検査する  */
		if (x & (1UL << i) ) /* 1になっているビットを検出  */
			return i + 1;
	}

out:
	return 0;  /* xが0である  */
}
