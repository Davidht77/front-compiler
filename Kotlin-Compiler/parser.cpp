#include <iostream>
#include "token.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"

const long long INT_MAX_32BIT = 2147483647LL;

using namespace std;

// =============================
// Métodos de la clase Parser
// =============================

Parser::Parser(Scanner* sc) : scanner(sc) {
    previous = nullptr;
    current = scanner->nextToken();
    if (current->type == Token::ERR) {
        throw runtime_error("Error léxico");
    }
}

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;

        if (check(Token::ERR)) {
            throw runtime_error("Error lexico");
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}


// =============================
// Reglas gramaticales
// =============================

Program* Parser::parseProgram() {
    Program* p = new Program();
    // VarDecList ::= (VarDec)*
    // Se revisa inicio de VarDec: const, val, var
    while (check(Token::CONST) || check(Token::VAL) || check(Token::VAR)) {
        p->vdlist.push_back(parseVarDec());
    }
    
    // FunDecList ::= (FunDec)+
    // Debe haber al menos una función
    if (check(Token::FUN)) {
        p->fdlist.push_back(parseFunDec());
        while (check(Token::FUN)) {
            p->fdlist.push_back(parseFunDec());
        }
    } else {
        if (!isAtEnd()) {
             throw runtime_error("Expected function declaration");
        }
    }
    
    cout << "Parser exitoso" << endl;
    return p;
}

VarDec* Parser::parseVarDec() {
    // VarDec ::= VarSymbol id TypeAnnotationOpt InitializerOpt StmtTerminator
    // VarSymbol ::= ("const" | ε) ("val" | "var")
    
    bool isConst = false;
    if (match(Token::CONST)) {
        isConst = true;
    }
    
    if (match(Token::VAL)) {
        if (!isConst) isConst = true; 
    } else if (match(Token::VAR)) {
        // var es mutable
    } else {
        throw runtime_error("Expected 'val' or 'var'");
    }
    
    if (!match(Token::ID)) throw runtime_error("Expected variable name");
    string name = previous->text;
    
    string type = "";
    // TypeAnnotationOpt ::= ":" Type | ε
    if (match(Token::COLON)) {
        if (!match(Token::ID)) throw runtime_error("Expected type");
        type = previous->text;
    }
    
    Exp* init = nullptr;
    // InitializerOpt ::= "=" Exp | ε
    if (match(Token::ASSIGN)) {
        init = parseExp();
    }
    
    // StmtTerminator ::= ";" | salto de línea (se usa SEMICOL)
    match(Token::SEMICOL);
    
    return new VarDec(name, type, init, isConst);
}

FunDec* Parser::parseFunDec() {
    // FunDec ::= "fun" id "(" ParamListOpt ")" TypeAnnotationOpt Block
    match(Token::FUN);
    
    if (!match(Token::ID)) throw runtime_error("Expected function name");
    string name = previous->text;
    
    match(Token::LPAREN);
    
    vector<string> pNames;
    vector<string> pTypes;
    
    // ParamListOpt ::= (ParamDec ("," ParamDec)*) | ε
    // ParamDec ::= VarSymbol id TypeAnnotationOpt
    if (!check(Token::RPAREN)) {
        // Leer el primer parámetro
        // VarSymbol
        bool paramConst = false;
        if (match(Token::CONST)) paramConst = true;
        if (match(Token::VAL)) { /* val */ }
        else if (match(Token::VAR)) { /* var */ }
        // si no: opcional, se asume val implícito
        
        if (!match(Token::ID)) throw runtime_error("Expected parameter name");
        pNames.push_back(previous->text);
        
        string pType = "";
        if (match(Token::COLON)) {
            if (!match(Token::ID)) throw runtime_error("Expected parameter type");
            pType = previous->text;
        }
        pTypes.push_back(pType);

        // Leer los parámetros restantes
        while (match(Token::COMA)){
            // VarSymbol
            paramConst = false;
            if (match(Token::CONST)) paramConst = true;
            if (match(Token::VAL)) { /* val */ }
            else if (match(Token::VAR)) { /* var */ }
            // si no: opcional
            
            if (!match(Token::ID)) throw runtime_error("Expected parameter name");
            pNames.push_back(previous->text);
            
            pType = "";
            if (match(Token::COLON)) {
                if (!match(Token::ID)) throw runtime_error("Expected parameter type");
                pType = previous->text;
            }
            pTypes.push_back(pType);
            
        }
    }
    
    match(Token::RPAREN);
    
    string returnType = "";
    if (match(Token::COLON)) {
        if (!match(Token::ID)) throw runtime_error("Expected return type");
        returnType = previous->text;
    }
    
    Block* body = parseBlock();
    
    return new FunDec(name, returnType, pTypes, pNames, body);
}

