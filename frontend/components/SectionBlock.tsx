interface SectionBlockProps {
  id?: string;
  label?: string;
  title: string;
  titleHighlight?: string;
  subtitle?: string;
  children: React.ReactNode;
  className?: string;
  centered?: boolean;
}

export default function SectionBlock({
  id,
  label,
  title,
  titleHighlight,
  subtitle,
  children,
  className = "",
  centered = false,
}: SectionBlockProps) {
  return (
    <section
      id={id}
      className={`py-20 px-4 sm:px-6 max-w-6xl mx-auto ${className}`}
    >
      <div className={`mb-12 ${centered ? "text-center" : ""}`}>
        {label && (
          <span className="inline-block font-mono text-xs tracking-widest text-blue-400 uppercase mb-3 border border-blue-500/20 rounded-full px-3 py-1 bg-blue-500/5">
            {label}
          </span>
        )}
        <h2 className="text-3xl sm:text-4xl font-bold text-[#e2e8f0] leading-tight">
          {title}
          {titleHighlight && (
            <>
              {" "}
              <span className="text-gradient-blue">{titleHighlight}</span>
            </>
          )}
        </h2>
        {subtitle && (
          <p
            className={`mt-3 text-[#8b9ab3] text-base leading-relaxed max-w-2xl ${
              centered ? "mx-auto" : ""
            }`}
          >
            {subtitle}
          </p>
        )}
      </div>

      {children}
    </section>
  );
}
