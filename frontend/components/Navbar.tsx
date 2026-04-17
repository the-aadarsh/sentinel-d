"use client";

import Link from "next/link";
import { usePathname } from "next/navigation";
import { useState } from "react";

const GITHUB_URL = "https://github.com/the-aadarsh/sentinel-d";

const navLinks = [
  { href: "/", label: "Overview" },
  { href: "/how-it-works", label: "How It Works" },
  { href: "/get-started", label: "Get Started" },
];

export default function Navbar() {
  const pathname = usePathname();
  const [mobileOpen, setMobileOpen] = useState(false);

  return (
    <nav
      id="navbar"
      className="fixed top-0 left-0 right-0 z-50 glass border-b border-[#1e2530]"
    >
      <div className="max-w-6xl mx-auto px-4 sm:px-6">
        <div className="flex items-center justify-between h-14">
          {/* Logo */}
          <Link href="/" className="flex items-center gap-2 group" id="nav-logo">
            <div className="relative w-7 h-7 flex items-center justify-center">
              <div className="absolute inset-0 rounded-md bg-blue-500/20 group-hover:bg-blue-500/30 transition-colors" />
              <svg
                width="16"
                height="16"
                viewBox="0 0 16 16"
                fill="none"
                className="relative z-10"
              >
                <circle
                  cx="8"
                  cy="8"
                  r="3"
                  fill="#3b82f6"
                  className="animate-pulse"
                />
                <circle
                  cx="8"
                  cy="8"
                  r="6.5"
                  stroke="#3b82f6"
                  strokeWidth="1"
                  strokeOpacity="0.4"
                />
                <circle
                  cx="8"
                  cy="8"
                  r="4"
                  stroke="#60a5fa"
                  strokeWidth="0.5"
                  strokeDasharray="2 2"
                  className="animate-spin"
                  style={{ animationDuration: "8s" }}
                />
              </svg>
            </div>
            <span className="font-mono text-sm font-semibold tracking-widest text-[#e2e8f0] uppercase">
              Sentinel-D
            </span>
            <span className="hidden sm:inline text-xs font-mono text-blue-400/70 border border-blue-500/20 rounded px-1.5 py-0.5 bg-blue-500/5">
              v1.0.0
            </span>
          </Link>

          {/* Desktop Nav */}
          <div className="hidden md:flex items-center gap-1">
            {navLinks.map((link) => {
              const active = pathname === link.href;
              return (
                <Link
                  key={link.href}
                  href={link.href}
                  id={`nav-${link.label.toLowerCase().replace(/\s+/g, "-")}`}
                  className={`px-3 py-1.5 rounded-md text-sm font-medium transition-all duration-200 ${
                    active
                      ? "text-blue-400 bg-blue-500/10"
                      : "text-[#8b9ab3] hover:text-[#e2e8f0] hover:bg-white/5"
                  }`}
                >
                  {link.label}
                </Link>
              );
            })}
          </div>

          {/* Right: GitHub */}
          <div className="flex items-center gap-3">
            <a
              href={GITHUB_URL}
              target="_blank"
              rel="noopener noreferrer"
              id="nav-github"
              className="hidden md:flex items-center gap-1.5 text-sm text-[#8b9ab3] hover:text-[#e2e8f0] transition-colors"
            >
              <svg
                width="16"
                height="16"
                viewBox="0 0 24 24"
                fill="currentColor"
              >
                <path d="M12 0C5.374 0 0 5.373 0 12c0 5.302 3.438 9.8 8.207 11.387.599.111.793-.261.793-.577v-2.234c-3.338.726-4.033-1.416-4.033-1.416-.546-1.387-1.333-1.756-1.333-1.756-1.089-.745.083-.729.083-.729 1.205.084 1.839 1.237 1.839 1.237 1.07 1.834 2.807 1.304 3.492.997.107-.775.418-1.305.762-1.604-2.665-.305-5.467-1.334-5.467-5.931 0-1.311.469-2.381 1.236-3.221-.124-.303-.535-1.524.117-3.176 0 0 1.008-.322 3.301 1.23A11.509 11.509 0 0 1 12 5.803c1.02.005 2.047.138 3.006.404 2.291-1.552 3.297-1.23 3.297-1.23.653 1.653.242 2.874.118 3.176.77.84 1.235 1.911 1.235 3.221 0 4.609-2.807 5.624-5.479 5.921.43.372.823 1.102.823 2.222v3.293c0 .319.192.694.801.576C20.566 21.797 24 17.3 24 12c0-6.627-5.373-12-12-12z" />
              </svg>
              GitHub
            </a>

            {/* Mobile hamburger */}
            <button
              className="md:hidden p-1.5 rounded-md text-[#8b9ab3] hover:text-[#e2e8f0] hover:bg-white/5 transition-colors"
              onClick={() => setMobileOpen(!mobileOpen)}
              id="nav-mobile-toggle"
              aria-label="Toggle menu"
            >
              {mobileOpen ? (
                <svg
                  width="18"
                  height="18"
                  viewBox="0 0 24 24"
                  fill="none"
                  stroke="currentColor"
                  strokeWidth="2"
                >
                  <path d="M18 6L6 18M6 6l12 12" />
                </svg>
              ) : (
                <svg
                  width="18"
                  height="18"
                  viewBox="0 0 24 24"
                  fill="none"
                  stroke="currentColor"
                  strokeWidth="2"
                >
                  <path d="M3 12h18M3 6h18M3 18h18" />
                </svg>
              )}
            </button>
          </div>
        </div>

        {/* Mobile menu */}
        {mobileOpen && (
          <div className="md:hidden border-t border-[#1e2530] py-3 space-y-1">
            {navLinks.map((link) => {
              const active = pathname === link.href;
              return (
                <Link
                  key={link.href}
                  href={link.href}
                  onClick={() => setMobileOpen(false)}
                  className={`block px-3 py-2 rounded-md text-sm font-medium transition-colors ${
                    active
                      ? "text-blue-400 bg-blue-500/10"
                      : "text-[#8b9ab3] hover:text-[#e2e8f0] hover:bg-white/5"
                  }`}
                >
                  {link.label}
                </Link>
              );
            })}
            <a
              href={GITHUB_URL}
              target="_blank"
              rel="noopener noreferrer"
              className="flex items-center gap-2 px-3 py-2 text-sm text-[#8b9ab3] hover:text-[#e2e8f0] transition-colors"
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
          </div>
        )}
      </div>
    </nav>
  );
}
