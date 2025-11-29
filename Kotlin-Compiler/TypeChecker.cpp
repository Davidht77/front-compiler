#include "typechecker.h"
#include <iostream>
#include <stdexcept>
using namespace std;


Type* NumberExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* DoubleExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* BoolExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* IdExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* BinaryExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* FcallExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* StringExp::accept(TypeVisitor* v) { return v->visit(this); } // Agregado

Type* AssignExp::accept(TypeVisitor* v) { return v->visit(this); } // Cambiado desde AssignStm
Type* PrintStm::accept(TypeVisitor* v) { return v->visit(this); }
Type* ReturnStm::accept(TypeVisitor* v) { return v->visit(this); }
Type* WhileStmt::accept(TypeVisitor* v) { return v->visit(this); } // Agregado
Type* IfStmt::accept(TypeVisitor* v) { return v->visit(this); }    // Agregado
Type* ForStmt::accept(TypeVisitor* v) { return v->visit(this); }   // Agregado

Type* VarDec::accept(TypeVisitor* v) { return v->visit(this); }
Type* FunDec::accept(TypeVisitor* v) { return v->visit(this); }
Type* Block::accept(TypeVisitor* v) { return v->visit(this); } // Cambiado desde Body
Type* Program::accept(TypeVisitor* v) { return v->visit(this); }

// ===========================================================
//   Constructor del TypeChecker
// ===========================================================

TypeChecker::TypeChecker() {
    intType = new Type(Type::INT);
    boolType = new Type(Type::BOOL);
    voidType = new Type(Type::VOID);
    stringType = new Type(Type::STRING); // Agregado
    rangeType = new Type(Type::RANGE); // Agregado
    currentVarCount = 0;
}

// ===========================================================
//   Registrar funciones globales
// ===========================================================

// Ayuda para inferir tipo de retorno a partir del cuerpo de la función
Type* TypeChecker::inferReturnType(Stm* s) {
    if (!s) return nullptr;

    if (ReturnStm* ret = dynamic_cast<ReturnStm*>(s)) {
        if (ret->e) {
            return ret->e->accept(this);
        } else {
            return voidType;
        }
    }

    if (Block* b = dynamic_cast<Block*>(s)) {
        for (Stm* stmt : b->stmts) {
            Type* t = inferReturnType(stmt);
            if (t) return t;
        }
    }

    if (IfStmt* ifStmt = dynamic_cast<IfStmt*>(s)) {
        Type* t = inferReturnType(ifStmt->thenBlock);
        if (t) return t;
        if (ifStmt->elseBlock) {
            return inferReturnType(ifStmt->elseBlock);
        }
    }

    if (WhileStmt* whileStmt = dynamic_cast<WhileStmt*>(s)) {
        return inferReturnType(whileStmt->block);
    }

    if (ForStmt* forStmt = dynamic_cast<ForStmt*>(s)) {
        return inferReturnType(forStmt->block);
    }

    return nullptr;
}

void TypeChecker::add_function(FunDec* fd) {
    if (functions.find(fd->nombre) != functions.end()) {
        cerr << "Error: función '" << fd->nombre << "' ya fue declarada." << endl;
        exit(0);
    }

    Type* returnType = new Type();
    if (fd->tipo.empty()) {
        // Lógica de inferencia
        // Agrega parámetros al entorno temporalmente para poder revisar tipos en el cuerpo
        env.add_level();
        for (size_t i = 0; i < fd->Pnombres.size(); ++i) {
            Type* pt = new Type();
             // Se asume que los parámetros deben tener tipo explícito
             if (fd->Ptipos[i].empty()) {
                 cerr << "Error: parámetros deben tener tipo explícito en función '" << fd->nombre << "'." << endl;
                 exit(0);
             }
            if (!pt->set_basic_type(fd->Ptipos[i])) {
                cerr << "Error: tipo de parámetro inválido en función '" << fd->nombre << "'." << endl;
                exit(0);
            }
            env.add_var(fd->Pnombres[i], pt);
        }

        Type* inferred = inferReturnType(fd->cuerpo);
        env.remove_level();

        if (inferred) {
            returnType = inferred;
        } else {
            returnType->ttype = Type::VOID;
        }
    } else if (!returnType->set_basic_type(fd->tipo)) {
        cerr << "Error: tipo de retorno no válido en función '" << fd->nombre << "'." << endl;
        exit(0);
    }

    functions[fd->nombre] = returnType;
}

