import type { Metadata } from "next";
import SectionBlock from "@/components/SectionBlock";
import Link from "next/link";

export const metadata: Metadata = {
  title: "How It Works — SENTINEL-D",
  description:
    "A deep dive into SENTINEL-D's internals: execution flow, module architecture, data flow, and algorithms used.",
};

// ── Reusable Step component ──────────────────────────────────────────────────
function Step({
  number,
  title,
  description,
  code,
}: {
  number: number;
  title: string;
  description: string;
  code?: string[];
}) {
  return (
    <div id={`step-${number}`} className="flex gap-5">
      {/* Connector line + number */}
      <div className="flex flex-col items-center">
        <div className="shrink-0 w-10 h-10 rounded-full border border-blue-500/40 bg-blue-500/10 flex items-center justify-center text-blue-400 font-mono font-bold text-sm">
          {number.toString().padStart(2, "0")}
        </div>
        <div className="flex-1 w-px bg-gradient-to-b from-blue-500/20 to-transparent mt-2" />
      </div>

      {/* Content */}
      <div className="flex-1 pb-10">
        <h3 className="text-[#e2e8f0] font-semibold text-base mb-1.5">
          {title}
        </h3>
        <p className="text-[#8b9ab3] text-sm leading-relaxed mb-3">
          {description}
        </p>
        {code && (
          <div className="code-block text-xs sm:text-sm">
            {code.map((line, i) => (
              <div key={i}>
                <span
                  className={
                    line.startsWith("//") || line.startsWith("#")
                      ? "text-[#4a5568]"
                      : line.includes("LOG_INFO") || line.includes("LOG_DEBUG")
                      ? "text-cyan-400"
                      : line.includes("LOG_WARN")
                      ? "text-yellow-400"
                      : line.includes("LOG_ERROR")
                      ? "text-red-400"
                      : line.includes("alert_send") ||
                        line.includes("HTTP_ERR")
                      ? "text-green-400"
                      : "text-[#cbd5e1]"
                  }
                >
                  {line}
                </span>
              </div>
            ))}
          </div>
        )}
      </div>
    </div>
  );
}

// ── Reusable Module Card ─────────────────────────────────────────────────────
function ModuleCard({
  file,
  header,
  role,
  details,
  accent = "blue",
}: {
  file: string;
  header: string;
  role: string;
  details: string[];
  accent?: "blue" | "green" | "purple" | "yellow" | "orange" | "cyan" | "red";
}) {
  const colorMap: Record<string, string> = {
    blue: "border-blue-500/30 bg-blue-500/5 text-blue-400",
    green: "border-green-500/30 bg-green-500/5 text-green-400",
    purple: "border-purple-500/30 bg-purple-500/5 text-purple-400",
    yellow: "border-yellow-500/30 bg-yellow-500/5 text-yellow-400",
    orange: "border-orange-500/30 bg-orange-500/5 text-orange-400",
    cyan: "border-cyan-500/30 bg-cyan-500/5 text-cyan-400",
    red: "border-red-500/30 bg-red-500/5 text-red-400",
  };

  return (
    <div
      id={`module-${file.replace(/[^a-z0-9]/gi, "-").toLowerCase()}`}
      className={`p-5 rounded-xl border ${colorMap[accent]} transition-all duration-300 hover:scale-[1.01]`}
    >
      <div className="mb-3">
        <span className="font-mono text-xs font-bold">{file}</span>
        <span className="text-[#4a5568] font-mono text-xs"> / {header}</span>
      </div>
      <p className="text-[#e2e8f0] text-sm font-medium mb-2">{role}</p>
      <ul className="space-y-1">
        {details.map((d) => (
          <li
            key={d}
            className="text-[#8b9ab3] text-xs flex items-start gap-1.5"
          >
            <span className="mt-1 w-1 h-1 rounded-full bg-current shrink-0 opacity-50" />
            {d}
          </li>
        ))}
      </ul>
    </div>
  );
}

