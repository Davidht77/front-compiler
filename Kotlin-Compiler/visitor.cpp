#include <iostream>
#include "ast.h"
#include "visitor.h"
#include "semantic_types.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <cstring>
#include <string>
using namespace std;

// Ayuda para obtener el nombre de registro según el tamaño
string getReg(string baseReg, int size) {
    if (baseReg == "rax" || baseReg == "%rax") {
        if (size == 1) return "%al";
        if (size == 2) return "%ax";
        if (size == 4) return "%eax";
        return "%rax";
    }
    if (baseReg == "rcx" || baseReg == "%rcx") {
        if (size == 1) return "%cl";
        if (size == 2) return "%cx";
        if (size == 4) return "%ecx";
        return "%rcx";
    }
    if (baseReg == "rdx" || baseReg == "%rdx") {
        if (size == 1) return "%dl";
        if (size == 2) return "%dx";
        if (size == 4) return "%edx";
        return "%rdx";
    }
    if (baseReg == "rbx" || baseReg == "%rbx") {
        if (size == 1) return "%bl";
        if (size == 2) return "%bx";
        if (size == 4) return "%ebx";
        return "%rbx";
    }
    if (baseReg == "rdi" || baseReg == "%rdi") {
        if (size == 1) return "%dil";
        if (size == 2) return "%di";
        if (size == 4) return "%edi";
        return "%rdi";
    }
    if (baseReg == "rsi" || baseReg == "%rsi") {
        if (size == 1) return "%sil";
        if (size == 2) return "%si";
        if (size == 4) return "%esi";
        return "%rsi";
    }
    if (baseReg == "r8" || baseReg == "%r8") {
        if (size == 1) return "%r8b";
        if (size == 2) return "%r8w";
        if (size == 4) return "%r8d";
        return "%r8";
    }
    if (baseReg == "r9" || baseReg == "%r9") {
        if (size == 1) return "%r9b";
        if (size == 2) return "%r9w";
        if (size == 4) return "%r9d";
        return "%r9";
    }
    // Add more if needed
    return (baseReg[0] == '%' ? "" : "%") + baseReg;
}

// Ayuda para obtener el sufijo de instrucción
string getSuffix(int size) {
    if (size == 1) return "b";
    if (size == 2) return "w";
    if (size == 4) return "l";
    return "q";
}

int getTypeSize(Type* t) {
    if (!t) return 8; // Por defecto 64 bits si se desconoce
    if (t->ttype == Type::BYTE || t->ttype == Type::UBYTE) return 1;
    if (t->ttype == Type::SHORT || t->ttype == Type::USHORT) return 2;

    if (t->ttype == Type::INT || t->ttype == Type::UINT) return 4;
    if (t->ttype == Type::FLOAT) return 4;
    if (t->ttype == Type::DOUBLE) return 8;
    if (t->ttype == Type::LONG || t->ttype == Type::ULONG) return 8;
    if (t->ttype == Type::BOOL) return 1; // Bool as byte
    return 8; // Valor por defecto
}

// Convertir el valor en RAX al tipo destino, dejando el resultado en RAX/EAX
static void convertValueTo(Type* src, Type* dst, ostream& out) {
    if (!src || !dst || src->ttype == dst->ttype) return;

    // Destino flotante
    if (dst->ttype == Type::FLOAT) {
        if (src->ttype == Type::DOUBLE) {
            out << " movq %rax, %xmm0\n cvtsd2ss %xmm0, %xmm0\n movd %xmm0, %eax\n";
            return;
        }
        if (src->ttype == Type::FLOAT) return;

        // Entero -> float
        int sz = getTypeSize(src);
        if (sz == 1) out << " movsbq %al, %rax\n";
        else if (sz == 2) out << " movswq %ax, %rax\n";
        else if (sz == 4) out << " movslq %eax, %rax\n";
        out << " cvtsi2ssq %rax, %xmm0\n movd %xmm0, %eax\n";
        return;
    }

    if (dst->ttype == Type::DOUBLE) {
        if (src->ttype == Type::FLOAT) {
            out << " movd %eax, %xmm0\n cvtss2sd %xmm0, %xmm0\n movq %xmm0, %rax\n";
            return;
        }
        if (src->ttype == Type::DOUBLE) return;

        int sz = getTypeSize(src);
        if (sz == 1) out << " movsbq %al, %rax\n";
        else if (sz == 2) out << " movswq %ax, %rax\n";
        else if (sz == 4) out << " movslq %eax, %rax\n";
        out << " cvtsi2sdq %rax, %xmm0\n movq %xmm0, %rax\n";
        return;
    }

    // Destino entero
    if (src->ttype == Type::DOUBLE || src->ttype == Type::FLOAT) {
        out << " movq %rax, %xmm0\n";
        if (src->ttype == Type::DOUBLE)
            out << " cvttsd2siq %xmm0, %rax\n";
        else
            out << " cvttss2siq %xmm0, %rax\n";
    }
    int dsz = getTypeSize(dst);
    if (dsz == 1) out << " movsbq %al, %rax\n";
    else if (dsz == 2) out << " movswq %ax, %rax\n";
    else if (dsz == 4) out << " movslq %eax, %rax\n";
}

