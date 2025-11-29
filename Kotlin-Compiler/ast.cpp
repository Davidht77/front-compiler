#include "ast.h"
#include <iostream>
#include <algorithm>

using namespace std;

Exp::~Exp() {}
Stm::~Stm() {}

string Exp::binopToChar(BinaryOp op) {
    switch(op) {
        case PLUS_OP: return "+";
        case MINUS_OP: return "-";
        case MUL_OP: return "*";
        case DIV_OP: return "/";
        case POW_OP: return "^";
        case MOD_OP: return "%";
        case LE_OP: return "<=";
        case LT_OP: return "<";
        case GT_OP: return ">";
        case GE_OP: return ">=";
        case EQ_OP: return "==";
        case NE_OP: return "!=";
        case AND_OP: return "&&";
        case OR_OP: return "||";
        default: return "?";
    }
}

BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp op) : left(l), right(r), op(op) {
    // Plegado de constantes
    if (left && right && left->isnumber && right->isnumber) {
        isnumber = true;
        switch(op) {
            case PLUS_OP:  valor = left->valor + right->valor; break;
            case MINUS_OP: valor = left->valor - right->valor; break;
            case MUL_OP:   valor = left->valor * right->valor; break;
            case DIV_OP:   valor = (right->valor != 0) ? left->valor / right->valor : 0; break;
            case MOD_OP:   valor = (right->valor != 0) ? left->valor % right->valor : 0; break;
            case LE_OP:    valor = left->valor <= right->valor; break;
            case LT_OP:    valor = left->valor <  right->valor; break;
            case GT_OP:    valor = left->valor >  right->valor; break;
            case GE_OP:    valor = left->valor >= right->valor; break;
            case EQ_OP:    valor = left->valor == right->valor; break;
            case NE_OP:    valor = left->valor != right->valor; break;
            case AND_OP:   valor = (left->valor != 0) && (right->valor != 0); break;
            case OR_OP:    valor = (left->valor != 0) || (right->valor != 0); break;
            default:       valor = 0; break;
        }
    } else {
        isnumber = false;
        valor = 0;
    }

    // Peso Sethi-Ullman
    int le = left ? left->etiqueta : 0;
    int ri = right ? right->etiqueta : 0;
    etiqueta = (le == ri) ? le + 1 : max(le, ri);
}
BinaryExp::~BinaryExp() { delete left; delete right; }

NumberExp::NumberExp(int v) : value(v) { isnumber = true; valor = v; etiqueta = 0; }
NumberExp::~NumberExp() {}

DoubleExp::DoubleExp(double v) : value(v) { isnumber = true; value = v; etiqueta = 0;}
DoubleExp::~DoubleExp() {}

LongExp::LongExp(long long v) : valor(v) { 
    isnumber = true; 
}
LongExp::~LongExp() {}

BoolExp::BoolExp(bool v) : value(v) { isnumber = true; valor = v ? 1 : 0; etiqueta = 0; }
BoolExp::~BoolExp() {}

// Implementación de StringExp
StringExp::StringExp(string v) : value(v) { isnumber = false; valor = 0; etiqueta = 0; }
StringExp::~StringExp() {}

IdExp::IdExp(string v) : value(v) { isnumber = false; valor = 0; etiqueta = 0; }
IdExp::~IdExp() {}

VarDec::VarDec(string name, string type, Exp* init, bool isConst) 
    : name(name), type(type), init(init), isConst(isConst) {}
VarDec::~VarDec() { if(init) delete init; }

Block::Block() {}
Block::~Block() {
    for (Stm* s : stmts) delete s;
}

IfStmt::IfStmt(Exp* condition, Block* thenBlock, Block* elseBlock) 
    : condition(condition), thenBlock(thenBlock), elseBlock(elseBlock) {}

WhileStmt::WhileStmt(Exp* condition, Block* block) 
    : condition(condition), block(block) {}

ForStmt::ForStmt(string varName, Exp* rangeExp, Block* block)
    : varName(varName), rangeExp(rangeExp), block(block) {}

AssignExp::AssignExp(string id, Exp* e) : id(id), e(e) {}
AssignExp::~AssignExp() { delete e; }

PrintStm::PrintStm(Exp* e) : e(e) {}
PrintStm::~PrintStm() { delete e; }

ReturnStm::ReturnStm(Exp* e) : e(e) {}

FcallExp::FcallExp(string nombre, vector<Exp*> args, Exp* receiver) 
    : nombre(nombre), argumentos(args), receiver(receiver) {}

FunDec::FunDec(string nombre, string tipo, vector<string> Ptipos, vector<string> Pnombres, Block* cuerpo)
    : nombre(nombre), tipo(tipo), Ptipos(Ptipos), Pnombres(Pnombres), cuerpo(cuerpo) {}

Program::Program() {}
Program::~Program() {
    for (VarDec* v : vdlist) delete v;
    for (FunDec* f : fdlist) delete f;
}
