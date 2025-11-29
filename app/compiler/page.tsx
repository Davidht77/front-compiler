"use client"

import { useState } from "react"
import { Zap, Copy, RotateCcw } from "lucide-react"
import { Button } from "@/components/ui/button"
import CodeEditor from "@/components/compiler/code-editor"
import X86Output from "@/components/compiler/x86-output"
import StackVisualizer from "@/components/compiler/stack-visualizer"
import X86Executor from "@/components/compiler/x86-executor"

export default function CompilerPage() {
  const [kotlinCode, setKotlinCode] = useState(`fun main() {
  val x = 42
  val y = x + 8
  println(y)
}`)
  const [x86Output, setX86Output] = useState("")
  const [stackState, setStackState] = useState<Array<{ register: string; value: string; type: string }>>([])
  const [isCompiling, setIsCompiling] = useState(false)
  const [activeTab, setActiveTab] = useState("compiler")
  const [executionSteps, setExecutionSteps] = useState<any[]>([])
  const [programOutput, setProgramOutput] = useState("")

  const handleCompile = async () => {
    setIsCompiling(true)
    try {
      const response = await fetch("/api/compile", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ code: kotlinCode }),
      })

      if (!response.ok) throw new Error("Compilation failed")

      const data = await response.json()
      setX86Output(data.x86 || data.assembly || "")
      setStackState(data.stackState || data.stack_state || [])
      const steps = data.steps || data.execution_steps || []
      setExecutionSteps(steps.length ? steps : data.stdout ? [{ instruction: "program output", registers: {}, output: data.stdout }] : [])
      setProgramOutput(data.stdout || data.execution_output || "")
    } catch (error) {
      console.error("Compilation error:", error)
      setX86Output("// Compilation error occurred")
    } finally {
      setIsCompiling(false)
    }
  }

  const handleReset = () => {
    setX86Output("")
    setStackState([])
    setExecutionSteps([])
    setProgramOutput("")
  }

  return (
    <div className="flex flex-col h-full bg-black text-white overflow-hidden">
      {/* Header */}
      <div className="h-12 bg-neutral-900 border-b border-orange-500/20 flex items-center justify-between px-6">
        <div className="flex items-center gap-3">
          <Zap className="w-5 h-5 text-orange-500" />
          <span className="text-sm font-mono text-orange-500">KOTLIN → X86 COMPILER</span>
        </div>
        <div className="flex items-center gap-3">
          <Button
            onClick={handleReset}
            variant="outline"
            className="border-neutral-600 text-neutral-400 hover:text-orange-500 hover:border-orange-500 text-sm gap-2 bg-transparent"
          >
            <RotateCcw className="w-4 h-4" />
            RESET
          </Button>
          <Button
            onClick={handleCompile}
            disabled={isCompiling}
            className="bg-orange-500 hover:bg-orange-600 text-black font-bold text-sm gap-2"
          >
            <Zap className="w-4 h-4" />
            {isCompiling ? "COMPILING..." : "COMPILE"}
          </Button>
        </div>
      </div>

      {/* Tab Navigation */}
      <div className="h-10 bg-neutral-900 border-b border-neutral-700 flex items-center px-6 gap-6">
        {[
          { id: "compiler", label: "COMPILER PHASE" },
          { id: "executor", label: "EXECUTION ENGINE" },
        ].map((tab) => (
          <button
            key={tab.id}
            onClick={() => setActiveTab(tab.id)}
            className={`text-xs font-mono transition-colors ${
              activeTab === tab.id
                ? "text-orange-500 border-b-2 border-orange-500 pb-2"
                : "text-neutral-500 hover:text-neutral-400"
            }`}
          >
            {tab.label}
          </button>
        ))}
      </div>

      {/* Main Content */}
      {activeTab === "compiler" && (
        <div className="flex-1 flex gap-6 p-6 overflow-hidden bg-black">
          {/* Left: Kotlin Code Editor */}
          <div className="flex-1 flex flex-col min-w-0 border-2 border-orange-500/60 rounded-lg bg-neutral-950 shadow-lg shadow-orange-500/20">
            <div className="h-12 bg-gradient-to-r from-neutral-800 to-neutral-900 border-b-2 border-orange-500/60 flex items-center px-4">
              <span className="text-sm font-mono text-orange-400 font-bold tracking-wider">▌ SOURCE CODE (KOTLIN)</span>
            </div>
            <CodeEditor value={kotlinCode} onChange={setKotlinCode} />
          </div>

          {/* Right: X86 Output + Stack + Output */}
          <div className="flex-1 flex flex-col gap-6 min-w-0">
            {/* X86 Assembly */}
            <div className="flex-1 flex flex-col min-w-0 border-2 border-cyan-500/60 rounded-lg bg-neutral-950 shadow-lg shadow-cyan-500/20 overflow-hidden">
              <div className="h-12 bg-gradient-to-r from-neutral-800 to-neutral-900 border-b-2 border-cyan-500/60 flex items-center justify-between px-4">
                <span className="text-sm font-mono text-cyan-400 font-bold tracking-wider">
                  ▌ ASSEMBLY OUTPUT (X86)
                </span>
                <Button
                  variant="ghost"
                  size="sm"
                  className="h-6 w-6 p-0 text-cyan-400 hover:text-cyan-300 hover:bg-cyan-500/20"
                  onClick={() => x86Output && navigator.clipboard.writeText(x86Output)}
                >
                  <Copy className="w-4 h-4" />
                </Button>
              </div>
              <X86Output code={x86Output} />
            </div>

            {/* Stack Visualizer */}
            <div className="flex-1 flex flex-col min-w-0 border-2 border-green-500/60 rounded-lg bg-neutral-950 shadow-lg shadow-green-500/20 overflow-hidden">
              <div className="h-12 bg-gradient-to-r from-neutral-800 to-neutral-900 border-b-2 border-green-500/60 flex items-center px-4">
                <span className="text-sm font-mono text-green-400 font-bold tracking-wider">▌ MEMORY STACK</span>
              </div>
              <StackVisualizer
                stackState={stackState}
                frames={executionSteps.map((step) =>
                  Object.entries(step.registers || {}).map(([reg, value]) => ({
                    register: reg,
                    value: value,
                    type: "reg",
                  }))
                )}
              />
            </div>

            {/* Program Output */}
            <div className="flex-1 flex flex-col min-w-0 border-2 border-orange-500/60 rounded-lg bg-neutral-950 shadow-lg shadow-orange-500/20 overflow-hidden">
              <div className="h-12 bg-gradient-to-r from-neutral-800 to-neutral-900 border-b-2 border-orange-500/60 flex items-center px-4">
                <span className="text-sm font-mono text-orange-400 font-bold tracking-wider">�-O PROGRAM OUTPUT</span>
              </div>
              <div className="p-4 font-mono text-sm text-neutral-200 whitespace-pre-wrap min-h-[80px]">
                {programOutput || "Sin salida aún"}
              </div>
            </div>
          </div>
        </div>
      )}

      {activeTab === "executor" && (
        <div className="flex-1 overflow-auto p-4">
          <X86Executor x86Code={x86Output} executionSteps={executionSteps} />
        </div>
      )}
    </div>
  )
}
