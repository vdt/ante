#ifndef PTREE_H
#define PTREE_H

#include "parser.h"

//defined in lexer.cpp
extern char* lextxt;

Node* getRootNode(void);
Node* setRoot(Node* root);
Node* getRoot(void);
Node* setNext(Node* cur, Node* nxt);
Node* setElse(IfNode *c, IfNode *elif);

Node* mkIntLitNode(char* s);
Node* mkFltLitNode(char* s);
Node* mkStrLitNode(char* s);
Node* mkBoolLitNode(char b);
Node* mkModNode(TokenType mod);
Node* mkTypeNode(int type, char* typeName, Node *extTy = nullptr);
Node* mkBinOpNode(int op, Node* l, Node* r);
Node* mkNamedValNode(Node* nodes, Node* tExpr);
Node* mkFuncCallNode(char* s, Node* p);
Node* mkVarNode(char* s);
Node* mkRetNode(Node* expr);
Node* mkLetBindingNode(char* s, Node* mods, Node* tExpr, Node* expr);
Node* mkVarDeclNode(char* s, Node* mods, Node* tExpr, Node* expr);
Node* mkVarAssignNode(Node* var, Node* expr);

Node* mkIfNode(Node* con, Node* body, Node* els = nullptr);
Node* mkFuncDeclNode(char* s, Node* mods, Node* tExpr, Node* p, Node* body);
Node* mkDataDeclNode(char* s, Node* b);

#endif