// Recolección de llamadas para eliminar funciones no usadas
static void collectCallsFromExp(Exp* e, unordered_set<string>& calls) {
    if (!e) return;
    if (auto b = dynamic_cast<BinaryExp*>(e)) {
        collectCallsFromExp(b->left, calls);
        collectCallsFromExp(b->right, calls);
    } else if (auto f = dynamic_cast<FcallExp*>(e)) {
        calls.insert(f->nombre);
        for (auto arg : f->argumentos) collectCallsFromExp(arg, calls);
        collectCallsFromExp(f->receiver, calls);
    } else if (auto a = dynamic_cast<AssignExp*>(e)) {
        collectCallsFromExp(a->e, calls);
    }
}

static void collectCallsFromStm(Stm* s, unordered_set<string>& calls) {
    if (!s) return;

    if (auto e = dynamic_cast<Exp*>(s)) {
        collectCallsFromExp(e, calls);
        return;
    }
    if (auto v = dynamic_cast<VarDec*>(s)) {
        collectCallsFromExp(v->init, calls);
        return;
    }
    if (auto p = dynamic_cast<PrintStm*>(s)) {
        collectCallsFromExp(p->e, calls);
        return;
    }
    if (auto r = dynamic_cast<ReturnStm*>(s)) {
        collectCallsFromExp(r->e, calls);
        return;
    }
    if (auto b = dynamic_cast<Block*>(s)) {
        for (auto st : b->stmts) collectCallsFromStm(st, calls);
        return;
    }
    if (auto i = dynamic_cast<IfStmt*>(s)) {
        collectCallsFromExp(i->condition, calls);
        collectCallsFromStm(i->thenBlock, calls);
        collectCallsFromStm(i->elseBlock, calls);
        return;
    }
    if (auto w = dynamic_cast<WhileStmt*>(s)) {
        collectCallsFromExp(w->condition, calls);
        collectCallsFromStm(w->block, calls);
        return;
    }
    if (auto f = dynamic_cast<ForStmt*>(s)) {
        collectCallsFromExp(f->rangeExp, calls);
        collectCallsFromStm(f->block, calls);
        return;
    }
}

int BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int NumberExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int DoubleExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int BoolExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int StringExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int Program::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IdExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int PrintStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int AssignExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IfStmt::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int WhileStmt::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int ForStmt::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int Block::accept(Visitor* visitor){
    return visitor->visit(this);
}

int VarDec::accept(Visitor* visitor){
    return visitor->visit(this);
}

int FcallExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int FunDec::accept(Visitor* visitor){
    return visitor->visit(this);
}

int ReturnStm::accept(Visitor* visitor){
    return visitor->visit(this);
}


///////////////////////////////////////////////////////////////////////////////////
// Implementación de GenCodeVisitor

int GenCodeVisitor::generar(Program* program) {
    // Inicializar offset y estado de función antes de comenzar
    offset = -8; 
    entornoFuncion = false; 
    program->accept(this);
    return 0;
}

int GenCodeVisitor::visit(Program* program) {
    // 1. Sección de datos
    out << ".data\n";
    out << "print_fmt_num: .string \"%ld \\n\""<<endl;
    out << "print_fmt_float: .string \"%f\\n\""<<endl; // Formato para flotantes
    out << "print_fmt_str: .string \"%s\\n\""<<endl;
    out << "stack_fmt: .string \"STACK rsp=%p rbp=%p\\n\""<<endl;

    // A. Recorrer VarDecs Globales para registrarlas y definirlas estáticamente.
    for (auto dec : program->vdlist){
        memoriaGlobal[dec->name] = true; // Registrar la variable como global
        // Registrar tipo global
        Type* gtype = nullptr;
        if (!dec->type.empty()) {
            gtype = new Type();
            gtype->set_basic_type(dec->type);
        } else if (dec->init && dec->init->inferredType) {
            gtype = dec->init->inferredType;
        }
        tiposGlobales[dec->name] = gtype;

        // 1. Manejar StringExp para recolectar la literal (si es StringExp).
        if (dec->init) {
            StringExp* stringExp = dynamic_cast<StringExp*>(dec->init);
            if (stringExp) {
                if (!stringLiterals.count(stringExp->value)) {
                    string label = "str_" + to_string(stringCont++);
                    stringLiterals[stringExp->value] = label;
                }
            }
        }
        
        // 2. Generar la DEFINICIÓN ESTÁTICA (.quad).
        out << dec->name << ": .quad ";
        
        NumberExp* numExp = dynamic_cast<NumberExp*>(dec->init);
        BoolExp* boolExp = dynamic_cast<BoolExp*>(dec->init);
        
        if (numExp) {
            out << numExp->value << endl;
        } else if (boolExp) {
            out << (boolExp->value ? 1 : 0) << endl;
        } else {
            out << "0" << endl; 
        }
    }
    
    // 2. Sección de código
    out << ".text\n";
    out << ".global main\n";
    
    // Punto de entrada `main` por defecto (mantenemos su lógica)
    if (program->fdlist.empty()) {
        out << "main:\n";
        out << " pushq %rbp\n";
        out << " movq %rsp, %rbp\n";
        out << " movl $0, %eax\n";
        out << " popq %rbp\n";
        out << " ret\n";
    }

    // Determinar qué funciones están realmente usadas (alcanzables desde main)
    unordered_map<string, FunDec*> funcMap;
    for (auto dec : program->fdlist) funcMap[dec->nombre] = dec;

    unordered_set<string> used;
    if (funcMap.count("main")) {
        vector<string> stack = {"main"};
        while (!stack.empty()) {
            string fname = stack.back();
            stack.pop_back();
            if (!used.insert(fname).second) continue; // ya visitada
            auto it = funcMap.find(fname);
            if (it == funcMap.end()) continue;
            unordered_set<string> directCalls;
            collectCallsFromStm(it->second->cuerpo, directCalls);
            for (auto& c : directCalls) {
                if (funcMap.count(c) && !used.count(c)) stack.push_back(c);
            }
        }
    } else {
        // Sin main definido: no eliminamos nada por seguridad
        for (auto dec : program->fdlist) used.insert(dec->nombre);
    }

    // Funciones (solo las usadas)
    for (auto dec : program->fdlist){
        if (!used.count(dec->nombre)) continue; // función no utilizada
        dec->accept(this);
    }

    // B. Imprimir las literales de cadena recolectadas (al final para incluir las de funciones)
    if (!stringLiterals.empty()) {
        out << ".data\n";
        for (auto& pair : stringLiterals) {
            out << pair.second << ": .string \"" << pair.first << "\""<<endl;
        }
    }

    out << ".section .note.GNU-stack,\"\",@progbits"<<endl;
    return 0;
}

