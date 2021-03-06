/* ucoolib - Microcontroller object oriented library. {{{
 *
 * Copyright (C) 2012 Nicolas Schodet
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * }}} */
#include "ucoo/common.hh"
#include "syscalls.newlib.hh"

#include <reent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/times.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

ucoo::Stream *ucoo::syscalls_streams[8];

ucoo::FileSystem *ucoo::syscalls_file_system;

/** This is needed by C++ ABI, this simple definition will do.  See:
 * http://lists.debian.org/debian-gcc/2003/07/msg00057.html */
void *__dso_handle = (void*) &__dso_handle;

/** This function is called when a pure virtual function is called.  This is
 * needed by linker because when a abstract class constructor or destructor is
 * called, object is not complete.  Replace the one provided by the toolchain
 * to avoid including the world. */
extern "C" void
__cxa_pure_virtual (void)
{
    ucoo::halt ();
}

/** Increase program data space. */
extern "C" void *
_sbrk_r (struct _reent *ptr, int incr)
{
    extern char end; /* Defined in linker script. */
    static char *heap_end;
    char *prev_heap_end;
    if (heap_end == 0)
        heap_end = &end;
    prev_heap_end = heap_end;
    heap_end += incr;
    return (void *) prev_heap_end;
}

/** Some Newlib versions insist to call this function when exit is called,
 * make them happy. */
extern "C" void
_fini (void)
{
}

/** Exit program, endless loop to stop program, to be improved. */
extern "C" void
_exit (int n)
{
    ucoo::halt ();
}

/** Get PID, minimal implementation. */
extern "C" int
_getpid_r (struct _reent *ptr)
{
    return 1;
}

/** Whether file is a terminal, consider this is always true for stdin, stdout
 * and stderr. */
extern "C" int
_isatty_r (struct _reent *ptr, int fd)
{
    if (fd < ucoo::lengthof (ucoo::syscalls_streams)
        && ucoo::syscalls_streams[fd])
        return fd < 3;
    else
    {
        ptr->_errno = EBADF;
        return -1;
    }
}

/** Send a signal, no process, no signal. */
extern "C" int
_kill_r (struct _reent *ptr, int pid, int sig)
{
    ptr->_errno = EINVAL;
    return -1;
}

/** Set position in a file, unimplemented. */
extern "C" off_t
_lseek_r (struct _reent *ptr, int fd, off_t pos, int whence)
{
    ptr->_errno = ENOSYS;
    return -1;
}

/** Open a file. */
extern "C" int
_open_r (struct _reent *ptr, const char *file, int flags, int mode)
{
    if (ucoo::syscalls_file_system)
    {
        int i;
        for (i = 0; i < ucoo::lengthof (ucoo::syscalls_streams)
             && ucoo::syscalls_streams[i]; i++)
            ;
        if (i == ucoo::lengthof (ucoo::syscalls_streams))
        {
            ptr->_errno = ENFILE;
            return -1;
        }
        else
        {
            ucoo::FileSystem::Mode mode;
#ifdef O_BINARY
            flags &= ~O_BINARY;
#endif
#ifdef O_TEXT
            flags &= ~O_TEXT;
#endif
            if (flags == O_RDONLY)
                mode = ucoo::FileSystem::Mode::READ;
            else if (flags == (O_WRONLY | O_CREAT | O_TRUNC))
                mode = ucoo::FileSystem::Mode::WRITE;
            else
            {
                ptr->_errno = ENOSYS;
                return -1;
            }
            ucoo::FileSystem::Error error;
            ucoo::syscalls_streams[i] =
                ucoo::syscalls_file_system->open (file, mode, error);
            if (!ucoo::syscalls_streams[i])
            {
                if (error == ucoo::FileSystem::Error::ACCESS_DENIED)
                    ptr->_errno = EACCES;
                else if (error == ucoo::FileSystem::Error::TOO_MANY_OPEN_FILES)
                    ptr->_errno = ENFILE;
                else if (error == ucoo::FileSystem::Error::NAME_TOO_LONG)
                    ptr->_errno = ENAMETOOLONG;
                else if (error == ucoo::FileSystem::Error::NO_SUCH_FILE)
                    ptr->_errno = ENOENT;
                else if (error == ucoo::FileSystem::Error::NO_SPACE_LEFT)
                    ptr->_errno = ENOSPC;
                else if (error == ucoo::FileSystem::Error::READ_ONLY)
                    ptr->_errno = EROFS;
                else
                    ucoo::assert_unreachable ();
                return -1;
            }
            else
                return i;
        }
    }
    else
    {
        ptr->_errno = ENOSYS;
        return -1;
    }
}

