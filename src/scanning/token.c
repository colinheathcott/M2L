#include "token.h"

TokenList TLNew() {
    List tokens = ListNew(sizeof(Token), INIT_TOKEN_LIST_CAP);
    
    if (!ListIsValid(&tokens))
        return (TokenList) { NULL_LIST };
    
    return (TokenList) { .tokens = tokens };
}

void TLPush(TokenList *self, const Token *token) {
    if (!self || !ListIsValid(&self->tokens))
        return;

    // @(invariant) assume ListPush works
    /* discard */ ListPush(&self->tokens, token);
}