#ifndef DEBUG_H
#define DEBUG_H
#ifdef DEBUG
	#include <stdio.h>
	#include <errno.h>
	#include <time.h>

	#include "config.h"
	#define DEBUG_PRINT(x, ...) do { printf(x, ##__VA_ARGS__); } while (0)
	#define DEBUG_PRINT_ERR(x, ...) do { fprintf(stderr, x, ##__VA_ARGS__); } while (0)

	// Macro for printing indented output
	#define PRINT_INDENT() do { \
		for (int _i = 0; _i < timing_nest_level; ++_i) { \
			printf("    "); /* 4 spaces indentation */ \
		} \
	} while (0)

	#define DEBUG_PRINT_CONFIG_VALUES() do { DEBUG_PRINT("\n------ Config Values ------\n"); print_config(); DEBUG_PRINT("----------------------------\n\n"); } while (0)
	#define TIME_FUNCTION(func, ...) do { clock_t start = clock(); DEBUG_PRINT("---- @Calling %s ----\n", #func); func(__VA_ARGS__); DEBUG_PRINT("Finished %s in %f seconds\n", #func, (double)(clock() - start) / CLOCKS_PER_SEC); } while(0)

	#ifdef DEBUG_TIME
		#include <stdarg.h>

		// Global or thread-local variable to track nesting level
		extern int timing_nest_level;

		// Function to format variadic arguments into a string (up to a buffer size)
		extern void format_varargs(char *buffer, size_t buffer_size, const char *fmt, ...);

		// Macros for timing functions with a return value
		#define TIME_FUNCTION_RETURN_INT(func, fmt, ...) ({ \
			clock_t start = clock(); \
			PRINT_INDENT(); \
			char args_buffer[256]; \
			format_varargs(args_buffer, sizeof(args_buffer), fmt, __VA_ARGS__); \
			DEBUG_PRINT("---- @Calling %s with args %s ----\n", #func, args_buffer); \
			timing_nest_level++; \
			int result = func(__VA_ARGS__); \
			timing_nest_level--; \
			PRINT_INDENT(); \
			DEBUG_PRINT("%sFinished %s in %f seconds\n", (timing_nest_level > 0 ? "| " : ""), #func, (double)(clock() - start) / CLOCKS_PER_SEC); \
			result; \
		})
		#define TIME_FUNCTION_RETURN_BOOL(func, fmt, ...) ({ \
			clock_t start = clock(); \
			PRINT_INDENT(); \
			char args_buffer[256]; \
			format_varargs(args_buffer, sizeof(args_buffer), fmt, __VA_ARGS__); \
			DEBUG_PRINT("---- @Calling %s with args %s ----\n", #func, args_buffer); \
			timing_nest_level++; \
			bool result = func(__VA_ARGS__); \
			timing_nest_level--; \
			PRINT_INDENT(); \
			DEBUG_PRINT("%sFinished %s in %f seconds\n", (timing_nest_level > 0 ? "| " : ""), #func, (double)(clock() - start) / CLOCKS_PER_SEC); \
			result; \
		})
		#define TIME_FUNCTION_RETURN_DOUBLE(func, fmt, ...) ({ \
			clock_t start = clock(); \
			PRINT_INDENT(); \
			char args_buffer[256]; \
			format_varargs(args_buffer, sizeof(args_buffer), fmt, __VA_ARGS__); \
			DEBUG_PRINT("---- @Calling %s with args %s ----\n", #func, args_buffer); \
			timing_nest_level++; \
			double result = func(__VA_ARGS__); \
			timing_nest_level--; \
			PRINT_INDENT(); \
			DEBUG_PRINT("%sFinished %s in %f seconds\n", (timing_nest_level > 0 ? "| " : ""), #func, (double)(clock() - start) / CLOCKS_PER_SEC); \
			result; \
		})

		// Macro for timing void functions (functions that don't return a value)
		#define TIME_FUNCTION_VOID(func, ...) do { \
			clock_t start = clock(); \
			PRINT_INDENT(); \
			DEBUG_PRINT("---- @Calling %s ----\n", #func); \
			timing_nest_level++; \
			func(__VA_ARGS__); \
			timing_nest_level--; \
			PRINT_INDENT(); \
			DEBUG_PRINT("%sFinished %s in %f seconds\n", (timing_nest_level > 0 ? "| " : ""), #func, (double)(clock() - start) / CLOCKS_PER_SEC); \
		} while (0)
	#else
		#define TIME_FUNCTION_RETURN_INT(func, fmt, ...) func(__VA_ARGS__)
		#define TIME_FUNCTION_RETURN_BOOL(func, fmt, ...) func(__VA_ARGS__)
		#define TIME_FUNCTION_RETURN_DOUBLE(func, fmt, ...) func(__VA_ARGS__)
		#define TIME_FUNCTION_VOID(func, ...) func(__VA_ARGS__)
	#endif
#else
	#define DEBUG_PRINT(x, ...) do {} while (0)
	#define DEBUG_PRINT_ERR(x, ...) do {} while (0)
	#define DEBUG_PRINT_CONFIG_VALUES() do {} while (0)
	#define TIME_FUNCTION_RETURN_INT(func, fmt, ...) func(__VA_ARGS__)
	#define TIME_FUNCTION_RETURN_BOOL(func, fmt, ...) func(__VA_ARGS__)
	#define TIME_FUNCTION_RETURN_DOUBLE(func, fmt, ...) func(__VA_ARGS__)
	#define TIME_FUNCTION_VOID(func, ...) func(__VA_ARGS__)
#endif

#endif // DEBUG_H