int GenCodeVisitor::visit(VarDec* stm) {
    string var = stm->name;
    
    if (!entornoFuncion) {
        memoriaGlobal[var] = true;
        Type* destType = nullptr;
        if (!stm->type.empty()) {
            destType = new Type();
            destType->set_basic_type(stm->type);
        } else if (stm->init && stm->init->inferredType) {
            destType = stm->init->inferredType;
        }
        tiposGlobales[var] = destType;
        if (stm->init) {
            StringExp* stringExp = dynamic_cast<StringExp*>(stm->init);
            if (stringExp) {
                if (!stringLiterals.count(stringExp->value)) {
                    string label = "str_" + to_string(stringCont++);
                    stringLiterals[stringExp->value] = label;
                }
            }
        }
        return 0; 
    } else { 
        // Usa el entorno para variables locales
        env.add_var(var, offset);
        Type* destType = nullptr;
        if (!stm->type.empty()) {
            destType = new Type();
            destType->set_basic_type(stm->type);
        } else if (stm->init && stm->init->inferredType) {
            destType = stm->init->inferredType;
        }
        typeEnv.add_var(var, destType);
        int varOffset = offset;
        offset -= 8;
        
        if (stm->init) {
            stm->init->accept(this); 
            // Determinar tipo destino
            convertValueTo(stm->init->inferredType, destType, out);
            int size = getTypeSize(destType ? destType : stm->init->inferredType);
            string reg = getReg("rax", size);
            out << " mov" << getSuffix(size) << " " << reg << ", " << varOffset << "(%rbp)"<<endl;
            // Log de pila tras inicializar variable local
            out << " leaq stack_fmt(%rip), %rdi\n";
            out << " movq %rsp, %rsi\n";
            out << " movq %rbp, %rdx\n";
            out << " movl $0, %eax\n";
            out << " call printf@PLT\n";
        }
    }
    return 0;
}

int GenCodeVisitor::visit(NumberExp* exp) {
    int size = getTypeSize(exp->inferredType);
    string reg = getReg("rax", size);
    out << " mov" << getSuffix(size) << " $" << exp->value << ", " << reg << endl;
    return 0;
}

int GenCodeVisitor::visit(DoubleExp* exp) {
    long long bits;
    double d = exp->value;
    memcpy(&bits, &d, sizeof(bits));
    out << " movabsq $" << bits << ", %rax\n";
    return 0;
}

int GenCodeVisitor::visit(BoolExp* exp) {
    int size = getTypeSize(exp->inferredType);
    string reg = getReg("rax", size);
    out << " mov" << getSuffix(size) << " $" << (exp->value ? 1 : 0) << ", " << reg << endl;
    return 0;
}

int GenCodeVisitor::visit(StringExp* exp) {
    string label;
    if (stringLiterals.count(exp->value)) {
        label = stringLiterals[exp->value];
    } else {
        label = "str_" + to_string(stringCont++);
        stringLiterals[exp->value] = label;
    }
    out << " leaq " << label << "(%rip), %rax\n"; 
    return 0;
}

