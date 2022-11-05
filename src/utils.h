// dbf-tools: utils.h
// Copyright (c) 2022 José Miguel (Josemi) Rodríguez M.
// https://github.com/josemirm/dbf-tools

#ifndef __DBF_TOOLS_UTILS__
#define __DBF_TOOLS_UTILS__

#define printLicense() \
	fprintf(stderr, "Copyright (C) José Miguel Rodríguez M. 2022. Licencia MIT\n"); \
	fprintf(stderr, "https://github.com/josemirm/dbf-tools\n\n")

char* skipStartWhitespaces(char *str, int* len);
char* removeEndWhitespaces(char *str, int *len);

char const* typeToDBaseName(const char type);

#endif