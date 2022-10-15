#include <stdio.h>
#include <stdlib.h>
#include "dbf-csv.h"

static void imprimirCabeceraCSV(FmtColumn *fcol, FILE *salida) {
	int col = fcol->columnas - 1;

	// Imprime las primeras columnas separadas por comas...
	for (int c=0; c<col; ++c) {
		fprintf(salida, "%s,", fcol->nombreCol[c]);
	}

	// ... y la última sin la coma al final y terminando en '\n'
	fprintf(salida, "%s\n", fcol->nombreCol[col]);
	fflush(salida);
}


static inline char* limpiarCelda(char* celda, int *tamCol, char* bufferAux) {
	// Se le quitan los espacios en blanco al comienzo y al final de las celdas

	celda = removeEndWhitespaces(skipStartWhitespaces(celda, tamCol), tamCol);

	// Hay que cambiar todas las dobles comillas por dos dobles comillas,
	// es decir, 'abc "de" fg' hay que convertirlo a 'abc ""de"" fg'

	// Si se encuentran dobles comillas, hay que copiar lo que había en el
	// buffer general hacia el auxiliar. Así se tiene un espacio aparte
	// donde poder añadir las dobles comillas extras que so necesarias.

	// No se usa strchr() para no analizar todo el buffer, sólo los caracteres
	// de la celda. Por lo contrario, habría partes del buffer que se
	// analizarían más de una vez, ralentizando el proceso.

	int tam = *tamCol;
	int idx1 = 0;
	int posAux = 0;
	for (int i=0; i<tam; ++i) {

		if (celda[i] == '\"') {
			// Cuando se encuentras dos comillas dobles no habría que hacer
			// ningún cambio y se continúa el proceso
			if (celda[i+1] == '\"') {
				++i;
				continue;
			}

			// En caso contrario, se comienza copiando al buffer todo lo que no
			// se haya copiado hasta este momento
			int cantCopiar = i - idx1 + 1;
			memcpy(&(bufferAux[posAux]), &(celda[idx1]), cantCopiar);
			posAux = posAux + cantCopiar;

			// Después se añaden las comillas extras
			bufferAux[posAux++] = '\"';
			idx1 = i+1;
		}
	}

	// En el caso de haberse usado el buffer auxiliar, se termina de copiar el
	// resto y se devuelve el puntero al buffer auxiliar para que se lea desde
	// tal buffer
	if (posAux > 0) {
		if (idx1 < tam) {
			int cantCopiar = tam - idx1 + 1;
			memcpy(&(bufferAux[posAux]), &(celda[idx1]), cantCopiar);
			bufferAux[posAux + cantCopiar] = '\0';
		}

		return bufferAux;
	}


	return celda;
}


