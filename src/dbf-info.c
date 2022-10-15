#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "libdbf.h"
#include "utils.h"

void printSQLType(P_DBF *f, int col) {
	if (col < 0 || f == NULL) return;

	const char type = dbf_ColumnType(f, col);

	switch (type) {
		case 'N':
			int decs = dbf_ColumnDecimals(f, col);

			if (decs > 0) {
				printf("DECIMAL(32, %i)", decs);
			} else {
				printf("INTEGER");
			}

			break;

		case 'C':
			printf("VARCHAR(%i)", dbf_ColumnSize(f, col));
			break;

		case 'D':
			printf("BLOB");
			break;

		case 'M':
			printf("MEDIUMTEXT");
			break;

		case 'L':
			printf("BOOL");
			break;

		default:
		// Prints nothing
	}
}


void printColumnsSQL(P_DBF *f) {
	int cols = dbf_NumCols(f);
	
	if (cols < 1) return;

	puts("CREATE TABLE IF NOT EXISTS \"Table1\"(");

	int lastCol = cols - 1;
	for (int i=0; i<lastCol; ++i) {
		printf("\t\"%s\" ", dbf_ColumnName(f, i));
		printSQLType(f, i);
		printf(",\n");
	}

	printf("\t\"%s\" ", dbf_ColumnName(f, lastCol));
	printSQLType(f, lastCol);
	printf(");\n");

}


void printColumnsCSV(P_DBF *f) {
	int col = dbf_NumCols(f);

	if (col < 1) return;

	puts("\"Nombre\",\"Tipo\",\"Tamaño (bytes)\",\"Decimales (si procede)\"");
	for (int i=0; i<col; ++i) {
		const char type = dbf_ColumnType(f, i);

		printf("\"%s\",\"%s\",%i,", dbf_ColumnName(f, i), typeToDBaseName(type), dbf_ColumnSize(f, i));

		if (type == 'N') printf("%i", dbf_ColumnDecimals(f, i));

		puts("");
	}

	puts("");
}

void leerColumnas(P_DBF *f) {
	int col = dbf_NumCols(f);

	if (col == 0) {
		puts("- La tabla no tiene columnas");
		return;
	}

	if (col < 1) {
		fprintf(stderr, "Error leyendo las columnas de la tabla\n");
		exit(EXIT_FAILURE);
	}

	printf("\n- Columnas:\n");
	printColumnsCSV(f);
}


void printBasicInfo(P_DBF *f, char const *fname) {
	printf("Fichero: %s\n", fname);
	printf("- Tipo de base de datos: %s\n", dbf_GetStringVersion(f));
	printf("- Número de filas: %i\n", dbf_NumRows(f));
	printf("- Número de columnas: %i\n", dbf_NumCols(f));
}


P_DBF* openFile(char const *fname) {
	P_DBF* f = dbf_Open(fname);

	if (f == NULL) {
		printf("Error al abrir el fichero '%s'\n", fname);
		exit(1);
	}

	return f;
}


int main(int argc, char const *argv[])
{
	fprintf(stderr, "dbf-info 0.1\n");
	printLicense();

	if (argc < 2) {
		goto main_NoArgs;
	}

	bool printCols = false;
	bool printCSVColsSchema = false;
	bool printSQLColsSchema = false;

	for (int i=1; i<(argc-1); ++i) {
		if (argv[i][0] != '-') continue;

		if (strcmp("--col", argv[i]) == 0 || strcmp("-c", argv[i]) == 0 ) {
			printCols = true;
		} else if (strcmp("--csv", argv[i]) == 0) {
			printCSVColsSchema = true;
		} else if (strcmp("--sql", argv[i]) == 0) {
			printSQLColsSchema = true;
		} else {
			goto main_InvalidArgs;
		}
	}

	// Abre el fichero y lee su información más básica
	P_DBF *f = openFile(argv[argc - 1]);

	// Se ejecutan las opciones adicionales indicadas
	if (printSQLColsSchema) {
		printColumnsSQL(f);
	} else if (printCSVColsSchema) {
		printColumnsCSV(f);
	} else {
		printBasicInfo(f, argv[argc - 1]);

		if (printCols) {
			leerColumnas(f);
		}
	}

	// Se cierra el fichero
	if (dbf_Close(f)) {
		puts("Error cerrando el fichero");
	}

	return EXIT_SUCCESS;



	main_NoArgs:
		puts("Se necesita al menos un archivo para leer");

	main_InvalidArgs:
		printf("\nUso del programa: %s <opciones> <fichero>\n", argv[0]);
		puts("Opciones disponibles:");
		puts("   -c");
		puts("--col: Imprime información de las columnas en formato CSV\n");
		puts("--csv: Imprime sólo la información de las columnas en formato CSV\n");
		puts("--sql: Imprime en formato SQL el esquema que tiene la tabla");
		
		printf("\nEjemplo: %s --csv file.dbf\n", argv[0]);

		exit(EXIT_FAILURE);
}