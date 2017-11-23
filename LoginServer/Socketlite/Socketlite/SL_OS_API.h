#ifndef SOCKETLITE_OS_API_H
#define SOCKETLITE_OS_API_H
#include "SL_Config.h"

#ifdef SOCKETLITE_OS_WINDOWS

// We can't just use _snprintf as a drop-in replacement, because it
// doesn't always NUL-terminate. :-(
extern int snprintf(char *str, size_t size, const char *format, ...);

#endif

class SOCKETLITE_API SL_OS_API
{
private:
    SL_OS_API();
    ~SL_OS_API();

public:
    static int futil_stat(const SL_TCHAR *path_name, SL_STAT *stat_buf);
    static int futil_access(const SL_TCHAR *path_name, int mode);
    static int futil_chmod(const SL_TCHAR  *path_name, int mode);
    static int futil_rename(const SL_TCHAR *old_name, const SL_TCHAR *new_name);
    static int futil_remove(const SL_TCHAR *file_name);
    static int futil_unlink(const SL_TCHAR *file_name);

    static int futil_chdir(const SL_TCHAR *dir_name);
    static int futil_mkdir(const SL_TCHAR *dir_name, int mode);
    static int futil_mkdir_multilevel(const SL_TCHAR *dir_name, int mode);
    static int futil_rmdir(const SL_TCHAR *dir_name);

    //检测文件是否存在
    static bool futil_exists(const SL_TCHAR *path_name);

    //取得文件大小
    static int futil_filesize(const SL_TCHAR *file_name, unsigned long &file_size);
    static int futil_filelength(const SL_TCHAR *file_name, unsigned long &file_size);

    static time_t util_strtotime(const SL_TCHAR *time, const SL_TCHAR *format);

};

#endif

