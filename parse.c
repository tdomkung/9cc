#include "9cc.h"

Obj *locals; // All local variable instances created during parsing are accumulated to this list.

static Node *expr      (Token **rest, Token *tok);
static Node *expr_stmt (Token **rest, Token *tok);
static Node *assign    (Token **rest, Token *tok);
static Node *equality  (Token **rest, Token *tok);
static Node *relational(Token **rest, Token *tok);
static Node *add       (Token **rest, Token *tok);
static Node *mul       (Token **rest, Token *tok);
static Node *unary     (Token **rest, Token *tok);
static Node *primary   (Token **rest, Token *tok);


static Obj *find_var(Token *tok) { //::: Find a local variable by name.
   for (Obj *var = locals; var; var = var->next)
      if (strlen(var->name) == tok->len && !strncmp(tok->loc, var->name, tok->len))
         return var;
   return NULL;
} //;;;


static Node *new_node  (NodeKind kind) { //:::
   Node *node = calloc(1, sizeof(Node));
   node->kind = kind;
   return node;
} //;;;
static Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) { //:::
   Node *node = new_node(kind);
   node->lhs = lhs;
   node->rhs = rhs;
   return node;
} //;;;
static Node *new_unary (NodeKind kind, Node *expr) { //:::
   Node *node = new_node(kind);
   node->lhs = expr;
   return node;
} //;;;
static Node *new_num   (int val) { //:::
   Node *node = new_node(ND_NUM);
   node->val = val;
   return node;
} //;;;
static Node *new_var_node(Obj *var) { //:::
   Node *node = new_node(ND_VAR);
   node->var = var;
   return node;
} //;;;

static Obj *new_lvar(char *name) { //:::
  Obj *var = calloc(1, sizeof(Obj));
  var->name = name;
  var->next = locals;
  locals = var;
  return var;
} //;;;


static Node *stmt      (Token **rest, Token *tok) {  //::: stmt = expr-stmt
   return expr_stmt(rest, tok);
} //;;;
static Node *expr_stmt (Token **rest, Token *tok) {  //::: expr-stmt = expr ";"
   Node *node = new_unary(ND_EXPR_STMT, expr(&tok, tok));
   *rest = skip(tok, ";");
   return node;
} //;;;
static Node *expr      (Token **rest, Token *tok) {  //::: expr = assign
   return assign(rest, tok);
} //;;;
static Node *assign    (Token **rest, Token *tok) {  //::: assign = equality ("=" assign)?
   Node *node = equality(&tok, tok);
   if (equal(tok, "="))
      node = new_binary(ND_ASSIGN, node, assign(&tok, tok->next));
   *rest = tok;
   return node;
} //;;;
static Node *equality  (Token **rest, Token *tok) {  //::: equality = relational ("==" relational | "!=" relational)*
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
} //;;;
static Node *relational(Token **rest, Token *tok) {  //::: relational = add ("<" add | "<=" add | ">" add | ">=" add)*
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
} //;;;
static Node *add       (Token **rest, Token *tok) {  //::: add = mul ("+" mul | "-" mul)*
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
} //;;;
static Node *mul       (Token **rest, Token *tok) {  //::: mul = unary ("*" unary | "/" unary)*
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
} //;;;
static Node *unary     (Token **rest, Token *tok) {  //::: unary = ("+" | "-") unary    |    primary
   if (equal(tok, "+"))
      return unary(rest, tok->next);

   if (equal(tok, "-"))
      return new_unary(ND_NEG, unary(rest, tok->next));

   return primary(rest, tok);
} //;;;
static Node *primary   (Token **rest, Token *tok) {  //::: primary = "(" expr ")" | ident | num
   if (equal(tok, "(")) {
      Node *node = expr(&tok, tok->next);
      *rest = skip(tok, ")");
      return node;
   }

   if (tok->kind == TK_IDENT) {
      Obj *var = find_var(tok);
      if (!var) {
         var = new_lvar(strndup(tok->loc, tok->len));
      }
      *rest = tok->next;
      return new_var_node(var);

      //Node *node = new_var_node(*tok->loc);
      //*rest = tok->next;
      //return node;
   }

   if (tok->kind == TK_NUM) {
      Node *node = new_num(tok->val);
      *rest = tok->next;
      return node;
   }

   error_tok(tok, "expected an expression");
} //;;;


Function *parse(Token *tok) { //:::
  Node head = {};
  Node *cur = &head;

  while (tok->kind != TK_EOF)
    cur = cur->next = stmt(&tok, tok);

  Function *prog = calloc(1, sizeof(Function));
  prog->body = head.next;
  prog->locals = locals;
  return prog;
} //;;;

//Node *parse            (Token *tok) {                //::: program = stmt*
//   Node head = {};
//   Node *cur = &head;
//   while (tok->kind != TK_EOF) {
//      cur = cur->next = stmt(&tok, tok);
//   }
//   return head.next;
//} //;;;


