## Optimizaciones en la generación de código

### Plegado de constantes
- En el ctor de `BinaryExp`: si ambos hijos son constantes, `isnumber=true` y `valor` guarda el resultado (aritmética, comparaciones, lógicas).
- El codegen detecta `isnumber` y emite un solo `mov` inmediato.
- Ejemplo: `inputs/input16.txt` (`2 + 3 * 4`) se resuelve en compilación y solo imprime 14.

### Eliminación de código muerto (`if`)
- En `visit(IfStmt)`, si la condición es constante:
  - Distinta de cero: solo se emite el bloque `then`.
  - Cero: solo el `else` (si existe).
- Ejemplo: `inputs/input17.txt` mantiene solo la rama `println(42)`.

### Orden Sethi-Ullman
- Cada `Exp` lleva `etiqueta`; `BinaryExp` la calcula desde sus hijos.
- El codegen visita primero el subárbol más pesado para reducir `push/pop`; intercambia operandos para dejar `RAX`=izq, `RCX`=der.
- Beneficia expresiones como `inputs/input18.txt` con productos y sumas encadenadas.

### Eliminación de funciones no usadas
- En `visit(Program)`, se recolectan llamadas alcanzables desde `main`; solo esas funciones se emiten en ensamblador.
- Ejemplo: `inputs/input9.txt` omite la función `unused`.

### Otras mejoras
- Pool de literales de string (`stringLiterals`).
- Loads/stores conscientes del tamaño evitan truncar bytes/shorts.
- Operaciones `Float/Double` en registros XMM; enteros usan el ancho mínimo necesario.
