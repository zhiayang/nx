// krt.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include "sys/cdefs.h"

#define __min(a, b) ((a) > (b) ? (b) : (a))
#define __max(a, b) ((a) > (b) ? (a) : (b))
#define __abs(x)    ((x) < (0) ? (-x) : (x))

__BEGIN_DECLS

int atoi(const char* nptr);
long atol(const char* nptr);
double atof(const char* nptr);
long long atoll(const char* nptr);

float strtof(const char* nptr, char** endptr);
double strtod(const char* nptr, char** endptr);
long double strtold(const char* nptr, char** endptr);

long strtol(const char* nptr, char** endptr, int base);
long long strtoll(const char* nptr, char** endptr, int base);
unsigned long strtoul(const char* nptr, char** endptr, int base);
unsigned long long strtoull(const char* nptr, char** endptr, int base);

int abs(int j);
long int labs(long int j);
long long llabs(long long int j);

char* itoa(int num, char* dest, int base);
char* ltoa(long num, char* dest, int base);
char* lltoa(long long num, char* dest, int base);


int vcbprintf(void* ctx, size_t (*callback)(void*, const char*, size_t), const char* format, va_list parameters);
int print(const char* fmt, ...);


__END_DECLS













