/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Bit wise operations                                               */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

/**  int型で与えられた変数x中のMSB(1となっている最上位ビットの意味)の位置を算出する
     @param[in]     x 調査対象の変数
     @param[out] *res MSB bit position
     @retval 0     正常終了
     @retval ESRCH xが0だった
 */
int 
find_msr_bit(int x, int *res) {
	int rc = 0;
	int  i = 0;

	rc = ESRCH;
	if ( x == 0 ) {
		
		*res = 0;
		goto out;
	}

	for(i = (sizeof(x)*BIT_PER_BYTE - 1); i >= 0; --i) {

		/*  最上位ビットから順番にビットが立っているか検査する  */
		if (x & (1UL << i) ) { /* 1になっているビットを検出  */

			*res = i;
			rc = 0;
			break;
		}
	}

out:
	return rc;
}

/**  int型で与えられた変数中のLSB(1となっている最下位ビットの意味)の位置を算出する
     @param[in]  x 調査対象の変数
     @param[out] *res LSB bit position
     @retval 0 正常終了
     @retval ESRCH xが0だった
 */
int 
find_lsr_bit(int x, int *res) {
	int rc = 0;
	int  i = 0;

	rc = ESRCH;
	if ( x == 0 ) {
		
		*res = 0;
		goto out;
	}

	for(i = 0; sizeof(x)*BIT_PER_BYTE > i; ++i) {

		/*  最下位ビットから順番にビットが立っているか検査する  */
		if (x & (1UL << i) ) { /* 1になっているビットを検出  */

			*res = i;
			rc = 0;
			break;
		}
	}

out:
	return rc;
}
