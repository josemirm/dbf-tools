#ifndef __DBF_TOOLS_UTILS__
#define __DBF_TOOLS_UTILS__

#define printLicense() \
	fprintf(stderr, "\nCopyright (C) José Miguel Rodríguez M. 2022\n"); \
	fprintf(stderr, "Licencia MIT. La licencia junto al código fuente se encuentra en http://github.com/josemirm/dbf-tools\n\n")

char* skipStartWhitespaces(char *str, int* len);
char* removeEndWhitespaces(char *str, int *len);

char const* typeToDBaseName(const char type);

#endif