int GenCodeVisitor::visit(IdExp* exp) {
    int size = getTypeSize(exp->inferredType);
    if (exp->inferredType && exp->inferredType->ttype == Type::FLOAT) {
        auto load = [&](const string& addr) {
            out << " movl " << addr << ", %eax\n";
        };
        if (memoriaGlobal.count(exp->value))
            load(exp->value + "(%rip)");
        else {
            int varOffset = env.lookup(exp->value);
            load(to_string(varOffset) + "(%rbp)");
        }
        return 0;
    }
    if (exp->inferredType && exp->inferredType->ttype == Type::DOUBLE) {
        auto load = [&](const string& addr) { out << " movq " << addr << ", %rax\n"; };
        if (memoriaGlobal.count(exp->value))
            load(exp->value + "(%rip)");
        else {
            int varOffset = env.lookup(exp->value);
            load(to_string(varOffset) + "(%rbp)");
        }
        return 0;
    }

    bool isUnsigned = exp->inferredType && (
        exp->inferredType->ttype == Type::UBYTE ||
        exp->inferredType->ttype == Type::USHORT ||
        exp->inferredType->ttype == Type::UINT ||
        exp->inferredType->ttype == Type::ULONG
    );

    auto emitLoad = [&](const string& addr) {
        if (size == 1) out << (isUnsigned ? " movzbq " : " movsbq ") << addr << ", %rax\n";
        else if (size == 2) out << (isUnsigned ? " movzwq " : " movswq ") << addr << ", %rax\n";
        else if (size == 4) out << " movslq " << addr << ", %rax\n";
        else out << " movq " << addr << ", %rax\n";
    };

    if (memoriaGlobal.count(exp->value))
        emitLoad(exp->value + "(%rip)");
    else {
        int varOffset = env.lookup(exp->value);
        emitLoad(to_string(varOffset) + "(%rbp)");
    }
    return 0;
}

