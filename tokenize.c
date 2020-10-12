#include "9cc.h"

static char *current_input;

// Reports an error and exit.
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// Reports an error location and exit.
static void verror_at(char *loc, char *fmt, va_list ap) {
  int pos = loc - current_input;
  fprintf(stderr, "%s\n", current_input);
  fprintf(stderr, "%*s", pos, ""); // print pos spaces.
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verror_at(loc, fmt, ap);
}

void error_tok(Token *tok, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verror_at(tok->loc, fmt, ap);
}

// Consumes the current token if it matches `op`.
bool equal(Token *tok, char *op) {
  return memcmp(tok->loc, op, tok->len) == 0 && op[tok->len] == '\0';
}

// Ensure that the current token is `op`.
Token *skip(Token *tok, char *op) {
  if (!equal(tok, op))
    error_tok(tok, "expected '%s'", op);
  return tok->next;
}

// Create a new token.
static Token *new_token(TokenKind kind, char *start, char *end) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->loc = start;
  tok->len = end - start;
  return tok;
}

static bool startswith(char *p, char *q) {
  return strncmp(p, q, strlen(q)) == 0;
}

// Read a punctuator token from p and returns its length.
static int read_punct(char *p) {
  if (startswith(p, "==") || startswith(p, "!=") ||
      startswith(p, "<=") || startswith(p, ">="))
    return 2;

  return ispunct(*p) ? 1 : 0;
}

// Tokenize `current_input` and returns new tokens.
Token *tokenize(char *p) {
  current_input = p;
  Token head = {};
  Token *cur = &head;

  while (*p) {
    // Skip whitespace characters.
    if (isspace(*p)) {
      p++;
      continue;
    }

    // Numeric literal
    if (isdigit(*p)) {
      cur = cur->next = new_token(TK_NUM, p, p);
      char *q = p;
      cur->val = strtoul(p, &p, 10);
      cur->len = p - q;
      continue;
    }

    // Identifier
    if ('a' <= *p && *p <= 'z') {
      cur = cur->next = new_token(TK_IDENT, p, p + 1);
      p++;
      continue;
    }

    // Punctuators
    int punct_len = read_punct(p);
    if (punct_len) {
      cur = cur->next = new_token(TK_PUNCT, p, p + punct_len);
      p += cur->len;
      continue;
    }

    error_at(p, "invalid token");
  }

  cur = cur->next = new_token(TK_EOF, p, p);
  return head.next;
}



