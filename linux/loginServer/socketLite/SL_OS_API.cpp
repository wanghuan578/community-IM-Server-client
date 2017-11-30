#include "SL_OS_API.h"
#include "SL_ByteBuffer.h"

#ifdef SOCKETLITE_OS_WINDOWS
#include <io.h>
#include <direct.h>
#include "Shlwapi.h"
#include <stdarg.h>    // for va_list, va_start, va_end
#pragma comment(lib, "shlwapi.lib")

// Calls the windows _vsnprintf, but always NUL-terminate.
int snprintf(char *str, size_t size, const char *format, ...) 
{
    if (0 == size)          // not even room for a \0?
    {
        return -1;          // not what C99 says to do, but what windows does
    }
    str[size-1] = '\0';
    va_list ap;
    va_start(ap, format);
    const int rc = _vsnprintf(str, size-1, format, ap);
    va_end(ap);
    return rc;
}
#endif

SL_OS_API::SL_OS_API()
{
}

SL_OS_API::~SL_OS_API()
{
}

int SL_OS_API::futil_stat(const SL_TCHAR *file_name, SL_STAT *stat_buf)
{
#ifdef SOCKETLITE_OS_WINDOWS
    return _tstat(file_name, stat_buf);
#else
    return stat(file_name, stat_buf);
#endif    
}

int SL_OS_API::futil_access(const SL_TCHAR *path_name, int mode)
{
#ifdef SOCKETLITE_OS_WINDOWS
    return _taccess(path_name, mode);
#else
    return access(path_name, mode);
#endif
}

int SL_OS_API::futil_chmod(const SL_TCHAR *path_name, int mode)
{
#ifdef SOCKETLITE_OS_WINDOWS
    return _tchmod(path_name, mode);
#else
    return chmod(path_name, mode);
#endif
}

int SL_OS_API::futil_rename(const SL_TCHAR *old_name, const SL_TCHAR *new_name)
{
#ifdef SOCKETLITE_OS_WINDOWS
    return _trename(old_name, new_name);
#else
    return rename(old_name, new_name);    
#endif    
}

int SL_OS_API::futil_remove(const SL_TCHAR *file_name)
{
#ifdef SOCKETLITE_OS_WINDOWS
    return _tremove(file_name);
#else
    return remove(file_name);
#endif    
}

int SL_OS_API::futil_unlink(const SL_TCHAR *file_name)
{
#ifdef SOCKETLITE_OS_WINDOWS
    return _tunlink(file_name);
#else
    return unlink(file_name);
#endif
}

int SL_OS_API::futil_chdir(const SL_TCHAR *dir_name)
{
#ifdef SOCKETLITE_OS_WINDOWS
    return _tchdir(dir_name);
#else
    return chdir(dir_name);
#endif
}

int SL_OS_API::futil_mkdir(const SL_TCHAR *dir_name, int mode)
{
#ifdef SOCKETLITE_OS_WINDOWS
    return _tmkdir(dir_name);
#else
    return mkdir(dir_name, mode);
#endif
}

//建立多级目录
int SL_OS_API::futil_mkdir_multilevel(const SL_TCHAR *dir_name, int mode)
{
    int ret = futil_mkdir(dir_name, mode);
    if (ret != 0)
    {
        int error_code = errno;
        if (error_code != EEXIST)
        {
            SL_TSTRING sub_dir;
            sub_dir.reserve(1024);
            SL_TCHAR *temp_dir_name = (SL_TCHAR *)dir_name;
            while ((*temp_dir_name) != SL_TEXT('\0'))
            {
                if (
                    (*temp_dir_name == SL_TEXT('\\')) || 
                    (*temp_dir_name == SL_TEXT('/'))
                    )
                {
                    sub_dir.clear();
                    sub_dir.append(dir_name, temp_dir_name-dir_name);
                    ret = futil_mkdir(sub_dir.c_str(), mode);
                }
                ++temp_dir_name;
            }

            //处理不是以'\','/'结尾的目录
            --temp_dir_name;
            if (
                (temp_dir_name >= dir_name) && 
                ( !((*temp_dir_name == SL_TEXT('\\')) || (*temp_dir_name == SL_TEXT('/'))) )
                )
            {
                ret = futil_mkdir(dir_name, mode);
            }
        }
    }
    return ret;
}

int SL_OS_API::futil_rmdir(const SL_TCHAR *dir_name)
{
#ifdef SOCKETLITE_OS_WINDOWS
    return _trmdir(dir_name);
#else
    return rmdir(dir_name);
#endif
}

bool SL_OS_API::futil_exists(const SL_TCHAR *path_name)
{
#ifdef SOCKETLITE_OS_WINDOWS
    //方法一
    return PathFileExists(path_name);

    //方法二 access, _taccess(_access/_waccess)
    //return _taccess(path_name, 0);
#else
    int rc = access(path_name, F_OK);
    if (rc < 0)
    {
        return false;
    }
    return true;
#endif
}

int SL_OS_API::futil_filesize(const SL_TCHAR *file_name, unsigned long &file_size)
{
#ifdef SOCKETLITE_OS_WINDOWS
    //方法一:
    //HANDLE handle = CreateFileA(file_name, FILE_READ_EA, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    //if (handle != INVALID_HANDLE_VALUE)
    //{
    //    file_size = GetFileSize(handle, NULL);
    //    CloseHandle(handle);
    //    return 0;
    //}
    //return -1;

    //方法二 stat,_tstat(_stat/_wstat)
    //Get data associated with the file descriptor.
    struct _stat statbuf;
    int ret     = _tstat(file_name, &statbuf);
    file_size   = statbuf.st_size;
    return ret;
#else
    struct stat statbuf;    
    int ret     = stat(file_name, &statbuf);
    file_size   = statbuf.st_size;
    return ret;
#endif
}

int SL_OS_API::futil_filelength(const SL_TCHAR *file_name, unsigned long &file_size)
{
    return futil_filesize(file_name, file_size);
}

time_t SL_OS_API::util_strtotime(const SL_TCHAR *time, const SL_TCHAR *format)
{
    //struct tm  tm_date_test = {0};
    //sscanf(date_test, "%d-%d-%d %d:%d:%d", &tm_date_test.tm_year, 
    //    &tm_date_test.tm_mon, 
    //    &tm_date_test.tm_mday,
    //    &tm_date_test.tm_hour,
    //    &tm_date_test.tm_min,
    //    &tm_date_test.tm_sec);
    //mktime(tm_date_test);
    return 0;
}

