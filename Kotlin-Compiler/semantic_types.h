#ifndef SEMANTIC_TYPES_H
#define SEMANTIC_TYPES_H

#include <iostream>
#include <string>
using namespace std;

// ===========================================================
//  Representación de tipos básicos del lenguaje
// ===========================================================

class Type {
public:
    enum TType { NOTYPE, VOID, INT, BOOL, STRING, RANGE, BYTE, SHORT, LONG, FLOAT, DOUBLE, UBYTE, USHORT, UINT, ULONG };
    static const char* type_names[15];

    TType ttype;

    Type() : ttype(NOTYPE) {}
    Type(TType tt) : ttype(tt) {}

    // Comparación de tipos
    bool match(Type* t) const {
        return this->ttype == t->ttype;
    }

    bool isNumeric() const {
        return ttype == INT || ttype == BYTE || ttype == SHORT || ttype == LONG ||
               ttype == UBYTE || ttype == USHORT || ttype == UINT || ttype == ULONG ||
               ttype == FLOAT || ttype == DOUBLE;
    }

    bool canAssignTo(Type* target) const {
        if (match(target)) return true;
        if (this->isNumeric() && target->isNumeric()) return true;
        return false;
    }

    // Asignación de tipo básico desde string
    bool set_basic_type(const string& s) {
        TType tt = string_to_type(s);
        if (tt == NOTYPE) return false;
        ttype = tt;
        return true;
    }

    // Conversión string 
    static TType string_to_type(const string& s) {
        if (s == "int" || s == "Int") return INT;
        if (s == "bool" || s == "Bool") return BOOL;
        if (s == "void" || s == "Void") return VOID;
        if (s == "string" || s == "String") return STRING;
        if (s == "Byte") return BYTE;
        if (s == "Short") return SHORT;
        if (s == "Long") return LONG;
        if (s == "Float") return FLOAT;
        if (s == "Double") return DOUBLE;
        if (s == "UByte") return UBYTE;
        if (s == "UShort") return USHORT;
        if (s == "UInt") return UINT;
        if (s == "ULong") return ULONG;
        return NOTYPE;
    }


};

inline const char* Type::type_names[15] = { "notype", "void", "Int", "bool", "string", "range", "byte", "short", "long", "float", "double", "ubyte", "ushort", "uint", "ulong" };

#endif // SEMANTIC_TYPES_H
