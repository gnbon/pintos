#ifndef _PROJECTS_PROJECT1_DEBUG_H__
#define _PROJECTS_PROJECT1_DEBUG_H__

#include <errno.h>

#ifdef DEBUG
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
            fprintf(stderr, "%s[%s] %s:%d: " format "%s\n", \
                type, component, __FILE__, __LINE__, __VA_ARGS__, COLOR_RESET); \
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

#define ACT(component, format, ...) \
    DEBUG_PRINT(COLOR_CYAN "ACT", component, format, ##__VA_ARGS__)


# endif