#ifndef SOCKETLITE_TEST_H
#define SOCKETLITE_TEST_H
#include "SL_Config.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>    // for strlen(), strcmp()
#include <assert.h>
#include <errno.h>     // for errno

#define SL_LOG_MAX_LINE_LEN 4096
#define SL_WRITE_TO_STDERR(buf, len)    write(SL_STDERR_FILENO, buf, len)

enum SL_CHECK_ACTION
{ 
    SL_CHECK_ACTION_CONTINUE = 0, 
    SL_CHECK_ACTION_PAUSE, 
    SL_CHECK_ACTION_EXIT, 
    SL_CHECK_ACTION_ABORT 
};

#define SL_CHECK_ACTION(action_type)                                            \
    {                                                                           \
        if ((action_type) == SL_CHECK_ACTION_PAUSE)                             \
        {                                                                       \
            SL_WRITE_TO_STDERR("Press <return> key to continue",                \
                sizeof("Press <return> key to continue")-1);                    \
            getchar();                                                          \
        }                                                                       \
        else if ((action_type) == SL_CHECK_ACTION_EXIT)                         \
        {                                                                       \
            exit(1);                                                            \
            break;                                                              \
        }                                                                       \
        else if ((action_type) == SL_CHECK_ACTION_ABORT)                        \
        {                                                                       \
            abort();                                                            \
            break;                                                              \
        }                                                                       \
    }

