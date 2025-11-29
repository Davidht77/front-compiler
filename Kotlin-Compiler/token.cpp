#include <iostream>
#include "token.h"

using namespace std;

// -----------------------------
// Constructores
// -----------------------------

Token::Token(Type type) 
    : type(type), text("") { }

Token::Token(Type type, char c) 
    : type(type), text(string(1, c)) { }

Token::Token(Type type, const string& source, int first, int last) 
    : type(type), text(source.substr(first, last)) { }

// -----------------------------
// Sobrecarga de operador <<
// -----------------------------

// Para Token por referencia
ostream& operator<<(ostream& outs, const Token& tok) {
    switch (tok.type) {
        case Token::PLUS:   outs << "TOKEN(PLUS, \""   << tok.text << "\")"; break;
        case Token::MINUS:  outs << "TOKEN(MINUS, \""  << tok.text << "\")"; break;
        case Token::MUL:    outs << "TOKEN(MUL, \""    << tok.text << "\")"; break;
        case Token::DIV:    outs << "TOKEN(DIV, \""    << tok.text << "\")"; break;
        case Token::LPAREN:    outs << "TOKEN(LPAREN, \""    << tok.text << "\")"; break;
        case Token::RPAREN:    outs << "TOKEN(RPAREN, \""    << tok.text << "\")"; break;
        case Token::POW:    outs << "TOKEN(POW, \""    << tok.text << "\")"; break;
        case Token::SQRT:    outs << "TOKEN(SQRT, \""    << tok.text << "\")"; break;
        case Token::ID:     outs << "TOKEN(ID, \""     << tok.text << "\")"; break;
        case Token::NUM:    outs << "TOKEN(NUM, \""    << tok.text << "\")"; break;
        case Token::ERR:    outs << "TOKEN(ERR, \""    << tok.text << "\")"; break;
        case Token::LE:     outs << "TOKEN(LE, \""     << tok.text << "\")"; break;
        case Token::PRINT:  outs << "TOKEN(PRINT, \""  << tok.text << "\")"; break;
        case Token::SEMICOL: outs << "TOKEN(SEMICOL, \"" << tok.text << "\")"; break;
        case Token::ASSIGN: outs << "TOKEN(ASSIGN, \"" << tok.text << "\")"; break;
        case Token::IF:     outs << "TOKEN(IF, \""     << tok.text << "\")"; break;
        case Token::WHILE:  outs << "TOKEN(WHILE, \""  << tok.text << "\")"; break;
        case Token::ELSE:   outs << "TOKEN(ELSE, \""   << tok.text << "\")"; break;
        case Token::VAR:    outs << "TOKEN(VAR, \""    << tok.text << "\")"; break;
        case Token::COMA:   outs << "TOKEN(COMA, \""   << tok.text << "\")"; break;

        case Token::FUN:    outs << "TOKEN(FUN, \""    << tok.text << "\")"; break;
        case Token::RETURN: outs << "TOKEN(RETURN, \"" << tok.text << "\")"; break;

        case Token::GE:     outs << "TOKEN(GE, \""     << tok.text << "\")"; break;
        case Token::EQ:     outs << "TOKEN(EQ, \""     << tok.text << "\")"; break;
        case Token::NE:     outs << "TOKEN(NE, \""     << tok.text << "\")"; break;
        case Token::LT:     outs << "TOKEN(LT, \""     << tok.text << "\")"; break;
        case Token::GT:     outs << "TOKEN(GT, \""     << tok.text << "\")"; break;
        
        case Token::LKEY:   outs << "TOKEN(LKEY, \""   << tok.text << "\")"; break;
        case Token::RKEY:   outs << "TOKEN(RKEY, \""   << tok.text << "\")"; break;
        case Token::COLON:  outs << "TOKEN(COLON, \""  << tok.text << "\")"; break;
        case Token::TRUE:   outs << "TOKEN(TRUE, \""   << tok.text << "\")"; break;
        case Token::FALSE:  outs << "TOKEN(FALSE, \""  << tok.text << "\")"; break;
        case Token::PRINTLN: outs << "TOKEN(PRINTLN, \"" << tok.text << "\")"; break;
        case Token::MOD:    outs << "TOKEN(MOD, \""    << tok.text << "\")"; break;

        case Token::DQM:    outs << "TOKEN(DQM, \""    << tok.text << "\")"; break;
        case Token::SQM:    outs << "TOKEN(SQM, \""    << tok.text << "\")"; break;
        case Token::STRING_LIT: outs << "TOKEN(STRING_LIT, \"" << tok.text << "\")"; break; // <-- NUEVO
        case Token::FLOAT_LIT: outs << "TOKEN(FLOAT_LIT, \"" << tok.text << "\")"; break;
        case Token::CONJ:   outs << "TOKEN(CONJ, \""   << tok.text << "\")"; break;
        case Token::DISJ:   outs << "TOKEN(DISJ, \""   << tok.text << "\")"; break;
        case Token::NOT:    outs << "TOKEN(NOT, \""    << tok.text << "\")"; break;
        case Token::VAL:    outs << "TOKEN(VAL, \""    << tok.text << "\")"; break;
        case Token::CONST:  outs << "TOKEN(CONST, \""  << tok.text << "\")"; break;
        case Token::FOR:    outs << "TOKEN(FOR, \""    << tok.text << "\")"; break;
        case Token::IN:     outs << "TOKEN(IN, \""     << tok.text << "\")"; break;
        case Token::DOT:    outs << "TOKEN(DOT, \""    << tok.text << "\")"; break;
        case Token::LONG_LIT:    outs << "TOKEN(LONG_LIT, \""    << tok.text << "\")"; break;

        case Token::END:    outs << "TOKEN(END)"; break;
    }
    return outs;
}

// Para Token puntero
ostream& operator<<(ostream& outs, const Token* tok) {
    if (!tok) return outs << "TOKEN(NULL)";
    return outs << *tok;    // delega al otro
}