int GenCodeVisitor::visit(BinaryExp* exp) {
    // Constant folding: si ya está evaluado, emite inmediato
    if (exp->isnumber) {
        int size = getTypeSize(exp->inferredType);
        string reg = getReg("rax", size);
        out << " mov" << getSuffix(size) << " $" << exp->valor << ", " << reg << "\n";
        return 0;
    }

    bool leftFirst = exp->left->etiqueta >= exp->right->etiqueta;
    bool operandsAreDouble = (exp->left->inferredType && (exp->left->inferredType->ttype == Type::DOUBLE || exp->left->inferredType->ttype == Type::FLOAT)) ||
                             (exp->right->inferredType && (exp->right->inferredType->ttype == Type::DOUBLE || exp->right->inferredType->ttype == Type::FLOAT));

    if (operandsAreDouble) {
        auto loadToXmm = [&](Exp* e, const string& xmm) {
            e->accept(this);
            Type* t = e->inferredType;
            if (t->ttype == Type::DOUBLE) {
                out << " movq %rax, " << xmm << "\n";
            } else if (t->ttype == Type::FLOAT) {
                out << " movd %eax, " << xmm << "\n";
                out << " cvtss2sd " << xmm << ", " << xmm << "\n";
            } else { // entero
                int sz = getTypeSize(t);
                if (sz == 1) out << " movsbq %al, %rax\n";
                else if (sz == 2) out << " movswq %ax, %rax\n";
                else if (sz == 4) out << " movslq %eax, %rax\n";
                out << " cvtsi2sdq %rax, " << xmm << "\n";
            }
        };

        if (leftFirst) {
            loadToXmm(exp->left, "%xmm0");
            loadToXmm(exp->right, "%xmm1");
        } else {
            loadToXmm(exp->right, "%xmm1");
            loadToXmm(exp->left, "%xmm0");
        }

        switch (exp->op) {
            case PLUS_OP:  out << " addsd %xmm1, %xmm0\n"; break;
            case MINUS_OP: out << " subsd %xmm1, %xmm0\n"; break;
            case MUL_OP:   out << " mulsd %xmm1, %xmm0\n"; break;
            case DIV_OP:   out << " divsd %xmm1, %xmm0\n"; break;
            // Comparisons
            case LE_OP:
                out << " ucomisd %xmm1, %xmm0\n"
                    << " movl $0, %eax\n"
                    << " setbe %al\n" // Below or Equal (unsigned check for floats)
                    << " movzbq %al, %rax\n";
                return 0;
            case LT_OP:
                out << " ucomisd %xmm1, %xmm0\n"
                    << " movl $0, %eax\n"
                    << " setb %al\n" // Below
                    << " movzbq %al, %rax\n";
                return 0;
            case GT_OP:
                out << " ucomisd %xmm1, %xmm0\n"
                    << " movl $0, %eax\n"
                    << " setja %al\n" // Above (unsigned check for floats)
                    << " movzbq %al, %rax\n";
                return 0;
            case GE_OP:
                out << " ucomisd %xmm1, %xmm0\n"
                    << " movl $0, %eax\n"
                    << " setae %al\n" // Above or Equal
                    << " movzbq %al, %rax\n";
                return 0;
            case EQ_OP:
                out << " ucomisd %xmm1, %xmm0\n"
                    << " movl $0, %eax\n"
                    << " sete %al\n" 
                    << " movzbq %al, %rax\n";
                if (exp->op == NE_OP) out << " xorq $1, %rax\n"; // Invert for NE
                return 0;
            case NE_OP:
                out << " ucomisd %xmm1, %xmm0\n"
                    << " movl $0, %eax\n"
                    << " setne %al\n" 
                    << " movzbq %al, %rax\n";
                return 0;
            default: out << " # Operador no soportado para doubles\n"; break;
        }
        
        // Result is in XMM0. Convert to float if needed.
        if (exp->inferredType && exp->inferredType->ttype == Type::FLOAT) {
            out << " cvtsd2ss %xmm0, %xmm0\n";
            out << " movd %xmm0, %eax\n";
        } else {
            out << " movq %xmm0, %rax\n";
        }
        return 0;
    }

    if (leftFirst) {
        exp->left->accept(this);
        out << " pushq %rax\n";
        exp->right->accept(this);
        out << " movq %rax, %rcx\n popq %rax\n";
    } else {
        exp->right->accept(this);
        out << " pushq %rax\n";
        exp->left->accept(this);
        out << " movq %rax, %rcx\n popq %rax\n";
        out << " xchgq %rax, %rcx\n"; // asegurar rax=izq, rcx=der
    }

    int size = getTypeSize(exp->inferredType); // Usar tamaño del tipo resultante
    string suffix = getSuffix(size);
    string regAx = getReg("rax", size);
    string regCx = getReg("rcx", size);

    switch (exp->op) {
        case PLUS_OP:  out << " add" << suffix << " " << regCx << ", " << regAx << "\n"; break;
        case MINUS_OP: out << " sub" << suffix << " " << regCx << ", " << regAx << "\n"; break;
        case MUL_OP:   out << " imul" << suffix << " " << regCx << ", " << regAx << "\n"; break;
        case DIV_OP:   
            if (size == 1) out << " cbw\n idivb " << regCx << "\n";
            else if (size == 2) out << " cwd\n idivw " << regCx << "\n";
            else if (size == 4) out << " cdq\n idivl " << regCx << "\n";
            else out << " cqo\n idivq " << regCx << "\n";
            break; 
        case MOD_OP:   
            if (size == 1) out << " cbw\n idivb " << regCx << "\n movzbq %ah, %rax\n"; // Remainder in AH
            else if (size == 2) out << " cwd\n idivw " << regCx << "\n movzwq %dx, %rax\n"; // Remainder in DX
            else if (size == 4) out << " cdq\n idivl " << regCx << "\n movslq %edx, %rax\n"; // Remainder in EDX
            else out << " cqo\n idivq " << regCx << "\n movq %rdx, %rax\n"; // Remainder in RDX
            break; 
        case POW_OP:   out << " # WARNING: Operador POW (**) no implementado\n"; break;

        // Operadores de Comparación
        case LE_OP:
            out << " cmp" << suffix << " " << regCx << ", " << regAx << "\n"
                      << " movl $0, %eax\n"
                      << " setle %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case LT_OP:
            out << " cmp" << suffix << " " << regCx << ", " << regAx << "\n"
                      << " movl $0, %eax\n"
                      << " setl %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case GT_OP:
            out << " cmp" << suffix << " " << regCx << ", " << regAx << "\n"
                      << " movl $0, %eax\n"
                      << " setg %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case GE_OP:
            out << " cmp" << suffix << " " << regCx << ", " << regAx << "\n"
                      << " movl $0, %eax\n"
                      << " setge %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case EQ_OP:
            out << " cmp" << suffix << " " << regCx << ", " << regAx << "\n"
                      << " movl $0, %eax\n"
                      << " sete %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case NE_OP:
            out << " cmp" << suffix << " " << regCx << ", " << regAx << "\n"
                      << " movl $0, %eax\n"
                      << " setne %al\n"
                      << " movzbq %al, %rax\n";
            break;
        // Operadores Lógicos (simplificados)
        case AND_OP: out << " and" << suffix << " " << regCx << ", " << regAx << "\n"; break;
        case OR_OP:  out << " or" << suffix << " " << regCx << ", " << regAx << "\n"; break;
        default: break;
    }
    return 0;
}

int GenCodeVisitor::visit(AssignExp* stm) {
    stm->e->accept(this);
    Type* destType = nullptr;
    if (memoriaGlobal.count(stm->id)) {
        if (tiposGlobales.count(stm->id)) destType = tiposGlobales[stm->id];
    } else {
        destType = typeEnv.lookup(stm->id);
    }
    convertValueTo(stm->e->inferredType, destType, out);
    int size = getTypeSize(destType ? destType : stm->e->inferredType);
    string reg = getReg("rax", size);

    if (memoriaGlobal.count(stm->id))
        out << " mov" << getSuffix(size) << " " << reg << ", " << stm->id << "(%rip)"<<endl;
    else {
        int varOffset = env.lookup(stm->id);
        out << " mov" << getSuffix(size) << " " << reg << ", " << varOffset << "(%rbp)"<<endl;
    }
    return 0;
}

