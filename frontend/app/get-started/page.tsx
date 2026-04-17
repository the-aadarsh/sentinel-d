import type { Metadata } from "next";

export const metadata: Metadata = {
  title: "Get Started — SENTINEL-D",
  description:
    "Installation, configuration, and CLI usage for SENTINEL-D — coming soon.",
};

const steps = [
  {
    number: "01",
    title: "Install prerequisites",
    status: "planned",
    preview: "GCC or Clang · Make · OpenSSL dev headers",
  },
  {
    number: "02",
    title: "Clone & build",
    status: "planned",
    preview: "git clone · make → bin/sentinel-d",
  },
  {
    number: "03",
    title: "Configure sentinel.conf",
    status: "planned",
    preview: "ENDPOINTS · DISCORD_WEBHOOK_URL · intervals",
  },
  {
    number: "04",
    title: "Run & monitor",
    status: "planned",
    preview: "./bin/sentinel-d -c sentinel.conf -f",
  },
];

export default function GetStartedPage() {
  return (
    <div className="bg-[#0a0d10] min-h-screen pt-14">
      {/* ── Hero ── */}
      <div
        id="getstarted-hero"
        className="relative py-24 px-4 sm:px-6 text-center grid-bg overflow-hidden"
      >
        {/* Glow */}
        <div className="absolute inset-0 pointer-events-none" aria-hidden="true">
          <div className="absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2 w-[500px] h-[400px] rounded-full bg-blue-500/5 blur-3xl" />
        </div>

        <div className="relative z-10 max-w-2xl mx-auto">
          {/* Status badge */}
          <div className="inline-flex items-center gap-2 px-3 py-1.5 rounded-full border border-yellow-500/30 bg-yellow-500/5 text-yellow-400 text-xs font-mono tracking-wider mb-8">
            <span className="w-1.5 h-1.5 rounded-full bg-yellow-400 animate-pulse" />
            Coming Soon
          </div>

          <h1 className="text-4xl sm:text-5xl font-bold text-gradient mb-4">
            Get Started
          </h1>
          <p className="text-[#8b9ab3] text-lg leading-relaxed">
            Full installation guide, configuration reference, and CLI usage
            docs are coming soon. In the meantime, explore the source and README.
          </p>
        </div>
      </div>

      {/* ── Preview steps ── */}
      <div className="max-w-3xl mx-auto px-4 sm:px-6 py-16">
        <h2 className="text-xl font-semibold text-[#e2e8f0] mb-8 text-center">
          Here&apos;s what&apos;s coming…
        </h2>

        <div className="space-y-4">
          {steps.map(({ number, title, preview }, i) => (
            <div
              key={number}
              id={`step-preview-${number}`}
              className="flex items-center gap-4 p-5 rounded-xl border border-[#1e2530] bg-[#111318] opacity-70 relative overflow-hidden group"
            >
              {/* Shimmer on hover */}
              <div className="absolute inset-0 bg-gradient-to-r from-transparent via-white/[0.02] to-transparent -translate-x-full group-hover:translate-x-full transition-transform duration-700" />

              <div className="shrink-0 w-10 h-10 rounded-lg border border-[#1e2530] bg-[#0a0d10] flex items-center justify-center font-mono text-xs text-[#4a5568] font-bold">
                {number}
              </div>

              <div className="flex-1 min-w-0">
                <div className="text-[#8b9ab3] font-medium text-sm">{title}</div>
                <div className="text-[#4a5568] text-xs font-mono mt-0.5 truncate">
                  {preview}
                </div>
              </div>

              <div className="shrink-0 px-2 py-0.5 rounded-full border border-yellow-500/20 bg-yellow-500/5 text-yellow-400/70 text-xs font-mono">
                soon
              </div>
            </div>
          ))}
        </div>

        {/* ── Teaser config block ── */}
        <div className="mt-12">
          <div className="text-[#4a5568] font-mono text-xs mb-2 text-center">
            # Preview: sentinel.conf
          </div>
          <div className="code-block text-sm relative">
            {/* Blur overlay */}
            <div className="absolute inset-0 bg-gradient-to-b from-transparent via-[#0d1117]/50 to-[#0d1117] rounded-lg backdrop-blur-[1px] flex items-end justify-center pb-6">
              <span className="px-3 py-1.5 rounded-full border border-yellow-500/30 bg-[#0d1117] text-yellow-400 text-xs font-mono">
                Full guide coming soon
              </span>
            </div>

            <div className="text-[#4a5568]"># sentinel.conf</div>
            <div className="text-purple-400">SERVICE_NAME</div>
            <div className="text-[#4a5568]">=MyApp</div>
            <div className="text-purple-400">ENVIRONMENT</div>
            <div className="text-[#4a5568]">=production</div>
            <div className="text-yellow-400">CHECK_INTERVAL_SECONDS</div>
            <div className="text-[#4a5568]">=30</div>
            <div className="text-yellow-400">FAILURE_THRESHOLD</div>
            <div className="text-[#4a5568]">=2</div>
            <div className="text-yellow-400">COOLDOWN_SECONDS</div>
            <div className="text-[#4a5568]">=300</div>
            <div className="text-green-400">ENDPOINTS</div>
            <div className="text-[#4a5568]">=https://api.example.com/health</div>
            <div className="text-red-400">DISCORD_WEBHOOK_URL</div>
            <div className="text-[#4a5568]">=https://discord.com/api/webhooks/...</div>
          </div>
        </div>

        {/* ── CTA ── */}
        <div
          id="getstarted-cta"
          className="mt-12 text-center space-y-4"
        >
          <p className="text-[#8b9ab3] text-sm">
            Until then, the full source and README are available on GitHub.
          </p>
          <div className="flex flex-col sm:flex-row items-center justify-center gap-3">
            <a
              href="https://github.com/the-aadarsh/sentinel-d"
              target="_blank"
              rel="noopener noreferrer"
              id="getstarted-github-btn"
              className="inline-flex items-center gap-2 px-6 py-3 rounded-lg bg-[#111318] border border-[#1e2530] hover:border-[#2e3a4e] text-[#e2e8f0] font-medium text-sm transition-colors"
            >
              <svg
                width="15"
                height="15"
                viewBox="0 0 24 24"
                fill="currentColor"
              >
                <path d="M12 0C5.374 0 0 5.373 0 12c0 5.302 3.438 9.8 8.207 11.387.599.111.793-.261.793-.577v-2.234c-3.338.726-4.033-1.416-4.033-1.416-.546-1.387-1.333-1.756-1.333-1.756-1.089-.745.083-.729.083-.729 1.205.084 1.839 1.237 1.839 1.237 1.07 1.834 2.807 1.304 3.492.997.107-.775.418-1.305.762-1.604-2.665-.305-5.467-1.334-5.467-5.931 0-1.311.469-2.381 1.236-3.221-.124-.303-.535-1.524.117-3.176 0 0 1.008-.322 3.301 1.23A11.509 11.509 0 0 1 12 5.803c1.02.005 2.047.138 3.006.404 2.291-1.552 3.297-1.23 3.297-1.23.653 1.653.242 2.874.118 3.176.77.84 1.235 1.911 1.235 3.221 0 4.609-2.807 5.624-5.479 5.921.43.372.823 1.102.823 2.222v3.293c0 .319.192.694.801.576C20.566 21.797 24 17.3 24 12c0-6.627-5.373-12-12-12z" />
              </svg>
              View on GitHub
            </a>
            <a
              href="https://github.com/the-aadarsh/sentinel-d/blob/main/README.md"
              target="_blank"
              rel="noopener noreferrer"
              id="getstarted-readme-btn"
              className="inline-flex items-center gap-2 px-6 py-3 rounded-lg bg-blue-600 hover:bg-blue-500 text-white font-medium text-sm transition-colors"
            >
              Read the README →
            </a>
          </div>
        </div>
      </div>

      {/* ── Footer ── */}
      <footer
        id="getstarted-footer"
        className="border-t border-[#1e2530] py-8 px-4 text-center"
      >
        <p className="text-[#8b9ab3] text-sm font-mono">
          SENTINEL-D · Pure C11 · MIT License ·{" "}
          <a
            href="https://github.com/the-aadarsh/sentinel-d"
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
