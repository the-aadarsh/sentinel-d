# What I Need From You

To run the `sentinel-d` health monitoring daemon in your environment, you need to provide a few configuration values. 

You can provide these either in a configuration file (e.g., `sentinel.conf`) or as environment variables.

## Required Inputs

1. **`DISCORD_WEBHOOK_URL`**  
   **Where it belongs:** Config file or ENV var.  
   **What it is:** The destination URL where alerts will be sent.  
   **How to get it:** Go to your Discord Server settings -> Integrations -> Webhooks -> New Webhook, and copy the URL.  
   *Keep this secret. Never commit it to a public repository.*

2. **`ENDPOINTS`**  
   **Where it belongs:** Config file or ENV var.  
   **What it is:** A comma-separated list of HTTP or HTTPS URLs you want to monitor.  
   *Example:* `https://api.mycompany.com/health, https://payments.mycompany.com/ping`

## Optional (but highly recommended) Inputs

3. **`SERVICE_NAME`**  
   **What it is:** The name of the service or infrastructure being monitored. It appears prominently in Discord alerts.  
   *Default:* `sentinel-d`  
   *Example:* `core-api`, `database-layer`, `payment-gateway`

4. **`ENVIRONMENT`**  
   **What it is:** The deployment name, which helps distinguish between alerts coming from `staging` vs `production`.  
   *Default:* `production`

5. **`LOG_FILE`**  
   **What it is:** Absolute path to where the daemon should append its logs. If omitted when running in daemon mode, logs will be silently discarded to `/dev/null`.  
   *Example:* `/var/log/sentinel-d.log`

6. **`PID_FILE`**  
   **What it is:** Absolute path to a temporary file tracking the daemon's process ID. Required if you want to cleanly stop the daemon later and prevent accidental duplicate instances from running.  
   *Example:* `/var/run/sentinel-d.pid`

## Polling and Alert Policies

You can tune the monitoring behavior using these keys:

- **`CHECK_INTERVAL_SECONDS`**: How often to poll the endpoints. (Default: 30)
- **`HTTP_TIMEOUT_SECONDS`**: Drop the connection and report failure if the endpoint takes longer than this to respond. (Default: 10)
- **`RETRY_COUNT`**: How many immediate retries to attempt upon a failed request before marking the cycle as a failure. (Default: 0)
- **`FAILURE_THRESHOLD`**: How many consecutive failing cycles are required before sending an alert to Discord. Set to `1` for immediate alerting. (Default: 1)
- **`COOLDOWN_SECONDS`**: If an endpoint is down for a long time, the daemon will wait this many seconds before sending another reminder alert. (Default: 300 / 5 minutes)

---

## Example Config Template

Copy the file below into `sentinel.conf` and adjust the values to fit your needs.

```ini
# Identity
SERVICE_NAME=my-ecommerce-api
ENVIRONMENT=production

# Health check behaviour
CHECK_INTERVAL_SECONDS=60
HTTP_TIMEOUT_SECONDS=5
RETRY_COUNT=1
FAILURE_THRESHOLD=3
COOLDOWN_SECONDS=600

# Targets
ENDPOINTS=https://api.myshop.com/health, https://api.myshop.com/ready

# Alerting Destination (keep secret!)
DISCORD_WEBHOOK_URL=https://discord.com/api/webhooks/123456789/abcdefghijklmnopqrstuvwxyz

# Observability (paths)
LOG_FILE=/var/log/sentinel-d.log
PID_FILE=/var/run/sentinel-d.pid
LOG_LEVEL=INFO
```
