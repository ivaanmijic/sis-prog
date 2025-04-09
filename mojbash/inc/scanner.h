#ifndef SCANNER_H
#define SCANNER_H

char **tokenize(char *buffer, int *num_tokens);

int is_valid(const char *cmd);

#endif
