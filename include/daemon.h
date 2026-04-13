#ifndef SENTINEL_DAEMON_H
#define SENTINEL_DAEMON_H

/* ---------------------------------------------------------------------------
 * Daemonization helpers (POSIX only).
 * --------------------------------------------------------------------------- */

/* Perform double-fork daemonization:
 *   - fork, setsid, fork again
 *   - umask(0)
 *   - chdir("/")
 *   - redirect stdin/stdout/stderr to /dev/null
 * Returns 0 on success (in daemon process), -1 on error.
 * The parent process exits with EXIT_SUCCESS after a successful fork. */
int daemon_start(void);

/* Write the current process PID to path.
 * Returns 0 on success, -1 on error. */
int daemon_write_pidfile(const char *path);

/* Remove the PID file at path (best-effort, ignores errors). */
void daemon_remove_pidfile(const char *path);

/* Check whether another SENTINEL-D instance is already running by reading
 * the PID file and sending signal 0 to that PID.
 * Returns 1 if running, 0 if not, -1 on error. */
int daemon_already_running(const char *path);

#endif /* SENTINEL_DAEMON_H */
