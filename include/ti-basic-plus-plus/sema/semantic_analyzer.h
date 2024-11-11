#ifndef SEMA_H
#define SEMA_H

#include <ti-basic-plus-plus/parser/ast.h>
#include <ti-basic-plus-plus/basic/diagnostics.h>

void analyze_semantics(ast_node_t* node, diagnostics_t* d);

#endif // SEMA_H
