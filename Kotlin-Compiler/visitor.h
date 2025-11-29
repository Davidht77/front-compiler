#ifndef VISITOR_H
#define VISITOR_H
#include "ast.h"
#include "environment.h" // Agregado
#include <list>
#include <vector>
#include <unordered_map>
#include <string>
#include <ostream> // Incluir para std::ostream
using namespace std;

class BinaryExp;
class NumberExp;
class BoolExp; 
class Program;
class PrintStm;
class WhileStmt; 
class IfStmt;    
class AssignExp; 
class Block;     
class VarDec;
class FcallExp;
class ReturnStm;
class FunDec;
class ForStmt;   
class IdExp;
class StringExp; // Declaración de StringExp
class DoubleExp; // Declaración de DoubleExp
class LongExp; // Declaración de DoubleExp
class Visitor {
public:
    virtual int visit(BinaryExp* exp) = 0;
    virtual int visit(NumberExp* exp) = 0;
    virtual int visit(DoubleExp* exp) = 0; // Método para DoubleExp
    virtual int visit(LongExp* exp) = 0; // Método para DoubleExp
    virtual int visit(BoolExp* exp) = 0; 
    virtual int visit(IdExp* exp) = 0;
    virtual int visit(StringExp* exp) = 0; // Método para StringExp
    virtual int visit(Program* p) = 0;
    virtual int visit(PrintStm* stm) = 0;
    virtual int visit(WhileStmt* stm) = 0;
    virtual int visit(IfStmt* stm) = 0;
    virtual int visit(AssignExp* stm) = 0; 
    virtual int visit(Block* block) = 0; 
    virtual int visit(VarDec* vd) = 0;
    virtual int visit(FcallExp* fcall) = 0;
    virtual int visit(ReturnStm* r) = 0;
    virtual int visit(FunDec* fd) = 0;
    virtual int visit(ForStmt* stm) = 0; 
};


class GenCodeVisitor : public Visitor {
private:
    std::ostream& out;
    unordered_map<string, int> functionVarCounts; // Agregado

public:
    GenCodeVisitor(std::ostream& out, unordered_map<string, int> counts) : out(out), functionVarCounts(counts) {}
    int generar(Program* program);

    // Contexto de generación de código
    Environment<int> env; // Memoria de offsets
    unordered_map<string, bool> memoriaGlobal;
    unordered_map<string, Type*> tiposGlobales;
    Environment<Type*> typeEnv; // Tipos locales
    unordered_map<string, string> stringLiterals; // Pool de strings y etiquetas
    int stringCont = 0; // Contador de etiquetas de strings
    int offset = -8;
    int labelcont = 0;
    bool entornoFuncion = false;
    string nombreFuncion;

    // Métodos de visita
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(DoubleExp* exp) override;
    int visit(LongExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(IdExp* exp) override;
    int visit(StringExp* exp) override;
    int visit(Program* p) override ;
    int visit(PrintStm* stm) override;
    int visit(WhileStmt* stm) override;
    int visit(IfStmt* stm) override;
    int visit(AssignExp* stm) override;
    int visit(Block* block) override;
    int visit(VarDec* vd) override;
    int visit(FcallExp* fcall) override;
    int visit(ReturnStm* r) override;
    int visit(FunDec* fd) override;
    int visit(ForStmt* stm) override;
};

#endif // VISITOR_H
