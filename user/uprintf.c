#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#include <kern/kernel.h>

#include <uprintf.h>

int
uprintf(const char *fmt, ...) {
	va_list          ap;
	size_t          len;
	char buf[PRFBUFLEN];

	va_start(ap, fmt);
	len = kvprintf(PRFBUFLEN, buf, fmt, 10, ap);
	va_end(ap);
	buf[len] = '\0';

	fs_write(1, buf, len);	

	return len;
}

void
cls(void) {

	uprintf("\033[2J");
}

void
locate(int x, int y) {

	uprintf("\033[%d;%dH", y, x);
}
