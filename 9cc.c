#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//token kind
typedef enum {
   TK_RESERVED, //symbol
   TK_NUM,      //Integer
   TK_EOF,      //End of Input
} TokenKind;

typedef struct Token Token;
//Token type
struct Token {
   TokenKind kind;
   Token *next;
   int val;
   char *str;
};

// Current token
Token *token;

// Node kind for Abstract Grammer Tree 抽象構文木
typedef enum {
   ND_ADD, // +
   ND_SUB, // -
   ND_MUL, // *
   ND_DIV, // /
   ND_NUM, // Integer
} NodeKind;

typedef struct Node Node;
// Node type for Abstract Grammer Tree
struct Node {
   NodeKind kind;
   Node *lhs;
   Node *rhs;
   int val;
};

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
bool consume(char op) {
   if (token->kind != TK_RESERVED || token->str[0] != op) {
      return false;
   }
   token = token->next;
   return true;
}

//Error printing
//sam args of printf
void error(char *fmt, ...) {
   va_list ap;
   va_start(ap, fmt);
   vfprintf(stderr, fmt, ap);
   fprintf(stderr, "\n");
   exit(1);
}

//Input program
char *user_input;

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
void expect(char op) {
   if (token->kind != TK_RESERVED || token->str[0] != op) {
      //error("'%c' expected", op);
      error_at(token->str, "'%c' expected", op);
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
   if (consume('(')) {
      Node *node = expr();
      expect(')');
      return node;
   }
   return new_node_num(expect_number());
}

Node *unary() {
   if (consume('+')) {
      return primary();
   }
   if (consume('-')) {
      return new_node(ND_SUB, new_node_num(0), primary());
   }
   return primary();
}

Node *mul() {
   Node *node = unary();

   for(;;) {
      if (consume('*')) {
         node = new_node(ND_MUL, node, unary());
      } else if (consume('/')) {
         node = new_node(ND_DIV, node, unary()); 
      } else {
         return node;
      }
   }
}

Node *expr() {
   Node *node = mul();

   for (;;) {
      if (consume('+')) {
         node = new_node(ND_ADD, node, mul());
      } else if (consume('-')) {
         node = new_node(ND_SUB, node, mul());
      } else {
         return node;
      }
   }
}


void gen(Node *node) {
   if (node->kind == ND_NUM) {
      printf("   push %d\n", node->val);
      return;
   }

   gen(node->lhs);
   gen(node->rhs);

   printf("   pop rdi\n");
   printf("   pop rax\n");

   switch (node->kind) {
   case ND_ADD:
      printf("   add rax, rdi\n");
      break;
   case ND_SUB:
      printf("   sub rax, rdi\n");
      break;
   case ND_MUL:
      printf("   imul rax, rdi\n");
      break;
   case ND_DIV:
      printf("   cqo\n");
      printf("   idiv rdi\n");
      break;
   }

   printf(" push rax\n");
}

bool at_eof() {
   return token->kind == TK_EOF;
}

// create new_token
Token *new_token(TokenKind kind, Token *cur, char *str) {
   Token *tok = calloc(1, sizeof(Token));
   tok->kind = kind;
   tok->str = str;
   cur->next = tok;
   return tok;
}

// read next token
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

      if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
         cur = new_token(TK_RESERVED, cur, p++);
         continue;
      }

      if (isdigit(*p)) {
         cur = new_token(TK_NUM, cur, p);
         cur->val = strtol(p, &p, 10);
         continue;
      }

      error_at(p, "impossible tokenize");
      //error("impossible tokenize");
   }

   new_token(TK_EOF, cur, p);
   return head.next;
}


int main(int argc, char **argv) {
   if (argc != 2) {
      fprintf(stderr, "argc must be 2\n");
      return 1;
   }

   user_input = argv[1];
   token = tokenize(user_input);
   Node *node = expr();

   //print head of assenbler
   printf(".intel_syntax noprefix\n");
   printf(".globl main\n");
   printf("main:\n");

   gen(node);

   printf("   pop rax\n");
   printf("   ret\n");
   return 0;

   //// print first 'mov' instruction
   //printf("   mov rax, %d\n", expect_number());

   //// consume '+ number' or '- number'
   //while (!at_eof()) {
   //   if (consume('+')) {
   //      printf("   add rax, %d\n", expect_number());
   //      continue;
   //   }

   //   expect('-');
   //   printf("   sub rax, %d\n", expect_number());
   //}

   //printf("   ret\n");
   //return 0;
}
