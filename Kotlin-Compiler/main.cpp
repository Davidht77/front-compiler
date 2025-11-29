#include <iostream>
#include <fstream>
#include <string>
#include "scanner.h"
#include "parser.h"
#include "ast.h"
#include "visitor.h"
#include "TypeChecker.h"

using namespace std;

int main(int argc, const char* argv[]) {
    // Verificar número de argumentos
    if (argc != 2) {
        cout << "Número incorrecto de argumentos.\n";
        cout << "Uso: " << argv[0] << " <archivo_de_entrada>" << endl;
        return 1;
    }

    // Abrir archivo de entrada
    ifstream infile(argv[1]);
    if (!infile.is_open()) {
        cout << "No se pudo abrir el archivo: " << argv[1] << endl;
        return 1;
    }

    // Leer contenido completo del archivo en un string
    string input, line;
    while (getline(infile, line)) {
        input += line + '\n';
    }
    infile.close();

    // Crear instancias de Scanner 
    Scanner scanner1(input.c_str());
    //ejecutar_scanner(&scanner1, argv[1]);
    cout << "Scanner exitoso" << endl;

    // Crear instancias de Parser
    Parser parser(&scanner1);
    cout << "Creacion parser exitoso" << endl;
    // Parsear y generar AST
    Program* program = parser.parseProgram();     
        cout << "PASS" << endl;
        string inputFile(argv[1]);
        cout << "PASS" << endl;
        size_t dotPos = inputFile.find_last_of('.');
        cout << "PASS" << endl;
        string baseName = (dotPos == string::npos) ? inputFile : inputFile.substr(0, dotPos);
        cout << "PASS" << endl;
        string outputFilename = baseName + ".s";
        cout << "PASS" << endl;
        ofstream outfile(outputFilename);
        cout << "PASS" << endl;
        if (!outfile.is_open()) {
            cerr << "Error al crear el archivo de salida: " << outputFilename << endl;
            return 1;
        }
    cout << "Parseo exitoso" << endl;

    // Revisión de tipos y conteo de variables
    cout << "Iniciando TypeChecker..." << endl;
    TypeChecker typeChecker;
    typeChecker.typecheck(program);
    cout << "TypeChecker finalizado." << endl;

    cout << "Generando codigo ensamblador en " << outputFilename << endl;
    GenCodeVisitor codigo(outfile, typeChecker.functionVarCounts); // Pass variable counts
    codigo.generar(program);
    outfile.close();
    
    return 0;
}
