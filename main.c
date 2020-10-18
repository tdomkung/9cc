#include "9cc.h"

int main(int argc, char **argv) {
   if (argc != 2) {
      error("%s: invalid number of arguments", argv[0]);
   }

   Token *tok = tokenize(argv[1]);
   Function *prog = parse(tok);
   codegen(prog);

   //Node *node = parse(tok);
   //codegen(node);
   return 0;
}