// #include "9cc.h"
// 
// static char *current_input;
// 
// void error(char *fmt, ...) {
//    va_list ap;
//    va_start(ap, fmt);
//    vfprintf(stderr, fmt, ap);
//    fprintf(stderr, "\n");
//    exit(1);
// }
// 
// static void verror_at(char *loc, char *fmt, va_list ap) {
//   int pos = loc - current_input;
//   fprintf(stderr, "%s\n", current_input);
//   fprintf(stderr, "%*s", pos, ""); // print pos spaces.
//   fprintf(stderr, "^ ");
//   vfprintf(stderr, fmt, ap);
//   fprintf(stderr, "\n");
//   exit(1);
// }
// 
// void error_at(char *loc, char *fmt, ...) {
//    va_list ap;
//    va_start(ap, fmt);
//    verror_at(loc, fmt, ap);
//    //int pos = loc - user_input;
//    //fprintf(stderr, "%s\n", user_input);
//    //fprintf(stderr, "%*s", pos, ""); //print pos count space
//    //fprintf(stderr, "^ ");
//    //vfprintf(stderr, fmt, ap);
//    //fprintf(stderr, "\n");
//    //exit(1);
// }
// void  error_tok(Token *tok, char *fmt, ...) {
//    va_list ap;
//    va_start(ap,fmt);
//    verror_at(tok->loc, fmt, ap);
// }
// 
// // Consumes the current token if it matches `op`.
// bool equal(Token *tok, char *op) {
//   return memcmp(tok->loc, op, tok->len) == 0 && op[tok->len] == '\0';
// }
// 
// // Ensure that the current token is `op`.
// Token *skip(Token *tok, char *op) {
//   if (!equal(tok, op))
//     error_tok(tok, "expected '%s'", op);
//   return tok->next;
// }
// 
// // Create a new token.
// static Token *new_token(TokenKind kind, char *start, char *end) {
//   Token *tok = calloc(1, sizeof(Token));
//   tok->kind = kind;
//   tok->loc = start;
//   tok->len = end - start;
//   return tok;
// }
// 
// static bool startswith(char *p, char *q) {
//   return strncmp(p, q, strlen(q)) == 0;
// }
// 
// // Read a punctuator token from p and returns its length.
// static int read_punct(char *p) {
//   if (startswith(p, "==") || startswith(p, "!=") ||
//       startswith(p, "<=") || startswith(p, ">="))
//     return 2;
// 
//   return ispunct(*p) ? 1 : 0;
// }
// 
// // Tokenize `current_input` and returns new tokens.
// Token *tokenize(char *p) {
//   current_input = p;
//   Token head = {};
//   Token *cur = &head;
// 
//   while (*p) {
//     // Skip whitespace characters.
//     if (isspace(*p)) {
//       p++;
//       continue;
//     }
// 
//     // Numeric literal
//     if (isdigit(*p)) {
//       cur = cur->next = new_token(TK_NUM, p, p);
//       char *q = p;
//       cur->val = strtoul(p, &p, 10);
//       cur->len = p - q;
//       continue;
//     }
// 
//     // Identifier
//     if ('a' <= *p && *p <= 'z') {
//       cur = cur->next = new_token(TK_IDENT, p, p + 1);
//       p++;
//       continue;
//     }
// 
//     // Punctuators
//     int punct_len = read_punct(p);
//     if (punct_len) {
//       cur = cur->next = new_token(TK_PUNCT, p, p + punct_len);
//       p += cur->len;
//       continue;
//     }
// 
//     error_at(p, "invalid token");
//   }
// 
//   cur = cur->next = new_token(TK_EOF, p, p);
//   return head.next;
// }
// 
// 
// 
// Node *code[100];
// 
// Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
//    Node *node = calloc(1, sizeof(Node));
//    node->kind = kind;
//    node->lhs = lhs;
//    node->rhs = rhs;
//    return node;
// }
// 
// Node *new_node_num(int val) {
//    Node *node = calloc(1, sizeof(Node));
//    node->kind = ND_NUM;
//    node->val = val;
//    return node;
// }
// 
// 
// //consume next token
// bool consume(char *op) {
//    if (token->kind != TK_RESERVED || 
//       strlen(op) != token->len ||
//       memcmp(token->str, op, token->len)) {
//       return false;
//    }
//    token = token->next;
//    return true;
// }
// 
// 
// //fetch next token
// void expect(char *op) {
//    if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)) {
//       //error("'%c' expected", op);
//       error_at(token->str, "'%s' expected", op);
//    }
//    token = token->next;
// }
// 
// int expect_number() {
//    if (token->kind != TK_NUM) {
//       error_at(token->str, " Not a number");
//    }
//    int val = token->val;
//    token = token->next;
//    return val;
// }
// 
// Token *consume_ident() {
//    Token *t = token;
//    if (token->kind != TK_IDENT) {
//       return NULL;
//    }
//    token = token->next;
//    return t;
// }
// 
// 
// //--------------------------
// 
// Node *primary() {
//    if (consume("(")) {
//       Node *node = expr();
//       expect(")");
//       return node;
//    }
//    Token *tok = consume_ident();
//    if (tok) {
//       Node *node = calloc(1, sizeof(Node));
//       node->kind = ND_LVAR;
//       node->offset = (tok->str[0] - 'a' +1)*8;
//       return node;
//    }
//    return new_node_num(expect_number());
// }
// 
// Node *unary() {          //temporary
//    if (consume("+")) {
//       return primary();
//    }
//    if (consume("-")) {
//       return new_node(ND_SUB, new_node_num(0), primary());
//    }
//    return primary();
// }
// 
// Node *mul() {
//    Node *node = unary();
//    for(;;) {
//       if (consume("*")) {
//          node = new_node(ND_MUL, node, unary());
//       } else if (consume("/")) {
//          node = new_node(ND_DIV, node, unary()); 
//       } else {
//          return node;
//       }
//    }
// }
// 
// Node *add() {
//    Node *node = mul();
// 
//    for (;;) {
//       if (consume("+")) {
//          node = new_node(ND_ADD, node, mul());
//       } else if (consume("-")) {
//          node = new_node(ND_SUB, node, mul());
//       } else {
//          return node;
//       }
//    }
// }
// 
// Node *relational() {
//    Node *node = add();
// 
//    for (;;) {
//       if (consume("<")) {
//          node = new_node(ND_LTH, node, add());
//       } else if (consume("<=")) {
//          node = new_node(ND_LEQ, node, add());
//       } else if (consume(">")) {
//          return new_node(ND_LTH, add(), node);
//          //node = new_node(ND_GTH, node, add());
//       } else if (consume(">=")) {
//          return new_node(ND_LEQ, add(), node);
//          //node = new_node(ND_GEQ, node, add());
//       } else {
//          return node;
//       }
//    }
// }
// 
// Node *equality() {
//    Node *node = relational();
// 
//    for (;;) {
//       if (consume("==")) {
//          node = new_node(ND_EQU, node, relational());
//       } else if (consume("!=")) {
//          node = new_node(ND_NEQ, node, relational());
//       } else {
//          return node;
//       }
//    }
// }
// 
// Node *assign() {
//    Node *node = equality();
//    if (consume("="))
//       node = new_node(ND_ASSIGN, node, assign());
// }
// 
// Node *expr() {
//    Node *node = assign();
//    return node;
// }
// 
// Node *stmt() {
//    Node *node = expr();
//    expect(";");
//    return node;
// }
// 
// bool at_eof() {
//    return token->kind == TK_EOF;
// }
// 
// void program() {
//    int i=0;
//    while (!at_eof()) {
//       code[i++] = stmt();
//    }
//    code[i] = NULL;
// }
// 
// 
// // create new_token
// Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
//    print_NodeKind(kind);
// 
//    Token *tok = calloc(1, sizeof(Token));
//    tok->kind = kind;
//    tok->str = str;
//    tok->len = len;
//    cur->next = tok;
//    return tok;
// }
// 
// Token *tokenize() {
//    char *p = user_input;
//    Token head;
//    head.next = NULL;
//    Token *cur = &head;
// 
//    while (*p) {
//       //skip space
//       if (isspace(*p)) {
//          p++;
//          continue;
//       }
//       if ((*p == '<' && *(p+1) == '=') || (*p == '>' && *(p+1) == '=')
//        || (*p == '=' && *(p+1) == '=') || (*p == '!' && *(p+1) == '=')) {
//          cur = new_token(TK_RESERVED, cur, p, 2);
//          p=p+2;
//          continue;
//       }
// 
//       if ('a' <= *p && *p <= 'z') {
//          cur = new_token(TK_IDENT, cur, p++, 1);
//          cur->len = 1;
//          continue;
//       }
// 
//       if (*p == '+' || *p == '-' || *p == '*' || *p == '/'
//        || *p == '<' || *p == '>'
//        || *p == '(' || *p == ')'
//        || *p == ';') {
//          cur = new_token(TK_RESERVED, cur, p++, 1);
//          continue;
//       }
//       if (isdigit(*p)) {
//          cur = new_token(TK_NUM, cur, p, 0);
//          //char *t = p;
//          cur->val = strtol(p, &p, 10);
//          //cur->len = p-t;
//          continue;
//       }
// 
//       error_at(p, "impossible tokenize");
//       //error("impossible tokenize");
//    }
// 
//    new_token(TK_EOF, cur, p, 0);
//    return head.next;
// }
// 
// 
// void print_NodeKind(NodeKind nk) {
//    switch (nk) {
//    case ND_NUM:     printf("ND_NUM");     break;  // Integer
//    case ND_MUL:     printf("ND_MUL");     break;  // *
//    case ND_DIV:     printf("ND_DIV");     break;  // /
//    case ND_ADD:     printf("ND_ADD");     break;  // +
//    case ND_SUB:     printf("ND_SUB");     break;  // -
//    case ND_LTH:     printf("ND_LTH");     break;  // <
//    case ND_LEQ:     printf("ND_LEQ");     break;  // <=
//    case ND_GTH:     printf("ND_GTH");     break;  // >
//    case ND_GEQ:     printf("ND_GEQ");     break;  // >=
//    case ND_EQU:     printf("ND_EQU");     break;  // ==
//    case ND_NEQ:     printf("ND_NEQ");     break;  // !=
//    case ND_ASSIGN:  printf("ND_ASSIGN");  break;  // =
//    case ND_LVAR:    printf("ND_LVAR");    break;  // a...z
//    }
// }
// 
// void print_node(Node *nod) {
//    fprintf(stderr, "\n");
// }
