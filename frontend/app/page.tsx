import Hero from "@/components/Hero";
import FeatureCard from "@/components/FeatureCard";
import SectionBlock from "@/components/SectionBlock";
import Link from "next/link";

const GITHUB_URL = "https://github.com/the-aadarsh/sentinel-d";

export default function HomePage() {
  return (
    <div className="bg-[#0a0d10]">
      {/* ── Hero ── */}
      <Hero
        badge="Pure C11 · POSIX · OpenSSL · v1.0.0"
        title="Silent failures"
        titleHighlight="never again."
        subtitle="SENTINEL-D is a lightweight, production-grade UNIX daemon written in pure C that polls your HTTP/HTTPS endpoints and fires structured Discord alerts the moment something breaks."
        primaryAction={{ label: "Get Started", href: "/get-started" }}
        secondaryAction={{
          label: "View on GitHub",
          href: GITHUB_URL,
          external: true,
        }}
      />

      {/* ── Problem / Solution ── */}
      <SectionBlock
        id="problem-solution"
        label="Why Sentinel-D"
        title="Infrastructure fails"
        titleHighlight="silently."
        subtitle="Services crash, APIs time out, and DNS breaks — often unnoticed for minutes or hours. Your users find out before you do."
      >
        <div className="grid md:grid-cols-2 gap-6">
          {/* Problem */}
          <div
            id="problem-card"
            className="p-6 rounded-xl border border-red-500/20 bg-red-500/5 relative overflow-hidden"
          >
            <div className="absolute top-0 right-0 w-24 h-24 rounded-full bg-red-500/5 blur-2xl" />
            <div className="flex items-center gap-2 mb-4">
              <span className="w-2.5 h-2.5 rounded-full bg-red-400 animate-pulse" />
              <span className="text-red-400 text-sm font-mono tracking-wider uppercase">
                The Problem
              </span>
            </div>
            <h3 className="text-xl font-semibold text-[#e2e8f0] mb-3">
              Silent infrastructure failures
            </h3>
            <ul className="space-y-2 text-[#8b9ab3] text-sm">
              {[
                "Endpoints go down undetected",
                "DNS resolves to nothing",
                "TLS certificates expire quietly",
                "Non-200 responses break downstream systems",
                "You hear about it from users, not your tools",
              ].map((item) => (
                <li key={item} className="flex items-start gap-2">
                  <span className="text-red-400 mt-0.5">✗</span>
                  {item}
                </li>
              ))}
            </ul>
          </div>

          {/* Solution */}
          <div
            id="solution-card"
            className="p-6 rounded-xl border border-green-500/20 bg-green-500/5 relative overflow-hidden"
          >
            <div className="absolute top-0 right-0 w-24 h-24 rounded-full bg-green-500/5 blur-2xl" />
            <div className="flex items-center gap-2 mb-4">
              <span className="w-2.5 h-2.5 rounded-full bg-green-400 animate-pulse" />
              <span className="text-green-400 text-sm font-mono tracking-wider uppercase">
                The Solution
              </span>
            </div>
            <h3 className="text-xl font-semibold text-[#e2e8f0] mb-3">
              SENTINEL-D watches so you don&apos;t have to
            </h3>
            <ul className="space-y-2 text-[#8b9ab3] text-sm">
              {[
                "Polls HTTP/HTTPS endpoints on a configurable interval",
                "Detects timeouts, DNS failures, TLS errors, and non-200 status",
                "Triggers Discord alerts after a configurable failure threshold",
                "Suppresses alert spam with a smart cooldown mechanism",
                "Sends an immediate green recovery alert when service restores",
              ].map((item) => (
                <li key={item} className="flex items-start gap-2">
                  <span className="text-green-400 mt-0.5">✓</span>
                  {item}
                </li>
              ))}
            </ul>
          </div>
        </div>
      </SectionBlock>

      {/* ── Architecture Diagram ── */}
      <div className="py-8 px-4 sm:px-6 max-w-6xl mx-auto">
        <div
          id="architecture-diagram"
          className="rounded-2xl border border-[#1e2530] bg-[#0d1117] p-6 sm:p-10 overflow-x-auto"
        >
          <div className="text-center mb-8">
            <span className="font-mono text-xs tracking-widest text-blue-400 uppercase">
              System Architecture
            </span>
            <h2 className="text-2xl font-bold text-[#e2e8f0] mt-2">
              How the modules connect
            </h2>
          </div>

          {/* ASCII-style module diagram */}
          <div className="font-mono text-xs sm:text-sm text-center">
            {/* Row 1 – main.c */}
            <div className="flex justify-center mb-2">
              <div className="px-4 py-2 rounded-lg border border-blue-500/40 bg-blue-500/10 text-blue-300 font-semibold">
                main.c
              </div>
            </div>
            {/* Arrows down */}
            <div className="flex justify-center gap-12 sm:gap-20 text-[#8b9ab3] mb-2 text-xs">
              <span>config</span>
              <span>daemon</span>
              <span>logger</span>
              <span>scheduler</span>
            </div>
            <div className="flex justify-center gap-12 sm:gap-20 text-[#2e3a4e] mb-2">
              <span>↓</span>
              <span>↓</span>
              <span>↓</span>
              <span>↓</span>
            </div>
            {/* Row 2 – modules */}
            <div className="flex justify-center flex-wrap gap-2 sm:gap-4 mb-8">
              {[
                { label: "config.c", color: "purple" },
                { label: "daemon.c", color: "yellow" },
                { label: "logger.c", color: "cyan" },
                { label: "scheduler.c", color: "green", primary: true },
              ].map(({ label, color, primary }) => (
                <div
                  key={label}
                  className={`px-3 py-2 rounded-lg border text-xs font-medium ${
                    primary
                      ? "border-green-500/40 bg-green-500/10 text-green-300"
                      : color === "purple"
                      ? "border-purple-500/30 bg-purple-500/8 text-purple-300"
                      : color === "yellow"
                      ? "border-yellow-500/30 bg-yellow-500/8 text-yellow-300"
                      : "border-cyan-500/30 bg-cyan-500/8 text-cyan-300"
                  }`}
                >
                  {label}
                </div>
              ))}
            </div>

            {/* scheduler → http_client + alert */}
            <div className="flex justify-center gap-8 text-[#8b9ab3] text-xs mb-2">
              <span>polls via</span>
              <span>triggers</span>
            </div>
            <div className="flex justify-center gap-8 text-[#2e3a4e] mb-2">
              <span>↓</span>
              <span>↓</span>
            </div>
            <div className="flex justify-center gap-4 mb-6">
              <div className="px-3 py-2 rounded-lg border border-orange-500/30 bg-orange-500/8 text-orange-300 text-xs font-medium">
                http_client.c
              </div>
              <div className="px-3 py-2 rounded-lg border border-red-500/30 bg-red-500/8 text-red-300 text-xs font-medium">
                alert.c
              </div>
            </div>

            <div className="flex justify-center gap-4 text-[#2e3a4e] mb-2">
              <span>↓ TLS/TCP</span>
              <span>↓ POST via HTTPS</span>
            </div>
            <div className="flex justify-center gap-4">
              <div className="px-3 py-2 rounded-lg border border-[#1e2530] bg-[#111318] text-[#8b9ab3] text-xs font-medium">
                OpenSSL libssl
              </div>
              <div className="px-3 py-2 rounded-lg border border-[#1e2530] bg-[#111318] text-[#8b9ab3] text-xs font-medium">
                Discord Webhook
              </div>
            </div>
          </div>
        </div>
      </div>

      {/* ── Key Features ── */}
      <SectionBlock
        id="features"
        label="Features"
        title="Built for"
        titleHighlight="production."
        subtitle="Every feature derived directly from the C source — no invented capabilities, just what the code actually does."
      >
        <div className="grid sm:grid-cols-2 lg:grid-cols-3 gap-4">
          <FeatureCard
            id="feature-pure-c"
            accent="blue"
            icon={
              <svg width="18" height="18" viewBox="0 0 24 24" fill="currentColor">
                <path d="M9.4 16.6L4.8 12l4.6-4.6L8 6l-6 6 6 6 1.4-1.4zm5.2 0l4.6-4.6-4.6-4.6L16 6l6 6-6 6-1.4-1.4z" />
              </svg>
            }
            title="Pure C11 Implementation"
            description="Zero external framework dependencies. Written in C11 using POSIX sockets and OpenSSL — fast, deterministic, and memory-safe."
          />
          <FeatureCard
            id="feature-daemon"
            accent="blue"
            icon={
              <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
                <circle cx="12" cy="12" r="3" />
                <path d="M12 1v4M12 19v4M4.22 4.22l2.83 2.83M16.95 16.95l2.83 2.83M1 12h4M19 12h4M4.22 19.78l2.83-2.83M16.95 7.05l2.83-2.83" />
              </svg>
            }
            title="True UNIX Daemon"
            description="Uses the classic double-fork technique (fork → setsid → fork) to fully detach from the controlling terminal and runs invisibly in the background."
          />
          <FeatureCard
            id="feature-smart-alerting"
            accent="green"
            icon={
              <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
                <path d="M18 8A6 6 0 0 0 6 8c0 7-3 9-3 9h18s-3-2-3-9" />
                <path d="M13.73 21a2 2 0 0 1-3.46 0" />
              </svg>
            }
            title="Smart Alerting"
            description="Configurable failure threshold prevents noise. Cooldown periods stop alert storms. Immediate recovery alerts when a service comes back online."
          />
          <FeatureCard
            id="feature-discord"
            accent="blue"
            icon={
              <svg width="18" height="18" viewBox="0 0 24 24" fill="currentColor">
                <path d="M20.317 4.37a19.791 19.791 0 0 0-4.885-1.515.074.074 0 0 0-.079.037c-.21.375-.444.864-.608 1.25a18.27 18.27 0 0 0-5.487 0 12.64 12.64 0 0 0-.617-1.25.077.077 0 0 0-.079-.037A19.736 19.736 0 0 0 3.677 4.37a.07.07 0 0 0-.032.027C.533 9.046-.32 13.58.099 18.057.1 18.098.12 18.138.15 18.162a19.9 19.9 0 0 0 5.993 3.03.078.078 0 0 0 .084-.028c.462-.63.874-1.295 1.226-1.994a.076.076 0 0 0-.041-.106 13.107 13.107 0 0 1-1.872-.892.077.077 0 0 1-.008-.128 10.2 10.2 0 0 0 .372-.292.074.074 0 0 1 .077-.01c3.928 1.793 8.18 1.793 12.062 0a.074.074 0 0 1 .078.01c.12.098.246.198.373.292a.077.077 0 0 1-.006.127 12.299 12.299 0 0 1-1.873.892.077.077 0 0 0-.041.107c.36.698.772 1.362 1.225 1.993a.076.076 0 0 0 .084.028 19.839 19.839 0 0 0 6.002-3.03.077.077 0 0 0 .032-.054c.5-5.177-.838-9.674-3.549-13.66a.061.061 0 0 0-.031-.03z" />
              </svg>
            }
            title="Discord Integration"
            description="Sends color-coded rich embed payloads to a Discord webhook over TLS. Red for failures, green for recovery — no third-party alert SDK required."
          />
          <FeatureCard
            id="feature-networking"
            accent="yellow"
            icon={
              <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
                <path d="M5 12.55a11 11 0 0 1 14.08 0" />
                <path d="M1.42 9a16 16 0 0 1 21.16 0" />
                <path d="M8.53 16.11a6 6 0 0 1 6.95 0" />
                <line x1="12" y1="20" x2="12.01" y2="20" />
              </svg>
            }
            title="Accurate Error Detection"
            description="Explicitly classifies every failure: connection refused, DNS resolution failure, TLS handshake error, HTTP timeout, or non-200 status code."
          />
          <FeatureCard
            id="feature-config"
            accent="blue"
            icon={
              <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
                <circle cx="12" cy="12" r="3" />
                <path d="M19.07 4.93l-1.41 1.41M6.34 17.66l-1.41 1.41M2 12H4M20 12h2M6.34 6.34L4.93 4.93M17.66 17.66l1.41 1.41M12 2v2M12 20v2" />
              </svg>
            }
            title="Highly Configurable"
            description="Plain-text KEY=VALUE config file with environment variable overrides. Control polling intervals, timeouts, retries, failure thresholds, and cooldowns."
          />
        </div>
      </SectionBlock>

      {/* ── Stats Bar ── */}
      <div className="py-12 px-4 sm:px-6 max-w-6xl mx-auto">
        <div
          id="stats-bar"
          className="grid grid-cols-2 sm:grid-cols-4 gap-4 p-6 rounded-2xl border border-[#1e2530] bg-[#111318]"
        >
          {[
            { value: "8", label: "Source modules", sub: ".c + .h files" },
            { value: "64", label: "Max endpoints", sub: "monitored concurrently" },
            { value: "0", label: "Dependencies", sub: "beyond OpenSSL" },
            { value: "3", label: "Unit test suites", sub: "config · utils · alert" },
          ].map(({ value, label, sub }) => (
            <div key={label} className="text-center">
              <div className="text-3xl font-bold text-gradient-blue font-mono">
                {value}
              </div>
              <div className="text-[#e2e8f0] text-sm font-medium mt-1">
                {label}
              </div>
              <div className="text-[#8b9ab3] text-xs mt-0.5">{sub}</div>
            </div>
          ))}
        </div>
      </div>

      {/* ── Log Levels ── */}
      <SectionBlock
        id="log-levels"
        label="Observability"
        title="Full visibility,"
        titleHighlight="zero noise."
        subtitle="Four log levels let you tune verbosity from production silence to full debug tracing."
      >
        <div className="code-block">
          <div className="space-y-2">
            {[
              {
                level: "DEBUG",
                color: "text-cyan-400",
                desc: "Traces every poll cycle, retry attempt, and cooldown timer",
              },
              {
                level: "INFO ",
                color: "text-blue-400",
                desc: "Startup, shutdown, and alert transmissions  (Default)",
              },
              {
                level: "WARN ",
                color: "text-yellow-400",
                desc: "Individual health check failures",
              },
              {
                level: "ERROR",
                color: "text-red-400",
                desc: "Unrecoverable system or webhook transmission errors",
              },
            ].map(({ level, color, desc }) => (
              <div key={level} className="flex items-start gap-4">
                <span className={`${color} font-bold w-14 shrink-0`}>
                  [{level}]
                </span>
                <span className="text-[#8b9ab3]">{desc}</span>
              </div>
            ))}
          </div>
          <div className="mt-4 pt-4 border-t border-[#1e2530] text-[#4a5568] text-xs">
            # Secrets (webhook URL) are NEVER written to logs
          </div>
        </div>
      </SectionBlock>

      {/* ── CTA Footer ── */}
      <div className="py-20 px-4 sm:px-6">
        <div
          id="cta-section"
          className="max-w-3xl mx-auto text-center rounded-2xl border border-blue-500/20 bg-gradient-to-b from-blue-500/5 to-transparent p-12"
        >
          <h2 className="text-3xl sm:text-4xl font-bold text-[#e2e8f0] mb-4">
            Ready to add eyes to your infrastructure?
          </h2>
          <p className="text-[#8b9ab3] mb-8">
            Compile it, point it at your endpoints, and never miss a failure again.
          </p>
          <div className="flex flex-col sm:flex-row items-center justify-center gap-3">
            <Link
              href="/how-it-works"
              id="cta-learn-btn"
              className="px-6 py-3 rounded-lg bg-blue-600 hover:bg-blue-500 text-white font-medium text-sm transition-all duration-200 hover:shadow-lg hover:shadow-blue-500/20"
            >
              Learn How It Works
            </Link>
            <a
              href={GITHUB_URL}
              target="_blank"
              rel="noopener noreferrer"
              id="cta-github-btn"
              className="px-6 py-3 rounded-lg border border-[#1e2530] hover:border-[#2e3a4e] text-[#e2e8f0] font-medium text-sm transition-all duration-200"
            >
              View Source on GitHub
            </a>
          </div>
        </div>
      </div>

      {/* ── Footer ── */}
      <footer
        id="footer"
        className="border-t border-[#1e2530] py-8 px-4 text-center"
      >
        <p className="text-[#8b9ab3] text-sm font-mono">
          SENTINEL-D · Pure C11 · MIT License ·{" "}
          <a
            href={GITHUB_URL}
            target="_blank"
            rel="noopener noreferrer"
            className="text-blue-400 hover:text-blue-300 transition-colors"
          >
            github.com/the-aadarsh/sentinel-d
          </a>
        </p>
      </footer>
    </div>
  );
}