// CHECK dies with a fatal error if condition is not true.  It is *not*
// controlled by NDEBUG, so the check will be executed regardless of
// compilation mode.  Therefore, it is safe to do things like:
//    CHECK(fp->Write(x) == 4)
// Note we use write instead of printf/puts to avoid the risk we'll
// call malloc().
#define SL_CHECK(action_type, condition)                                        \
    do                                                                          \
    {                                                                           \
        if (!(condition))                                                       \
        {                                                                       \
            SL_WRITE_TO_STDERR("Check failed: " #condition "\n",                \
            sizeof("Check failed: " #condition "\n")-1);                        \
            SL_CHECK_ACTION(action_type);                                       \
        }                                                                       \
    } while (0)                                                                 

// This takes a message to print.  The name is historical.
#define SL_RAW_CHECK(action_type, condition, message)                           \
    do                                                                          \
    {                                                                           \
        if (!(condition))                                                       \
        {                                                                       \
            SL_WRITE_TO_STDERR("Check failed: " #condition ": " message "\n",   \
            sizeof("Check failed: " #condition ": " message "\n")-1);           \
            SL_CHECK_ACTION(action_type)                                        \
        }                                                                       \
    } while (0)

// This is like SL_RAW_CHECK, but only in debug-mode
#ifdef NDEBUG
    enum { SL_DEBUG_MODE = 0 };
    #define SL_RAW_DCHECK(condition, message)
#else
    enum { SL_DEBUG_MODE = 1 };
    #define SL_RAW_DCHECK(action_type, condition, message) SL_RAW_CHECK(action_type, condition, message)
#endif

// This prints errno as well.  Note we use write instead of printf/puts to
// avoid the risk we'll call malloc().
#define SL_PCHECK(action_type, condition)                                       \
    do                                                                          \
    {                                                                           \
        if (!(condition))                                                       \
        {                                                                       \
            const int err_no = errno;                                           \
            SL_WRITE_TO_STDERR("Check failed: " #condition ": ",                \
            sizeof("Check failed: " #condition ": ")-1);                        \
            SL_WRITE_TO_STDERR(strerror(err_no), strlen(strerror(err_no)));     \
            SL_WRITE_TO_STDERR("\n", sizeof("\n")-1);                           \
            SL_CHECK_ACTION(action_type);                                       \
        }                                                                       \
    } while (0)

// Helper macro for binary operators; prints the two values on error
// Don't use this macro directly in your code, use CHECK_EQ et al below

// WARNING: These don't compile correctly if one of the arguments is a pointer
// and the other is NULL. To work around this, simply static_cast NULL to the
// type of the desired pointer.

// TODO(jandrews): Also print the values in case of failure.  Requires some
// sort of type-sensitive ToString() function.
#define SL_CHECK_OP(action_type, op, val1, val2)                                \
    do                                                                          \
    {                                                                           \
        if (!((val1) op (val2)))                                                \
        {                                                                       \
            fprintf(stderr, "Check failed: %s %s %s\n", #val1, #op, #val2);     \
            SL_CHECK_ACTION(action_type);                                       \
        }                                                                       \
    } while (0)

#define SL_CHECK_EQ(action_type, val1, val2) SL_CHECK_OP(action_type, ==, val1, val2)
#define SL_CHECK_NE(action_type, val1, val2) SL_CHECK_OP(action_type, !=, val1, val2)
#define SL_CHECK_LE(action_type, val1, val2) SL_CHECK_OP(action_type, <=, val1, val2)
#define SL_CHECK_LT(action_type, val1, val2) SL_CHECK_OP(action_type, < , val1, val2)
#define SL_CHECK_GE(action_type, val1, val2) SL_CHECK_OP(action_type, >=, val1, val2)
#define SL_CHECK_GT(action_type, val1, val2) SL_CHECK_OP(action_type, > , val1, val2)

// A synonym for CHECK_* that is used in some unittests.
#define SL_EXPECT_EQ(action_type, val1, val2) SL_CHECK_EQ(action_type, val1, val2)
#define SL_EXPECT_NE(action_type, val1, val2) SL_CHECK_NE(action_type, val1, val2)
#define SL_EXPECT_LE(action_type, val1, val2) SL_CHECK_LE(action_type, val1, val2)
#define SL_EXPECT_LT(action_type, val1, val2) SL_CHECK_LT(action_type, val1, val2)
#define SL_EXPECT_GE(action_type, val1, val2) SL_CHECK_GE(action_type, val1, val2)
#define SL_EXPECT_GT(action_type, val1, val2) SL_CHECK_GT(action_type, val1, val2)
#define SL_ASSERT_EQ(action_type, val1, val2) SL_CHECK_EQ(action_type, val1, val2)
#define SL_ASSERT_NE(action_type, val1, val2) SL_CHECK_NE(action_type, val1, val2)
#define SL_ASSERT_LE(action_type, val1, val2) SL_CHECK_LE(action_type, val1, val2)
#define SL_ASSERT_LT(action_type, val1, val2) SL_CHECK_LT(action_type, val1, val2)
#define SL_ASSERT_GE(action_type, val1, val2) SL_CHECK_GE(action_type, val1, val2)
#define SL_ASSERT_GT(action_type, val1, val2) SL_CHECK_GT(action_type, val1, val2)

// As are these variants.
#define SL_EXPECT_TRUE(action_type, cond)     SL_CHECK(action_type, cond)
#define SL_EXPECT_FALSE(action_type, cond)    SL_CHECK(action_type, (!(cond)))
#define SL_EXPECT_STREQ(action_type, a, b)    SL_CHECK(action_type, (strcmp(a, b) == 0))
#define SL_ASSERT_TRUE(action_type, cond)     SL_CHECK(action_type, cond)
#define SL_ASSERT_FALSE(action_type, cond)    SL_CHECK(action_type, (!(cond)))
#define SL_ASSERT_STREQ(action_type, a, b)    SL_CHECK(action_type, (strcmp(a, b) == 0))

// Used for (libc) functions that return -1 and set errno
#define SL_CHECK_ERR(action_type, invocation) SL_PCHECK(action_type, ((invocation) != -1))

// A few more checks that only happen in debug mode
#ifdef SOCKETLITE_NDEBUG
    #define SL_DCHECK_EQ(action_type, val1, val2)
    #define SL_DCHECK_NE(action_type, val1, val2)
    #define SL_DCHECK_LE(action_type, val1, val2)
    #define SL_DCHECK_LT(action_type, val1, val2)
    #define SL_DCHECK_GE(action_type, val1, val2)
    #define SL_DCHECK_GT(action_type, val1, val2)
#else
    #define SL_DCHECK_EQ(action_type, val1, val2)  SL_CHECK_EQ(action_type, val1, val2)
    #define SL_DCHECK_NE(action_type, val1, val2)  SL_CHECK_NE(action_type, val1, val2)
    #define SL_DCHECK_LE(action_type, val1, val2)  SL_CHECK_LE(action_type, val1, val2)
    #define SL_DCHECK_LT(action_type, val1, val2)  SL_CHECK_LT(action_type, val1, val2)
    #define SL_DCHECK_GE(action_type, val1, val2)  SL_CHECK_GE(action_type, val1, val2)
    #define SL_DCHECK_GT(action_type, val1, val2)  SL_CHECK_GT(action_type, val1, val2)
#endif

#endif

