# SENTINEL-D

A lightweight, production-grade UNIX health-monitoring daemon written in pure C.

## Overview

SENTINEL-D solves the problem of silent infrastructure failures. It runs as a background process, polls one or more HTTP/HTTPS endpoints on a configurable interval, and sends structured alerts to a Discord webhook when an endpoint becomes unhealthy or unreachable.

It is designed to be extremely lightweight, using minimal system resources. It has no external framework dependencies and connects directly to POSIX sockets for plain HTTP, utilizing OpenSSL (already installed on most UNIX systems) for secure HTTPS connections and webhook delivery.

## Key Features

- **Pure C11 Implementation**: Fast, predictable, and memory-safe. No heavy garbage collection or runtime overhead.
- **Stand-alone Daemon**: Fully detaches from the terminal using standard UNIX double-forking.
- **Highly Configurable**: Control polling intervals, timeouts, retries, and cooldowns.
- **Smart Alerting**:
  - Implements a failure threshold before triggering alerts.
  - Implements a cooldown period to prevent alert storms during prolonged outages.
  - Sends immediate "Recovery" alerts when a service comes back online.
- **Discord Integration**: Pushes rich JSON embed payloads directly to Discord with color-coded severity.
- **Robust Networking**: Accurate detection of connection refusals, DNS failures, TLS errors, timeouts, and non-200 HTTP statuses.

## Architecture

See [docs/architecture.md](docs/architecture.md) for a deep dive into the module design.

## Getting Started

### Prerequisites

- Linux or macOS (uses POSIX APIs). Native Windows is not supported (see Windows Setup below).
- GCC or Clang.
- Make.
- OpenSSL development headers (`libssl-dev` on Debian/Ubuntu, `openssl` on macOS via Homebrew).

### Windows Setup (via WSL)

Because SENTINEL-D relies heavily on POSIX APIs (like UNIX socket connections and double-forking), it cannot be compiled natively on Windows. However, Windows users can easily run it using the **Windows Subsystem for Linux (WSL)**.

1. Open PowerShell as an Administrator and install WSL:

   ```powershell
   wsl --install
   ```

2. Restart your computer and set up your Linux username/password when prompted.
3. In your new WSL terminal (Ubuntu), install the required build tools and OpenSSL headers:

   ```bash
   sudo apt update
   sudo apt install build-essential make gcc libssl-dev
   ```

4. Navigate to your project folder. Your Windows files are accessible in WSL under the `/mnt/c/` directory. For example:

   ```bash
   cd /mnt/c/Users/YourUsername/Desktop/sentinel-d
   ```

5. You can now follow the standard **Build Instructions** and run the program within this terminal.

### Build Instructions

To build the optimized release binary:

```bash
make
```

The binary will be compiled into `bin/sentinel-d`.

To build with debug symbols and sanitizers (AddressSanitizer and UndefinedBehaviorSanitizer):

```bash
make debug
```

To run the unit test suite:

```bash
make test
```

To install the binary to `/usr/local/bin`:

```bash
sudo make install
```

### Configuration

Configuration is managed via a plain text `.conf` file. See `config/sentinel.conf.example` for a template.

Environment variables will always override configuration file values.

You need to provide:

1. `ENDPOINTS`: A comma-separated list of URLs to monitor.
2. `DISCORD_WEBHOOK_URL`: The destination for alerts.

See [WHAT_I_NEED.md](WHAT_I_NEED.md) for detailed configuration setup instructions and requirements.

### Running the Daemon

#### Foreground Mode (for testing)

Run the daemon in the foreground to verify configuration. Logs will stream to stderr.

```bash
./bin/sentinel-d -c sentinel.conf -f
```

#### Daemon Mode (Production)

Run the daemon in the background:

```bash
./bin/sentinel-d -c sentinel.conf
```

Remember to specify a `LOG_FILE` and `PID_FILE` in your configuration so you can inspect output and manage the daemon lifecycle.

To stop the daemon:

```bash
kill $(cat /path/to/your/pid_file.pid)
```

The daemon intercepts `SIGTERM` and `SIGINT` to shut down gracefully and summarize its total health check statistics to the log file before exiting.

## Log Levels

Configure `LOG_LEVEL` to adjust verbosity:

- `DEBUG`: Traces every poll cycle and retry attempt.
- `INFO`: (Default) Logs startup, shutdown, and alert transmissions.
- `WARN`: Logs individual health check failures.
- `ERROR`: Logs unrecoverable system or webhook transmission errors.

Secrets (like the webhook URL) are heavily protected and will **never** be written to logs.

## Troubleshooting

- **No alerts in Discord**: Ensure your webhook URL is correct. Check the daemon log for `Alert: TLS handshake failed` or `Alert: DNS lookup failed`, indicating the host cannot reach Discord.
- **Spurious timeout alerts**: Try increasing `HTTP_TIMEOUT_SECONDS` or `RETRY_COUNT` in your config.
- **Daemon won't start**: Check for earlier instances via `ps aux | grep sentinel-d`, or verify a stale PID file isn't present if configured.

## Future Improvements

- **Metrics Export**: Expose a mini HTTP server bound to localhost to export Prometheus metrics on endpoint latency and uptime ratios.
- **Dynamic Configuration Reloading**: Listen for `SIGHUP` to reload endpoints from the config file without fully restarting the daemon.
- **Custom Alerting Payloads**: Allow generic Webhook payload templates to support Slack/Teams integrations.
