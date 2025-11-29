"use client"

import { useState } from "react"
import { Play, StepForward } from "lucide-react"
import { Button } from "@/components/ui/button"

interface ExecutionStep {
  instruction: string
  registers: Record<string, string>
  output?: string
}

interface X86ExecutorProps {
  x86Code: string
  executionSteps: ExecutionStep[]
}

export default function X86Executor({ x86Code, executionSteps }: X86ExecutorProps) {
  const [currentStep, setCurrentStep] = useState(0)
  const [isRunning, setIsRunning] = useState(false)

  const handleStep = () => {
    if (currentStep < executionSteps.length) {
      setCurrentStep(currentStep + 1)
    }
  }

  const handleRun = () => {
    setIsRunning(!isRunning)
  }

  if (!x86Code) {
    return (
      <div className="flex items-center justify-center h-full text-neutral-500 text-sm font-mono">
        <div className="text-center">
          <div className="mb-2">▌ NO X86 CODE</div>
          <div className="text-xs text-neutral-600">Compile Kotlin code first</div>
        </div>
      </div>
    )
  }

  const currentExecution = executionSteps[currentStep] || {}

  return (
    <div className="max-w-4xl mx-auto">
      {/* Controls */}
      <div className="flex gap-3 mb-6">
        <Button onClick={handleRun} className="bg-orange-500 hover:bg-orange-600 text-black font-bold gap-2">
          <Play className="w-4 h-4" />
          {isRunning ? "PAUSE" : "EXECUTE"}
        </Button>
        <Button
          onClick={handleStep}
          variant="outline"
          className="border-orange-500 text-orange-500 hover:bg-orange-500/10 gap-2 bg-transparent"
        >
          <StepForward className="w-4 h-4" />
          STEP
        </Button>
        <div className="ml-auto text-sm font-mono text-neutral-500">
          Step {currentStep} / {executionSteps.length}
        </div>
      </div>

      {/* Execution Display */}
      <div className="grid grid-cols-2 gap-4">
        {/* Current Instruction */}
        <div className="border border-orange-500/20 rounded bg-neutral-900/50 p-4">
          <div className="text-xs font-mono text-orange-500 mb-3">CURRENT INSTRUCTION</div>
          <div className="p-3 bg-neutral-800 rounded font-mono text-sm text-green-400">
            {currentExecution.instruction || "awaiting execution..."}
          </div>
        </div>

        {/* Register State */}
        <div className="border border-orange-500/20 rounded bg-neutral-900/50 p-4">
          <div className="text-xs font-mono text-orange-500 mb-3">REGISTER STATE</div>
          <div className="space-y-2">
            {Object.entries(currentExecution.registers || {}).map(([reg, value]) => (
              <div key={reg} className="flex justify-between text-xs font-mono">
                <span className="text-cyan-400">{reg}</span>
                <span className="text-green-400">{value}</span>
              </div>
            ))}
          </div>
        </div>
      </div>

      {/* Output */}
      {currentExecution.output && (
        <div className="mt-4 border border-orange-500/20 rounded bg-neutral-900/50 p-4">
          <div className="text-xs font-mono text-orange-500 mb-3">STDOUT</div>
          <pre className="font-mono text-sm text-green-400">{currentExecution.output}</pre>
        </div>
      )}
    </div>
  )
}
