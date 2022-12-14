# dbf-tools

Set of tools to handle dBase/FoxPro databases/tables in DBF files.


### dbf-info

Tool to get the basic information of a DBF file.

It can provide the number of rows, number of columns, and type of columns the database, and an SQL schema to provide more detailed information of the columns.

To know how too use the tool, execute it without any file or option


### dbf-csv

Tool to convert all the data in the table to a CSV (Comma Separated Values) file.

The output file use the standard CSV format, i.e., using the comma as a column separator, using the dot as a decimal numerical separator, and inserting strings between double quotations.

All the starting and trailing whitespaces are removed from the data, and this is done due to the use of the space character to fill the available unused space of fixed sized columns.

This tool doesn't have any option. Just need an input file and an output file.

Depending of the software used to manage the table it could have different encoding, so it's a good practice to transform the encoding of the output file to the one its needed. This can be done easily with the `iconv` tool. For example: `iconv -f CP437 -t UTF-8 -o output.csv input.csv` will convert a file with western MS-DOS enconding to a UTF-8 one.


### Building the software

Those tools/libraries are necessary:
- autoconf + automake
- CMake
- GCC/Clang

To build the software, clone the repository and make sure a folder called `build` exist inside of it. In a terminal, go to the `build` repository and execute those commands:
```
cmake ..
cmake --build .
```

With that, the `dbf-csv` and `dbf-info` must be created inside the `build` folders.


### Special thanks

Thanks to Björn Berg to create `libdbf` as an open source software and thanks to Quentin de Metz (@quentindemetz) to saving it in GitHub and making it 'compilable'  :-)


### Legal details

The author isn't involved in any of the brand, company, or commercial product mentioned in the text or in any part of the software. Those products aren't used in any way, shape, or form in this software

dBase, FoxPro, and other commercial software mentioned are part of their respective companies.

libdbf is a software made by Björn Berg with GNU General Public License. A copy of the license is available in addition to the code of this library.


### License

MIT License

Copyright (c) 2022 José Miguel (Josemi) Rodríguez M.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
