//codegen.c

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"


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
   case ND_EQU:
      printf("   cmp rax, rdi\n");
      printf("   sete al\n");
      printf("   movzb rax, al\n");
      break;
   case ND_NEQ:
      printf("   cmp rax, rdi\n");
      printf("   setne al\n");
      printf("   movzb rax, al\n");
      break;
   case ND_LTH:
      printf("   cmp rax, rdi\n");
      printf("   setl al\n");
      printf("   movzb rax, al\n");
      break;
   case ND_LEQ:
      printf("   cmp rax, rdi\n");
      printf("   setle al\n");
      printf("   movzb rax, al\n");
      break;
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
