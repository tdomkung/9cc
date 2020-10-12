#include "9cc.h"

static Node *expr(Token **rest, Token *tok);
static Node *expr_stmt(Token **rest, Token *tok);
static Node *assign(Token **rest, Token *tok);
static Node *equality(Token **rest, Token *tok);
static Node *relational(Token **rest, Token *tok);
static Node *add(Token **rest, Token *tok);
static Node *mul(Token **rest, Token *tok);
static Node *unary(Token **rest, Token *tok);
static Node *primary(Token **rest, Token *tok);

static Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

static Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

static Node *new_unary(NodeKind kind, Node *expr) {
  Node *node = new_node(kind);
  node->lhs = expr;
  return node;
}

static Node *new_num(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

static Node *new_var_node(char name) {
  Node *node = new_node(ND_VAR);
  node->name = name;
  return node;
}

// stmt = expr-stmt
static Node *stmt(Token **rest, Token *tok) {
  return expr_stmt(rest, tok);
}

// expr-stmt = expr ";"
static Node *expr_stmt(Token **rest, Token *tok) {
  Node *node = new_unary(ND_EXPR_STMT, expr(&tok, tok));
  *rest = skip(tok, ";");
  return node;
}

// expr = assign
static Node *expr(Token **rest, Token *tok) {
  return assign(rest, tok);
}

// assign = equality ("=" assign)?
static Node *assign(Token **rest, Token *tok) {
  Node *node = equality(&tok, tok);
  if (equal(tok, "="))
    node = new_binary(ND_ASSIGN, node, assign(&tok, tok->next));
  *rest = tok;
  return node;
}

// equality = relational ("==" relational | "!=" relational)*
static Node *equality(Token **rest, Token *tok) {
  Node *node = relational(&tok, tok);

  for (;;) {
    if (equal(tok, "==")) {
      node = new_binary(ND_EQ, node, relational(&tok, tok->next));
      continue;
    }

    if (equal(tok, "!=")) {
      node = new_binary(ND_NE, node, relational(&tok, tok->next));
      continue;
    }

    *rest = tok;
    return node;
  }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
static Node *relational(Token **rest, Token *tok) {
  Node *node = add(&tok, tok);

  for (;;) {
    if (equal(tok, "<")) {
      node = new_binary(ND_LT, node, add(&tok, tok->next));
      continue;
    }

    if (equal(tok, "<=")) {
      node = new_binary(ND_LE, node, add(&tok, tok->next));
      continue;
    }

    if (equal(tok, ">")) {
      node = new_binary(ND_LT, add(&tok, tok->next), node);
      continue;
    }

    if (equal(tok, ">=")) {
      node = new_binary(ND_LE, add(&tok, tok->next), node);
      continue;
    }

    *rest = tok;
    return node;
  }
}

// add = mul ("+" mul | "-" mul)*
static Node *add(Token **rest, Token *tok) {
  Node *node = mul(&tok, tok);

  for (;;) {
    if (equal(tok, "+")) {
      node = new_binary(ND_ADD, node, mul(&tok, tok->next));
      continue;
    }

    if (equal(tok, "-")) {
      node = new_binary(ND_SUB, node, mul(&tok, tok->next));
      continue;
    }

    *rest = tok;
    return node;
  }
}

// mul = unary ("*" unary | "/" unary)*
static Node *mul(Token **rest, Token *tok) {
  Node *node = unary(&tok, tok);

  for (;;) {
    if (equal(tok, "*")) {
      node = new_binary(ND_MUL, node, unary(&tok, tok->next));
      continue;
    }

    if (equal(tok, "/")) {
      node = new_binary(ND_DIV, node, unary(&tok, tok->next));
      continue;
    }

    *rest = tok;
    return node;
  }
}

// unary = ("+" | "-") unary
//       | primary
static Node *unary(Token **rest, Token *tok) {
  if (equal(tok, "+"))
    return unary(rest, tok->next);

  if (equal(tok, "-"))
    return new_unary(ND_NEG, unary(rest, tok->next));

  return primary(rest, tok);
}

// primary = "(" expr ")" | ident | num
static Node *primary(Token **rest, Token *tok) {
  if (equal(tok, "(")) {
    Node *node = expr(&tok, tok->next);
    *rest = skip(tok, ")");
    return node;
  }

  if (tok->kind == TK_IDENT) {
    Node *node = new_var_node(*tok->loc);
    *rest = tok->next;
    return node;
  }

  if (tok->kind == TK_NUM) {
    Node *node = new_num(tok->val);
    *rest = tok->next;
    return node;
  }

  error_tok(tok, "expected an expression");
}

// program = stmt*
Node *parse(Token *tok) {
  Node head = {};
  Node *cur = &head;
  while (tok->kind != TK_EOF)
    cur = cur->next = stmt(&tok, tok);
  return head.next;
}


//#include <ctype.h>
//#include <stdarg.h>
//#include <stdbool.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include "9cc.h"
//
//// program     = stmt*
//// stmt        = expr ";"
//// expr        = assign
//// assign      = equality ("=" assign)?
//// equality    = relational ("==" relatioonal | "!=" relational)*
//// relational  = add ("<" add | "<=" add | ">" add | ">=" add)*
//// add         = mul ("+" mul | "-" mul)*
//// mul         = unary ("*" unuary | "/" unary)*
//// unary       = ("+" | "-")? primary                              ;temporary
//// primary     = num | "(" expr ")"
//
//Node *code[100];
//
//Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
//   Node *node = calloc(1, sizeof(Node));
//   node->kind = kind;
//   node->lhs = lhs;
//   node->rhs = rhs;
//   return node;
//}
//
//Node *new_node_num(int val) {
//   Node *node = calloc(1, sizeof(Node));
//   node->kind = ND_NUM;
//   node->val = val;
//   return node;
//}
//
//
////consume next token
//bool consume(char *op) {
//   if (token->kind != TK_RESERVED || 
//      strlen(op) != token->len ||
//      memcmp(token->str, op, token->len)) {
//      return false;
//   }
//   token = token->next;
//   return true;
//}
//
//
////fetch next token
//void expect(char *op) {
//   if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)) {
//      //error("'%c' expected", op);
//      error_at(token->str, "'%s' expected", op);
//   }
//   token = token->next;
//}
//
//int expect_number() {
//   if (token->kind != TK_NUM) {
//      error_at(token->str, " Not a number");
//   }
//   int val = token->val;
//   token = token->next;
//   return val;
//}
//
//Token *consume_ident() {
//   Token *t = token;
//   if (token->kind != TK_IDENT) {
//      return NULL;
//   }
//   token = token->next;
//   return t;
//}
//
//
////--------------------------
//
//Node *primary() {
//   if (consume("(")) {
//      Node *node = expr();
//      expect(")");
//      return node;
//   }
//   Token *tok = consume_ident();
//   if (tok) {
//      Node *node = calloc(1, sizeof(Node));
//      node->kind = ND_LVAR;
//      node->offset = (tok->str[0] - 'a' +1)*8;
//      return node;
//   }
//   return new_node_num(expect_number());
//}
//
//Node *unary() {          //temporary
//   if (consume("+")) {
//      return primary();
//   }
//   if (consume("-")) {
//      return new_node(ND_SUB, new_node_num(0), primary());
//   }
//   return primary();
//}
//
//Node *mul() {
//   Node *node = unary();
//   for(;;) {
//      if (consume("*")) {
//         node = new_node(ND_MUL, node, unary());
//      } else if (consume("/")) {
//         node = new_node(ND_DIV, node, unary()); 
//      } else {
//         return node;
//      }
//   }
//}
//
//Node *add() {
//   Node *node = mul();
//
//   for (;;) {
//      if (consume("+")) {
//         node = new_node(ND_ADD, node, mul());
//      } else if (consume("-")) {
//         node = new_node(ND_SUB, node, mul());
//      } else {
//         return node;
//      }
//   }
//}
//
//Node *relational() {
//   Node *node = add();
//
//   for (;;) {
//      if (consume("<")) {
//         node = new_node(ND_LTH, node, add());
//      } else if (consume("<=")) {
//         node = new_node(ND_LEQ, node, add());
//      } else if (consume(">")) {
//         return new_node(ND_LTH, add(), node);
//         //node = new_node(ND_GTH, node, add());
//      } else if (consume(">=")) {
//         return new_node(ND_LEQ, add(), node);
//         //node = new_node(ND_GEQ, node, add());
//      } else {
//         return node;
//      }
//   }
//}
//
//Node *equality() {
//   Node *node = relational();
//
//   for (;;) {
//      if (consume("==")) {
//         node = new_node(ND_EQU, node, relational());
//      } else if (consume("!=")) {
//         node = new_node(ND_NEQ, node, relational());
//      } else {
//         return node;
//      }
//   }
//}
//
//Node *assign() {
//   Node *node = equality();
//   if (consume("="))
//      node = new_node(ND_ASSIGN, node, assign());
//}
//
//Node *expr() {
//   Node *node = assign();
//   return node;
//}
//
//Node *stmt() {
//   Node *node = expr();
//   expect(";");
//   return node;
//}
//
//bool at_eof() {
//   return token->kind == TK_EOF;
//}
//
//void program() {
//   int i=0;
//   while (!at_eof()) {
//      code[i++] = stmt();
//   }
//   code[i] = NULL;
//}
//
//
//// create new_token
//Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
//   print_NodeKind(kind);
//
//   Token *tok = calloc(1, sizeof(Token));
//   tok->kind = kind;
//   tok->str = str;
//   tok->len = len;
//   cur->next = tok;
//   return tok;
//}
//
//Token *tokenize() {
//   char *p = user_input;
//   Token head;
//   head.next = NULL;
//   Token *cur = &head;
//
//   while (*p) {
//      //skip space
//      if (isspace(*p)) {
//         p++;
//         continue;
//      }
//      if ((*p == '<' && *(p+1) == '=') || (*p == '>' && *(p+1) == '=')
//       || (*p == '=' && *(p+1) == '=') || (*p == '!' && *(p+1) == '=')) {
//         cur = new_token(TK_RESERVED, cur, p, 2);
//         p=p+2;
//         continue;
//      }
//
//      if ('a' <= *p && *p <= 'z') {
//         cur = new_token(TK_IDENT, cur, p++, 1);
//         cur->len = 1;
//         continue;
//      }
//
//      if (*p == '+' || *p == '-' || *p == '*' || *p == '/'
//       || *p == '<' || *p == '>'
//       || *p == '(' || *p == ')'
//       || *p == ';') {
//         cur = new_token(TK_RESERVED, cur, p++, 1);
//         continue;
//      }
//      if (isdigit(*p)) {
//         cur = new_token(TK_NUM, cur, p, 0);
//         //char *t = p;
//         cur->val = strtol(p, &p, 10);
//         //cur->len = p-t;
//         continue;
//      }
//
//      error_at(p, "impossible tokenize");
//      //error("impossible tokenize");
//   }
//
//   new_token(TK_EOF, cur, p, 0);
//   return head.next;
//}
//
//
//void print_NodeKind(NodeKind nk) {
//   switch (nk) {
//   case ND_NUM:     printf("ND_NUM");     break;  // Integer
//   case ND_MUL:     printf("ND_MUL");     break;  // *
//   case ND_DIV:     printf("ND_DIV");     break;  // /
//   case ND_ADD:     printf("ND_ADD");     break;  // +
//   case ND_SUB:     printf("ND_SUB");     break;  // -
//   case ND_LTH:     printf("ND_LTH");     break;  // <
//   case ND_LEQ:     printf("ND_LEQ");     break;  // <=
//   case ND_GTH:     printf("ND_GTH");     break;  // >
//   case ND_GEQ:     printf("ND_GEQ");     break;  // >=
//   case ND_EQU:     printf("ND_EQU");     break;  // ==
//   case ND_NEQ:     printf("ND_NEQ");     break;  // !=
//   case ND_ASSIGN:  printf("ND_ASSIGN");  break;  // =
//   case ND_LVAR:    printf("ND_LVAR");    break;  // a...z
//   }
//}
//
//void print_node(Node *nod) {
//   fprintf(stderr, "\n");
//}
