#include "9cc.h"

int main(int argc, char **argv) {
  if (argc != 2)
    error("%s: invalid number of arguments", argv[0]);

  Token *tok = tokenize(argv[1]);
  Node *node = parse(tok);
  codegen(node);
  return 0;
}

// int main(int argc, char **argv) {
//    if (argc != 2) {
//       error("%s: invalid nunmber of arguments", argv[0]);
//       //fprintf(stderr, "argc must be 2\n");
//       //return 1;
//    }
// 
//    Token *tok = tokenize(argv[1]);
//    Node *node = Parse(tok);
//    codegen(node);
//    return 0;
// 
// 
// //   //print head of assenbler
// //   printf(".intel_syntax noprefix\n");
// //   printf(".globl main\n");
// //   printf("main:\n");
// //   printf("   mov rax, 0\n");
// //
// //
// //   printf("   push rbp\n");
// //   printf("   mov rbp, rsp\n");
// //   printf("   sub rsp, 208\n");   //208 = 26(a-z) x 8byte
// //
// //   for (int i=0; code[i]; i++) {
// //      gen(code[i]);
// //
// //      printf(" pop rax\n");
// //   }
// //
// //   //printf("   pop rax\n");
// //
// //   printf("   mov rsp, rbp\n");
// //   printf("   pop rbp\n");
// //   printf("   ret\n");
// //   return 0;
// }