static int imprimirTablaCSV(P_DBF *entrada, FmtColumn *fcol, FILE *salida) {
	// Imprime la cabecera del archivo CSV (el nombre de las columnas)
	imprimirCabeceraCSV(fcol, salida);

	// Se obtienen el total de columnas que componen cada fila
	int totalFilas = dbf_NumRows(entrada);
	int totalCol = fcol->columnas;
	int ultimaCol = totalCol - 1;

	// Se puede usar un array fijo lo suficientemente grande como buffer como
	// para albergar una fila entera, o directamente usar un buffer a partir
	// del tamaño de todas las columnas juntas.

	// char buffer[2000];

	// Se le deja al buffer un byte de más adrede
	int tamBuffer = 1;
	for (int i=0; i<totalCol; ++i) {
		tamBuffer += fcol->tamCol[i];
	}

	char* buffer = (char*) malloc(tamBuffer * sizeof(char));
	if (buffer == NULL) {
		perror("Error al crear el buffer de lectura");
		return -1;
	}

	// El buffer auxiliar se usará para modificar partes de cadenas de texto
	// antes de imprimirlos por salida. Si se modificara en el buffer donde se
	// almacena el registro enteŕo sería más difícil localizar posteriormente
	// la posición donde empieza los datos de cada columna del registro leído.
	char bufferAux[200];

	for (int fila=0; fila<totalFilas; ++fila) {
		// Se lee una fila entera
		if (dbf_ReadRecord(entrada, buffer, 0) < 0) {
			fprintf(stderr, "Error al leer la entrada\n");
			goto imprimirTablaCSV_OtroError;
		}

		// Imprime columna por columna los datos de una fila
		for (int col=0; col<ultimaCol; ++col) {
			int tamCol = fcol->tamCol[col];
			char* celda = dbf_GetRecordData(entrada, buffer, col);
			celda = limpiarCelda(celda, &tamCol, bufferAux);
			int escrito;

			// Añade las comillas necesarias en caso de ser una columna de texto
			char tipo = fcol->tipoCol[col];

			// Abre las comillas antes del texto
			if (tipo == 'C' || tipo == 'M') {
				escrito = fwrite("\"", sizeof(char), 1, salida);
				if (escrito < 0) {
					goto imprimirTablaCSV_ErrorEscribirFichero;
				}
			}


			escrito = fwrite(celda, sizeof(char), tamCol, salida);
			if (escrito < 0) {
				goto imprimirTablaCSV_ErrorEscribirFichero;
			}

			// Cierra las comillas después del texto
			if (tipo == 'C' || tipo == 'M') {
				escrito = fwrite("\"", sizeof(char), 1, salida);
				if (escrito < 0) {
					goto imprimirTablaCSV_ErrorEscribirFichero;
				}
			}

			escrito = fwrite(",", sizeof(char), 1, salida);
			if (escrito < 0) {
				goto imprimirTablaCSV_ErrorEscribirFichero;
			}
		}

		// La última columna (o la primera si sólo hay una) se imprime aparte para
		// que tenermine en '\n' en vez de una coma
		int tamCol = fcol->tamCol[ultimaCol];
		char tipo = fcol->tipoCol[ultimaCol];
		char* celda = dbf_GetRecordData(entrada, buffer, ultimaCol);
		celda = limpiarCelda(celda, &tamCol, bufferAux);
		int escrito;

		// Abre las comillas antes del texto
		if (tipo == 'C' || tipo == 'M') {
			escrito = fwrite("\"", sizeof(char), 1, salida);
			if (escrito < 0) {
				goto imprimirTablaCSV_ErrorEscribirFichero;
			}
		}

		escrito = fwrite(celda, sizeof(char), tamCol, salida);
		if (escrito < 0) {
			goto imprimirTablaCSV_ErrorEscribirFichero;
		}

		// Cierra las comillas después del texto
		if (tipo == 'C' || tipo == 'M') {
			escrito = fwrite("\"", sizeof(char), 1, salida);
			if (escrito < 0) {
				goto imprimirTablaCSV_ErrorEscribirFichero;
			}
		}

		escrito = fwrite("\n", sizeof(char), 1, salida);
		if (escrito < 0) {
			goto imprimirTablaCSV_ErrorEscribirFichero;
		}
	}

	fflush(salida);

	free(buffer);

	return 0;


	imprimirTablaCSV_ErrorEscribirFichero:
		perror("Error al escribir el fichero");
	imprimirTablaCSV_OtroError:
		free(buffer);
		return -1;
}


static void leerTMP(P_DBF *f, FmtColumn *fcol) {
	// u_int32_t addrCol3 = dbf_ColumnAddress(f, 3);
	// if (dbf_SetRecordOffset(f, addrCol3) < 0) {
	// 	fprintf(stderr, "Error al cambiar el offset\n");
	// 	exit(EXIT_FAILURE);
	// }


	// Leer un registro
	int col = fcol->columnas;
	int totalBytes = 0;
	for (int i=0; i<col; ++i) {
		totalBytes += fcol->tamCol[i];
	}

	char *buffer = (char*) malloc(sizeof(char) * (totalBytes + 1));

	if (dbf_ReadRecord(f, buffer, 0) < 0) {
		fprintf(stderr, "Error al leer la entrada\n");
		exit(EXIT_FAILURE);
	}

	buffer[totalBytes] = '\0';

	printf("recordLength: %i\n", dbf_RecordLength(f));
	printf("Total de bytes por leer: %i\n", totalBytes);
	printf("Leido: %s\n", buffer);

	FILE *out = fopen("salida.txt", "wb");

	if (out == NULL) {
		perror("Error al escribir el fichero de salida");
		exit(EXIT_FAILURE);
	}

	fwrite(buffer, sizeof(char), totalBytes, out);
	fclose(out);

	free(buffer);
}


static void imprimirColumnasTMP(FmtColumn *fcol) {
	int cols = fcol->columnas;
	for (int i=0; i<cols; ++i) {
		printf("Columna '%s':\n", fcol->nombreCol[i]);

		printf("Tipo: ");
		char tipo = fcol->tipoCol[i];
		switch(tipo) {
			case 'N':
				puts("Number");
				break;

			case 'C':
				puts("String");
				break;

			case 'D':
				puts("Data");
				break;

			case 'M':
				puts("Memo");
				break;

			case 'L':
				puts("Boolean");
				break;

			default:
				printf("Tipo '%c' no reconocido\n", tipo);
		}

		printf("Tamaño: %i\n", fcol->tamCol[i]);

		if (tipo == 'N') {
			printf("Decimales: %i\n", fcol->decimCol[i]);
		}

		puts("");
	}
}


