#include "utils.h"


char* skipStartWhitespaces(char *str, int* len) {
	if (*len <= 0) return str;
	
	int pos = 0;
	int oldLen = *len;

	while (pos < oldLen && str[pos] == ' ') {
		++pos;
	}

	*len = oldLen - pos;

	return &(str[pos]);
}


char* removeEndWhitespaces(char *str, int *len) {
	if (*len <= 0) return str;

	int end = *len - 1;

	while (end > 0 && str[end] == ' ') {
		--end;
	}

	if (++end < *len) {
		str[end] = '\0';
		*len = end;
	}

	return str;
}


char const* typeToDBaseName(const char type) {
	switch(type) {
		case 'N':
			return "Number";
			break;

		case 'C':
			return "String";
			break;

		case 'D':
			return "Data";
			break;

		case 'M':
			return "Memo";
			break;

		case 'L':
			return "Boolean";
			break;

		default:
			return "(Unknown)";
	}
}