
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>

#include "debug.h"

static int g_debug_level = SEYRUSEFER_DEBUG_LEVEL_DEBUG;

const char * seyrusefer_debug_level_string (int level)
{
        if (level <= SEYRUSEFER_DEBUG_LEVEL_SILENT) {
                return "silent";
        } else if (level <= SEYRUSEFER_DEBUG_LEVEL_ASSERT) {
                return "assert";
        } else if (level <= SEYRUSEFER_DEBUG_LEVEL_ERROR) {
                return "error";
        } else if (level <= SEYRUSEFER_DEBUG_LEVEL_WARNING) {
                return "warning";
        } else if (level <= SEYRUSEFER_DEBUG_LEVEL_INFO) {
                return "info";
        } else if (level <= SEYRUSEFER_DEBUG_LEVEL_DEBUG) {
                return "debug";
        } else if (level <= SEYRUSEFER_DEBUG_LEVEL_TRACE) {
                return "trace";
        } else {
                return "noisy";
        }
}

int seyrusefer_debug_level_value (const char *level)
{
        if (strcasecmp(level, "silent") == 0 || strcasecmp(level, "s") == 0) {
                return SEYRUSEFER_DEBUG_LEVEL_SILENT;
        }else if (strcasecmp(level, "assert") == 0 || strcasecmp(level, "a") == 0) {
                return SEYRUSEFER_DEBUG_LEVEL_ASSERT;
        } else if (strcasecmp(level, "error") == 0 || strcasecmp(level, "e") == 0) {
                return SEYRUSEFER_DEBUG_LEVEL_ERROR;
        } else if (strcasecmp(level, "warning") == 0 || strcasecmp(level, "w") == 0) {
                return SEYRUSEFER_DEBUG_LEVEL_WARNING;
        } else if (strcasecmp(level, "info") == 0 || strcasecmp(level, "i") == 0) {
                return SEYRUSEFER_DEBUG_LEVEL_INFO;
        } else if (strcasecmp(level, "debug") == 0 || strcasecmp(level, "d") == 0) {
                return SEYRUSEFER_DEBUG_LEVEL_DEBUG;
        } else if (strcasecmp(level, "trace") == 0 || strcasecmp(level, "t") == 0) {
                return SEYRUSEFER_DEBUG_LEVEL_TRACE;
        } else {
                return SEYRUSEFER_DEBUG_LEVEL_TRACE;
        }
}

int seyrusefer_debug_set_level (unsigned int level)
{
        g_debug_level = level;
        return 0;
}

int seyrusefer_debug_get_level (void)
{
        return g_debug_level;
}

void seyrusefer_debug_printf (const char *tag, unsigned int level, const char *function, const char *file, unsigned int line, const char *format, ...)
{
        va_list ap;
        struct timeval timeval;
        if (g_debug_level < level) {
                return;
        }
        gettimeofday(&timeval, NULL);
        fprintf(stderr, "\033[0G[%lld.%03ld %-7s:%-10s] ", timeval.tv_sec, timeval.tv_usec / 1000, seyrusefer_debug_level_string(level), tag);
        va_start(ap, format);
        vfprintf(stderr, format, ap);
        va_end(ap);
        fprintf(stderr, " @ %s %s:%d\r\n\033[K", function, file, line);
}
