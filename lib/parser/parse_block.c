#include <ti-basic-plus-plus/parser/parser.h>

#include <assert.h>

#include "parser_internal.h"

ast_node_t* parse_block(token_t** t,
                        source_range_t location,
                        diagnostics_t* d) {
  assert(t != NULL);
  assert(*t != NULL);
  assert(d != NULL);

  ast_node_t* block = ast_node_create(AST_BLOCK);

  ast_node_t* statement;
  source_range_t statement_location;
  while (true) {
    statement = NULL;
    statement_location = (*t)->location;

    token_kind_t token_kind = (*t)->kind;
    if (token_kind == TOKEN_UNKNOWN || token_kind == TOKEN_EOF) {
      unexpected_token(*t, TOKEN_UNKNOWN, d);
      goto CLEANUP;
    }

    if (token_kind == TOKEN_NEWLINE) {
      TOK_ITER(t);
      continue;
    } else if (token_kind == TOKEN_KEYWORD) {
      keyword_kind_t keyword_kind = (*t)->data.keyword;
      switch (keyword_kind) {
        case KW_NUMBER:
        case KW_STRING:
        case KW_MATRIX:
        case KW_LIST: {
          TOK_ITER(t);
          if ((*t)->kind != TOKEN_IDENTIFIER) {
            unexpected_token(*t, TOKEN_IDENTIFIER, d);
            goto CLEANUP;
          }
          const char* name = (*t)->data.string;
          TOK_ITER(t);
          statement =
              parse_variable_decl(t, keyword_to_variable_type(keyword_kind),
                                  name, statement_location, d);
        } break;
        case KW_IF:
          TOK_ITER(t);
          statement = parse_if_statement(t, statement_location, d);
          break;
        case KW_WHILE:
          TOK_ITER(t);
          statement = parse_while_statement(t, statement_location, d);
          break;
        /* case KW_FOR: */
        /*   TOK_ITER(t); */
        /*   statement = parse_for_statement(t, statement_location, d); */
        /*   break; */
        case KW_RETURN:
          TOK_ITER(t);
          statement = parse_return_statement(t, statement_location, d);
          break;
        case KW_NOT:
          statement = parse_expression(t, d);
          break;
        default:
          unexpected_token(*t, TOKEN_KEYWORD, d);
          break;
      }
    } else if (token_kind == TOKEN_PUNCTUATOR) {
      punctuator_kind_t punctuator_kind =
          comp_punctuator_kind(*t, 5, PUNCT_LBRACKET, PUNCT_ADD, PUNCT_SUB,
                               PUNCT_LBRACE, PUNCT_RBRACE);

      switch (punctuator_kind) {
        case PUNCT_LBRACKET:
        case PUNCT_ADD:
        case PUNCT_SUB:
          statement = parse_expression(t, d);
          break;
        case PUNCT_LBRACE:
          TOK_ITER(t);
          statement = parse_block(t, statement_location, d);
          break;
        case PUNCT_RBRACE:
          location = range_cat(&location, &statement_location);
          TOK_ITER(t);
          goto LOOP_END;
        default:
          unexpected_token(*t, TOKEN_PUNCTUATOR, d);
          break;
      }
    } else {
      statement = parse_expression(t, d);
    }

    if (statement == NULL) {
      goto CLEANUP;
    }
    location = range_cat(&location, &statement->location);
    arrput(block->children, statement);
  }

LOOP_END:
  block->location = location;

  return block;

CLEANUP:
  ast_node_destroy(block);
  return NULL;
}