int GenCodeVisitor::visit(PrintStm* stm) {
    stm->e->accept(this); 

    StringExp* stringExp = dynamic_cast<StringExp*>(stm->e); 

    if (stringExp) {
        out << " movq %rax, %rsi\n"; 
        out << " leaq print_fmt_str(%rip), %rdi\n"; 
        out << " movl $0, %eax\n";
    } else if (stm->e->inferredType && (stm->e->inferredType->ttype == Type::DOUBLE || stm->e->inferredType->ttype == Type::FLOAT)) {
        if (stm->e->inferredType->ttype == Type::FLOAT) {
            out << " movd %eax, %xmm0\n";
            out << " cvtss2sd %xmm0, %xmm0\n";
        } else {
            out << " movq %rax, %xmm0\n"; 
        }
        out << " leaq print_fmt_float(%rip), %rdi\n"; 
        out << " movl $1, %eax\n";
    } else {
        // Asegurar extensión de signo a 64 bits para printf
        int size = getTypeSize(stm->e->inferredType);
        if (size == 1) out << " movsbq %al, %rsi\n";
        else if (size == 2) out << " movswq %ax, %rsi\n";
        else if (size == 4) out << " movslq %eax, %rsi\n";
        else out << " movq %rax, %rsi\n"; 
        
        out << " leaq print_fmt_num(%rip), %rdi\n"; 
        out << " movl $0, %eax\n";
    }
    
    out << " call printf@PLT\n";
    return 0;
}

int GenCodeVisitor::visit(Block* b) {
    env.add_level(); // Nuevo alcance
    typeEnv.add_level();
    for (auto s : b->stmts){
        s->accept(this);
    }
    env.remove_level(); // Fin de alcance
    typeEnv.remove_level();
    return 0;
}

int GenCodeVisitor::visit(IfStmt* stm) {
    // Dead code elimination cuando la condición es constante
    if (stm->condition->isnumber) {
        if (stm->condition->valor != 0) {
            stm->thenBlock->accept(this);
        } else if (stm->elseBlock) {
            stm->elseBlock->accept(this);
        }
        return 0;
    }

    int label = labelcont++;
    stm->condition->accept(this);
    out << " cmpq $0, %rax"<<endl;
    out << " je else_" << label << endl;
    stm->thenBlock->accept(this);
    out << " jmp endif_" << label << endl;
    out << "else_" << label << ":"<< endl;
    if (stm->elseBlock) stm->elseBlock->accept(this);
    out << "endif_" << label << ":"<< endl;
    return 0;
}

int GenCodeVisitor::visit(WhileStmt* stm) {
    int label = labelcont++;
    out << "while_" << label << ":"<<endl;
    stm->condition->accept(this);
    out << " cmpq $0, %rax" << endl;
    out << " je endwhile_" << label << endl;
    stm->block->accept(this);
    out << " jmp while_" << label << endl;
    out << "endwhile_" << label << ":"<< endl;
    return 0;
}

int GenCodeVisitor::visit(ForStmt* stm) {
    int label = labelcont++;
    int saved_offset = offset;
    
    env.add_level(); // Scope for loop variable
    typeEnv.add_level();

    Exp* range = stm->rangeExp;
    Exp* start = nullptr;
    Exp* end = nullptr;
    Exp* step = nullptr;
    bool isDownTo = false;
    
    BinaryExp* stepExp = dynamic_cast<BinaryExp*>(range);
    if (stepExp && stepExp->op == STEP_OP) {
        step = stepExp->right;
        range = stepExp->left; 
    }
    
    BinaryExp* rangeBin = dynamic_cast<BinaryExp*>(range);
    if (rangeBin) {
        if (rangeBin->op == RANGE_OP) {
            start = rangeBin->left;
            end = rangeBin->right;
            isDownTo = false;
        } else if (rangeBin->op == DOWNTO_OP) {
            start = rangeBin->left;
            end = rangeBin->right;
            isDownTo = true;
        }
    }
    
    if (!start || !end) {
        start = new NumberExp(0); 
    }

    // Determine type size for loop variable (assume Int or based on range start)
    // Ideally we should check inferredType of start/end.
    // For now, let's assume Int (4 bytes) as standard for loops unless specified otherwise.
    // Or better, check start->inferredType.
    int size = 4; 
    if (start->inferredType) size = getTypeSize(start->inferredType);

    string suffix = getSuffix(size);
    string regAx = getReg("rax", size);
    string regCx = getReg("rcx", size);

    start->accept(this);
    int start_offset = offset;
    offset -= 8;
    out << " mov" << suffix << " " << regAx << ", " << start_offset << "(%rbp)\n";
    
    end->accept(this);
    int end_offset = offset;
    offset -= 8;
    out << " mov" << suffix << " " << regAx << ", " << end_offset << "(%rbp)\n";
    
    int step_offset = offset;
    offset -= 8;
    if (step) {
        step->accept(this);
    } else {
        out << " mov" << suffix << " $1, " << regAx << "\n";
    }
    out << " mov" << suffix << " " << regAx << ", " << step_offset << "(%rbp)\n";

    string var = stm->varName;
    // Add loop variable to environment
    env.add_var(var, offset);
    static Type loopInt(Type::INT);
    typeEnv.add_var(var, &loopInt);
    int varOffset = offset;
    offset -= 8;
    
    out << " mov" << suffix << " " << start_offset << "(%rbp), " << regAx << "\n";
    out << " mov" << suffix << " " << regAx << ", " << varOffset << "(%rbp)\n";
    
    out << "loop_" << label << ":\n";
    
    out << " mov" << suffix << " " << varOffset << "(%rbp), " << regAx << "\n"; 
    out << " mov" << suffix << " " << end_offset << "(%rbp), " << regCx << "\n";   
    out << " cmp" << suffix << " " << regCx << ", " << regAx << "\n"; 
    
    if (isDownTo) {
        out << " jl endloop_" << label << "\n"; 
    } else {
        out << " jg endloop_" << label << "\n";
    }
    
    stm->block->accept(this);
    
    out << " mov" << suffix << " " << varOffset << "(%rbp), " << regAx << "\n";
    out << " mov" << suffix << " " << step_offset << "(%rbp), " << regCx << "\n";
    if (isDownTo) {
        out << " sub" << suffix << " " << regCx << ", " << regAx << "\n"; 
    } else {
        out << " add" << suffix << " " << regCx << ", " << regAx << "\n"; 
    }
    out << " mov" << suffix << " " << regAx << ", " << varOffset << "(%rbp)\n";
    
    out << " jmp loop_" << label << "\n";
    
    out << "endloop_" << label << ":\n";
    
    env.remove_level(); // End loop scope
    typeEnv.remove_level();
    offset = saved_offset;
    return 0;
}

