/*
 *      ptree.cpp
 *  Provide a C API to be used in parser.c generated from
 *  syntax.y which creates and links nodes to a parse tree.
 */
#include "parser.h"
#include <stack>

stack<Node*> roots;

Node* ante::parser::getRootNode(){
    return roots.top();
}

/*
 *  Saves the root of a new block and returns it.
 */
Node* setRoot(Node* node){
    roots.push(node);
    return node;
}

/*
 *  Pops and returns the root of the current block
 */
Node* getRoot(){
    Node* ret = roots.top();
    roots.pop();
    return ret;
}

Node* setNext(Node* cur, Node* nxt){
    cur->next.reset(nxt);
    nxt->prev = cur;
    return nxt;
}

/*
 *  Sets the else of an ifnode to a given ifnode representing
 *  either an else or an elif.
 */
Node* setElse(IfNode *c, IfNode *elif){
    c->elseN.reset(elif);
    return elif;
}

Node* mkIntLitNode(char* s){
    string str = s;
    int type = Tok_I32;

    //check for type suffix
    int len = str.length();
    if(len > 2){
        if(len > 3 && (str[len -3] == 'u' || str[len - 3] == 'i')){
            char sign = str[len - 3];
            switch(str[len - 2]){
                case '1':
                    type = sign == 'i'? Tok_I16 : Tok_U16;
                    str = str.substr(0, len-3);
                    break;
                case '3':
                    type = sign == 'i'? Tok_I32 : Tok_U32;
                    str = str.substr(0, len-3);
                    break;
                case '6':
                    type = sign == 'i'? Tok_I64 : Tok_U64;
                    str = str.substr(0, len-3);
                    break;
                default:
                    break;
            }
        }else{
            char sign = str[len - 2];
            if(sign == 'u' || sign == 'i'){
                str = str.substr(0, len-2);
                type = sign == 'i'? Tok_I8 : Tok_U8;
            }
        }
    }

    auto* ret = new IntLitNode(str, type);
    ret->col = yylexer->getCol();
    ret->row = yylexer->getRow();
    return ret;
}

Node* mkFltLitNode(char* s){
    string str = s;
    int len = str.length();
    int type = Tok_F64;

    if(len > 3 && str[len - 3] == 'f'){
        char fltSize = str[len - 2];
        if(fltSize == '1'){ //16 bit IEEE half
            type = Tok_F16;
            str = str.substr(0, len-3);
        }else if(fltSize == '3'){ //32 bit IEEE single
            type = Tok_F32;
            str = str.substr(0, len-3);
        }else if(fltSize == '6'){ //64 bit IEEE double
            type = Tok_F64;
            str = str.substr(0, len-3);
        }
    }

    auto *ret = new FltLitNode(str, type);
    ret->col = yylexer->getCol();
    ret->row = yylexer->getRow();
    return ret;
}

Node* mkStrLitNode(char* s){
    auto *ret = new StrLitNode(s);
    ret->col = yylexer->getCol();
    ret->row = yylexer->getRow();
    return ret;
}

Node* mkBoolLitNode(char b){
    auto *ret = new BoolLitNode(b);
    ret->col = yylexer->getCol();
    ret->row = yylexer->getRow();
    return ret;
}

Node* mkModNode(TokenType mod){
    auto *ret = new ModNode(mod);
    ret->col = yylexer->getCol();
    ret->row = yylexer->getRow();
    return ret;
}

Node* mkTypeNode(int type, char* typeName, Node* extTy = nullptr){
    auto *ret = new TypeNode(type, typeName, static_cast<TypeNode*>(extTy));
    ret->col = yylexer->getCol();
    ret->row = yylexer->getRow();
    return ret;
}

Node* mkBinOpNode(int op, Node* l, Node* r){
    auto *ret = new BinOpNode(op, l, r);
    ret->col = yylexer->getCol();
    ret->row = yylexer->getRow();
    return ret;
}

Node* mkRetNode(Node* expr){
    auto *ret = new RetNode(expr);
    ret->col = yylexer->getCol();
    ret->row = yylexer->getRow();
    return ret;
}

/*
 *  This may create several NamedVal nodes depending on the
 *  number of VarNodes contained within varNodes.
 *  This is used for the shortcut when declaring multiple
 *  variables of the same type, e.g. i32 a b c
 */
Node* mkNamedValNode(Node* varNodes, Node* tExpr){
    //Note: there will always be at least one varNode
    const TypeNode* ty = (TypeNode*)tExpr;
    VarNode* vn = (VarNode*)varNodes;
    Node *ret = new NamedValNode(vn->name, tExpr);
    Node *nxt = ret;

    while((vn = (VarNode*)vn->next.get())){
        //FIXME: tExpr must be deep-copied
        TypeNode *tyNode = new TypeNode(ty->type, ty->typeName, nullptr);
        nxt->next.reset(new NamedValNode(vn->name, tyNode));
        nxt->next->prev = nxt;
        nxt = nxt->next.get();
        nxt->col = yylexer->getCol();
        nxt->row = yylexer->getRow();
    }
    delete varNodes;
    return ret;
}

Node* mkFuncCallNode(char* s, Node* p){
    auto *ret = new FuncCallNode(s, p);
    ret->col = yylexer->getCol();
    ret->row = yylexer->getRow();
    return ret;
}

Node* mkVarNode(char* s){
    auto *ret = new VarNode(s);
    ret->col = yylexer->getCol();
    ret->row = yylexer->getRow();
    return ret;
}

Node* mkLetBindingNode(char* s, Node* mods, Node* tExpr, Node* expr){
    auto *ret = new LetBindingNode(s, mods, tExpr, expr);
    ret->col = yylexer->getCol();
    ret->row = yylexer->getRow();
    return ret;
}

Node* mkVarDeclNode(char* s, Node* mods, Node* tExpr, Node* expr){
    auto *ret = new VarDeclNode(s, mods, tExpr, expr);
    ret->col = yylexer->getCol();
    ret->row = yylexer->getRow();
    return ret;
}

Node* mkVarAssignNode(Node* var, Node* expr){
    auto *ret = new VarAssignNode(var, expr);
    ret->col = yylexer->getCol();
    ret->row = yylexer->getRow();
    return ret;
}

ParentNode* mkIfNode(Node* con, Node* body, Node* els = nullptr){
    auto *ret = new IfNode(con, body, (IfNode*)els);
    ret->col = yylexer->getCol();
    ret->row = yylexer->getRow();
    return ret;
}

ParentNode* mkFuncDeclNode(char* s, Node* mods, Node* tExpr, Node* p, Node* b){
    auto *ret = new FuncDeclNode(s, mods, tExpr, p, b);
    ret->col = yylexer->getCol();
    ret->row = yylexer->getRow();
    return ret;
}

ParentNode* mkDataDeclNode(char* s, Node* b){
    auto *ret = new DataDeclNode(s, b);
    ret->col = yylexer->getCol();
    ret->row = yylexer->getRow();
    return ret;
}