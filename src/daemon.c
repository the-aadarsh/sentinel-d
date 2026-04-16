/*
 * daemon.c – UNIX daemonization for SENTINEL-D
 *
 * Implements the classic double-fork technique to detach fully from the
 * controlling terminal.  Also manages a PID file so the daemon can be
 * tracked and prevented from running in duplicate.
 */

#include "daemon.h"
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <signal.h> /* for kill() */
/* ---------------------------------------------------------------------------
 * daemon_start
 *
 * Steps:
 *   1. Fork #1: parent exits → process is no longer a process group leader.
 *   2. setsid(): create a new session; process becomes session leader.
 *   3. Fork #2: parent (session leader) exits → child can never re-acquire
 *      a controlling terminal.
 *   4. umask(0): don't inherit file-creation masks.
 *   5. chdir("/"): avoid holding open any mount point.
 *   6. Redirect stdin/stdout/stderr to /dev/null.
 * --------------------------------------------------------------------------- */
int daemon_start(void)
{
    pid_t pid;

    /* --- Fork #1 --- */
    pid = fork();
    if (pid < 0)
    {
        perror("fork #1");
        return -1;
    }
    if (pid > 0)
    {
        /* Parent exits; child continues. */
        exit(EXIT_SUCCESS);
    }

    /* --- Become session leader --- */
    if (setsid() < 0)
    {
        perror("setsid");
        return -1;
    }

    /* --- Fork #2: ensure we can't re-acquire a terminal --- */
    pid = fork();
    if (pid < 0)
    {
        perror("fork #2");
        return -1;
    }
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    /* --- Reset file-creation mask --- */
    umask(0);

    /* --- Move to a safe directory --- */
    if (chdir("/") < 0)
    {
        perror("chdir");
        return -1;
    }

    /* --- Redirect standard streams to /dev/null --- */
    int devnull = open("/dev/null", O_RDWR);
    if (devnull < 0)
    {
        perror("open /dev/null");
        return -1;
    }
    dup2(devnull, STDIN_FILENO);
    dup2(devnull, STDOUT_FILENO);
    dup2(devnull, STDERR_FILENO);
    if (devnull > STDERR_FILENO)
    {
        close(devnull);
    }

    return 0;
}

/* ---------------------------------------------------------------------------
 * PID file management
 * --------------------------------------------------------------------------- */

int daemon_write_pidfile(const char *path)
{
    if (!path || path[0] == '\0')
        return 0;

    FILE *fp = fopen(path, "w");
    if (!fp)
    {
        LOG_ERROR("Failed to open PID file '%s': %s", path, strerror(errno));
        return -1;
    }
    fprintf(fp, "%ld\n", (long)getpid());
    fclose(fp);
    LOG_DEBUG("PID file written: %s (pid=%ld)", path, (long)getpid());
    return 0;
}

void daemon_remove_pidfile(const char *path)
{
    if (!path || path[0] == '\0')
        return;
    if (unlink(path) < 0 && errno != ENOENT)
    {
        LOG_WARN("Could not remove PID file '%s': %s", path, strerror(errno));
    }
}

int daemon_already_running(const char *path)
{
    if (!path || path[0] == '\0')
        return 0;

    FILE *fp = fopen(path, "r");
    if (!fp)
    {
        if (errno == ENOENT)
            return 0; /* no PID file → not running */
        return -1;
    }

    long existing_pid = 0;
    if (fscanf(fp, "%ld", &existing_pid) != 1)
    {
        fclose(fp);
        return 0; /* unreadable PID file → assume stale */
    }
    fclose(fp);

    if (existing_pid <= 0)
        return 0;

    /* Signal 0 checks existence without sending anything */
    if (kill((pid_t)existing_pid, 0) == 0)
    {
        return 1; /* process is alive */
    }
    if (errno == ESRCH)
    {
        return 0; /* process no longer exists → stale PID file */
    }
    return -1; /* EPERM or other error */
}
