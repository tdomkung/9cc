#include "9cc.h"

static char *current_input;

void error(char *fmt, ...) { //::: Reports an error and exit.
   va_list ap;
   va_start(ap, fmt);
   vfprintf(stderr, fmt, ap);
   fprintf(stderr, "\n");
   exit(1);
} //;;;
static void verror_at(char *loc, char *fmt, va_list ap) {  //::: Reports an error location and exit.
   int pos = loc - current_input;
   fprintf(stderr, "%s\n", current_input);
   fprintf(stderr, "%*s", pos, ""); // print pos spaces.
   fprintf(stderr, "^ ");
   vfprintf(stderr, fmt, ap);
   fprintf(stderr, "\n");
   exit(1);
} //;;;
void error_at(char *loc, char *fmt, ...) { //:::
   va_list ap;
   va_start(ap, fmt);
   verror_at(loc, fmt, ap);
} //;;;
void error_tok(Token *tok, char *fmt, ...) { //:::
   va_list ap;
   va_start(ap, fmt);
   verror_at(tok->loc, fmt, ap);
} //;;;

bool equal(Token *tok, char *op) { //::: Consumes the current token if it matches `op`.
   return memcmp(tok->loc, op, tok->len) == 0 && op[tok->len] == '\0';
} //;;;
Token *skip(Token *tok, char *op) { //::: Ensure that the current token is `op`.
   if (!equal(tok, op))
      error_tok(tok, "expected '%s'", op);
   return tok->next;
} //;;;

static Token *new_token(TokenKind kind, char *start, char *end) { //::: Create a new token.
   Token *tok = calloc(1, sizeof(Token));
   tok->kind = kind;
   tok->loc = start;
   tok->len = end - start;
   return tok;
} //;;;
static bool startswith(char *p, char *q) { //:::
   return strncmp(p, q, strlen(q)) == 0;
} //;;;

// Returns true if c is valid as the first character of an identifier.
static bool is_ident1(char c) {
   return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

// Returns true if c is valid as a non-first character of an identifier.
static bool is_ident2(char c) {
   return is_ident1(c) || ('0' <= c && c <= '9');
}


static int read_punct(char *p) { //::: Read a punctuator token from p and returns its length.
   if (startswith(p, "==") || startswith(p, "!=") ||
         startswith(p, "<=") || startswith(p, ">="))
      return 2;

   return ispunct(*p) ? 1 : 0;
} //;;;

Token *tokenize(char *p) { //::: Tokenize `current_input` and returns new tokens.
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
    if (is_ident1(*p)) {
      char *start = p;
      do {
        p++;
      } while (is_ident2(*p));
      cur = cur->next = new_token(TK_IDENT, start, p);
      continue;
    }
    //if ('a' <= *p && *p <= 'z') {
    //  cur = cur->next = new_token(TK_IDENT, p, p + 1);
    //  p++;
    //  continue;
    //}

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
} //;;;



