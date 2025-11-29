"use client"

import React from "react"

interface StackEntry {
  register: string
  value: string
  type: string
}

interface StackVisualizerProps {
  stackState: StackEntry[]
  frames?: StackEntry[][]
  autoplay?: boolean
  intervalMs?: number
}

export default function StackVisualizer({ stackState, frames, autoplay = true, intervalMs = 800 }: StackVisualizerProps) {
  const timeline = frames && frames.length ? frames : stackState.length ? [stackState] : []
  const [frameIdx, setFrameIdx] = React.useState(0)

  React.useEffect(() => {
    if (!autoplay || timeline.length <= 1) return
    const id = setInterval(() => {
      setFrameIdx((idx) => (idx + 1) % timeline.length)
    }, intervalMs)
    return () => clearInterval(id)
  }, [autoplay, intervalMs, timeline.length])

  const current = timeline[frameIdx] || []

  if (timeline.length === 0) {
    return (
      <div className="flex-1 flex items-center justify-center text-neutral-500 text-sm font-mono">
        <div className="text-center">
          <div className="mb-2">⚡ STACK EMPTY</div>
          <div className="text-xs text-neutral-600">Execute code to see memory state</div>
        </div>
      </div>
    )
  }

  return (
    <div className="flex-1 overflow-auto p-4 space-y-3">
      <div className="flex items-center justify-between text-xs text-neutral-500 font-mono">
        <span>
          Frame {frameIdx + 1} / {timeline.length}
        </span>
        {timeline.length > 1 && <span className="text-orange-400 animate-pulse">Reproduciendo pila...</span>}
      </div>
      <div className="space-y-2 transition-all duration-300">
        {current.map((entry, idx) => (
          <div
            key={idx}
            className="flex items-center gap-4 p-3 bg-neutral-800 rounded border border-orange-500/20 transition-transform duration-300"
          >
            <div className="min-w-fit font-mono text-orange-500 text-sm font-bold">{entry.register}</div>
            <div className="flex-1 font-mono text-xs">
              <span className="text-green-400">{entry.value}</span>
              <span className="text-neutral-500 ml-2">({entry.type})</span>
            </div>
            <div className="text-xs text-neutral-600 min-w-fit">{`0x${Math.random().toString(16).slice(2, 10).toUpperCase()}`}</div>
          </div>
        ))}
      </div>
    </div>
  )
}
