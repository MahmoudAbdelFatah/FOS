
#include <inc/lib.h>

char *argv0;

/*
 * Panic is called on unresolvable fatal errors.
 * It prints "panic: <message>", then causes a breakpoint exception,
 * which causes FOS to enter the FOS kernel monitor.
 */
void
_panic(const char *file, int line, const char *fmt,...)
{
	va_list ap;
	va_start(ap, fmt);
	// Print the panic message
	if (argv0)
		cprintf("%s: ", argv0);
	cprintf("user panic in %s at %s:%d: ", binaryname, file, line);
	vcprintf(fmt, ap);
	vcprintf("\n", NULL);

	// Cause a breakpoint exception
//	while (1);
//		asm volatile("int3");

	//2013: exit the panic env only
	exit() ;

	// should not return here
	while (1) ;
}

