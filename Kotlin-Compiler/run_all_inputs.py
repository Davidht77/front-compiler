import os
import subprocess
import shutil

# Archivos c++
programa = ["main.cpp", "scanner.cpp", "token.cpp", "parser.cpp", "ast.cpp", "visitor.cpp", "TypeChecker.cpp"]
scanner_test = ["test_scanner.cpp", "scanner.cpp", "token.cpp"]

# Compilar Main
compile = ["g++", "-o", "main.exe"] + programa
print("Compilando Main:", " ".join(compile))
result = subprocess.run(compile, capture_output=True, text=True)

if result.returncode != 0:
    print("Error en compilación Main:\n", result.stderr)
    exit(1)

# Compilar Scanner Test
compile_scanner = ["g++", "-o", "scanner_test.exe"] + scanner_test
print("Compilando Scanner Test:", " ".join(compile_scanner))
result_scanner = subprocess.run(compile_scanner, capture_output=True, text=True)

if result_scanner.returncode != 0:
    print("Error en compilación Scanner Test:\n", result_scanner.stderr)
    exit(1)

print("Compilación exitosa")

# Ejecutar
input_dir = "inputs"
output_dir = "outputs"
os.makedirs(output_dir, exist_ok=True)

for i in range(1, 19):
    filename = f"input{i}.txt"
    filepath = os.path.join(input_dir, filename)

    if os.path.isfile(filepath):
        print(f"Ejecutando {filename}")
        
        # Ejecutar Main
        run_cmd = ["./main.exe", filepath]
        result_run = subprocess.run(run_cmd, capture_output=True, text=True)
        
        # Archivos generados por Main
        tokens_file = os.path.join(input_dir, f"input{i}.s")
        if os.path.isfile(tokens_file):
            dest_tokens = os.path.join(output_dir, f"input_{i}.s")
            shutil.move(tokens_file, dest_tokens)

        # Ejecutar Scanner Test
        print(f"Ejecutando Scanner para {filename}")
        run_scanner_cmd = ["./scanner_test.exe", filepath]
        subprocess.run(run_scanner_cmd, capture_output=True, text=True)

        # Archivos generados por Scanner
        scanner_out_file = os.path.join(input_dir, f"input{i}_tokens.txt")
        if os.path.isfile(scanner_out_file):
            dest_scanner = os.path.join(output_dir, f"input_{i}_tokens.txt")
            shutil.move(scanner_out_file, dest_scanner)

    else:
        print(filename, "no encontrado en", input_dir)
