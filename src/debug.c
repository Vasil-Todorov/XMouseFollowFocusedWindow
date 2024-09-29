#include "debug.h"

#ifdef DEBUG
	#ifdef DEBUG_TIME
		// Define timing nest level only when DEBUG is enabled
		int timing_nest_level = 0;
		void format_varargs(char *buffer, size_t buffer_size, const char *fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			vsnprintf(buffer, buffer_size, fmt, args);
			va_end(args);
		};
	#endif
#endif
