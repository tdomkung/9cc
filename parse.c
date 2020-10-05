#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"


// expr      = equality
// equality  = relational ("==" relatioonal | "!=" relational)*
// relational   = add ("<" add | "<=" add | ">" add | ">=" add)*
// add  = mul ("+" mul | "-" mul)*
// mul    = unary ("*" unuary | "/" unary)*
// unary  = ("+" | "-")? primary       ;temporary
// primary  = num | "(" expr ")"


Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
   Node *node = calloc(1, sizeof(Node));
   node->kind = kind;
   node->lhs = lhs;
   node->rhs = rhs;
   return node;
}

Node *new_node_num(int val) {
   Node *node = calloc(1, sizeof(Node));
   node->kind = ND_NUM;
   node->val = val;
   return node;
}


//consume next token
bool consume(char *op) {
   if (token->kind != TK_RESERVED || 
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len)) {
      return false;
   }
   token = token->next;
   return true;
}

//Error printing same args of printf
void error(char *fmt, ...) {
   va_list ap;
   va_start(ap, fmt);
   vfprintf(stderr, fmt, ap);
   fprintf(stderr, "\n");
   exit(1);
}

//print error position
void error_at(char *loc, char *fmt, ...) {
   va_list ap;
   va_start(ap, fmt);

   int pos = loc - user_input;
   fprintf(stderr, "%s\n", user_input);
   fprintf(stderr, "%*s", pos, ""); //print pos count space
   fprintf(stderr, "^ ");
   vfprintf(stderr, fmt, ap);
   fprintf(stderr, "\n");
   exit(1);
}


//fetch next token
void expect(char *op) {
   if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)) {
      //error("'%c' expected", op);
      error_at(token->str, "'%s' expected", op);
   }
   token = token->next;
}

//read integer
int expect_number() {
   if (token->kind != TK_NUM) {
      error_at(token->str, " Not a number");
      //error("Not a number");
   }
   int val = token->val;
   token = token->next;
   return val;
}




Node *expr();

Node *primary() {
   if (consume("(")) {
      Node *node = expr();
      expect(")");
      return node;
   }
   return new_node_num(expect_number());
}

Node *unary() {          //temporary
   if (consume("+")) {
      return primary();
   }
   if (consume("-")) {
      return new_node(ND_SUB, new_node_num(0), primary());
   }
   return primary();
}

Node *mul() {
   Node *node = unary();
   for(;;) {
      if (consume("*")) {
         node = new_node(ND_MUL, node, unary());
      } else if (consume("/")) {
         node = new_node(ND_DIV, node, unary()); 
      } else {
         return node;
      }
   }
}

Node *add() {
   Node *node = mul();

   for (;;) {
      if (consume("+")) {
         node = new_node(ND_ADD, node, mul());
      } else if (consume("-")) {
         node = new_node(ND_SUB, node, mul());
      } else {
         return node;
      }
   }
}

Node *relational() {
   Node *node = add();

   for (;;) {
      if (consume("<")) {
         node = new_node(ND_LTH, node, add());
      } else if (consume("<=")) {
         node = new_node(ND_LEQ, node, add());
      } else if (consume(">")) {
         return new_node(ND_LTH, add(), node);
         //node = new_node(ND_GTH, node, add());
      } else if (consume(">=")) {
         return new_node(ND_LEQ, add(), node);
         //node = new_node(ND_GEQ, node, add());
      } else {
         return node;
      }
   }
}

Node *equality() {
   Node *node = relational();

   for (;;) {
      if (consume("==")) {
         node = new_node(ND_EQU, node, relational());
      } else if (consume("!=")) {
         node = new_node(ND_NEQ, node, relational());
      } else {
         return node;
      }
   }
}

Node *expr() {
   Node *node = equality();
   return node;
}


bool at_eof() {
   return token->kind == TK_EOF;
}

// create new_token
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
   Token *tok = calloc(1, sizeof(Token));
   tok->kind = kind;
   tok->str = str;
   tok->len = len;
   cur->next = tok;
   return tok;
}

//+ - * / ( )
//== != < <= > >=
Token *tokenize(char *p) {
   Token head;
   head.next = NULL;
   Token *cur = &head;

   while (*p) {
      //skip space
      if (isspace(*p)) {
         p++;
         continue;
      }
      if ((*p == '<' && *(p+1) == '=') || (*p == '>' && *(p+1) == '=')
       || (*p == '=' && *(p+1) == '=') || (*p == '!' && *(p+1) == '=')) {
         cur = new_token(TK_RESERVED, cur, p, 2);
         p=p+2;
         continue;
      }


      if (*p == '+' || *p == '-' || *p == '*' || *p == '/'
       || *p == '<' || *p == '>'
       || *p == '(' || *p == ')') {
         cur = new_token(TK_RESERVED, cur, p++, 1);
         continue;
      }
      if (isdigit(*p)) {
         cur = new_token(TK_NUM, cur, p, 0);
         //char *t = p;
         cur->val = strtol(p, &p, 10);
         //cur->len = p-t;
         continue;
      }

      error_at(p, "impossible tokenize");
      //error("impossible tokenize");
   }

   new_token(TK_EOF, cur, p, 0);
   return head.next;
}