void TypeChecker::typecheck(Program* program) {
    if (program) program->accept(this);
    // cout << "Revisión exitosa" << endl; // Optional: Comment out to reduce noise
}

// ===========================================================
//   Nivel superior: Programa y Bloque
// ===========================================================

Type* TypeChecker::visit(Program* p) {
    // Primero registrar funciones
    for (auto f : p->fdlist)
        add_function(f);

    env.add_level();
    for (auto v : p->vdlist)
        v->accept(this);  
    for (auto f : p->fdlist)
        f->accept(this);  
    env.remove_level();
    return voidType;
}

Type* TypeChecker::visit(Block* b) {
    env.add_level();
    for (auto s : b->stmts)
        s->accept(this); 
    env.remove_level();
    return voidType;
}

// ===========================================================
//   Declaraciones
// ===========================================================

Type* TypeChecker::visit(VarDec* v) {
    Type* t = new Type();
    if (v->type.empty()) {
        // Inferencia desde el inicializador
        if (v->init) {
             t = v->init->accept(this);
        } else {
            cerr << "Error: variable '" << v->name << "' sin tipo ni inicializador." << endl;
            exit(0);
        }
    } else if (!t->set_basic_type(v->type)) {
        cerr << "Error: tipo de variable no válido: '" << v->type << "'" << endl;
        // Depuración: imprimir valores ascii
        cerr << "Debug: ";
        for (char c : v->type) cerr << (int)c << " ";
        cerr << endl;
        exit(0);
    }

    if (!v->type.empty() && v->init) {
        Type* initType = v->init->accept(this);
        if (!initType->canAssignTo(t)) {
             cerr << "Error: tipo de inicializador incompatible con variable '" << v->name << "'." << endl;
             exit(0);
        }
    }

    if (env.check(v->name)) { // Changed from v->variables loop to v->name
        cerr << "Error: variable '" << v->name << "' ya declarada." << endl;
        exit(0);
    }
    env.add_var(v->name, t);

    // Lógica de conteo de variables
    if (!currentFunction.empty()) {
        currentVarCount++;
        functionVarCounts[currentFunction] = currentVarCount;
    }
    return voidType;
}

Type* TypeChecker::visit(FunDec* f) {
    currentFunction = f->nombre;
    currentVarCount = 0;
    functionVarCounts[currentFunction] = 0; // Inicializa

    env.add_level();
    for (size_t i = 0; i < f->Pnombres.size(); ++i) {
        Type* pt = new Type();
        if (!pt->set_basic_type(f->Ptipos[i])) { // Cambiado de Tparametros a Ptipos
            cerr << "Error: tipo de parámetro inválido en función '" << f->nombre << "'." << endl;
            exit(0);
        }
        env.add_var(f->Pnombres[i], pt); // Cambiado de Nparametros a Pnombres
        
        currentVarCount++;
    }
    functionVarCounts[currentFunction] = currentVarCount;

    functionVarCounts[currentFunction] = currentVarCount;

    // Usa el tipo ya registrado en add_function
    Type* returnType = functions[f->nombre];
    retornodefuncion = returnType;
    f->cuerpo->accept(this);

    env.remove_level();
    currentFunction = "";
    return voidType;
}

// ===========================================================
//   Sentencias
// ===========================================================

Type* TypeChecker::visit(PrintStm* stm) {
    Type* t = stm->e->accept(this);
    if (!(t->isNumeric() || t->match(boolType) || t->match(stringType))) { 
        cerr << "Error: tipo invalido en print (solo tipos numericos, bool o string)." << endl;
        exit(0);
    }
    return voidType;
}

