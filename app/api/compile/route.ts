import { NextResponse } from "next/server"
import { promises as fs } from "fs"
import path from "path"
import os from "os"
import { spawn } from "child_process"

function toWslPath(winPath: string) {
  const normalized = winPath.replace(/\\/g, "/")
  const match = normalized.match(/^([a-zA-Z]):\/(.*)$/)
  if (!match) return normalized
  const drive = match[1].toLowerCase()
  const rest = match[2]
  return `/mnt/${drive}/${rest}`
}

async function runCompiler(source: string) {
  const projectRoot = path.resolve(process.cwd(), "Kotlin-Compiler")
  const tmpDir = await fs.mkdtemp(path.join(projectRoot, "tmp-"))
  const inputPath = path.join(tmpDir, "input.kt")
  const asmPath = path.join(tmpDir, "input.s")

  await fs.writeFile(inputPath, source, "utf8")

  const isWin = process.platform === "win32"
  const compilerPath = path.join(projectRoot, "main.exe")

  await new Promise<void>((resolve, reject) => {
    let cmd: string
    let args: string[]
    let options = {}

    if (isWin) {
      const wslProject = toWslPath(projectRoot)
      const wslInput = toWslPath(inputPath)
      cmd = "wsl"
      args = ["bash", "-lc", `cd "${wslProject}" && ./main.exe "${wslInput}"`]
    } else {
      cmd = compilerPath
      args = [inputPath]
      options = { cwd: projectRoot }
    }

    const proc = spawn(cmd, args, options)
    let stderr = ""
    proc.stderr.on("data", (d) => (stderr += d.toString()))
    proc.on("error", reject)
    proc.on("close", (code) => {
      if (code !== 0) reject(new Error(stderr || `Compiler exited with code ${code}`))
      else resolve()
    })
  })

  const assembly = await fs.readFile(asmPath, "utf8").catch(() => "")

  return { assembly, asmPath, projectRoot, tmpDir }
}

async function runAssembly(asmPath: string, projectRoot: string) {
  const isWin = process.platform === "win32"
  const exePath = asmPath.replace(/\.s$/, ".out")

  return await new Promise<{ stdout: string }>((resolve, reject) => {
    let cmd: string
    let args: string[]
    let options = {}

    if (isWin) {
      const wslAsm = toWslPath(asmPath)
      const wslExe = toWslPath(exePath)
      cmd = "wsl"
      args = ["bash", "-lc", `g++ "${wslAsm}" -no-pie -o "${wslExe}" && "${wslExe}"`]
    } else {
      cmd = "bash"
      args = ["-lc", `g++ "${asmPath}" -no-pie -o "${exePath}" && "${exePath}"`]
      options = { cwd: projectRoot }
    }

    const proc = spawn(cmd, args, options)
    let stderr = ""
    let stdout = ""
    proc.stderr.on("data", (d) => (stderr += d.toString()))
    proc.stdout.on("data", (d) => (stdout += d.toString()))
    proc.on("error", reject)
    proc.on("close", (code) => {
      if (code !== 0) reject(new Error(stderr || `Runner exited with code ${code}`))
      else resolve({ stdout })
    })
  })
}

export async function POST(request: Request) {
  try {
    const body = await request.json()
    const source: string = body.kotlin_code || body.code || ""
    if (!source.trim()) {
      return NextResponse.json({ error: "Código vacío" }, { status: 400 })
    }

    const { assembly, asmPath, projectRoot, tmpDir } = await runCompiler(source)
    let execution_output = ""
    try {
      const result = await runAssembly(asmPath, projectRoot)
      execution_output = result.stdout.trim()
    } catch (e) {
      execution_output = ""
    }

    fs.rm(tmpDir, { recursive: true, force: true }).catch(() => {})

    return NextResponse.json({
      assembly,
      x86: assembly,
      stdout: execution_output,
      execution_output,
      stack_state: [],
      execution_steps: execution_output
        ? [{ instruction: "program output", registers: {}, output: execution_output }]
        : [],
      steps: execution_output ? [{ instruction: "program output", registers: {}, output: execution_output }] : [],
      stackState: [],
    })
  } catch (error: any) {
    return NextResponse.json({ error: error?.message || "Error de compilación" }, { status: 500 })
  }
}
