// $Id: bin2hdr.cpp 457 2010-02-04 22:02:58Z felfert $
//
// Copyright (C) 2006 The OpenNX Team
// Author: Fritz Elfert
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>


#define MAXARR 0x00010000

static int iscchar(int c)
{
	return (isalnum(c) || (c == '_'));
}

static void usage()
{
    fprintf(stderr, "Usage: bin2hdr <infile> <outfile> <identifier>\n");
    exit(1);
}

int main(int argc, char **argv) {
    long isize = -1;
    unsigned char buf[1024];

    if ((argc < 4) || (strlen(argv[1]) < 1) ||
        (strlen(argv[2]) < 1) || (strlen(argv[3]) < 1))
        usage();

    if (strcmp(argv[1], "-") != 0) {
        struct stat st;
        if (stat(argv[1], &st) != 0) {
            perror(argv[1]);
            exit(-1);
        }
        isize = st.st_size;
    }
    FILE *fi = (strcmp(argv[1], "-") == 0) ? stdin : fopen(argv[1], "rb");
    char *ident = argv[3];
    if (!fi) {
        perror(argv[1]);
        exit(-1);
    }
    FILE *fo = (strcmp(argv[2], "-") == 0) ? stdout :fopen(argv[2], "w");
    if (!fo) {
        perror(argv[2]);
        exit(-1);
    }
    bool hdr = false;
    int n = 0;
    int i;

    if (stdout == fo)
        fprintf(fo, "#ifndef _BIN2HDR_STDOUT_H_\n#define _BIN2HDR_STDOUT_H_\n");
    else {
        char *p = strrchr(argv[2], '\\');
        if (p == NULL)
            p = strrchr(argv[2], '/');
        strcpy((char *)buf, "_");
        if (p)
            strcat((char *)buf, p);
        else
            strcat((char *)buf, argv[2]);
        strcat((char *)buf, "_");
        for (i = 0; i < (int)strlen((char *)buf); i++) {
            if (!iscchar(buf[i]))
                buf[i] = '_';
        }
        fprintf(fo, "#ifndef %s\n#define %s\n", buf, buf);
        if (isize > MAXARR)
            fprintf(fo, "#include <stdlib.h>\n#include <string.h>\n");
    }
    long bcount = 0;
    long bssize = (isize > MAXARR) ? MAXARR : isize;
    while ((i = fread(buf, 1, sizeof(buf), fi)) > 0) {
        if (!hdr) {
            fprintf(fo, "static const unsigned long cnt_%s = %ld;\n", ident, isize);
            fprintf(fo, "static const unsigned char %s%ld[] = {\n", ident, bcount++);
            hdr = true;
        }
        for (int j = 0; j < i; j++) {
            fprintf(fo, "%s0x%02x,%c", (n % 16) ? "" : "    ", buf[j], ((n + 1) %16) ? ' ' : '\n');
            n++;
        }
        if ((n % bssize) == 0) {
            fprintf(fo, "%s};\n", (n % 16) ? "\n" : "");
            fprintf(fo, "static const unsigned char %s%ld[] = {\n", ident, bcount++);
        }
    }
    if (n) {
        fprintf(fo, "%s};\n\n", (n % 16) ? "\n" : "");
        if (bcount == 1) {
            fprintf(fo, "static const unsigned char *get_%s() { return %s; }\n", ident, ident);
            fprintf(fo, "static void free_%s(const unsigned char *) { }\n", ident);
        } else {
            long bi;
            fprintf(fo, "static const unsigned char *get_%s() {\n", ident);
            fprintf(fo, "    const unsigned char *ret = (const unsigned char *)malloc(%ld);\n", bssize * bcount);
            fprintf(fo, "    if (!ret) return ret;\n");
            for (bi = 0; bi < bcount; bi++)
                fprintf(fo, "    memcpy((void *)(ret + %ld), %s%ld, sizeof(%s%ld));\n", bssize * bi, ident, bi, ident, bi);
            fprintf(fo, "    return ret;\n}\n");
            fprintf(fo, "static void free_%s(const unsigned char *ptr) { if (ptr) free((void *)ptr); }\n", ident);
        }
        fprintf(fo, "#endif\n");
    }
    fclose(fi);
    fclose(fo);
    return 0;
}