int GenCodeVisitor::visit(FunDec* f) {
    entornoFuncion = true; // Set function context
    offset = -8;
    nombreFuncion = f->nombre;
    vector<std::string> argRegs = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"}; // Registros base
    out << ".globl " << f->nombre << endl;
    out <<f->nombre<<":" << endl;
    out << " pushq %rbp" << endl;
    out << " movq %rsp, %rbp" << endl;
    
    env.add_level(); // Scope for function arguments and local variables
    typeEnv.add_level();
    
    int size = f->Pnombres.size();
    for (int i = 0; i < size; i++) {
        Type* t = new Type();
        t->set_basic_type(f->Ptipos[i]);
        env.add_var(f->Pnombres[i], offset);
        typeEnv.add_var(f->Pnombres[i], t);
        
        int argSize = getTypeSize(t);
        string reg = getReg(argRegs[i], argSize);
        string suffix = getSuffix(argSize);

        if (i < argRegs.size()) {
            out << " mov" << suffix << " " << reg << "," << offset << "(%rbp)" << endl;
        } else {
            // Los argumentos en la pila son de 8 bytes en x86-64
            int arg_stack_pos = 16 + (i - (int)argRegs.size()) * 8; 
            out << " movq " << arg_stack_pos << "(%rbp), %rax\n"; // Leer 8 bytes de la pila
            // Guardar en variable local, posiblemente de menor tamaño
            string regAx = getReg("rax", argSize);
            out << " mov" << suffix << " " << regAx << ", " << offset << "(%rbp)" << endl;
        }
        offset -= 8;
    }
    
    // Calcular la reserva según la cantidad de variables
    int numVars = 0;
    if (functionVarCounts.count(f->nombre)) {
        numVars = functionVarCounts[f->nombre];
    } else {
        numVars = 16; // Valor por defecto
    }

    int reserva = (numVars * 8 + 15) / 16 * 16; // Redondear al múltiplo de 16
    
    out << " subq $" << reserva << ", %rsp" << endl;
    // Log de pila en entrada
    out << " leaq stack_fmt(%rip), %rdi\n";
    out << " movq %rsp, %rsi\n";
    out << " movq %rbp, %rdx\n";
    out << " movl $0, %eax\n";
    out << " call printf@PLT\n";
    
    f->cuerpo->accept(this);
    
    // Log de pila en main: imprime rsp y rbp reales
    out << " leaq stack_fmt(%rip), %rdi\n";
    out << " movq %rsp, %rsi\n";
    out << " movq %rbp, %rdx\n";
    out << " movl $0, %eax\n";
    out << " call printf@PLT\n";
    
    out << ".end_"<< f->nombre << ":"<< endl;
    out << "leave" << endl;
    out << "ret" << endl;
    env.remove_level(); // Remove argument scope
    typeEnv.remove_level();
    entornoFuncion = false;
    return 0;
}

int GenCodeVisitor::visit(ReturnStm* stm) {
    if (stm->e) {
        stm->e->accept(this); 
    }
    out << " leave\n";
    out << " ret\n";
    return 0;
}