Block* Parser::parseBlock() {
    // Block ::= "{" StmtListOpt "}"
    match(Token::LKEY);
    Block* b = new Block();
    
    // StmtListOpt ::= StmtList | ε
    // StmtList ::= (Stmt)*
    while (!check(Token::RKEY) && !isAtEnd()) {
        b->stmts.push_back(parseStmt());
       }
    match(Token::RKEY);
    return b;
}

Stm* Parser::parseStmt() {
    Stm* s = nullptr;
    
    if (check(Token::CONST) || check(Token::VAL) || check(Token::VAR)) {
        s = parseVarDec();
    }
    else if (match(Token::PRINT) || match(Token::PRINTLN)) {
        // match(Token::PRINT) o match(Token::PRINTLN) ya consumieron el token
        match(Token::LPAREN);
        Exp* e = nullptr;
        if (!check(Token::RPAREN)) {
            e = parseExp();
        }
        match(Token::RPAREN);
        match(Token::SEMICOL);
        s = new PrintStm(e);
    }
    else if (match(Token::IF)) {
        match(Token::LPAREN);
        Exp* cond = parseExp();
        match(Token::RPAREN);
        Block* thenB = parseBlock();
        Block* elseB = nullptr;
        if (match(Token::ELSE)) {
            elseB = parseBlock();
        }
        s = new IfStmt(cond, thenB, elseB);
    }
    else if (match(Token::WHILE)) {
        match(Token::LPAREN);
        Exp* cond = parseExp();
        match(Token::RPAREN);
        Block* b = parseBlock();
        s = new WhileStmt(cond, b);
    }
    else if (match(Token::FOR)) {
        match(Token::LPAREN);
        if (!match(Token::ID)) throw runtime_error("Expected variable in for");
        string varName = previous->text;
        if (!match(Token::IN)) throw runtime_error("Expected 'in'");
        Exp* range = parseExp();
        match(Token::RPAREN);
        Block* b = parseBlock();
        s = new ForStmt(varName, range, b);
    }
    else if (match(Token::RETURN)) {
        Exp* e = nullptr;
        // ExpOpt
        if (!check(Token::SEMICOL)) {
             e = parseExp();
        }
        match(Token::SEMICOL);
        s = new ReturnStm(e);
    }
    else {
        // Exp (asignación u otra expresión)
        Exp* e = parseExp();
        if (!e) throw runtime_error("Expected statement or expression");
        match(Token::SEMICOL);
        s = e; // Exp ahora hereda de Stm
    }
    
    return s;
}

// En parser.cpp:

// Exp ::= Assignment
Exp* Parser::parseExp() {
    // 1. Parsea la expresión con la siguiente precedencia (LogicOr).
    Exp* l = parseLogicOr();
    
    // 2. Verifica si la expresión fue seguida por el operador de asignación.
    if (match(Token::ASSIGN)) {
        
        // 3. Verifica si el lado izquierdo (l) es un IdExp válido para la asignación (l-value).
        IdExp* idExp = dynamic_cast<IdExp*>(l);
        
        if (!idExp) {
            throw runtime_error("Invalid assignment target: Left side must be an ID.");
        }
        
        string name = idExp->value;
        delete l; // Reemplazamos el IdExp por un AssignExp
        
        // Llamada recursiva para el lado derecho (soporte para asignación en cascada)
        Exp* r = parseExp(); 
        
        return new AssignExp(name, r); 
    }
    
    return l;
}

// LogicOr ::= LogicAnd ("||" LogicAnd)*
Exp* Parser::parseLogicOr() {
    Exp* l = parseLogicAnd();
    while (match(Token::DISJ)) {
        Exp* r = parseLogicAnd();
        l = new BinaryExp(l, r, OR_OP);
    }
    return l;
}

// LogicAnd ::= Equality ("&&" Equality)*
Exp* Parser::parseLogicAnd() {
    Exp* l = parseEquality();
    while (match(Token::CONJ)) {
        Exp* r = parseEquality();
        l = new BinaryExp(l, r, AND_OP);
    }
    return l;
}

