//codegen.c
#include "9cc.h"

static int depth;

static void push(void) {
  printf("  push %%rax\n");
  depth++;
}

static void pop(char *arg) {
  printf("  pop %s\n", arg);
  depth--;
}

// Compute the absolute address of a given node.
// It's an error if a given node does not reside in memory.
static void gen_addr(Node *node) {
  if (node->kind == ND_VAR) {
    int offset = (node->name - 'a' + 1) * 8;
    printf("  lea %d(%%rbp), %%rax\n", -offset);
    return;
  }

  error("not an lvalue");
}

// Generate code for a given node.
static void gen_expr(Node *node) {
  switch (node->kind) {
  case ND_NUM:
    printf("  mov $%d, %%rax\n", node->val);
    return;
  case ND_NEG:
    gen_expr(node->lhs);
    printf("  neg %%rax\n");
    return;
  case ND_VAR:
    gen_addr(node);
    printf("  mov (%%rax), %%rax\n");
    return;
  case ND_ASSIGN:
    gen_addr(node->lhs);
    push();
    gen_expr(node->rhs);
    pop("%rdi");
    printf("  mov %%rax, (%%rdi)\n");
    return;
  }

  gen_expr(node->rhs);
  push();
  gen_expr(node->lhs);
  pop("%rdi");

  switch (node->kind) {
  case ND_ADD:
    printf("  add %%rdi, %%rax\n");
    return;
  case ND_SUB:
    printf("  sub %%rdi, %%rax\n");
    return;
  case ND_MUL:
    printf("  imul %%rdi, %%rax\n");
    return;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv %%rdi\n");
    return;
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
    printf("  cmp %%rdi, %%rax\n");

    if (node->kind == ND_EQ)
      printf("  sete %%al\n");
    else if (node->kind == ND_NE)
      printf("  setne %%al\n");
    else if (node->kind == ND_LT)
      printf("  setl %%al\n");
    else if (node->kind == ND_LE)
      printf("  setle %%al\n");

    printf("  movzb %%al, %%rax\n");
    return;
  }

  error("invalid expression");
}

static void gen_stmt(Node *node) {
  if (node->kind == ND_EXPR_STMT) {
    gen_expr(node->lhs);
    return;
  }

  error("invalid statement");
}

void codegen(Node *node) {
  printf("  .globl main\n");
  printf("main:\n");

  // Prologue
  printf("  push %%rbp\n");
  printf("  mov %%rsp, %%rbp\n");
  printf("  sub $208, %%rsp\n");

  for (Node *n = node; n; n = n->next) {
    gen_stmt(n);
    assert(depth == 0);
  }

  printf("  mov %%rbp, %%rsp\n");
  printf("  pop %%rbp\n");
  printf("  ret\n");
}

////codegen.c
//
//#include <ctype.h>
//#include <stdarg.h>
//#include <stdbool.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include "9cc.h"
//
//void gen_lval(Node *node) {
//   if (node->kind != ND_LVAR) {
//      error("left node of assign is not variable");
//   }
//   printf("   mov rax, rbp\n");
//   printf("   sub rax %d\n", node->offset);
//   printf("   push rax\n");
//}
//
//void gen(Node *node) {
//   switch (node->kind) {
//   case ND_NUM:
//      printf("   push %d\n", node->val);
//      return;
//   case ND_LVAR:
//      gen_lval(node);
//      printf("   pop rax\n");
//      printf("   mov rax, [rax]\n");
//      printf("   push rax\n");
//      return;
//   case ND_ASSIGN:
//      gen_lval(node->lhs);
//      gen(node->lhs);
//      printf("   pop rdi\n");
//      printf("   pop rax\n");
//      printf("   mov [rax], rdi\n");
//      printf("   push rdi\n");
//      return;
//   }
//
//   gen(node->lhs);
//   gen(node->rhs);
//
//   printf("   pop rdi\n");
//   printf("   pop rax\n");
//
//   switch (node->kind) {
//   case ND_EQU:
//      printf("   cmp rax, rdi\n");
//      printf("   sete al\n");
//      printf("   movzb rax, al\n");
//      break;
//   case ND_NEQ:
//      printf("   cmp rax, rdi\n");
//      printf("   setne al\n");
//      printf("   movzb rax, al\n");
//      break;
//   case ND_LTH:
//      printf("   cmp rax, rdi\n");
//      printf("   setl al\n");
//      printf("   movzb rax, al\n");
//      break;
//   case ND_LEQ:
//      printf("   cmp rax, rdi\n");
//      printf("   setle al\n");
//      printf("   movzb rax, al\n");
//      break;
//   case ND_ADD:
//      printf("   add rax, rdi\n");
//      break;
//   case ND_SUB:
//      printf("   sub rax, rdi\n");
//      break;
//   case ND_MUL:
//      printf("   imul rax, rdi\n");
//      break;
//   case ND_DIV:
//      printf("   cqo\n");
//      printf("   idiv rdi\n");
//      break;
//   }
//
//   printf(" push rax\n");
//}
