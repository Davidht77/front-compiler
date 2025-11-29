"use client"

import { useRef } from "react"

interface CodeEditorProps {
  value: string
  onChange: (value: string) => void
}

export default function CodeEditor({ value, onChange }: CodeEditorProps) {
  const textareaRef = useRef<HTMLTextAreaElement>(null)

  return (
    <div className="flex flex-1 overflow-hidden bg-black">
      {/* Line numbers */}
      <div className="bg-neutral-950 border-r-2 border-orange-500/40 flex flex-col text-right pr-3 pt-4 overflow-hidden select-none min-w-fit">
        {value.split("\n").map((_, i) => (
          <div key={i} className="text-xs font-mono text-orange-600/70 h-6 leading-6 font-bold w-12">
            {String(i + 1).padStart(3)}
          </div>
        ))}
      </div>

      {/* Code textarea */}
      <textarea
        ref={textareaRef}
        value={value}
        onChange={(e) => onChange(e.target.value)}
        className="flex-1 p-4 text-sm font-mono bg-black text-green-400 border-0 outline-none resize-none overflow-auto focus:ring-2 focus:ring-orange-500/50"
        spellCheck="false"
        style={{
          caretColor: "#f97316",
          lineHeight: "1.5rem",
          fontFamily: "monospace",
        }}
      />
    </div>
  )
}
