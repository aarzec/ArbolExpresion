#include <iostream>
#include <stack>
#include <string>
#include <cctype>
#include <cstdlib>
#include <stdexcept>
#include "./model/Nodo.h"
#include "./utils/Utilidades.h"

using namespace std;

bool isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

string infixToPostfix(const string& infix) {
    stack<char> operators;
    string postfix = "";

    for (size_t i = 0; i < infix.size(); ++i) {
        char c = infix[i];

        if (isspace(c)) {
            continue;
        }

        if (isdigit(c)) {
            postfix += c;
            while (i + 1 < infix.size() && isdigit(infix[i + 1])) {
                postfix += infix[++i];
            }
            postfix += ' ';
        } else if (c == '(') {
            operators.push(c);
        } else if (c == ')') {
            while (!operators.empty() && operators.top() != '(') {
                postfix += operators.top();
                postfix += ' ';
                operators.pop();
            }
            operators.pop();
        } else if (isOperator(c)) {
            while (!operators.empty() && precedence(operators.top()) >= precedence(c)) {
                postfix += operators.top();
                postfix += ' ';
                operators.pop();
            }
            operators.push(c);
        }
    }

    while (!operators.empty()) {
        postfix += operators.top();
        postfix += ' ';
        operators.pop();
    }

    return postfix;
}


Nodo* constructExpressionTree(const string& postfix) {
    stack<Nodo*> nodeStack;

    for (size_t i = 0; i < postfix.size(); ++i) {
        char c = postfix[i];

        if (isspace(c)) {
            continue;
        }

        if (isdigit(c)) {
            string number(1, c);
            while (i + 1 < postfix.size() && isdigit(postfix[i + 1])) {
                number += postfix[++i];
            }
            nodeStack.push(new Nodo(number));
        } else if (isOperator(c)) {
            Nodo* derecha = nodeStack.top(); nodeStack.pop();
            Nodo* izquierda = nodeStack.top(); nodeStack.pop();
            Nodo* newNode = new Nodo(string(1, c));
            newNode->izquierda = izquierda;
            newNode->derecha = derecha;
            nodeStack.push(newNode);
        }
    }

    return nodeStack.top();
}

int evaluateExpressionTree(Nodo* root) {
    if (!root) throw invalid_argument("Árbol vacío");

    if (!root->izquierda && !root->derecha) {
        return stoi(root->valor);
    }

    int valorIzquierda = evaluateExpressionTree(root->izquierda);
    int valorDerecha = evaluateExpressionTree(root->derecha);

    if (root->valor == "+") return valorIzquierda + valorDerecha;
    if (root->valor == "-") return valorIzquierda - valorDerecha;
    if (root->valor == "*") return valorIzquierda * valorDerecha;
    if (root->valor == "/") return valorIzquierda / valorDerecha;

    throw invalid_argument("Operador desconocido");
}

void borrarArbol(Nodo* root) {
    if (!root) return;
    borrarArbol(root->izquierda);
    borrarArbol(root->derecha);
    delete root;
}

int main() {
    string infixExpr;

    Utilidades::clearConsole();
    cout << "Introduce una expresión en notación infija: ";
    getline(cin, infixExpr);

    try {
        string postfixExpr = infixToPostfix(infixExpr);
        Nodo* arbolExpresion = constructExpressionTree(postfixExpr);
        int result = evaluateExpressionTree(arbolExpresion);
        cout << "Resultado: " << result << endl;

        Nodo::imprimir(arbolExpresion, TipoRecorrido::IN_ORDEN);
        Nodo::visualizarArbol(arbolExpresion);

        borrarArbol(arbolExpresion);
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}
