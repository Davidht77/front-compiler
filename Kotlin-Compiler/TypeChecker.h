#ifndef TYPECHECKER_H
#define TYPECHECKER_H

#include <unordered_map>
#include <string>
#include "ast.h"
#include "environment.h"
#include "semantic_types.h"

using namespace std;

class BinaryExp;
class NumberExp;
class DoubleExp; // Agregado
class Program;
class PrintStm;
class AssignExp; // Cambiado desde AssignStm
class FunDec;
class ReturnStm;
class Block; // Cambiado desde Body
class VarDec;
class FcallExp;
class BoolExp;
class WhileStmt; // Agregado
class IfStmt;    // Agregado
class ForStmt;   // Agregado
class StringExp; // Agregado

class TypeVisitor {
public:

    // --- Nodos de nivel superior ---
    virtual Type* visit(Program* p) = 0;
    virtual Type* visit(Block* b) = 0; // Cambiado desde Body
    virtual Type* visit(VarDec* v) = 0;
    virtual Type* visit(FunDec* f) = 0;

    // --- Sentencias ---
    virtual Type* visit(PrintStm* stm) = 0;
    virtual Type* visit(AssignExp* stm) = 0; // Cambiado desde AssignStm
    virtual Type* visit(ReturnStm* stm) = 0;
    virtual Type* visit(WhileStmt* stm) = 0; // Agregado
    virtual Type* visit(IfStmt* stm) = 0;    // Agregado
    virtual Type* visit(ForStmt* stm) = 0;   // Agregado

    // --- Expresiones ---
    virtual Type* visit(BinaryExp* e) = 0;
    virtual Type* visit(NumberExp* e) = 0;
    virtual Type* visit(DoubleExp* e) = 0; // Agregado
    virtual Type* visit(IdExp* e) = 0;
    virtual Type* visit(BoolExp* e) = 0;
    virtual Type* visit(FcallExp* e) = 0;
    virtual Type* visit(StringExp* e) = 0; // Agregado
};



// ──────────────────────────────────────────────
//   CLASE TYPECHECKER
// ──────────────────────────────────────────────

class TypeChecker : public TypeVisitor {
private:
    Environment<Type*> env;                 // Entorno de variables y sus tipos
    unordered_map<string, Type*> functions; // Entorno de funciones

    // Tipos básicos
    Type* intType;
    Type* boolType;
    Type* voidType;
    Type* stringType; // Agregado
    Type* rangeType; // Agregado
    Type* retornodefuncion;
    // Registro de funciones
    void add_function(FunDec* fd);
    
    // Helper for return type inference
    Type* inferReturnType(Stm* s);

    // Variable counting
    string currentFunction;
    int currentVarCount;

public:
    unordered_map<string, int> functionVarCounts; // Public to access from main

    TypeChecker();

    // Método principal de verificación
    void typecheck(Program* program);

    // --- Visitas de alto nivel ---
    Type* visit(Program* p) override;
    Type* visit(Block* b) override; // Changed from Body
    Type* visit(VarDec* v) override;
    Type* visit(FunDec* f) override;

    // --- Sentencias ---
    Type* visit(PrintStm* stm) override;
    Type* visit(AssignExp* stm) override; // Changed from AssignStm
    Type* visit(ReturnStm* stm) override;
    Type* visit(WhileStmt* stm) override; // Agregado
    Type* visit(IfStmt* stm) override;    // Agregado
    Type* visit(ForStmt* stm) override;   // Agregado

    // --- Expresiones ---
    Type* visit(BinaryExp* e) override;
    Type* visit(NumberExp* e) override;
    Type* visit(DoubleExp* e) override; // Agregado
    Type* visit(IdExp* e) override;
    Type* visit(BoolExp* e) override;
    Type* visit(FcallExp* e) override;
    Type* visit(StringExp* e) override; // Agregado
};

#endif TYPECHECKER_H