Type* TypeChecker::visit(AssignExp* stm) { // Cambiado desde AssignStm
    if (!env.check(stm->id)) {
        cerr << "Error: variable '" << stm->id << "' no declarada." << endl;
        exit(0);
    }

    Type* varType = env.lookup(stm->id);
    Type* expType = stm->e->accept(this);

    if (!expType->canAssignTo(varType)) {
        cerr << "Error: tipos incompatibles en asignación a '" << stm->id << "'." << endl;
        exit(0);
    }
    return voidType;
}

Type* TypeChecker::visit(ReturnStm* stm) {
    if (stm->e) {
        Type* t = stm->e->accept(this);
        if (!(t->match(intType) || t->match(boolType) || t->match(voidType) || t->match(stringType))) {
            cerr << "Error: tipo inválido en return." << endl;
            exit(0);
        }
        // Nota: se podría comparar estrictamente con el tipo declarado de la función.
        // Aquí se valida solo la compatibilidad general. El original revisaba contra 'retornodefuncion'.
        if (!(t->canAssignTo(retornodefuncion))) {
             cerr << "Error: retorno distinto al declarado en la función." << endl;
             exit(0);
        }
    } else {
        if (!retornodefuncion->match(voidType)) {
            cerr << "Error: retorno vacío en función no void." << endl;
            exit(0);
        }
    }
    return voidType;
}

Type* TypeChecker::visit(WhileStmt* stm) {
    Type* t = stm->condition->accept(this);
    if (!t->match(boolType)) {
        cerr << "Error: condición de while debe ser bool." << endl;
        exit(0);
    }
    stm->block->accept(this);
    return voidType;
}

Type* TypeChecker::visit(IfStmt* stm) {
    Type* t = stm->condition->accept(this);
    if (!t->match(boolType)) {
        cerr << "Error: condición de if debe ser bool." << endl;
        exit(0);
    }
    stm->thenBlock->accept(this);
    if (stm->elseBlock) {
        stm->elseBlock->accept(this);
    }
    return voidType;
}

Type* TypeChecker::visit(ForStmt* stm) {
    
    env.add_level();
    env.add_var(stm->varName, intType);
    
    // Count the loop variable
    if (!currentFunction.empty()) {
        currentVarCount++;
        functionVarCounts[currentFunction] = currentVarCount;
    }

    Type* rangeT = stm->rangeExp->accept(this); // Visit range to check types there
    if (!rangeT->match(rangeType)) {
        cerr << "Error: for loop range must be a range type." << endl;
        exit(0);
    }
    stm->block->accept(this);
    env.remove_level();
    return voidType;
}

// ===========================================================
//   Expresiones
// ===========================================================

