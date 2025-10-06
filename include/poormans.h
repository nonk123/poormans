#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#include <windows.h>

void poormans_init()
#ifdef POORMANS_IMPLEMENTATION
	{

	}
#else
	;
#endif

int poormans_running()
#ifdef POORMANS_IMPLEMENTATION
{
	return 0;
}
#else
	;
#endif

void poormans_tick()
#ifdef POORMANS_IMPLEMENTATION
	{

	}
#else
	;
#endif