int GenCodeVisitor::visit(FcallExp* exp) {
    if (exp->receiver) {
        // Evaluate receiver first
        exp->receiver->accept(this);

        Type* targetType = exp->inferredType;
        Type* sourceType = exp->receiver->inferredType;
        int targetSize = getTypeSize(targetType);
        int sourceSize = getTypeSize(sourceType);
        bool isUnsigned = targetType && (
            targetType->ttype == Type::UBYTE ||
            targetType->ttype == Type::USHORT ||
            targetType->ttype == Type::UINT ||
            targetType->ttype == Type::ULONG
        );
        bool isUnsignedSrc = sourceType && (
            sourceType->ttype == Type::UBYTE ||
            sourceType->ttype == Type::USHORT ||
            sourceType->ttype == Type::UINT ||
            sourceType->ttype == Type::ULONG
        );

        // Floating targets
        if (targetType && (targetType->ttype == Type::DOUBLE || targetType->ttype == Type::FLOAT)) {
            // Si la fuente ya es float/double, usar conversion adecuada
            if (sourceType && (sourceType->ttype == Type::DOUBLE || sourceType->ttype == Type::FLOAT)) {
                out << " movq %rax, %xmm0\n";
                if (sourceType->ttype == Type::DOUBLE && targetType->ttype == Type::FLOAT) {
                    out << " cvtsd2ss %xmm0, %xmm0\n";
                } else if (sourceType->ttype == Type::FLOAT && targetType->ttype == Type::DOUBLE) {
                    out << " cvtss2sd %xmm0, %xmm0\n";
                }
                out << " movq %xmm0, %rax\n";
                return 0;
            }

            // Fuente entera -> float/double
            if (isUnsignedSrc) {
                if (sourceSize == 1) out << " movzbq %al, %rax\n";
                else if (sourceSize == 2) out << " movzwq %ax, %rax\n";
                else out << " movl %eax, %eax\n";
            } else {
                if (sourceSize == 1) out << " movsbq %al, %rax\n";
                else if (sourceSize == 2) out << " movswq %ax, %rax\n";
                else out << " movslq %eax, %rax\n";
            }
            if (targetType->ttype == Type::DOUBLE) {
                out << " cvtsi2sdq %rax, %xmm0\n";
            } else {
                out << " cvtsi2ssq %rax, %xmm0\n";
            }
            out << " movq %xmm0, %rax\n";
            return 0;
        }

        // Fuente float/double -> entero
        if (sourceType && (sourceType->ttype == Type::DOUBLE || sourceType->ttype == Type::FLOAT) && !(targetType && (targetType->ttype == Type::DOUBLE || targetType->ttype == Type::FLOAT))) {
            out << " movq %rax, %xmm0\n";
            if (targetSize == 8) {
                out << (sourceType->ttype == Type::DOUBLE ? " cvttsd2siq %xmm0, %rax\n" : " cvttss2siq %xmm0, %rax\n");
            } else {
                out << (sourceType->ttype == Type::DOUBLE ? " cvttsd2si %xmm0, %eax\n" : " cvttss2si %xmm0, %eax\n");
            }
            // Ajustar tamaño destino
            if (targetSize == 1) out << " movsbq %al, %rax\n";
            else if (targetSize == 2) out << " movswq %ax, %rax\n";
            else if (targetSize == 4) out << " movslq %eax, %rax\n";
            return 0;
        }

        // Normalize value in RAX according to the destination type
        if (targetSize == 1) {
            out << (isUnsigned ? " movzbq %al, %rax\n" : " movsbq %al, %rax\n");
        } else if (targetSize == 2) {
            out << (isUnsigned ? " movzwq %ax, %rax\n" : " movswq %ax, %rax\n");
        } else if (targetSize == 4) {
            out << (isUnsigned ? " movl %eax, %eax\n" : " movslq %eax, %rax\n");
        } else { // targetSize == 8
            if (isUnsigned) {
                if (sourceSize == 1) out << " movzbq %al, %rax\n";
                else if (sourceSize == 2) out << " movzwq %ax, %rax\n";
                else out << " movl %eax, %eax\n";
            } else {
                if (sourceSize == 1) out << " movsbq %al, %rax\n";
                else if (sourceSize == 2) out << " movswq %ax, %rax\n";
                else out << " movslq %eax, %rax\n";
            }
        }
        return 0;
    }

    vector<std::string> argRegs = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"}; // Registros base
    int size = exp->argumentos.size();
    
    int num_stack_args = max(0, size - (int)argRegs.size());
    for (int i = size - 1; i >= (int)argRegs.size(); i--) {
        exp->argumentos[i]->accept(this);
        // Push is always 64-bit, so we must ensure RAX has the value.
        // If accept returned a byte in AL, we should probably zero-extend it if we want to be safe,
        // but pushq %rax pushes whatever is in RAX.
        // For stack arguments, the callee expects them at specific offsets.
        // If callee expects Byte, it reads 1 byte.
        out << " pushq %rax\n";
    }

    for (int i = 0; i < min(size, (int)argRegs.size()); i++) {
        exp->argumentos[i]->accept(this);
        int argSize = getTypeSize(exp->argumentos[i]->inferredType);
        string reg = getReg(argRegs[i], argSize);
        string regAx = getReg("rax", argSize);
        out << " mov" << getSuffix(argSize) << " " << regAx << ", " << reg <<endl;
    }

    out << " movl $0, %eax\n"; 
    out << "call " << exp->nombre << endl;

    if (num_stack_args > 0) {
        out << " addq $" << num_stack_args * 8 << ", %rsp\n";
    }
    
    return 0;
}