void liberarEstrColumnas(FmtColumn *fcol) {
	if (fcol->columnas > 0) {
		free(fcol->nombreCol);
		free(fcol->tipoCol);
		free(fcol->tamCol);
		free(fcol->decimCol);
	}
}


FmtColumn obtenerColumnas(P_DBF *f) {
	// En caso de leer correctamente todas las columnas devuelve la cantidad de
	// columnas encontradas. En caso de error devuelve -1.
	FmtColumn fcol;

	int col = dbf_NumCols(f);

	if (col < 0) {
		fprintf(stderr, "Error: Menos de una columna en la tabla\n");

		fcol.columnas = -1;
		fcol.nombreCol = NULL;
		fcol.tipoCol = NULL;
		fcol.tamCol = NULL;
		fcol.decimCol = NULL;

		return fcol;
	}

	fcol.columnas = col;
	fcol.nombreCol = (char**) malloc(col * sizeof(char*));
	fcol.tipoCol = (char*) malloc(col * sizeof(char));
	fcol.tamCol = (int*) malloc(col * sizeof(int));
	fcol.decimCol = (int*) malloc(col * sizeof(int));

	if ( fcol.nombreCol == NULL || fcol.tipoCol == NULL ||
		 fcol.tamCol == NULL || fcol.decimCol == NULL ) {
		// Libera aquellas variables que se hayan reservado
		if (fcol.nombreCol != NULL) free(fcol.nombreCol);
		if (fcol.tipoCol != NULL) free(fcol.tipoCol);
		if (fcol.tamCol != NULL) free(fcol.tamCol);
		if (fcol.decimCol != NULL) free(fcol.decimCol);

		fprintf(stderr, "obtenerColumnas: Error al reservar memoria\n");
		fcol.columnas = -1;
		return fcol;
	}

	for (int i=0; i<col; ++i) {
		fcol.nombreCol[i] = (char*) dbf_ColumnName(f, i);
		char tipo = dbf_ColumnType(f, i);
		fcol.tipoCol[i] = tipo;
		fcol.tamCol[i] = dbf_ColumnSize(f, i);
		if (tipo == 'N') {
			fcol.decimCol[i] = dbf_ColumnDecimals(f, i);
		}
	}

	return fcol;
}


static P_DBF* abrirFichero(char const *fname) {
	// En caso de abrirse correctamente, devuelve el puntero a una estructura
	// donde estará la información. En caso de error devuelve NULL.
	P_DBF* f = dbf_Open(fname);

	if (f == NULL) {
		fprintf(stderr, "Error al abrir el fichero '%s'\n", fname);
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "Se ha abierto la tabla '%s', de tipo '%s'\n", fname, dbf_GetStringVersion(f));

	return f;
}


int main(int argc, char const *argv[]) {
	fprintf(stderr, "dbf-csv 0.1\n");
	printLicense();


	if (argc < 3) {
		fprintf(stderr, "%s: Se necesita un archivo que procesar\n", argv[0]);
		fprintf(stderr, "Uso: %s <archivo de entrada> <archivo CSV de salida>\n", argv[0]);
		exit(EXIT_FAILURE);
	}


	// Abre el fichero de entrada
	P_DBF* entrada = abrirFichero(argv[1]);

	// Abre el fichero de salida
	FILE* salida = fopen(argv[2], "wt");

	fprintf(stderr, "Fichero de salida '%s' abierto.\n", argv[2]);

	FmtColumn fcol = obtenerColumnas(entrada);
	if (fcol.columnas < 1) {
		fclose(salida);
		dbf_Close(entrada);
		exit(EXIT_FAILURE);
	}

	fprintf(stderr,
		"Se van a escribir %i filas de datos, cada una de ellas con %i columnas.\n",
		dbf_NumRows(entrada), fcol.columnas);
	fprintf(stderr, "Escribiendo...\n\n");

	// imprimirColumnasTMP(&fcol);

	// Leer una columna (descripción, strings de 30 bytes)
	// leerTMP(entrada, &fcol);

	// Imprimir fichero en CSV
	imprimirTablaCSV(entrada, &fcol, salida);

	liberarEstrColumnas(&fcol);

	// Cierra los ficheros de salida y de entrada
	fclose(salida);
	dbf_Close(entrada);

	fprintf(stderr, "Los datos se han escrito correctamente.\n");

	return EXIT_SUCCESS;
}
