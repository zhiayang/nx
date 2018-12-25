// strstuff.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "string.h"

extern "C" size_t strlen(const char* str)
{
	size_t len = 0;
	const char* endPtr = str;
	asm("repne scasb" : "+D"(endPtr) : "a"(0), "c"(~0) : "cc");
	len = (endPtr - str) - 1;
	return len;
}

extern "C" char* strncat(char* dest, const char* src, size_t n)
{
	size_t dest_len = strlen(dest);

	size_t i = 0;
	for(; i < n && src[i] != '\0'; i++)
		dest[dest_len + i] = src[i];

	dest[dest_len + i] = '\0';
	return dest;
}

extern "C" int strncmp(const char* s1, const char* s2, size_t n)
{
	return memcmp(s1, s2, n);
}

extern "C" char* strncpy(char* destination, const char* source, size_t n)
{
	size_t len = strlen(source) + 1;
	if(len > n) len = n;
	memcpy(destination, source, len);
	if(len < n) memset(destination + len, '\0', n - len);
	return destination;
}

extern "C" char* strrchr(const char* str, int character)
{
	int i = (int) strlen(str);
	for(; i >= 0; i--)
		if(str[i] == character)
			return (char*) str + i;

	return NULL;
}

extern "C" size_t strspn(const char* s1, const char* s2)
{
	int found = 0;
	size_t len = 0;
	do {
		int i;
		for(i = 0; i < (int) strlen(s2); i++)
		{
			if(s1[len] == s2[i])
			{
				len++;
				found = 1;
				break;
			}
		}
	} while(found);

	return len;
}

extern "C" char* strstr(const char* s1, const char* s2)
{
	size_t s2len = strlen(s2);
	for(; *s1 != '\0'; s1++)
	{
		int i;
		for(i = 0; i < (int) s2len; i++)
		{
			if(s2[i] == '\0')
				return (char*) s1;
			if(s2[i] == *s1)
				continue;
			break;
		}
	}
	return NULL;
}

extern "C" char* strtok(char* s1, const char* s2)
{
	static char* prevS1;
	if(s1 == NULL)
	{
		if(prevS1 == NULL)
			return NULL;
		s1 = prevS1;
	}

	s1 += strspn(s1, s2);
	if(*s1 == '\0')
		return NULL;
	size_t s1newLen = strcspn(s1, s2);

	if(s1[s1newLen] == '\0')
	{
		// we are at the end of the original string
		// next call should return NULL whatever s2 is
		prevS1 = NULL;
	}
	else
	{
		s1[s1newLen] = '\0';
		prevS1 = s1 + s1newLen + 1;
	}

	return s1;
}

extern "C" char* strpbrk(const char* s1, const char* s2)
{
	int s2len = (int) strlen(s2);
	for(; *s1 != '\0'; s1++)
	{
		int i;
		for(i = 0; i < s2len; i++)
			if(*s1 == s2[i])
				return (char*) s1;
	}
	return NULL;
}

extern "C" char* strcat(char* s1, const char* s2)
{
	strcpy(s1 + strlen(s1), s2);
	return s1;
}

extern "C" char* strchr(const char* s, int c)
{
	return (char*) memchr(s, c, strlen(s) + 1);
}

extern "C" int strcmp(const char* str1, const char* str2)
{
	size_t len1 = strlen(str1);
	size_t len2 = strlen(str2);

	int cmpResult = memcmp(str1, str2, (len1 < len2) ? len1 : len2);
	if(cmpResult != 0)
		return cmpResult;

	if(len1 > len2)
		return 1;
	else if(len1 < len2)
		return -1;

	return 0;
}

extern "C" char* strcpy(char* destination, const char* source)
{
	return (char*) memcpy(destination, source, strlen(source) + 1);
}

extern "C" size_t strcspn(const char* s1, const char* s2)
{
	char* pbrk = strpbrk(s1, s2);
	if(pbrk == NULL)
		return strlen(s1);

	return (size_t)(pbrk - s1);
}
















