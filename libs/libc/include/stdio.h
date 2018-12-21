// stdio.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stddef.h>
#include <stdarg.h>
#include "stdint.h"
#include "unistd.h"
#include "errno.h"
#include "defs/_file.h"

#define EOF			(-1)
#define FILENAME_MAX	255

#ifdef __cplusplus
extern "C" {
#endif

// std streams
extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;
extern FILE* stdlog;

// typedefs
typedef off_t fpos_t;


// functions taking a FILE* stream
size_t fwrite(const void* ptr, size_t size, size_t count, FILE* stream);
size_t fread(void* ptr, size_t size, size_t count, FILE* stream);

int fputc(int c, FILE* str);
int putc(int c, FILE* str);

int fputs(const char* s, FILE* str);

int fgetc(FILE* str);
int getc(FILE* str);

FILE* freopen(const char* path, const char* mode, FILE* str);
FILE* fopen(const char* path, const char* mode);
int fclose(FILE* str);

FILE* fdopen(int fd, const char* mode);
int fileno(FILE* str);

// direct to stdio/stdin
int puts(const char* str);
int getchar();
int putchar(int c);


// misc things
void perror(const char* msg);




// incomplete
int feof(FILE* str);
void clearerr(FILE* str);
int ferror(FILE* str);
int fflush(FILE* str);
int fgetpos(FILE* str, fpos_t* pos);
char* fgets(char* s, int count, FILE* str);
int fscanf(FILE* str, const char* fmt, ...);
int fseek(FILE* str, off_t offset, int origin);
int fsetpos(FILE* str, const fpos_t* pos);
off_t ftell(FILE* str);
void rewind(FILE* str);
int remove(const char* path);
int rename(const char* oldname, const char* newname);
int scanf(const char* fmt, ...);
int sscanf(const char* s, const char* fmt, ...);
void setbuf(FILE* str, char* buf);
int setvbuf(FILE* str, char* buf, int mode, size_t size);
FILE* tmpfile();
int ungetc(int c, FILE* str);




// printf family
int fprintf(FILE* stream, const char* format, ...);
int vfprintf(FILE* stream, const char* format, va_list ap);

int snprintf(char* str, size_t size, const char* format, ...);
int vsnprintf(char* str, size_t size, const char* format, va_list ap);


int printf(const char* format, ...);
int sprintf(char* str, const char* format, ...);

int vprintf(const char* format, va_list ap);
int vsprintf(char* str, const char* format, va_list ap);

int asprintf(char** result_ptr, const char* format, ...);
int vasprintf(char** result_ptr, const char* format, va_list ap);


// in addition to bufmode being 0, 1 or 2, when ORed with 0x4 or 0x8, it means either blocking or async.
#define __BUFMODE_BLOCK	0x4
#define __BUFMODE_ASYNC	0x8
#define	BUFSIZ			4096	// IDGAF
#define _IOFBF			1	// Fully buffered.
#define _IOLBF			2	// Line buffered.
#define _IONBF		4	// No buffering.


#ifdef __cplusplus
}
#endif














