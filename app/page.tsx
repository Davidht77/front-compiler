"use client"

import { useState } from "react"
import { Zap } from "lucide-react"
import { Button } from "@/components/ui/button"
import CodeEditor from "@/components/compiler/code-editor"
import X86Output from "@/components/compiler/x86-output"
import StackVisualizer from "@/components/compiler/stack-visualizer"
import X86Executor from "@/components/compiler/x86-executor"

interface CompilationResult {
  assembly?: string
  x86?: string
  stack_state?: Array<{
    register: string
    value: string
    type: string
  }>
  stackState?: Array<{
    register: string
    value: string
    type: string
  }>
  execution_steps?: Array<{
    instruction: string
    registers: Record<string, string>
  }>
  steps?: Array<{
    instruction: string
    registers: Record<string, string>
  }>
  stdout?: string
  execution_output?: string
}

export default function CompilerPage() {
  const [kotlinCode, setKotlinCode] = useState(`fun add(a: Int, b: Int): Int {
    return a + b
}

fun main() {
    val result = add(5, 3)
    println(result)
}`)

  const [compilationResult, setCompilationResult] = useState<CompilationResult | null>(null)
  const [loading, setLoading] = useState(false)
  const [activeTab, setActiveTab] = useState<"compiler" | "executor">("compiler")
  const handleCompile = async () => {
    setLoading(true)
    try {
      const response = await fetch("/api/compile", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ kotlin_code: kotlinCode }),
      })

      if (!response.ok) throw new Error("Compilación fallida")

      const data = await response.json()
      setCompilationResult(data)
    } catch (error) {
      console.error("Compilation error:", error)
    } finally {
      setLoading(false)
    }
  }

  return (
    <div className="min-h-screen p-6 bg-black text-white">
      {/* Header */}
      <div className="mb-8">
        <div className="flex items-center gap-3 mb-4">
          <Zap className="w-8 h-8 text-orange-500" />
          <h1 className="text-3xl font-bold text-orange-500 tracking-wider">KOTLIN COMPILER</h1>
        </div>
        <p className="text-neutral-400 text-sm">Visualiza Kotlin → x86 con seguimiento de pila</p>
      </div>

      {/* Tabs + acciones */}
      <div className="flex items-center justify-between gap-4 mb-6">
        <div className="flex gap-2">
          {[
            { id: "compiler", label: "COMPILER PHASE" },
            { id: "executor", label: "EXECUTION ENGINE" },
          ].map((tab) => (
            <button
              key={tab.id}
              onClick={() => setActiveTab(tab.id as typeof activeTab)}
              className={`px-4 py-2 border transition-colors ${
                activeTab === tab.id
                  ? "border-orange-500 text-orange-500 bg-orange-500/10"
                  : "border-neutral-700 text-neutral-400 hover:text-white hover:border-neutral-500"
              }`}
            >
              {tab.label}
            </button>
          ))}
        </div>
        <div className="flex items-center gap-3">
          <Button
            onClick={handleCompile}
            disabled={loading}
            className="bg-orange-500 hover:bg-orange-600 text-black font-bold px-6"
          >
            {loading ? "COMPILING..." : "COMPILE"}
          </Button>
        </div>
      </div>

      {activeTab === "compiler" && (
        <div className="space-y-6">
          {/* Main Layout */}
          <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
            {/* Left: Code Editor */}
            <div className="lg:row-span-2">
              <div className="border border-orange-500/50 rounded bg-black">
                <div className="px-4 py-3 border-b border-orange-500/50 bg-orange-500/10 flex items-center gap-2">
                  <div className="w-2 h-2 bg-orange-500 rounded-full"></div>
                  <span className="text-sm font-mono text-orange-500">SOURCE.KT</span>
                </div>
                <CodeEditor value={kotlinCode} onChange={setKotlinCode} />
              </div>
            </div>

            {/* Right: x86 Output */}
            <div>
              <div className="border border-orange-500/50 rounded bg-black h-full">
                <div className="px-4 py-3 border-b border-orange-500/50 bg-orange-500/10 flex items-center justify-between">
                  <div className="flex items-center gap-2">
                    <div className="w-2 h-2 bg-orange-500 rounded-full"></div>
                    <span className="text-sm font-mono text-orange-500">x86 OUTPUT</span>
                  </div>
                </div>
                <X86Output code={compilationResult?.assembly || compilationResult?.x86 || ""} />
              </div>
            </div>

            {/* Right: Stack Visualizer */}
            <div>
              <div className="border border-orange-500/50 rounded bg-black h-full">
                <div className="px-4 py-3 border-b border-orange-500/50 bg-orange-500/10 flex items-center gap-2">
                  <div className="w-2 h-2 bg-orange-500 rounded-full"></div>
                  <span className="text-sm font-mono text-orange-500">STACK MEMORY</span>
                </div>
                <StackVisualizer
                  stackState={compilationResult?.stack_state || compilationResult?.stackState || []}
                  frames={(compilationResult?.execution_steps || compilationResult?.steps || []).map((step) =>
                    Object.entries(step.registers || {}).map(([reg, value]) => ({
                      register: reg,
                      value: value,
                      type: "reg",
                    }))
                  )}
                />
              </div>
            </div>

            {/* Right: Program Output */}
            <div>
              <div className="border border-orange-500/50 rounded bg-black h-full">
                <div className="px-4 py-3 border-b border-orange-500/50 bg-orange-500/10 flex items-center gap-2">
                  <div className="w-2 h-2 bg-orange-500 rounded-full"></div>
                  <span className="text-sm font-mono text-orange-500">PROGRAM OUTPUT</span>
                </div>
                <div className="p-4 text-sm font-mono text-neutral-200 whitespace-pre-wrap min-h-[120px]">
                  {compilationResult?.stdout || compilationResult?.execution_output || "Sin salida aún"}
                </div>
              </div>
            </div>
          </div>
        </div>
      )}

      {activeTab === "executor" && compilationResult && (
        <div className="border border-orange-500/50 rounded bg-black">
          <div className="px-4 py-3 border-b border-orange-500/50 bg-orange-500/10 flex items-center gap-2">
            <div className="w-2 h-2 bg-orange-500 rounded-full"></div>
            <span className="text-sm font-mono text-orange-500">x86 EXECUTOR</span>
          </div>
          <X86Executor executionSteps={compilationResult.execution_steps || compilationResult.steps || []} />
        </div>
      )}

      {activeTab === "executor" && !compilationResult && (
        <div className="border border-orange-500/50 rounded bg-black p-8 text-center">
          <p className="text-neutral-400">Compile code first to enable the execution engine</p>
        </div>
      )}
    </div>
  )
}
