## Inferencia de tipos y conversiones

### Dónde se infiere
- **Declaración de variables**: si no hay anotación (`VarDec.type` vacío), el tipo se toma del inicializador (`visit(VarDec)`).
- **Retorno de funciones**: si no hay tipo declarado, `add_function` usa `inferReturnType` para recorrer el cuerpo y deducir el tipo de `return`; si no hay, se asume `void`.
- **Expresiones**: cada nodo guarda `inferredType` tras el chequeo para dimensionar registros en codegen.

### Literales numéricos
- Se aceptan parte fraccionaria y sufijos `f/F` o `l/L`. El scanner quita el sufijo del lexema; el parser decide `Double/Int` vs `Float` por la presencia de punto o sufijo.

### Conversiones como método
- Se parsean como llamada con receptor (ej. `100.toByte()`), el type checker valida que el receptor sea numérico y aplica el tipo destino del método.
- En codegen:
  - Se evalúa el receptor en `RAX`.
  - `convertValueTo` normaliza al tipo pedido (ancho entero o float/double).
  - El valor convertido queda en `RAX`.

### Almacenes y cargas respetan el tipo
- Inits y asignaciones convierten al tipo de la variable antes de guardar.
- Cargas:
  - Enteros se extienden con signo o cero según tipo y ancho.
  - `Float` se carga con `movl` a `EAX`; `Double` con `movq` a `RAX`.
  - Globals usan `tiposGlobales`; locales usan `typeEnv`.

### Impresión
- `print/println` aceptan numéricos, bool o string.
- `Float` se promueve a double antes de `printf` para encajar con `%f`.

### Ejemplos
- `inputs/input14.txt`: `100.toByte()` y `1000.toShort()` se convierten y suman, resultando 1100.
- `inputs/input13.txt`: `42.toFloat()` y luego `toLong()` imprimen `42.0` y `42`.
- `inputs/input15.txt`: mezcla `Int + Float + Double.toFloat()` y luego `toInt()`, respetando las conversiones en cada paso.
