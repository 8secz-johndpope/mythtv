#include "mythbaseutil.h"

// POSIX
#include <sys/types.h>  // for fnctl
#include <fcntl.h>      // for fnctl
#include <errno.h>      // for checking errno

// Qt
#include <QString>

// MythTV
#include "mythlogging.h"

/** \brief This sets up a unidirectional pipe.
 *
 *  The pipes are returned in pipefd and the flags on those
 *  pipes are returned in myflags.
 *
 *  At the 0 index is the read end of the pipe and at the 1
 *  index is the write index.
 *
 *  This will attempt to make the read end of the pipe non-blocking.
 *
 *  The pipe ends will be set to -1 if this fails to set up the pipe.
 *  The O_NONBLOCK flag will be missing on myflags[0] if this fails
 *  to make the read pipe non-blocking.
 */
#ifdef USING_MINGW
void setup_pipe(int[2], long[2]) {}
#else // !USING_MINGW
void setup_pipe(int mypipe[2], long myflags[2])
{
    int pipe_ret = pipe(mypipe);
    if (pipe_ret < 0)
    {
        LOG(VB_GENERAL, LOG_ERR, "Failed to open pipes" + ENO);
        mypipe[0] = mypipe[1] = -1;
    }
    else
    {
        errno = 0;
        long flags = fcntl(mypipe[0], F_GETFL);
        if (0 == errno)
        {
            int ret = fcntl(mypipe[0], F_SETFL, flags|O_NONBLOCK);
            if (ret < 0)
                LOG(VB_GENERAL, LOG_ERR, "Set pipe flags error" + ENO);
        }
        else
        {
            LOG(VB_GENERAL, LOG_ERR, "Get pipe flags error" + ENO);
        }

        for (uint i = 0; i < 2; i++)
        {
            errno = 0;
            flags = fcntl(mypipe[i], F_GETFL);
            if (0 == errno)
                myflags[i] = flags;
        }
    }
}
#endif // !USING_MINGW
