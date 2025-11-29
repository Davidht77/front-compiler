"use client"

interface X86OutputProps {
  code: string
}

export default function X86Output({ code }: X86OutputProps) {
  if (!code) {
    return (
      <div className="flex-1 flex items-center justify-center text-cyan-600/50 text-sm font-mono">
        <div className="text-center">
          <div className="mb-3 text-lg">█ AWAITING COMPILATION</div>
          <div className="text-xs text-cyan-600/40">Click COMPILE to generate x86 assembly output</div>
        </div>
      </div>
    )
  }

  const lines = code.split("\n")

  return (
    <div className="flex-1 overflow-auto">
      <pre className="p-5 text-xs font-mono bg-black">
        {lines.map((line, idx) => (
          <div key={idx} className="flex gap-4 hover:bg-cyan-500/5 transition-colors py-1">
            <span className="text-cyan-700/80 min-w-fit font-bold">{String(idx + 1).padStart(4)}</span>
            <span className="text-cyan-300 flex-1 font-mono">
              {line.split(/(\s+)/).map((part, i) => {
                if (/^\s+$/.test(part)) return part
                if (/^(mov|add|sub|xor|push|pop|call|ret|jmp|je|jne|cmp|lea|imul)$/.test(part)) {
                  return (
                    <span key={i} className="text-orange-400 font-bold">
                      {part}
                    </span>
                  )
                }
                if (/^\$/.test(part))
                  return (
                    <span key={i} className="text-yellow-300 font-bold">
                      {part}
                    </span>
                  )
                if (/^%/.test(part))
                  return (
                    <span key={i} className="text-green-300 font-bold">
                      {part}
                    </span>
                  )
                if (/^:/.test(part))
                  return (
                    <span key={i} className="text-purple-400">
                      {part}
                    </span>
                  )
                return part
              })}
            </span>
          </div>
        ))}
      </pre>
    </div>
  )
}
