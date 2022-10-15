// dbf-tools: dbf-csv.h
// Copyright (c) 2022 José Miguel (Josemi) Rodríguez M.
// https://github.com/josemirm/dbf-tools

#ifndef __DBF_CSV_H__
#define __DBF_CSV_H__

#include <string.h>
#include "libdbf.h"
#include "utils.h"

extern char *dbf_GetRecordData(P_DBF *p_dbf, char *record, int column);

typedef struct {
	int    columnas;
	char **nombreCol;
	char  *tipoCol;
	int   *tamCol;
	int	  *decimCol;
} FmtColumn;

void liberarEstrColumnas(FmtColumn *fcol);
FmtColumn obtenerColumnas(P_DBF *f);

#endif