// ── Page ─────────────────────────────────────────────────────────────────────
export default function HowItWorksPage() {
  return (
    <div className="bg-[#0a0d10] pt-14">
      {/* ── Page Header ── */}
      <div id="hiw-header" className="relative py-20 px-4 sm:px-6 text-center grid-bg overflow-hidden">
        <div className="absolute inset-0 pointer-events-none">
          <div className="absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2 w-[500px] h-[400px] rounded-full bg-blue-500/5 blur-3xl" />
        </div>
        <div className="relative z-10 max-w-3xl mx-auto">
          <span className="inline-block font-mono text-xs tracking-widest text-blue-400 uppercase mb-4 border border-blue-500/20 rounded-full px-3 py-1 bg-blue-500/5">
            Documentation · Presentation
          </span>
          <h1 className="text-4xl sm:text-5xl font-bold text-gradient mb-4">
            How SENTINEL-D Works
          </h1>
          <p className="text-[#8b9ab3] text-lg leading-relaxed">
            A complete walkthrough of the internals — from startup to graceful
            shutdown. Written for both developers exploring the codebase and students
            presenting the project.
          </p>
        </div>
      </div>

      {/* ── TOC ── */}
      <div className="max-w-6xl mx-auto px-4 sm:px-6 py-8">
        <div id="table-of-contents" className="flex flex-wrap gap-2">
          {[
            ["#overview", "1. Overview"],
            ["#execution-flow", "2. Execution Flow"],
            ["#architecture", "3. Architecture"],
            ["#data-flow", "4. Data Flow"],
            ["#algorithms", "5. Algorithms"],
            ["#error-types", "6. Error Types"],
          ].map(([href, label]) => (
            <a
              key={href}
              href={href}
              className="px-3 py-1.5 rounded-lg border border-[#1e2530] bg-[#111318] text-[#8b9ab3] hover:text-[#e2e8f0] hover:border-[#2e3a4e] text-xs font-mono transition-colors"
            >
              {label}
            </a>
          ))}
        </div>
      </div>

      {/* ── 1. Overview ── */}
      <SectionBlock
        id="overview"
        label="01 — Overview"
        title="What is SENTINEL-D?"
        subtitle="The short, plain-English explanation."
      >
        <div className="grid md:grid-cols-3 gap-4">
          {[
            {
              emoji: "👁️",
              title: "Watch",
              desc: "Runs as a background process on your Linux or macOS server. Periodically sends HTTP GET requests to each of your configured URLs.",
            },
            {
              emoji: "🔍",
              title: "Detect",
              desc: "Checks the response: did it connect? Was DNS resolved? Did TLS succeed? Was the HTTP status code 200? If any check fails, it records a failure.",
            },
            {
              emoji: "🚨",
              title: "Alert",
              desc: "After N consecutive failures (configurable), it fires a rich Discord embed with failure details. When the service recovers, it sends a green recovery message.",
            },
          ].map(({ emoji, title, desc }) => (
            <div
              key={title}
              id={`overview-${title.toLowerCase()}`}
              className="p-6 rounded-xl border border-[#1e2530] bg-[#111318] text-center"
            >
              <div className="text-4xl mb-4">{emoji}</div>
              <h3 className="text-[#e2e8f0] font-semibold mb-2">{title}</h3>
              <p className="text-[#8b9ab3] text-sm leading-relaxed">{desc}</p>
            </div>
          ))}
        </div>

        <div className="mt-6 p-5 rounded-xl border border-[#1e2530] bg-[#0d1117] code-block text-sm">
          <div className="text-[#4a5568] mb-2"># Simple mental model:</div>
          <div className="text-[#cbd5e1]">
            while (running) &#123;
          </div>
          <div className="pl-4 text-cyan-400">
            &nbsp;&nbsp;for each endpoint in config.endpoints &#123;
          </div>
          <div className="pl-8 text-[#cbd5e1]">
            &nbsp;&nbsp;&nbsp;&nbsp;result = http_check(endpoint)
          </div>
          <div className="pl-8 text-green-400">
            &nbsp;&nbsp;&nbsp;&nbsp;if (result != 200) → count failure
          </div>
          <div className="pl-8 text-yellow-400">
            &nbsp;&nbsp;&nbsp;&nbsp;if (failures &gt;= threshold &amp;&amp; !cooldown) → send Discord alert
          </div>
          <div className="pl-8 text-blue-400">
            &nbsp;&nbsp;&nbsp;&nbsp;if (was_failing &amp;&amp; now_healthy) → send recovery alert
          </div>
          <div className="pl-4 text-cyan-400">&nbsp;&nbsp;&#125;</div>
          <div className="text-[#cbd5e1]">
            &nbsp;&nbsp;sleep(check_interval_seconds)
          </div>
          <div className="text-[#cbd5e1]">&#125;</div>
        </div>
      </SectionBlock>

      {/* ── 2. Execution Flow ── */}
      <SectionBlock
        id="execution-flow"
        label="02 — Execution Flow"
        title="Step-by-step"
        titleHighlight="startup to shutdown."
        subtitle="Exactly what happens from the moment you run the binary."
      >
        <Step
          number={1}
          title="Parse CLI Arguments"
          description="main.c uses getopt() to parse flags. -c sets the config file path (default: sentinel.conf). -f runs in foreground mode (no daemon). -v prints version and exits."
          code={[
            "# Run in foreground for testing:",
            "./bin/sentinel-d -c sentinel.conf -f",
            "",
            "# Run as background daemon (production):",
            "./bin/sentinel-d -c sentinel.conf",
          ]}
        />
        <Step
          number={2}
          title="Load & Validate Configuration"
          description="config_load() reads the KEY=VALUE file, strips comments and whitespace, and populates a SentinelConfig struct. Environment variables override file values. config_validate() ensures required fields (ENDPOINTS, DISCORD_WEBHOOK_URL) are present."
          code={[
            "// Required: at least one endpoint and a webhook URL",
            "ENDPOINTS=https://api.example.com/health",
            "DISCORD_WEBHOOK_URL=https://discord.com/api/webhooks/...",
            "",
            "// Optional (with defaults):",
            "CHECK_INTERVAL_SECONDS=30",
            "HTTP_TIMEOUT_SECONDS=10",
            "RETRY_COUNT=0",
            "FAILURE_THRESHOLD=2",
            "COOLDOWN_SECONDS=300",
          ]}
        />
        <Step
          number={3}
          title="Daemonize (unless -f flag)"
          description="daemon_start() performs the UNIX double-fork: Fork #1 → parent exits, child is no longer process group leader. setsid() creates a new session. Fork #2 → session leader exits, grandchild can never re-acquire a terminal. Then stdin/stdout/stderr are redirected to /dev/null."
          code={[
            "// daemon.c — double fork sequence",
            "fork() → parent exits   // Step 1",
            "setsid()                // New session",
            "fork() → parent exits   // Step 2",
            'umask(0)               // Reset file mask',
            'chdir("/")             // Safe directory',
            'stdin/stdout/stderr → /dev/null',
          ]}
        />
        <Step
          number={4}
          title="Initialize Logger"
          description="log_init() opens the log file (or stays on stderr for foreground mode). Every log line gets an ISO-8601 UTC timestamp. The webhook URL is never written to logs."
          code={[
            "LOG_FILE=/tmp/sentinel-d.log",
            "LOG_LEVEL=INFO",
            "",
            "// Example log output:",
            "2026-04-17T03:43:21Z [INFO ] SENTINEL-D 1.0.0 starting",
            "2026-04-17T03:43:21Z [INFO ] Scheduler started: 2 endpoint(s), interval=30s",
          ]}
        />
        <Step
          number={5}
          title="Write PID File (optional)"
          description="If PID_FILE is configured, the daemon's process ID is written to a file. On startup, the daemon checks this file with kill(pid, 0) — a zero-signal that tests process existence without actually sending anything. This prevents duplicate instances."
          code={[
            "PID_FILE=/tmp/sentinel-d.pid",
            "",
            "// To stop the daemon:",
            "kill $(cat /tmp/sentinel-d.pid)",
          ]}
        />
        <Step
          number={6}
          title="Scheduler Poll Loop"
          description="scheduler_run() enters an infinite loop. For each configured endpoint, it calls http_check(). If RETRY_COUNT > 0, failed checks are retried before being processed. Results are passed to scheduler_process_result() for state machine evaluation."
          code={[
            "// For each poll cycle:",
            "LOG_INFO(\"Poll cycle start [%s]\", timestamp);",
            "",
            "for (i = 0; i < endpoint_count; i++) {",
            "  http_check(url, timeout, &result);",
            "  scheduler_process_result(&state[i], cfg, &result);",
            "}",
            "",
            "sleep(check_interval_seconds);",
          ]}
        />
        <Step
          number={7}
          title="Health Check via HTTP Client"
          description="http_client.c uses raw POSIX TCP sockets for HTTP and bridges to OpenSSL for HTTPS. It only reads the first line of the HTTP response (the status line: e.g. HTTP/1.1 200 OK) — saving memory and CPU. SO_RCVTIMEO and SO_SNDTIMEO socket options enforce the timeout deadline."
          code={[
            "// Supported protocols:",
            "http://internal-service:8080/ping   // raw TCP socket",
            "https://api.example.com/health      // OpenSSL TLS",
            "",
            "// Only the status line is parsed:",
            "HTTP/1.1 200 OK   ← healthy",
            "HTTP/1.1 503 Service Unavailable ← failure",
          ]}
        />
        <Step
          number={8}
          title="State Machine: Threshold + Cooldown + Recovery"
          description="scheduler_process_result() applies a three-layer decision: (1) Failure threshold — only alert after N consecutive failures. (2) Cooldown — suppress repeat alerts for COOLDOWN_SECONDS after the last alert. (3) Recovery — if is_alerting and the check suddenly passes, fire a green recovery alert."
        />
        <Step
          number={9}
          title="Discord Alert via HTTPS POST"
          description="alert_build_payload() constructs a JSON Discord Embed object (pure function, no side effects, fully unit testable). alert_send() posts this payload to the webhook URL over TLS using the same http_client infrastructure."
          code={[
            "// Failure alert payload (simplified):",
            '{',
            '  "embeds": [{',
            '    "color": 15158332,  // Red',
            '    "title": "🔴 Health Check Failed",',
            '    "description": "https://api.example.com/health",',
            '    "fields": [{ "name": "Error", "value": "Timeout" }]',
            '  }]',
            '}',
          ]}
        />
        <Step
          number={10}
          title="Graceful Shutdown on SIGTERM / SIGINT"
          description="SIGTERM and SIGINT handlers set a volatile g_stop flag. The nanosleep() in the poll loop is interrupted (EINTR), and the loop exits cleanly. Before terminating, a final summary is logged: total checks and total failures per endpoint."
          code={[
            "// Final log output on shutdown:",
            '2026-04-17T04:15:00Z [INFO] Scheduler received shutdown signal',
            '2026-04-17T04:15:00Z [INFO] Endpoint summary [https://api.example.com/health]: checks=288 failures=1',
            '2026-04-17T04:15:00Z [INFO] SENTINEL-D shutting down',
          ]}
        />
      </SectionBlock>

      {/* ── 3. Architecture ── */}
      <SectionBlock
        id="architecture"
        label="03 — Architecture"
        title="Module"
        titleHighlight="breakdown."
        subtitle="Each file has a single, narrow responsibility. No circular dependencies."
      >
        <div className="grid sm:grid-cols-2 gap-4">
          <ModuleCard
            file="main.c"
            header="(no header)"
            role="Orchestrator — entry point"
            accent="blue"
            details={[
              "Parses CLI args with getopt()",
              "Loads and validates config",
              "Triggers daemonization",
              "Initializes logger",
              "Hands off to scheduler_run()",
              "Cleans up heap allocations on exit",
            ]}
          />
          <ModuleCard
            file="config.c"
            header="config.h"
            role="Configuration parser"
            accent="purple"
            details={[
              "Reads KEY=VALUE .conf files",
              "Strips comments (#) and whitespace",
              "Overrides with environment variables",
              "Validates required fields",
              "Redacts webhook URL from logs",
              "Supports up to 64 endpoints",
            ]}
          />
          <ModuleCard
            file="daemon.c"
            header="daemon.h"
            role="UNIX daemonization"
            accent="yellow"
            details={[
              "Classic double-fork technique",
              "Creates new session with setsid()",
              "Resets umask to 0",
              "Changes working dir to /",
              "Redirects stdio to /dev/null",
              "Manages PID file lifecycle",
            ]}
          />
          <ModuleCard
            file="scheduler.c"
            header="scheduler.h"
            role="Poll loop + state machine"
            accent="green"
            details={[
              "Manages per-endpoint EndpointState",
              "Interruptible nanosleep between cycles",
              "Handles SIGTERM / SIGINT gracefully",
              "Applies failure threshold logic",
              "Enforces cooldown periods",
              "Detects healthy → alerting → recovery transitions",
            ]}
          />
          <ModuleCard
            file="http_client.c"
            header="http_client.h"
            role="Networking engine"
            accent="orange"
            details={[
              "Raw POSIX TCP sockets for HTTP",
              "OpenSSL for HTTPS connections",
              "Enforces SO_RCVTIMEO / SO_SNDTIMEO",
              "Reads only the HTTP status line",
              "Classifies errors into 7 HttpErrorType values",
              "Returns latency in milliseconds",
            ]}
          />
          <ModuleCard
            file="alert.c"
            header="alert.h"
            role="Discord webhook formatter"
            accent="red"
            details={[
              "alert_build_payload() is a pure function",
              "JSON-escapes all dynamic strings safely",
              "Builds Discord Embed with color coding",
              "Red embeds for failures, green for recovery",
              "Sends via HTTPS POST using http_client",
              "Fully unit-testable without network",
            ]}
          />
          <ModuleCard
            file="logger.c"
            header="logger.h"
            role="Structured log output"
            accent="cyan"
            details={[
              "ISO-8601 UTC timestamp on every line",
              "4 log levels: DEBUG / INFO / WARN / ERROR",
              "Level-gated output filtering",
              "Writes to file or stderr",
              "Never logs sensitive secrets",
            ]}
          />
          <ModuleCard
            file="utils.c"
            header="utils.h"
            role="Utility helpers"
            accent="purple"
            details={[
              "String trimming and parsing helpers",
              "ISO-8601 time formatting",
              "Monotonic clock for latency measurement",
              "JSON string escaping routines",
              "Used by config, alert, logger, http_client",
            ]}
          />
        </div>
      </SectionBlock>

      {/* ── 4. Data Flow ── */}
      <SectionBlock
        id="data-flow"
        label="04 — Data Flow"
        title="From config to"
        titleHighlight="Discord alert."
        subtitle="How data travels through the system on a single failed health check."
      >
        <div className="space-y-3">
          {[
            {
              step: "A",
              from: "sentinel.conf",
              to: "SentinelConfig struct",
              detail: "KEY=VALUE pairs parsed and validated into a typed struct. Environment variables override file values.",
              color: "purple",
            },
            {
              step: "B",
              from: "SentinelConfig",
              to: "scheduler_run()",
              detail: "Config passed by pointer as read-only data. Scheduler reads endpoint URLs, intervals, thresholds.",
              color: "blue",
            },
            {
              step: "C",
              from: "Endpoint URL",
              to: "HttpResult struct",
              detail: "http_check() dials TCP/TLS, sends GET request, reads status line, computes latency_ms. Returns error type + status code.",
              color: "orange",
            },
            {
              step: "D",
              from: "HttpResult + EndpointState",
              to: "Alert decision",
              detail: "scheduler_process_result() applies failure threshold and cooldown. Updates consecutive_fail, is_alerting, last_alert_time.",
              color: "green",
            },
            {
              step: "E",
              from: "Alert decision",
              to: "JSON payload (char buffer)",
              detail: "alert_build_payload() serializes url, error type, latency, consecutive failures, and timestamps into a Discord Embed JSON string.",
              color: "yellow",
            },
            {
              step: "F",
              from: "JSON payload",
              to: "Discord webhook (HTTPS POST)",
              detail: "alert_send() opens a TLS connection to discord.com and POSTs the JSON payload. Logs success or failure.",
              color: "red",
            },
          ].map(({ step, from, to, detail, color }) => {
            const bg: Record<string, string> = {
              purple: "border-purple-500/20 bg-purple-500/5",
              blue: "border-blue-500/20 bg-blue-500/5",
              orange: "border-orange-500/20 bg-orange-500/5",
              green: "border-green-500/20 bg-green-500/5",
              yellow: "border-yellow-500/20 bg-yellow-500/5",
              red: "border-red-500/20 bg-red-500/5",
            };
            const badge: Record<string, string> = {
              purple: "text-purple-400 bg-purple-500/10 border-purple-500/30",
              blue: "text-blue-400 bg-blue-500/10 border-blue-500/30",
              orange: "text-orange-400 bg-orange-500/10 border-orange-500/30",
              green: "text-green-400 bg-green-500/10 border-green-500/30",
              yellow: "text-yellow-400 bg-yellow-500/10 border-yellow-500/30",
              red: "text-red-400 bg-red-500/10 border-red-500/30",
            };
            return (
              <div
                key={step}
                id={`dataflow-${step.toLowerCase()}`}
                className={`flex gap-4 p-4 rounded-xl border ${bg[color]}`}
              >
                <div
                  className={`shrink-0 w-7 h-7 rounded-md border font-mono font-bold text-xs flex items-center justify-center ${badge[color]}`}
                >
                  {step}
                </div>
                <div className="flex-1">
                  <div className="flex flex-wrap items-center gap-2 mb-1">
                    <span className="text-[#e2e8f0] text-sm font-mono">
                      {from}
                    </span>
                    <span className="text-[#4a5568] text-xs">→</span>
                    <span className="text-[#e2e8f0] text-sm font-mono">
                      {to}
                    </span>
                  </div>
                  <p className="text-[#8b9ab3] text-xs leading-relaxed">
                    {detail}
                  </p>
                </div>
              </div>
            );
          })}
        </div>
      </SectionBlock>

      {/* ── 5. Algorithms ── */}
      <SectionBlock
        id="algorithms"
        label="05 — Algorithms"
        title="The logic"
        titleHighlight="behind smart alerting."
      >
        <div className="grid md:grid-cols-3 gap-6">
          {/* Failure Threshold */}
          <div
            id="algo-threshold"
            className="p-5 rounded-xl border border-[#1e2530] bg-[#111318]"
          >
            <h3 className="text-[#e2e8f0] font-semibold mb-3 flex items-center gap-2">
              <span className="w-2 h-2 rounded-full bg-yellow-400" />
              Failure Threshold
            </h3>
            <p className="text-[#8b9ab3] text-sm mb-4 leading-relaxed">
              Prevents alert fatigue from transient blips. An alert is only fired
              when consecutive_fail &ge; FAILURE_THRESHOLD.
            </p>
            <div className="code-block text-xs">
              <div className="text-[#4a5568]">// Example: FAILURE_THRESHOLD=2</div>
              <div className="text-yellow-400">Fail #1 → log WARN only</div>
              <div className="text-yellow-400">Fail #2 → 🚨 ALERT fires</div>
              <div className="text-yellow-400">Fail #3 → cooldown check</div>
            </div>
          </div>

          {/* Cooldown */}
          <div
            id="algo-cooldown"
            className="p-5 rounded-xl border border-[#1e2530] bg-[#111318]"
          >
            <h3 className="text-[#e2e8f0] font-semibold mb-3 flex items-center gap-2">
              <span className="w-2 h-2 rounded-full bg-blue-400" />
              Cooldown Period
            </h3>
            <p className="text-[#8b9ab3] text-sm mb-4 leading-relaxed">
              Prevents alert storms during long outages. Uses difftime() on the
              monotonic wall clock. No repeat alert until COOLDOWN_SECONDS elapsed.
            </p>
            <div className="code-block text-xs">
              <div className="text-[#4a5568]">// COOLDOWN_SECONDS=300 (5 min)</div>
              <div className="text-blue-400">Alert @ T+0 → sent</div>
              <div className="text-[#4a5568]">Alert @ T+60 → suppressed</div>
              <div className="text-[#4a5568]">Alert @ T+120 → suppressed</div>
              <div className="text-blue-400">Alert @ T+300 → sent again</div>
            </div>
          </div>

          {/* Recovery */}
          <div
            id="algo-recovery"
            className="p-5 rounded-xl border border-[#1e2530] bg-[#111318]"
          >
            <h3 className="text-[#e2e8f0] font-semibold mb-3 flex items-center gap-2">
              <span className="w-2 h-2 rounded-full bg-green-400" />
              Recovery Detection
            </h3>
            <p className="text-[#8b9ab3] text-sm mb-4 leading-relaxed">
              Detects the unhealthy → healthy transition on any check cycle.
              Fires an immediate green recovery alert and resets all state.
            </p>
            <div className="code-block text-xs">
              <div className="text-[#4a5568]">// state machine check:</div>
              <div className="text-red-400">is_alerting = 1 (was failing)</div>
              <div className="text-green-400">result.status = 200 (healthy!)</div>
              <div className="text-green-400">→ alert_send_recovery()</div>
              <div className="text-blue-400">→ is_alerting = 0</div>
              <div className="text-blue-400">→ consecutive_fail = 0</div>
            </div>
          </div>
        </div>

        {/* Retry logic */}
        <div
          id="algo-retry"
          className="mt-6 p-5 rounded-xl border border-[#1e2530] bg-[#111318]"
        >
          <h3 className="text-[#e2e8f0] font-semibold mb-3 flex items-center gap-2">
            <span className="w-2 h-2 rounded-full bg-purple-400" />
            Retry Loop
          </h3>
          <p className="text-[#8b9ab3] text-sm mb-4 leading-relaxed">
            If RETRY_COUNT &gt; 0, each failed attempt is retried up to RETRY_COUNT times before
            being declared a failure. A success on any retry prevents the failure from
            being counted. The retry loop is in the scheduler, not the HTTP client.
          </p>
          <div className="code-block text-xs">
            <div className="text-[#4a5568]">// RETRY_COUNT=2 → up to 3 total attempts</div>
            <div className="text-[#cbd5e1]">for (attempt = 0; attempt &lt;= retry_count; attempt++) &#123;</div>
            <div className="text-cyan-400">&nbsp;&nbsp;http_check(url, timeout, &amp;result);</div>
            <div className="text-green-400">&nbsp;&nbsp;if (result.status == 200) break; // success, stop retrying</div>
            <div className="text-[#cbd5e1]">&#125;</div>
          </div>
        </div>
      </SectionBlock>

      {/* ── 6. Error Types ── */}
      <SectionBlock
        id="error-types"
        label="06 — Error Types"
        title="Every failure,"
        titleHighlight="precisely classified."
        subtitle="http_client.c maps every failure into one of 7 error codes, giving you actionable information in the Discord alert."
      >
        <div className="overflow-hidden rounded-xl border border-[#1e2530]">
          <table className="w-full text-sm">
            <thead>
              <tr className="border-b border-[#1e2530] bg-[#111318]">
                <th className="text-left px-4 py-3 text-[#8b9ab3] font-mono text-xs font-medium">
                  Error Code
                </th>
                <th className="text-left px-4 py-3 text-[#8b9ab3] font-mono text-xs font-medium">
                  Meaning
                </th>
                <th className="text-left px-4 py-3 text-[#8b9ab3] font-mono text-xs font-medium hidden sm:table-cell">
                  Likely Cause
                </th>
              </tr>
            </thead>
            <tbody>
              {[
                {
                  code: "HTTP_ERR_NONE",
                  color: "text-green-400",
                  meaning: "Request completed (check status_code)",
                  cause: "Always returned when the server responds",
                },
                {
                  code: "HTTP_ERR_TIMEOUT",
                  color: "text-yellow-400",
                  meaning: "Connect or read timed out",
                  cause: "Server overloaded, network congestion",
                },
                {
                  code: "HTTP_ERR_CONNECT",
                  color: "text-red-400",
                  meaning: "TCP connection refused",
                  cause: "Process not listening, firewall, wrong port",
                },
                {
                  code: "HTTP_ERR_DNS",
                  color: "text-orange-400",
                  meaning: "Hostname could not be resolved",
                  cause: "DNS outage, typo in endpoint URL",
                },
                {
                  code: "HTTP_ERR_SSL",
                  color: "text-purple-400",
                  meaning: "TLS handshake or certificate error",
                  cause: "Expired cert, self-signed cert, mismatched SNI",
                },
                {
                  code: "HTTP_ERR_PROTO",
                  color: "text-blue-400",
                  meaning: "Malformed HTTP response",
                  cause: "Non-HTTP server responding on the port",
                },
                {
                  code: "HTTP_ERR_SYSTEM",
                  color: "text-red-300",
                  meaning: "OS-level error (errno set)",
                  cause: "Out of file descriptors, socket exhaustion",
                },
              ].map(({ code, color, meaning, cause }, i) => (
                <tr
                  key={code}
                  className={`border-b border-[#1e2530] ${
                    i % 2 === 0 ? "bg-[#0a0d10]" : "bg-[#0d1117]"
                  } hover:bg-[#111318] transition-colors`}
                >
                  <td className="px-4 py-3">
                    <code className={`font-mono text-xs ${color}`}>{code}</code>
                  </td>
                  <td className="px-4 py-3 text-[#cbd5e1] text-xs">{meaning}</td>
                  <td className="px-4 py-3 text-[#8b9ab3] text-xs hidden sm:table-cell">
                    {cause}
                  </td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      </SectionBlock>

      {/* ── Bottom CTA ── */}
      <div className="py-16 px-4 sm:px-6 max-w-6xl mx-auto">
        <div
          id="hiw-cta"
          className="flex flex-col sm:flex-row items-center justify-between gap-4 p-6 rounded-2xl border border-[#1e2530] bg-[#111318]"
        >
          <div>
            <h3 className="text-[#e2e8f0] font-semibold">
              Ready to run it yourself?
            </h3>
            <p className="text-[#8b9ab3] text-sm mt-1">
              Check the Get Started page for installation and CLI usage.
            </p>
          </div>
          <div className="flex gap-3 shrink-0">
            <Link
              href="/get-started"
              id="hiw-getstarted-btn"
              className="px-5 py-2.5 rounded-lg bg-blue-600 hover:bg-blue-500 text-white font-medium text-sm transition-colors"
            >
              Get Started
            </Link>
            <a
              href="https://github.com/the-aadarsh/sentinel-d"
              target="_blank"
              rel="noopener noreferrer"
              id="hiw-github-btn"
              className="px-5 py-2.5 rounded-lg border border-[#1e2530] hover:border-[#2e3a4e] text-[#e2e8f0] font-medium text-sm transition-colors"
            >
              GitHub →
            </a>
          </div>
        </div>
      </div>
    </div>
  );
}