/** Close a file. */
extern "C" int
_close_r (struct _reent *ptr, int fd)
{
    if (ucoo::syscalls_file_system
        && fd < ucoo::lengthof (ucoo::syscalls_streams)
        && ucoo::syscalls_streams[fd])
    {
        ucoo::syscalls_file_system->close (ucoo::syscalls_streams[fd]);
        ucoo::syscalls_streams[fd] = nullptr;
        return 0;
    }
    else
    {
        ptr->_errno = EBADF;
        return -1;
    }
}

/** Status of open file, consider all files as character devices. */
extern "C" int
_fstat_r (struct _reent *ptr, int fd, struct stat *st)
{
    if (fd < ucoo::lengthof (ucoo::syscalls_streams)
        && ucoo::syscalls_streams[fd])
    {
        st->st_mode = S_IFCHR;
        return 0;
    }
    else
    {
        ptr->_errno = EBADF;
        return -1;
    }
}

/** Read from file. */
extern "C" int
_read_r (struct _reent *ptr, int fd, void *buf, size_t cnt)
{
    if (fd < ucoo::lengthof (ucoo::syscalls_streams)
        && ucoo::syscalls_streams[fd])
    {
        ucoo::Stream &s = *ucoo::syscalls_streams[fd];
        int r = s.read (reinterpret_cast<char *> (buf), cnt);
        switch (r)
        {
        case -2:
            return 0;
        case -1:
            ptr->_errno = EIO;
            return -1;
        case 0:
            ptr->_errno = EAGAIN;
            return -1;
        default:
            return r;
        }
    }
    else
    {
        ptr->_errno = EBADF;
        return -1;
    }
}

/** Write to file. */
extern "C" int
_write_r (struct _reent *ptr, int fd, const void *buf, size_t cnt)
{
    if (fd < ucoo::lengthof (ucoo::syscalls_streams)
        && ucoo::syscalls_streams[fd])
    {
        ucoo::Stream &s = *ucoo::syscalls_streams[fd];
        int r = s.write (reinterpret_cast<const char *> (buf), cnt);
        switch (r)
        {
        case -1:
            ptr->_errno = EIO;
            return -1;
        case 0:
            ptr->_errno = EAGAIN;
            return -1;
        default:
            return r;
        }
    }
    else
    {
        ptr->_errno = EBADF;
        return -1;
    }
}

extern "C" int
_unlink_r (struct _reent *ptr, const char *pathname)
{
    if (ucoo::syscalls_file_system)
    {
        ucoo::syscalls_file_system->unlink (pathname);
        return 0;
    }
    else
    {
        ptr->_errno = ENOSYS;
        return -1;
    }
}

extern "C" int
_link_r (struct _reent *ptr, const char *old_name, const char *new_name)
{
    ptr->_errno = ENOSYS;
    return -1;
}

extern "C" clock_t
_times_r (struct _reent *ptr, struct tms *ptms)
{
    ptr->_errno = ENOSYS;
    return -1;
}

extern "C" int
_gettimeofday_r (struct _reent *ptr,
                 struct timeval *ptimeval,
                 void *ptimezone)
{
    ptr->_errno = ENOSYS;
    return -1;
}

