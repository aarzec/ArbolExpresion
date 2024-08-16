#include <iostream>
#include <stack>
#include <string>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <stdexcept>
#include "./model/Nodo.h"
#include "./utils/Utilidades.h"

using namespace std;

bool isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

bool isFunction(const string& s) {
    return (
        s == "sen" || s == "sin" ||
        s == "cos" ||
        s == "tan" ||
        s == "raiz" || s == "sqrt"
    );
}

int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

string infixToPostfix(const string& infix) {
    stack<string> operators;
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
            operators.push(string(1, c));
        } else if (c == ')') {
            while (!operators.empty() && operators.top() != "(") {
                postfix += operators.top();
                postfix += ' ';
                operators.pop();
            }
            operators.pop();

            if (!operators.empty() && isFunction(operators.top())) {
                postfix += operators.top();
                postfix += ' ';
                operators.pop();
            }
        } else if (isOperator(c)) {
            while (!operators.empty() && precedence(operators.top()[0]) >= precedence(c)) {
                postfix += operators.top();
                postfix += ' ';
                operators.pop();
            }
            operators.push(string(1, c));
        } else if (isalpha(c)) {
            string func(1, c);
            while (i + 1 < infix.size() && isalpha(infix[i + 1])) {
                func += infix[++i];
            }

            if (isFunction(func)) {
                operators.push(func);
            } else {
                throw invalid_argument("Función desconocida: " + func);
            }
        } else {
            throw invalid_argument("Expresión inválida");
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
        } else if (isalpha(c)) {
            string func(1, c);
            while (i + 1 < postfix.size() && isalpha(postfix[i + 1])) {
                func += postfix[++i];
            }

            if (isFunction(func)) {
                Nodo* operand = nodeStack.top(); nodeStack.pop();
                Nodo* newNode = new Nodo(func);
                newNode->derecha = operand;
                nodeStack.push(newNode);
            }
        }
    }

    return nodeStack.top();
}

double evaluateExpressionTree(Nodo* root) {
    if (!root) throw invalid_argument("Árbol vacío");

    if (!root->izquierda && !root->derecha) {
        return stod(root->valor);
    }

    double valorIzquierda = root->izquierda ? evaluateExpressionTree(root->izquierda) : 0;
    double valorDerecha = root->derecha ? evaluateExpressionTree(root->derecha) : 0;

    if (root->valor == "+") return valorIzquierda + valorDerecha;
    if (root->valor == "-") return valorIzquierda - valorDerecha;
    if (root->valor == "*") return valorIzquierda * valorDerecha;
    if (root->valor == "/") return valorIzquierda / valorDerecha;

    if (root->valor == "sen" || root->valor == "sin") return std::sin(valorDerecha);
    if (root->valor == "cos") return std::cos(valorDerecha);
    if (root->valor == "tan") return std::tan(valorDerecha);
    if (root->valor == "raiz" || root->valor == "sqrt") return std::sqrt(valorDerecha);

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
        cout << "Expresión en notación postfija: " << postfixExpr << endl;
        Nodo* arbolExpresion = constructExpressionTree(postfixExpr);
        double result = evaluateExpressionTree(arbolExpresion);
        cout << "Resultado: " << result << endl;

        Nodo::imprimir(arbolExpresion, TipoRecorrido::IN_ORDEN);
        Nodo::visualizarArbol(arbolExpresion);

        borrarArbol(arbolExpresion);
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}
