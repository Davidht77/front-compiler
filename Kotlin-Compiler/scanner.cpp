#include <iostream>
#include <cstring>
#include <fstream>
#include <cctype> // Incluir para isalnum/isalpha
#include "token.h"
#include "scanner.h"

using namespace std;

// -----------------------------
// Constructor
// -----------------------------
Scanner::Scanner(const char* s): input(s), first(0), current(0) { 
    }

// -----------------------------
// Función auxiliar
// -----------------------------

bool is_white_space(char c) {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

// -----------------------------
// nextToken: obtiene el siguiente token
// -----------------------------


Token* Scanner::nextToken() {
    Token* token;

    // 1. Saltar espacios en blanco
    while (current < input.length() && is_white_space(input[current])) 
        current++;

    // 2. Fin de la entrada
    if (current >= input.length()) 
        return new Token(Token::END);

    char c = input[current];
    first = current; // Guardar la posición de inicio del posible token

    // 3. Números (INT, LONG, FLOAT/DOUBLE)
    if (isdigit(c)) {
        Token::Type type = Token::NUM; // Por defecto: NUM (Int)
        
        // 1. Consumir parte entera
        while (current < input.length() && isdigit(input[current]))
            current++;
        
        // 2. Revisar punto decimal '.'
        bool is_float = false;
        // Si encontramos un punto ('.') Y:
        // a) Está seguido de un dígito (ej. 3.14) O
        // b) Es el final del número (ej. 3.0f), aunque en muchos lenguajes (Kotlin, Java) 3. es inválido.
        // Usaremos la convención estricta: punto seguido de dígito.
        if (current < input.length() && input[current] == '.') {
            // Marcamos como flotante. Asumimos que FLOAT_LIT cubre Float y Double.
            is_float = true; 
            type = Token::FLOAT_LIT; 
            current++; // Consumir el punto
            
            // Consumir dígitos decimales
            while (current < input.length() && isdigit(input[current]))
                current++;
        }
        
        // 3. Revisar sufijo (L/l para Long, F/f o D/d para Float/Double)
        if (current < input.length()) {
            char suffix = input[current];
            
            if (suffix == 'L' || suffix == 'l') {
                // Si NO era flotante (no tenía punto), lo marcamos como LONG_LIT.
                if (!is_float) {
                    type = Token::LONG_LIT; 
                    current++; // Consumir el sufijo
                }
                // Si ya era flotante (is_float == true), ignoramos 'L/l' o lo tratamos como error (depende de la gramática).
                // Por simplicidad, si ya es flotante, prevalece.
            } 
            // AÑADIDO: Manejo de sufijos F/f y D/d (Float/Double)
            else if (suffix == 'F' || suffix == 'f' || suffix == 'D' || suffix == 'd') {
                is_float = true; // Forzamos a que sea flotante si no tenía punto (ej. 100D)
                type = Token::FLOAT_LIT; 
                current++; // Consumir el sufijo
            }
        }
        
        // Si hubo punto decimal o un sufijo flotante, el tipo es FLOAT_LIT.
        // Si no hubo nada, se queda en NUM.
        token = new Token(type, input, first, current - first);
    }
    
    // 4. ID (Identificadores y Palabras Clave)
    else if (isalpha(c) || c == '_') {
        current++;
        while (current < input.length() && (isalnum(input[current]) || input[current] == '_'))
            current++;
        string lexema = input.substr(first, current - first);
        
        // Palabras clave (Keywords)
        if (lexema=="sqrt") return new Token(Token::SQRT, input, first, current - first);
        else if (lexema=="print") return new Token(Token::PRINT, input, first, current - first);
        else if (lexema=="println") return new Token(Token::PRINTLN, input, first, current - first);
        else if (lexema=="if") return new Token(Token::IF, input, first, current - first);
        else if (lexema=="while") return new Token(Token::WHILE, input, first, current - first);
        else if (lexema=="for") return new Token(Token::FOR, input, first, current - first); 
        else if (lexema=="in") return new Token(Token::IN, input, first, current - first);
        else if (lexema=="else") return new Token(Token::ELSE, input, first, current - first);
        else if (lexema=="var") return new Token(Token::VAR, input, first, current - first);
        else if (lexema=="val") return new Token(Token::VAL, input, first, current - first);
        else if (lexema=="const") return new Token(Token::CONST, input, first, current - first);
        else if (lexema=="true") return new Token(Token::TRUE, input, first, current - first);
        else if (lexema=="false") return new Token(Token::FALSE, input, first, current - first);
        else if (lexema=="fun") return new Token(Token::FUN, input, first, current - first);
        else if (lexema=="return") return new Token(Token::RETURN, input, first, current - first);
        else if (lexema=="downTo") return new Token(Token::DOWNTO, input, first, current - first);
        else if (lexema=="step") return new Token(Token::STEP, input, first, current - first);

        else return new Token(Token::ID, input, first, current - first);
    }
    
    // 5. Literal de Cadena (STRING_LIT)
    else if (c == '"') {
        current++; // Consumir la comilla inicial '"'
        first = current; // Iniciar la captura del contenido
        
        // Leer hasta encontrar la comilla de cierre '"'
        while (current < input.length() && input[current] != '"') {
            // Se puede añadir lógica para caracteres de escape aquí
            current++;
        }
        
        if (current < input.length() && input[current] == '"') {
            // String válido, capturar contenido
            token = new Token(Token::STRING_LIT, input, first, current - first);
            current++; // Consumir la comilla de cierre '"'
        } else {
            // Error: String no cerrado. Reportar error en la comilla inicial.
            // Usamos el carácter inicial que está en input[first]
            token = new Token(Token::ERR, input[first - 1]);
            // Dejar 'current' en la posición actual (final del input si no se encontró '"')
        }
        return token; // Retornar el token aquí ya que current fue gestionado
    }
    
    // 6. Operadores y delimitadores (Múltiples o simples)
    else if (strchr("+/-*();=<>,{}:%\'\"!&|.", c)) {
        
        switch (c) {
            case '<': 
                if (current + 1 < input.length() && input[current+1] == '=') {
                    current+=2; // <=
                    token = new Token(Token::LE, input, first, current - first);
                } else {
                    current++; // <
                    token = new Token(Token::LT, c); 
                }
                break;
            case '>': 
                if (current + 1 < input.length() && input[current+1] == '=') {
                    current+=2; // >=
                    token = new Token(Token::GE, input, first, current - first);
                } else {
                    current++; // >
                    token = new Token(Token::GT, c); 
                }
                break;
            case '=': 
                if (current + 1 < input.length() && input[current+1]=='=') {
                    current+=2; // ==
                    token = new Token(Token::EQ, input, first, current - first);
                } else {
                    current++; // =
                    token = new Token(Token::ASSIGN,c); 
                }
                break;
            case '!': 
                if (current + 1 < input.length() && input[current+1]=='=') {
                    current+=2; // !=
                    token = new Token(Token::NE, input, first, current - first);
                } else {
                    current++; // !
                    token = new Token(Token::NOT,c); 
                }
                break;
            case '&':
                if (current + 1 < input.length() && input[current+1]=='&') {
                    current+=2; // &&
                    token = new Token(Token::CONJ, input, first, current - first);
                } else {
                    current++; // &
                    token = new Token(Token::ERR, c); // Error: solo se admite &&
                }
                break;
            case '|':
                if (current + 1 < input.length() && input[current+1]=='|') {
                    current+=2; // ||
                    token = new Token(Token::DISJ, input, first, current - first);
                } else {
                    current++; // |
                    token = new Token(Token::ERR, c); // Error: solo se admite ||
                }
                break;
            case '*': 
                if (current + 1 < input.length() && input[current+1]=='*') {
                    current+=2; // **
                    token = new Token(Token::POW, input, first, current - first);
                }
                else{
                    current++; // *
                    token = new Token(Token::MUL, c);
                }
                break;
            case '+': current++; token = new Token(Token::PLUS, c); break;
            case '-': current++; token = new Token(Token::MINUS, c); break;
            case '/': current++; token = new Token(Token::DIV, c); break;
            case '%': current++; token = new Token(Token::MOD,c); break;
            case '(': current++; token = new Token(Token::LPAREN,c); break;
            case ')': current++; token = new Token(Token::RPAREN,c); break;
            case '{': current++; token = new Token(Token::LKEY,c); break;
            case '}': current++; token = new Token(Token::RKEY,c); break;
            case ';': current++; token = new Token(Token::SEMICOL,c); break;
            case ',': current++; token = new Token(Token::COMA,c); break;
            case ':': current++; token = new Token(Token::COLON,c); break;
            case '.': 
                if (current + 1 < input.length() && input[current+1] == '.') {
                    current+=2; // ..
                    token = new Token(Token::RANGE, input, first, current - first);
                } else {
                    current++; // .
                    token = new Token(Token::DOT,c); 
                }
                break;
            
            case '"': 
                // Ya se manejó arriba en el paso 5. Si llega aquí es redundante.
                // Lo dejamos para evitar warnings, pero el flujo correcto nunca llega aquí.
                current++;
                token = new Token(Token::DQM,c); 
                break;
            case '\'': 
                current++;
                token = new Token(Token::SQM,c); 
                break;
        }
    }

    // 7. Carácter inválido
    else {
        token = new Token(Token::ERR, c);
        current++;
    }

    return token;
}


// -----------------------------
// Destructor
// -----------------------------
Scanner::~Scanner() { }

// -----------------------------
// Función de prueba
// -----------------------------

int ejecutar_scanner(Scanner* scanner, const string& InputFile) {
    Token* tok;

    // Crear nombre para archivo de salida
    string OutputFileName = InputFile;
    size_t pos = OutputFileName.find_last_of(".");
    if (pos != string::npos) {
        OutputFileName = OutputFileName.substr(0, pos);
    }
    OutputFileName += "_tokens.txt";

    ofstream outFile(OutputFileName);
    if (!outFile.is_open()) {
        cerr << "Error: no se pudo abrir el archivo " << OutputFileName << endl;
        return 0;
    }

    outFile << "Scanner\n" << endl;

    while (true) {
        tok = scanner->nextToken();

        if (tok->type == Token::END) {
            outFile << *tok << endl;
            delete tok;
            outFile << "\nScanner exitoso" << endl << endl;
            outFile.close();
            return 0;
        }

        if (tok->type == Token::ERR) {
            outFile << *tok << endl;
            delete tok;
            outFile << "Caracter invalido" << endl << endl;
            outFile << "Scanner no exitoso" << endl << endl;
            outFile.close();
            return 0;
        }

        outFile << *tok << endl;
        delete tok;
    }
}
