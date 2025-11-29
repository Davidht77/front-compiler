## Promoción de tipos y operaciones numéricas

### Reglas de promoción en el verificador de tipos
- Aritmética (`+ - * / % **`) exige ambos operandos numéricos.
- Tipo de resultado:
  - Si algún lado es `Double` → resultado `Double`.
  - Si no, pero algún lado es `Float` → resultado `Float`.
  - En enteros, se toma el más ancho entre `Byte/UByte`, `Short/UShort`, `Int/UInt`, `Long/ULong` (ver `TypeChecker::visit(BinaryExp)`).
- Comparaciones permiten numéricos o `bool`; el resultado es `bool`.
- Operadores lógicos requieren `bool`.
- `..`, `downTo`, `step` requieren numéricos.

### Conversiones estilo método
- Llamadas como `x.toByte()/toShort()/toInt()/toLong()/toFloat()/toDouble()/toUByte()/toUShort()/toUInt()/toULong()` se parsean como llamadas con receptor.
- El type checker las acepta si el receptor es numérico y asigna el tipo destino correspondiente.

### Tamaños usados en codegen
- `getTypeSize`: `Byte/UByte/Bool` → 1, `Short/UShort` → 2, `Int/UInt/Float` → 4, `Long/ULong/Double` → 8.
- Sufijos de instrucciones y load/store usan ese tamaño para no truncar.

### Conversiones en tiempo de generación
- `convertValueTo(src,dst)` (en `visitor.cpp`) normaliza el valor en `RAX` al tipo destino:
  - Entero → `Float/Double` con `cvtsi2ssq` / `cvtsi2sdq`.
  - `Float ↔ Double` con `cvtss2sd` / `cvtsd2ss`.
  - `Float/Double` → entero con `cvttss2siq` / `cvttsd2siq`, luego se extiende al ancho destino.
- Inits y asignaciones llaman a `convertValueTo` antes de guardar.
- Loads de identificadores sign-extienden o zero-extienden según signo y tamaño; `Float/Double` se cargan en XMM preservando bits.

### Operaciones con flotantes
- Si algún operando es `Float/Double`, ambos se llevan a XMM; enteros se suben con `cvtsi2sdq`, `Float` se promueve a `Double`. Se usan `addsd/subsd/mulsd/divsd`; si el tipo resultado es `Float`, se aplica `cvtsd2ss` al final.

### Ejemplos
- `100.toByte() + 1000.toShort()` → resultado `Int`, se almacenan como `Byte` y `Short`, se cargan con extensión y se suman en 32 bits (ver `inputs/input14.txt`, salida 1100).
- `val op1 = 5 + 2.5F` → promoción a `Float`; `op1` se imprime con formato `%f` (ver `inputs/input15.txt`, salida 7.5).
- `val division = double / float` en `inputs/input6.txt` convierte `Float` a `Double` antes de dividir, produciendo `1.000001`.
