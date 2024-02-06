
#if !defined(SEYRUSEFER_DEBUG_TAG)
#define SEYRUSEFER_DEBUG_TAG            "unknown"
#endif

enum {
        SEYRUSEFER_DEBUG_LEVEL_SILENT   = 0,
        SEYRUSEFER_DEBUG_LEVEL_ASSERT   = 1,
        SEYRUSEFER_DEBUG_LEVEL_ERROR    = 2,
        SEYRUSEFER_DEBUG_LEVEL_WARNING  = 3,
        SEYRUSEFER_DEBUG_LEVEL_INFO     = 4,
        SEYRUSEFER_DEBUG_LEVEL_DEBUG    = 5,
        SEYRUSEFER_DEBUG_LEVEL_TRACE    = 6
#define SEYRUSEFER_DEBUG_LEVEL_SILENT   SEYRUSEFER_DEBUG_LEVEL_SILENT
#define SEYRUSEFER_DEBUG_LEVEL_ASSERT   SEYRUSEFER_DEBUG_LEVEL_ASSERT
#define SEYRUSEFER_DEBUG_LEVEL_ERROR    SEYRUSEFER_DEBUG_LEVEL_ERROR
#define SEYRUSEFER_DEBUG_LEVEL_WARNING  SEYRUSEFER_DEBUG_LEVEL_WARNING
#define SEYRUSEFER_DEBUG_LEVEL_INFO     SEYRUSEFER_DEBUG_LEVEL_INFO
#define SEYRUSEFER_DEBUG_LEVEL_DEBUG    SEYRUSEFER_DEBUG_LEVEL_DEBUG
#define SEYRUSEFER_DEBUG_LEVEL_TRACE    SEYRUSEFER_DEBUG_LEVEL_TRACE
};

#define seyrusefer_assertf(a...)        seyrusefer_debug_printf(SEYRUSEFER_DEBUG_TAG, SEYRUSEFER_DEBUG_LEVEL_ASSERT, __FUNCTION__, __FILE__, __LINE__, a)
#define seyrusefer_errorf(a...)         seyrusefer_debug_printf(SEYRUSEFER_DEBUG_TAG, SEYRUSEFER_DEBUG_LEVEL_ERROR, __FUNCTION__, __FILE__, __LINE__, a)
#define seyrusefer_warningf(a...)       seyrusefer_debug_printf(SEYRUSEFER_DEBUG_TAG, SEYRUSEFER_DEBUG_LEVEL_WARNING, __FUNCTION__, __FILE__, __LINE__, a)
#define seyrusefer_infof(a...)          seyrusefer_debug_printf(SEYRUSEFER_DEBUG_TAG, SEYRUSEFER_DEBUG_LEVEL_INFO, __FUNCTION__, __FILE__, __LINE__, a)
#define seyrusefer_debugf(a...)         seyrusefer_debug_printf(SEYRUSEFER_DEBUG_TAG, SEYRUSEFER_DEBUG_LEVEL_DEBUG, __FUNCTION__, __FILE__, __LINE__, a)
#define seyrusefer_tracef(a...)         seyrusefer_debug_printf(SEYRUSEFER_DEBUG_TAG, SEYRUSEFER_DEBUG_LEVEL_TRACE, __FUNCTION__, __FILE__, __LINE__, a)

int seyrusefer_debug_set_level (unsigned int level);
int seyrusefer_debug_get_level (void);
const char * seyrusefer_debug_level_string (int level);
int seyrusefer_debug_level_value (const char *level);
void seyrusefer_debug_printf (const char *tag, unsigned int level, const char *function, const char *file, unsigned int line, const char *format, ...);
