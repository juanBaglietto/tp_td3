#define DEBUG_HIG 2
#define DEBUG_MID 1
#define DEBUG_LOW 0

/*el siguiente define establece el nivel de depuracion*/
/*#define DEBUG_LVL DEBUG_LOW*/

#ifndef DEBUG_LVL
#define DEBUG_LVL (DEBUG_HIG+1)
#endif

#define TRACE_MSG(msg, ...) fprintf(stderr,"[%s:%s:%d]-(T%ld:S%d:F%d)::" msg, __FILE__, __FUNCTION__, __LINE__,syscall(SYS_gettid), getpid(), getppid(), ## __VA_ARGS__);

#if (DEBUG_LVL == DEBUG_HIG)
#define TRACE_HIG(msg, ...) TRACE_MSG(msg,## __VA_ARGS__)
#define TRACE_MID(msg, ...)
#define TRACE_LOW(msg, ...)
#elif (DEBUG_LVL == DEBUG_MID)
#define TRACE_LOW(msg, ...) TRACE_MSG(msg,## __VA_ARGS__)
#define TRACE_MID(msg, ...) TRACE_MSG(msg,## __VA_ARGS__)
#define TRACE_LOW(msg, ...)
#elif (DEBUG_LVL == DEBUG_LOW)
#define TRACE_LOW(msg, ...) TRACE_MSG(msg,## __VA_ARGS__)
#define TRACE_MID(msg, ...) TRACE_MSG(msg,## __VA_ARGS__)
#define TRACE_HIG(msg, ...) TRACE_MSG(msg,## __VA_ARGS__)
#else
#define TRACE_LOW(msg, ...)
#define TRACE_HIG(msg, ...)
#define TRACE_MID(msg, ...)
#endif
