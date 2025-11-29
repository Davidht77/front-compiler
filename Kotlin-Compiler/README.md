## Gramática

Program         ::= VarDecList FunDecList

VarDecList      ::= (VarDec)*

FunDecList      ::= (FunDec)+

FunDec          ::= "fun" id "(" ParamListOpt ")" TypeAnnotationOpt Block

ParamListOpt    ::= (ParamDec ("," ParamDec)*) | ε

ParamDec        ::= VarSymbolParam id TypeAnnotationOpt

VarDec          ::= VarSymbol id TypeAnnotationOpt InitializerOpt StmtTerminator

VarSymbol       ::= ("const" | ε) ("val" | "var")

VarSymbolParam  ::= ("const" | ε) ("val" | "var" | ε)

TypeAnnotationOpt ::= ":" Type | ε

Type            ::= id

InitializerOpt  ::= "=" Exp | ε

Block           ::= "{" StmtListOpt "}"

StmtList        ::= (Stmt StmtTerminator)*

StmtListOpt     ::= StmtList | ε

Stmt            ::= VarDec 
                 | Exp 
                 | PrintStmt 
                 | IfStmt 
                 | WhileStmt 
                 | ForStmt 
                 | ReturnStmt

PrintStmt       ::= ("println"|"print") "(" ExpOpt ")"

IfStmt          ::= "if" "(" Exp ")" Block ElseOpt

ElseOpt         ::= "else" Block | ε

WhileStmt       ::= "while" "(" Exp ")" Block

ForStmt         ::= "for" "(" id "in" Exp ")" Block

ReturnStmt      ::= "return" ExpOpt

ExpOpt          ::= Exp | ε

Exp             ::= Assignment

Assignment      ::= id "=" Assignment | LogicOr

LogicOr         ::= LogicAnd ("||" LogicAnd)*

LogicAnd        ::= Equality ("&&" Equality)*

Equality        ::= Relational (("=="|"!=") Relational)*

Relational      ::= Additive (("<"|">"|"<="|">=") Additive)*

Additive        ::= Multiplicative (("+"|"-") Multiplicative)*

Multiplicative  ::= Unary (("*"|"/"|"%") Unary)*

Unary           ::= ("+"|"-"|"!") Unary | Primary

Primary         ::= Atom Postfix*

Atom            ::= id
                 | Num
                 | Bool
                 | String
                 | "(" Exp ")"

Postfix         ::= "(" ArgListOpt ")"        // llamada tipo f(...)
                 | "." id CallArgsOpt         // llamada con receptor e.g. 100.toByte()

CallArgsOpt     ::= "(" ArgListOpt ")" | ε

ArgListOpt      ::= (Exp ("," Exp)*) | ε

StmtTerminator  ::= ";" | Newline

## Extensiones del lenguaje implementadas

- Conversión y promoción de tipos (.toString(), .toInt(), .toDouble(), .toLong(), .toByte(), etc)
- Inferencia de tipos (var x = 5; var y = 5.0; var z = true;)
- Tipos numericos (float, double, int, long, unsigned int, unsigned long)

## Optimizaciones implementadas (Al menos 3)

- Plegado de constantes (constant folding) en el AST y generación de código.
- Eliminación de ramas muertas cuando la condición es constante.
- Orden de evaluación con Sethi-Ullman para reducir uso de registros/pila.
- No se emite ensamblador para funciones no alcanzables desde `main` (eliminación de funciones no usadas).

## Casos de prueba

- 3 casos de funciones.
- 5 casos para la implementación base.
- 5 casos para las extensiones.
- 5 casos para optimización.
