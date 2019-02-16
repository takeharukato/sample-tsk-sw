/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2018 Takeharu KATO                                      */
/*                                                                    */
/*  User tasks - Life Game                                            */
/*                                                                    */
/**********************************************************************/
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#include <kern/kernel.h>

#include <MT.h>

#include <uprintf.h>

#define for_x for (x = 0; x < w; x++)
#define for_y for (y = 0; y < h; y++)
#define for_xy for_x for_y

thread_t *thrD;

void
show(void *u, int w, int h, int gen){
	int (*univ)[w] = u;
	int x,y;

	uprintf("\033[H");
	locate(0, 0);
	for_y {
		for_x uprintf(univ[y][x] ? "\033[07m  \033[m" : "  ");
		uprintf("\033[E");
	}
	locate(64, 0);
	uprintf("Generation: %02d", gen);
}
 
void
evolve(void *u, int w, int h){
	unsigned (*univ)[w] = u;
	unsigned new[h][w];
 	int x,y;

	for_y for_x {
		int n = 0;
		int x1, y1;

		for (y1 = y - 1; y1 <= y + 1; y1++)
			for (x1 = x - 1; x1 <= x + 1; x1++)
				if (univ[(y1 + h) % h][(x1 + w) % w])
					n++;
 
		if (univ[y][x]) n--;
		new[y][x] = (n == 3 || (n == 2 && univ[y][x]));
	}
	for_y for_x univ[y][x] = new[y][x];
}
 
void
game(int w, int h){
	unsigned univ[h][w];
	int x,y;
	int   i;


	while (1) {
		
		for(i = 0; 100 > i; ++i) {

			memset(univ, 0, sizeof (univ));

			for_xy univ[y][x] = genrand_int31() < 0x7fffffff / 10 ? 1 : 0;

			show(univ, w, h, i);
			evolve(univ, w, h);
			thr_delay(200);
		}
	}
}

void
threadD(void *arg) {

	game(30, 30);
}

void
user_init(void) {
	int  i;

	cls();
	for( i = 0; 50 > i; ++i)
		kprintf("\n");

	thr_create_thread(1, &thrD, NULL, threadD, NULL);
}
