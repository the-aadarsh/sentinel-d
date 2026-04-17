import Link from "next/link";

interface HeroProps {
  badge?: string;
  title: string;
  titleHighlight?: string;
  subtitle: string;
  primaryAction?: { label: string; href: string; external?: boolean };
  secondaryAction?: { label: string; href: string; external?: boolean };
  showGrid?: boolean;
}

export default function Hero({
  badge,
  title,
  titleHighlight,
  subtitle,
  primaryAction,
  secondaryAction,
  showGrid = true,
}: HeroProps) {
  return (
    <section
      id="hero"
      className={`relative min-h-[90vh] flex items-center justify-center overflow-hidden ${
        showGrid ? "grid-bg" : ""
      }`}
    >
      {/* Radial glow background */}
      <div
        className="absolute inset-0 pointer-events-none"
        aria-hidden="true"
      >
        <div className="absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2 w-[700px] h-[700px] rounded-full bg-blue-500/5 blur-3xl" />
        <div className="absolute top-1/3 left-1/4 w-[300px] h-[300px] rounded-full bg-indigo-500/5 blur-3xl" />
      </div>

      <div className="relative z-10 max-w-4xl mx-auto px-4 sm:px-6 text-center py-24">
        {/* Badge */}
        {badge && (
          <div className="inline-flex items-center gap-2 px-3 py-1.5 rounded-full border border-blue-500/20 bg-blue-500/5 text-blue-400 text-xs font-mono tracking-wider mb-8">
            <span className="w-1.5 h-1.5 rounded-full bg-blue-400 animate-pulse" />
            {badge}
          </div>
        )}

        {/* Title */}
        <h1 className="text-4xl sm:text-6xl lg:text-7xl font-bold tracking-tight leading-none mb-6">
          <span className="text-gradient">{title}</span>
          {titleHighlight && (
            <>
              <br />
              <span className="text-gradient-blue">{titleHighlight}</span>
            </>
          )}
        </h1>

        {/* Subtitle */}
        <p className="text-lg sm:text-xl text-[#8b9ab3] max-w-2xl mx-auto leading-relaxed mb-10">
          {subtitle}
        </p>

        {/* Actions */}
        {(primaryAction || secondaryAction) && (
          <div className="flex flex-col sm:flex-row items-center justify-center gap-3">
            {primaryAction &&
              (primaryAction.external ? (
                <a
                  href={primaryAction.href}
                  target="_blank"
                  rel="noopener noreferrer"
                  id="hero-primary-btn"
                  className="inline-flex items-center gap-2 px-6 py-3 rounded-lg bg-blue-600 hover:bg-blue-500 text-white font-medium text-sm transition-all duration-200 hover:shadow-lg hover:shadow-blue-500/20 active:scale-95"
                >
                  {primaryAction.label}
                  <svg
                    width="14"
                    height="14"
                    viewBox="0 0 24 24"
                    fill="none"
                    stroke="currentColor"
                    strokeWidth="2"
                  >
                    <path d="M7 17L17 7M17 7H7M17 7v10" />
                  </svg>
                </a>
              ) : (
                <Link
                  href={primaryAction.href}
                  id="hero-primary-btn"
                  className="inline-flex items-center gap-2 px-6 py-3 rounded-lg bg-blue-600 hover:bg-blue-500 text-white font-medium text-sm transition-all duration-200 hover:shadow-lg hover:shadow-blue-500/20 active:scale-95"
                >
                  {primaryAction.label}
                </Link>
              ))}

            {secondaryAction &&
              (secondaryAction.external ? (
                <a
                  href={secondaryAction.href}
                  target="_blank"
                  rel="noopener noreferrer"
                  id="hero-secondary-btn"
                  className="inline-flex items-center gap-2 px-6 py-3 rounded-lg border border-[#1e2530] bg-[#111318] hover:border-[#2e3a4e] hover:bg-[#151a22] text-[#e2e8f0] font-medium text-sm transition-all duration-200"
                >
                  <svg
                    width="14"
                    height="14"
                    viewBox="0 0 24 24"
                    fill="currentColor"
                  >
                    <path d="M12 0C5.374 0 0 5.373 0 12c0 5.302 3.438 9.8 8.207 11.387.599.111.793-.261.793-.577v-2.234c-3.338.726-4.033-1.416-4.033-1.416-.546-1.387-1.333-1.756-1.333-1.756-1.089-.745.083-.729.083-.729 1.205.084 1.839 1.237 1.839 1.237 1.07 1.834 2.807 1.304 3.492.997.107-.775.418-1.305.762-1.604-2.665-.305-5.467-1.334-5.467-5.931 0-1.311.469-2.381 1.236-3.221-.124-.303-.535-1.524.117-3.176 0 0 1.008-.322 3.301 1.23A11.509 11.509 0 0 1 12 5.803c1.02.005 2.047.138 3.006.404 2.291-1.552 3.297-1.23 3.297-1.23.653 1.653.242 2.874.118 3.176.77.84 1.235 1.911 1.235 3.221 0 4.609-2.807 5.624-5.479 5.921.43.372.823 1.102.823 2.222v3.293c0 .319.192.694.801.576C20.566 21.797 24 17.3 24 12c0-6.627-5.373-12-12-12z" />
                  </svg>
                  {secondaryAction.label}
                </a>
              ) : (
                <Link
                  href={secondaryAction.href}
                  id="hero-secondary-btn"
                  className="inline-flex items-center gap-2 px-6 py-3 rounded-lg border border-[#1e2530] bg-[#111318] hover:border-[#2e3a4e] hover:bg-[#151a22] text-[#e2e8f0] font-medium text-sm transition-all duration-200"
                >
                  {secondaryAction.label}
                </Link>
              ))}
          </div>
        )}
      </div>
    </section>
  );
}
