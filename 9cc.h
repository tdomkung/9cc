// 9cc.h

extern char *user_input;


typedef enum {
   TK_RESERVED,
   TK_NUM,
   TK_EOF,
} TokenKind;

typedef struct Token Token;
struct Token {
   TokenKind kind;
   Token *next;
   int val;
   char *str;
   int len;
};
Token *tokenize(char *p);

extern Token *token;

// Node kind for Abstract Grammer Tree 抽象構文木
typedef enum {
   ND_NUM, // Integer
   ND_MUL, // *
   ND_DIV, // /
   ND_ADD, // +
   ND_SUB, // -
   ND_LTH, // <
   ND_LEQ, // <=
   ND_GTH, // >
   ND_GEQ, // >=
   ND_EQU, // ==
   ND_NEQ, // !=
} NodeKind;

typedef struct Node Node;
struct Node {
   NodeKind kind;
   Node *lhs;
   Node *rhs;
   int val;
};


void gen(Node *node);

Node *expr();
