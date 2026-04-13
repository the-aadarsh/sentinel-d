/*
 * main.c – entry point for SENTINEL-D
 *
 * Responsibilities:
 *   1. Parse command-line arguments.
 *   2. Load and validate configuration.
 *   3. Initialize the logger.
 *   4. Optionally daemonize the process.
 *   5. Write PID file (if configured).
 *   6. Hand off to the scheduler.
 *   7. Clean up on exit.
 */

#include "config.h"
#include "daemon.h"
#include "logger.h"
#include "scheduler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SENTINEL_VERSION "1.0.0"
#define DEFAULT_CONFIG_PATH "sentinel.conf"

/* ---------------------------------------------------------------------------
 * Usage / version
 * --------------------------------------------------------------------------- */

static void print_usage(const char *argv0)
{
    fprintf(stderr,
        "Usage: %s [OPTIONS]\n"
        "\n"
        "SENTINEL-D – lightweight HTTP health monitoring daemon\n"
        "\n"
        "Options:\n"
        "  -c <path>    Path to config file (default: sentinel.conf)\n"
        "  -f           Run in foreground (do not daemonize)\n"
        "  -v           Print version and exit\n"
        "  -h           Print this help and exit\n"
        "\n"
        "Config file keys (KEY=value format):\n"
        "  SERVICE_NAME, ENVIRONMENT, CHECK_INTERVAL_SECONDS,\n"
        "  HTTP_TIMEOUT_SECONDS, RETRY_COUNT, COOLDOWN_SECONDS,\n"
        "  FAILURE_THRESHOLD, ENDPOINTS, DISCORD_WEBHOOK_URL,\n"
        "  LOG_LEVEL, LOG_FILE, PID_FILE\n"
        "\n"
        "All config keys can also be set via environment variables.\n",
        argv0);
}

static void print_version(void)
{
    printf("sentinel-d %s\n", SENTINEL_VERSION);
}

/* ---------------------------------------------------------------------------
 * main
 * --------------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
    const char *config_path = DEFAULT_CONFIG_PATH;
    int foreground = 0;
    int opt;

    while ((opt = getopt(argc, argv, "c:fvh")) != -1) {
        switch (opt) {
            case 'c':
                config_path = optarg;
                break;
            case 'f':
                foreground = 1;
                break;
            case 'v':
                print_version();
                return EXIT_SUCCESS;
            case 'h':
                print_usage(argv[0]);
                return EXIT_SUCCESS;
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    /* ---- Load configuration ---- */
    SentinelConfig cfg;
    if (config_load(config_path, &cfg) < 0) {
        fprintf(stderr, "Fatal: failed to load config from '%s'\n", config_path);
        return EXIT_FAILURE;
    }

    if (config_validate(&cfg) < 0) {
        fprintf(stderr, "Fatal: config validation failed\n");
        config_free(&cfg);
        return EXIT_FAILURE;
    }

    /* ---- Initialize logger (before daemonizing if foreground, or after
     *      if daemonizing to a log file).
     *      In daemon mode, if no log file is specified we lose logs (stderr
     *      is redirected to /dev/null).  Warn the user. ---- */
    if (!foreground && cfg.log_file[0] == '\0') {
        fprintf(stderr,
            "[WARN ] Daemonizing without LOG_FILE set — all log output "
            "will be lost. Consider setting LOG_FILE in your config.\n");
    }

    /* ---- Check for duplicate instance ---- */
    if (cfg.pid_file[0] != '\0') {
        int running = daemon_already_running(cfg.pid_file);
        if (running > 0) {
            fprintf(stderr,
                "Fatal: another sentinel-d instance is already running "
                "(PID file: %s)\n", cfg.pid_file);
            config_free(&cfg);
            return EXIT_FAILURE;
        }
    }

    /* ---- Daemonize (unless foreground mode) ---- */
    if (!foreground) {
        if (daemon_start() < 0) {
            fprintf(stderr, "Fatal: failed to daemonize\n");
            config_free(&cfg);
            return EXIT_FAILURE;
        }
        /* After daemon_start(), we are in the daemon child process.
         * The parent has already exited. */
    }

    /* ---- Initialize logger (now safe post-fork) ---- */
    if (log_init(cfg.log_level,
                 cfg.log_file[0] != '\0' ? cfg.log_file : NULL) < 0) {
        /* Can't log to file; stderr might be /dev/null in daemon mode */
        fprintf(stderr, "Fatal: failed to open log file\n");
        config_free(&cfg);
        return EXIT_FAILURE;
    }

    LOG_INFO("SENTINEL-D %s starting", SENTINEL_VERSION);

    /* ---- Write PID file ---- */
    if (cfg.pid_file[0] != '\0') {
        if (daemon_write_pidfile(cfg.pid_file) < 0) {
            LOG_WARN("Could not write PID file '%s' — continuing anyway",
                     cfg.pid_file);
        }
    }

    /* ---- Log configuration summary (no secrets) ---- */
    config_dump(&cfg);

    /* ---- Run the poll loop (blocks until SIGTERM/SIGINT) ---- */
    scheduler_run(&cfg);

    /* ---- Graceful cleanup ---- */
    LOG_INFO("SENTINEL-D shutting down");

    if (cfg.pid_file[0] != '\0') {
        daemon_remove_pidfile(cfg.pid_file);
    }

    log_close();
    config_free(&cfg);

    return EXIT_SUCCESS;
}