// Equality ::= Relational (("=="|"!=") Relational)*
Exp* Parser::parseEquality() {
    Exp* l = parseRelational();
    while (check(Token::EQ) || check(Token::NE)) {
        BinaryOp op = check(Token::EQ) ? EQ_OP : NE_OP;
        advance();
        Exp* r = parseRelational();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

// Relational ::= Range (("<"|">"|"<="|">=") Range)*
Exp* Parser::parseRelational() {
    Exp* l = parseRange();
    while (check(Token::LT) || check(Token::GT) || check(Token::LE) || check(Token::GE)) {
        BinaryOp op;
        if (check(Token::LT)) op = LT_OP;
        else if (check(Token::GT)) op = GT_OP;
        else if (check(Token::LE)) op = LE_OP;
        else op = GE_OP;
        advance();
        Exp* r = parseRange();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

// Range ::= Additive ((".." | "downTo") Additive)*
Exp* Parser::parseRange() {
    Exp* l = parseAdditive();
    while (check(Token::RANGE) || check(Token::DOWNTO)) {
        BinaryOp op = check(Token::RANGE) ? RANGE_OP : DOWNTO_OP;
        advance();
        Exp* r = parseAdditive();
        l = new BinaryExp(l, r, op);
    }
    // Check for 'step' after range
    if (check(Token::STEP)) {
        advance();
        Exp* stepVal = parseAdditive();
        l = new BinaryExp(l, stepVal, STEP_OP);
    }
    return l;
}

// Additive ::= Multiplicative (("+"|"-") Multiplicative)*
Exp* Parser::parseAdditive() {
    Exp* l = parseMultiplicative();
    while (check(Token::PLUS) || check(Token::MINUS)) {
        BinaryOp op = check(Token::PLUS) ? PLUS_OP : MINUS_OP;
        advance();
        Exp* r = parseMultiplicative();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

// Multiplicative ::= Unary (("*"|"/"|"%") Unary)*
Exp* Parser::parseMultiplicative() {
    Exp* l = parseUnary();
    while (check(Token::MUL) || check(Token::DIV) || check(Token::MOD)) {
        BinaryOp op;
        if (check(Token::MUL)) op = MUL_OP;
        else if (check(Token::DIV)) op = DIV_OP;
        else op = MOD_OP;
        advance();
        Exp* r = parseUnary();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

// Unary ::= ("+"|"-"|"!") Unary | Primary
Exp* Parser::parseUnary() {
    if (match(Token::PLUS)) {
        return parseUnary(); // Unary + is no-op
    } else if (match(Token::MINUS)) {
        Exp* e = parseUnary();
        // Generar 0 - e como una BinaryExp
        return new BinaryExp(new NumberExp(0), e, MINUS_OP); 
    } else if (match(Token::NOT)) {
        Exp* e = parseUnary();
        // Generar e == false como una BinaryExp
        return new BinaryExp(e, new BoolExp(false), EQ_OP); 
    }
    return parsePrimary();
}

// Dentro de parser.cpp

Exp* Parser::parsePrimary() {
    Exp* expr = nullptr;

    // 1. Literales numéricos
    // Nota: El scanner ya diferencia NUM (Int), LONG_LIT, y FLOAT_LIT.

    // A. Entero (32 bits): Token::NUM
    if (match(Token::NUM)) {
        string text = previous->text;
        
        try {
            // Usamos stoll (string to long long) para verificar el rango antes de convertir a int.
            long long val = stoll(text); 

            // Verificación de rango estricta para Int (32 bits)
            // Si el valor no cabe en un int de 32 bits, lanzamos un error o lo dejamos como Long.
            // Aquí elegimos lanzar un error si supera el rango de INT.
            if (val > INT_MAX_32BIT || val < -INT_MAX_32BIT - 1LL) {
                // Si la gramática permite Int de 64 bits por defecto, cambia esto a: 
                // return new LongExp(val);
                throw runtime_error("Error de rango: Literal entero fuera del rango de 32 bits: " + previous->text);
            }

            expr = new NumberExp((int)val); // Crea el nodo NumberExp (Int de 32 bits)
        } catch (const std::out_of_range& e) {
            throw runtime_error("Error de rango: Literal entero demasiado grande: " + previous->text);
        } catch (const std::invalid_argument& e) {
            throw runtime_error("Argumento invalido para literal entero: " + previous->text);
        }
    }
    
    // B. Long (64 bits): Token::LONG_LIT
    else if (match(Token::LONG_LIT)) {
        string text = previous->text; 
        
        // Eliminar el sufijo 'L' o 'l' al final
        if (text.length() > 0 && (text.back() == 'L' || text.back() == 'l')) {
            text.pop_back(); 
        }
        
        try {
            long long val = stoll(text); // Conversión a long long (64 bits)
            expr = new LongExp(val);     // Crea el nodo LongExp
        } catch (const std::out_of_range& e) {
            throw runtime_error("Error de rango: Literal Long fuera de rango: " + previous->text);
        } catch (const std::invalid_argument& e) {
            throw runtime_error("Argumento invalido para literal Long: " + previous->text);
        }
    }
    
    // C. Flotante / Double (64 bits): Token::FLOAT_LIT
    else if (match(Token::FLOAT_LIT)) {
        string text = previous->text;
        
        // Eliminar el sufijo 'F', 'f', 'D' o 'd' si existe
        char lastChar = text.empty() ? ' ' : text.back();
        if (lastChar == 'F' || lastChar == 'f' || lastChar == 'D' || lastChar == 'd') {
            text.pop_back(); 
        }
        
        try {
            double val = stod(text); // Conversión a double (64 bits)
            expr = new DoubleExp(val); // Crea el nodo DoubleExp
        } catch (const std::out_of_range& e) {
            throw runtime_error("Error de rango: Literal flotante fuera de rango: " + previous->text);
        } catch (const std::invalid_argument& e) {
            throw runtime_error("Argumento invalido para literal flotante: " + previous->text);
        }
    }
    
    // 2. Otros literales
    else if (match(Token::TRUE)) {
        expr = new BoolExp(true);
    } else if (match(Token::FALSE)) {
        expr = new BoolExp(false);
    } else if (match(Token::STRING_LIT)) {
        // La cadena almacenada en previous->text ya está limpia (sin comillas),
        // gracias a la lógica que incluiste en el scanner.
        expr = new StringExp(previous->text);
    }
    
    // 3. Agrupación (paréntesis)
    else if (match(Token::LPAREN)) {
        expr = parseExp();
        if (!match(Token::RPAREN)) throw runtime_error("Se esperaba ')' después de la expresión agrupada.");
    }
    
    // 4. Identificador (inicio de expresión ID o ID())
    else if (match(Token::ID)) {
        expr = new IdExp(previous->text);
    }
    
    // 5. Error
    else {
        throw runtime_error("Se esperaba una expresión primaria (literal, ID, o '()')");
    }

    // =========================================================================
    // 6. Postfijos: Llamada de función o método (ej. foo(), 100.toByte())
    // Esta parte es la que ya tenías en tu snippet
    // =========================================================================
    while (true) {
        if (check(Token::LPAREN)) {
            match(Token::LPAREN);
            vector<Exp*> args;
            
            // Si no está seguido de un ')' inmediato, parsea argumentos
            if (!check(Token::RPAREN)) {
                do {
                    args.push_back(parseExp());
                } while (match(Token::COMA));
            }
            
            if (!match(Token::RPAREN)) throw runtime_error("Se esperaba ')' después de los argumentos de la función");

            // Esto asume que el identificador (IdExp) se parseó justo antes
            IdExp* id = dynamic_cast<IdExp*>(expr);
            if (!id) throw runtime_error("Solo se pueden llamar identificadores directamente.");
            
            // El expr actual (IdExp) se convierte en la llamada a función (FcallExp)
            expr = new FcallExp(id->value, args);
        }
        else if (match(Token::DOT)) {
            // Manejo de métodos (ej. 10.toLong())
            if (!match(Token::ID)) throw runtime_error("Se esperaba un identificador de método después de '.'");
            string methodName = previous->text;

            vector<Exp*> args;
            // Los métodos pueden tener paréntesis para argumentos (o no si no tienen args)
            if (match(Token::LPAREN)) {
                if (!check(Token::RPAREN)) {
                    do {
                        args.push_back(parseExp());
                    } while (match(Token::COMA));
                }
                if (!match(Token::RPAREN)) throw runtime_error("Se esperaba ')' después de los argumentos del método");
            }
            
            // El expr actual (que puede ser NumberExp, LongExp, etc.) se convierte en el 'receiver' (receptor)
            // del método/llamada a función (FcallExp)
            Exp* receiver = expr; 
            expr = new FcallExp(methodName, args, receiver);
        }
        else {
            break; // No hay más postfijos
        }
    }

    return expr;
}