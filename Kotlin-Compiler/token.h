#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ostream>

using namespace std;

class Token {
public:
    // Tipos de token
    enum Type {
        PLUS,    // +
        MINUS,   // -
        MUL,     // *
        DIV,     // /
        MOD,     // %
        POW,     // **
        LPAREN,  // (
        RPAREN,  // )
        LKEY,    // {
        RKEY,    // }
        SQRT,    // sqrt
        NUM,     // Número
        ERR,     // Error
        ID,      // ID
        LE,      // <=
        GE,      // >=
        EQ,      // ==
        NE,      // !=
        LT,      // <
        GT,      // >
        FUN,     // fun
        RETURN,  // return
        SEMICOL, // ;
        COLON,   // :
        DOT,    // .
        ASSIGN,  // =
        PRINT,   // print
        PRINTLN, // println
        IF,      // if
        WHILE,   // while
        FOR,     // for
        IN,      // in
        ELSE,    // else
        END,     // Fin de entrada
        VAR,     // var
        VAL,     // val
        CONST,   // const
        COMA,    // ,
        TRUE,    // true
        FALSE,   // false
        DQM,     // "
        SQM,     // '
        STRING_LIT, // "Hola"
        CONJ,    // &&
        DISJ,    // ||
        NOT,     // !
        RANGE,   // ..
        DOWNTO,  // downTo
        STEP,    // step
        FLOAT_LIT // 3.14
    };

    // Atributos
    Type type;
    string text;

    // Constructores
    Token(Type type);
    Token(Type type, char c);
    Token(Type type, const string& source, int first, int last);

    // Sobrecarga de operadores de salida
    friend ostream& operator<<(ostream& outs, const Token& tok);
    friend ostream& operator<<(ostream& outs, const Token* tok);
};

#endif // TOKEN_H