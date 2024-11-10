#include <stdio.h>

#include <driver/options.h>
#include <ti-basic-plus-plus/basic/diagnostics.h>
#include <ti-basic-plus-plus/basic/input_file.h>
#include <ti-basic-plus-plus/lexer/lexer.h>
#include <ti-basic-plus-plus/parser/parser.h>
#include <ti-basic-plus-plus/sema/semantic_analyzer.h>

#include <assert.h>
#include <stb_ds.h>
#include <stdio.h>

static diagnostics_t d;

static void compile(void);

int main(int argc, const char** argv) {
  d = diagnostics_create();

  do {
    parse_arguments(argc, argv, &d);
    if (should_exit(&d)) {
      break;
    }

    if (driver_config.build) {
      compile();
      if (should_exit(&d)) {
        break;
      }
    }

    if (driver_config.send) {
      // TODO: Send to calculator.
    }
  } while (false);

  return get_exit_status(&d);
}

static void compile(void) {
  input_file_t input_file;

  if (!if_init(&input_file, driver_config.input_path)) {
    diag_report_file(&d, ERROR, driver_config.input_path,
                     "failed to open file");
    return;
  }

  token_t* head_token = NULL;
  ast_node_t* ast_root = NULL;

  // Lexical analysis

  head_token = tokenize_file(&input_file, &d);
  if (head_token == NULL) {
    return;
  }
  if (should_exit(&d)) {
    goto CLEANUP;
  }

  if (driver_config.dump_tokens) {
    print_token_list(head_token, stdout);
  }

  // Parsing

  ast_root = parse_tokens(head_token, &d);
  if (ast_root == NULL || should_exit(&d)) {
    goto CLEANUP;
  }

  // Semantic analysis
  analyze_semantics(ast_root, &d);
  if (should_exit(&d)) {
    goto CLEANUP;
  }

  if (driver_config.dump_ast) {
    print_ast(ast_root, stdout);
  }

  // This is temporary.
  FILE* output_file = fopen("output.tibasic", "w");
  if (!output_file) {
    goto CLEANUP;
  }
  output_ti_basic(ast_root, output_file);
  fclose(output_file);

CLEANUP:
  if (ast_root != NULL) {
    ast_node_destroy(ast_root);
  }
  if (head_token != NULL) {
    token_list_destroy(head_token);
  }
  if_destroy(&input_file);
}

