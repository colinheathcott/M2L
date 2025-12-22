#include "scanner.h"
#include "token.h"

Scanner Scanner_New(const Source *src) {
    // Make a new arena for the tokens
    List token_list = List_New(sizeof(Token), INIT_TOKEN_LIST_CAP);
    if (!List_Is_Valid(&token_list))
        return (Scanner) {0};
    
    // Return valid scanner
    return (Scanner) {
        .src = src,
        .x = 1,
        .y = 1,
        .offset = 0,
        .token_list = token_list
    };
}

void Scanner_Destroy(Scanner *self) {
    if (!self) return;
    List_Free(&self->token_list);
    *self = (Scanner) {0}; // poison
}