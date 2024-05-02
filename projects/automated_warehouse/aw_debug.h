#ifndef _PROJECTS_PROJECT1_DEBUG_H__
#define _PROJECTS_PROJECT1_DEBUG_H__

#include <stdio.h>
#include <stdlib.h>

#ifdef AW_DEBUG
#define DEBUG_MODE 1
#else
#define DEBUG_MODE 0
#endif

/*******************
 * Terminal colors *
 *******************/

# define COLOR_BLACK   "\x1b[30m"
# define COLOR_RED     "\x1b[31m"
# define COLOR_GREEN   "\x1b[32m"
# define COLOR_YELLOW  "\x1b[33m"
# define COLOR_BLUE    "\x1b[34m"
# define COLOR_MAGENTA "\x1b[35m"
# define COLOR_CYAN    "\x1b[36m"
# define COLOR_WHITE   "\x1b[37m"
# define COLOR_RESET   "\x1b[0m"

/************************
 * Debug & error macros *
 ************************/

#define DEBUG_PRINT(type, component, format, ...) \
    do { \
        if (DEBUG_MODE) { \
            printf("[%s](%s) " format " %s:%d%s\n", \
                type, component, __VA_ARGS__, __FILE__, __LINE__, COLOR_RESET); \
        } \
    } while (0)

/* Show a prefixed warning. */

#define WARN(component, format, ...) \
    DEBUG_PRINT(COLOR_YELLOW "WARN", component, format, ##__VA_ARGS__)

/* Show a prefixed fatal error message (not used in afl). */

#define ERROR(component, format, ...) \
    DEBUG_PRINT(COLOR_RED "ERROR", component, format, ##__VA_ARGS__)

/* Show a prefixed "success" message. */

#define SUCCESS(component, format, ...) \
    DEBUG_PRINT(COLOR_GREEN "SUCCESS", component, format, ##__VA_ARGS__)

/* Show a prefixed "doing something" message. */

#define INFO(component, format, ...) \
    DEBUG_PRINT(COLOR_CYAN "INFO", component, format, ##__VA_ARGS__)


# endif