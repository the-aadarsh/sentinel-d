import type { Metadata } from "next";
import { Inter, JetBrains_Mono } from "next/font/google";
import "./globals.css";
import Navbar from "@/components/Navbar";

const inter = Inter({
  subsets: ["latin"],
  variable: "--font-inter",
  display: "swap",
});

const jetbrainsMono = JetBrains_Mono({
  subsets: ["latin"],
  variable: "--font-mono",
  display: "swap",
});

export const metadata: Metadata = {
  title: "SENTINEL-D — Lightweight HTTP Health Monitoring Daemon",
  description:
    "A production-grade UNIX health-monitoring daemon written in pure C. Polls HTTP/HTTPS endpoints, detects failures, and sends structured Discord alerts.",
  keywords: [
    "health monitoring",
    "daemon",
    "C programming",
    "UNIX",
    "HTTP monitoring",
    "Discord alerts",
    "infrastructure",
    "SENTINEL-D",
  ],
  authors: [{ name: "the-aadarsh", url: "https://github.com/the-aadarsh" }],
  openGraph: {
    title: "SENTINEL-D — Lightweight HTTP Health Monitoring Daemon",
    description:
      "A production-grade UNIX health-monitoring daemon in pure C. Intelligent alerting, Discord webhooks, zero runtime overhead.",
    type: "website",
    url: "https://github.com/the-aadarsh/sentinel-d",
  },
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="en" className={`${inter.variable} ${jetbrainsMono.variable}`}>
      <body className="antialiased">
        <Navbar />
        <main>{children}</main>
      </body>
    </html>
  );
}