Type* TypeChecker::visit(BinaryExp* e) {
    Type* left = e->left->accept(this);
    Type* right = e->right->accept(this);
    Type* resultType = nullptr;

    switch (e->op) {
        case PLUS_OP: 
        case MINUS_OP: 
        case MUL_OP: 
        case DIV_OP: 
        case POW_OP:
        case MOD_OP: 
            // Permitir todos los tipos numéricos
            if (!((left->isNumeric()) && (right->isNumeric()))) {
                cerr << "Error: operación aritmética requiere operandos numéricos." << endl;
                exit(0);
            }
            
            // Type promotion logic
            if (left->ttype == Type::DOUBLE || right->ttype == Type::DOUBLE) {
                 resultType = new Type(Type::DOUBLE);
            }
            else if (left->ttype == Type::FLOAT || right->ttype == Type::FLOAT) {
                 resultType = new Type(Type::FLOAT);
            }
            else {
                auto rank = [](Type* t) {
                    switch (t->ttype) {
                        case Type::BYTE:
                        case Type::UBYTE: return 1;
                        case Type::SHORT:
                        case Type::USHORT: return 2;
                        case Type::INT:
                        case Type::UINT: return 3;
                        case Type::LONG:
                        case Type::ULONG: return 4;
                        default: return 0;
                    }
                };
                Type* wider = (rank(left) >= rank(right)) ? left : right;
                resultType = new Type(wider->ttype);
            }
            break;

        case LE_OP:
        case LT_OP:
        case GT_OP:
        case GE_OP:
        case EQ_OP:
        case NE_OP:
            if (!((left->ttype >= Type::INT && left->ttype <= Type::ULONG) && 
                  (right->ttype >= Type::INT && right->ttype <= Type::ULONG)) &&
                !((left->match(boolType) && right->match(boolType)))) {
                 // Permitir comparación de números o bools
                 if (!left->match(right)) {
                    // Relaxed check for numbers?
                    if (!((left->ttype >= Type::INT && left->ttype <= Type::ULONG) && 
                          (right->ttype >= Type::INT && right->ttype <= Type::ULONG))) {
                        cerr << "Error: tipos incompatibles en comparación." << endl;
                        exit(0);
                    }
                 }
            }
            resultType = boolType;
            break;

        case AND_OP:
        case OR_OP:
            if (!(left->match(boolType) && right->match(boolType))) {
                cerr << "Error: operación lógicas requiere operandos bool." << endl;
                exit(0);
            }
            resultType = boolType;
            break;
            
        case RANGE_OP:
        case DOWNTO_OP:
             if (!(left->isNumeric() && right->isNumeric())) {
                cerr << "Error: rango requiere operandos numéricos." << endl;
                exit(0);
             }
             resultType = rangeType;
             break;

        case STEP_OP:
             if (!((left->match(rangeType) || left->isNumeric()) && right->isNumeric())) {
                cerr << "Error: step requiere un rango (o número) y un paso numérico." << endl;
                exit(0);
             }
             resultType = rangeType;
             break;

        default:
            cerr << "Error: operador binario no soportado." << endl;
            exit(0);
    }
    
    e->inferredType = resultType;
    return resultType;
}

Type* TypeChecker::visit(NumberExp* e) { 
    e->inferredType = intType; 
    return intType; 
}

Type* TypeChecker::visit(BoolExp* e) { 
    e->inferredType = boolType;
    return boolType; 
}

Type* TypeChecker::visit(StringExp* e) { 
    e->inferredType = stringType;
    return stringType; 
} 

Type* TypeChecker::visit(DoubleExp* e) {
    Type* doubleType = new Type(Type::DOUBLE);
    e->inferredType = doubleType;
    return doubleType;
}

Type* TypeChecker::visit(IdExp* e) {
    if (!env.check(e->value)) {
        cerr << "Error: variable '" << e->value << "' no declarada." << endl;
        exit(0);
    }
    Type* t = env.lookup(e->value);
    e->inferredType = t;
    return t;
}

Type* TypeChecker::visit(FcallExp* e) {
    if (e->receiver) {
        Type* recvType = e->receiver->accept(this);
        for (auto arg : e->argumentos) {
            arg->accept(this);
        }

        static unordered_map<string, Type::TType> conversions = {
            {"toByte", Type::BYTE},
            {"toShort", Type::SHORT},
            {"toInt", Type::INT},
            {"toLong", Type::LONG},
            {"toFloat", Type::FLOAT},
            {"toDouble", Type::DOUBLE},
            {"toUByte", Type::UBYTE},
            {"toUShort", Type::USHORT},
            {"toUInt", Type::UINT},
            {"toULong", Type::ULONG}
        };

        auto itConv = conversions.find(e->nombre);
        if (itConv == conversions.end()) {
            cerr << "Error: metodo '" << e->nombre << "' no soportado." << endl;
            exit(0);
        }

        if (!recvType->isNumeric()) {
            cerr << "Error: conversion '" << e->nombre << "' solo permitida desde tipos numericos." << endl;
            exit(0);
        }

        Type* result = new Type(itConv->second);
        e->inferredType = result;
        return result;
    }

    for (auto arg : e->argumentos) {
        arg->accept(this);
    }

    auto it = functions.find(e->nombre);
    if (it == functions.end()) {
        cerr << "Error: llamada a funcion no declarada '" << e->nombre << "'." << endl;
        exit(0);
    }

    Type* t = it->second;
    e->inferredType = t;
    return t;
}
