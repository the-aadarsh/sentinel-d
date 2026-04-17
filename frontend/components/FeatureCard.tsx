interface FeatureCardProps {
  icon: React.ReactNode;
  title: string;
  description: string;
  accent?: "blue" | "green" | "yellow" | "red";
  id?: string;
}

const accentMap = {
  blue: {
    border: "hover:border-blue-500/30",
    glow: "hover:shadow-blue-500/10",
    icon: "bg-blue-500/10 text-blue-400",
    ring: "group-hover:ring-blue-500/20",
  },
  green: {
    border: "hover:border-green-500/30",
    glow: "hover:shadow-green-500/10",
    icon: "bg-green-500/10 text-green-400",
    ring: "group-hover:ring-green-500/20",
  },
  yellow: {
    border: "hover:border-yellow-500/30",
    glow: "hover:shadow-yellow-500/10",
    icon: "bg-yellow-500/10 text-yellow-400",
    ring: "group-hover:ring-yellow-500/20",
  },
  red: {
    border: "hover:border-red-500/30",
    glow: "hover:shadow-red-500/10",
    icon: "bg-red-500/10 text-red-400",
    ring: "group-hover:ring-red-500/20",
  },
};

export default function FeatureCard({
  icon,
  title,
  description,
  accent = "blue",
  id,
}: FeatureCardProps) {
  const colors = accentMap[accent];

  return (
    <div
      id={id}
      className={`group relative p-6 rounded-xl border border-[#1e2530] bg-[#111318] transition-all duration-300 ${colors.border} hover:shadow-lg ${colors.glow}`}
    >
      {/* Subtle top line accent on hover */}
      <div
        className={`absolute inset-x-0 top-0 h-px rounded-full opacity-0 group-hover:opacity-100 transition-opacity duration-300 ${
          accent === "blue"
            ? "bg-gradient-to-r from-transparent via-blue-500/50 to-transparent"
            : accent === "green"
            ? "bg-gradient-to-r from-transparent via-green-500/50 to-transparent"
            : accent === "yellow"
            ? "bg-gradient-to-r from-transparent via-yellow-500/50 to-transparent"
            : "bg-gradient-to-r from-transparent via-red-500/50 to-transparent"
        }`}
      />

      <div
        className={`inline-flex items-center justify-center w-10 h-10 rounded-lg ${colors.icon} mb-4 transition-transform duration-200 group-hover:scale-110`}
      >
        {icon}
      </div>

      <h3 className="text-[#e2e8f0] font-semibold text-base mb-2">{title}</h3>
      <p className="text-[#8b9ab3] text-sm leading-relaxed">{description}</p>
    </div>
  );
}
