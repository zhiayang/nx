// params.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "string.h"
#include "ctype.h"

#include "bfx.h"

namespace bfx
{
	namespace params
	{
		void readParams(nx::BootInfo* bi, void* initrdPtr, size_t initrdSize)
		{
			// find the config file.
			auto [ ptr, len ] = initrd::findFile(initrdPtr, initrdSize, "boot/bfx_kernel_params");
			if(!ptr || len == 0) println("warning: no kernel params");

			// line separated; NAME=VALUE. spaces before = and after = but before the first non-whitespace
			// char will be ignored.

			#define SKIP_TO_NEXT_LINE() do { while(buf[idx] != '\n') idx++; idx++; line++; } while(0)

			char* paramBuf = (char*) pmm::allocate(2);
			size_t paramSz = 0;
			size_t paramCount = 0;

			size_t idx = 0;
			size_t line = 1;
			char* buf = (char*) ptr;
			while(idx < len)
			{
				top:

				// skip leading spaces.
				while(buf[idx] == ' ' || buf[idx] == '\t')
					idx++;

				if(buf[idx] == '#' || buf[idx] == '\n')
				{
					SKIP_TO_NEXT_LINE();
				}
				else
				{
					// read the thing in.
					char name[256] = { 0 };
					{
						size_t ni = 0;
						while(idx < len && (buf[idx] != '=') && (buf[idx] != '\n'))
						{
							name[ni++] = buf[idx++];
							if(ni == 256)
							{
								println("malformed config on line %zu: name too long (>256), skipping", line);
								SKIP_TO_NEXT_LINE();
								goto top;
							}
						}
					}

					// we already skipped leading spaces, so name[0] should be a character. if it's =, then complain.
					if(strlen(name) == 0)
					{
						println("malformed config on line %zu: no name provided before '='", line);
						SKIP_TO_NEXT_LINE();

						goto top;
					}


					// trim backwards.
					{
						size_t i = strlen(name) - 1;
						while(i >= 0 && (name[i] == ' ' || name[i] == '\t'))
							name[i--] = 0;
					}

					// if the current char is a newline, or there is no current char (EOF), then we're done;
					// it's a "flag param", with no value.
					if(idx == len - 1 || buf[idx] == '\n')
					{
						memcpy(paramBuf + paramSz, name, strlen(name));
						paramSz += strlen(name);
						paramBuf[paramSz++] = 0;
						paramCount++;
					}
					else
					{
						// current char should be =
						if(buf[idx] != '=')
						{
							println("malformed config on line %zu: expected '='", line);
							SKIP_TO_NEXT_LINE();

							goto top;
						}
						idx++;

						// skip more leading whitespace.
						while(idx < len && (buf[idx] == ' ' || buf[idx] == '\t'))
							idx++;

						if(idx == len - 1 && buf[idx] == '\n')
						{
							println("malformed config on line %zu: expected value after '='", line);
							SKIP_TO_NEXT_LINE();

							goto top;
						}

						char value[256] = { 0 };
						{
							size_t vi = 0;
							while(idx < len && buf[idx] != '\n')
							{
								value[vi++] = buf[idx++];
								if(vi == 256)
								{
									println("malformed config on line %zu: value too long (>256), skipping", line);
									SKIP_TO_NEXT_LINE();
									goto top;
								}
							}
						}

						// trim backwards.
						{
							size_t i = strlen(value) - 1;
							while(i >= 0 && (value[i] == ' ' || value[i] == '\t'))
								value[i--] = 0;
						}

						// ok, copy it over.
						{
							// first the name:
							memcpy(paramBuf + paramSz, name, strlen(name));
							paramSz += strlen(name);
							paramBuf[paramSz++] = '=';

							// then the value.
							memcpy(paramBuf + paramSz, value, strlen(value));
							paramSz += strlen(value);
							paramBuf[paramSz++] = 0;

							paramCount++;
						}
					}
				}
			}


			// print the params.
			{
				print("kernel parameters:");

				char* ptr = paramBuf;
				for(size_t i = 0; i < paramCount; i++)
				{
					print(" %s", ptr);
					ptr += strlen(ptr) + 1;
				}

				print("\n");
			}

			// ok, tell bootinfo.
			bi->kernelParams        = paramBuf;
			bi->numKernelParams     = paramCount;
			bi->kernelParamsLength  = paramSz;
		}
	}
}






