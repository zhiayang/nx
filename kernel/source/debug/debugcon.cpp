// debugcon.cpp
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace debugcon
{
	constexpr size_t MAX_BUFFER_SIZE        = 1024;
	constexpr char BREAK_CHAR               = 'B';
	constexpr uint64_t ESCAPE_TIMEOUT       = 50'000'000;

	static struct {
		size_t cursor = 0;
		size_t length = 0;

		char buffer[MAX_BUFFER_SIZE];

	} line;

	static enum { MODE_RUNNING, MODE_DEBUGGER } currentMode = MODE_RUNNING;

	static uint64_t lastInputTimestamp = 0;
	static krt::circularbuf<uint8_t, _allocator, _aborter> inputBuffer;

	void startDebugger()
	{
		currentMode = MODE_DEBUGGER;
	}

	static void do_command(char* cmd, size_t len)
	{
		// move to the next line
		serial::debugprint('\n');

		// trim whitespace at the beginning:
		while(*cmd && (*cmd == ' ' || *cmd == '\t'))
			cmd++, len--;

		// get the command word by looking for the first tab or space
		size_t cmd_len = len;
		for(size_t i = 0; i < len; i++)
		{
			if(cmd[i] == ' ' || cmd[i] == '\t')
			{
				cmd_len = i;
				break;
			}
		}

		if(cmd_len == 0)
			return;

		auto match = [&cmd, &cmd_len](const char* s) -> bool {
			return strncmp(s, cmd, cmd_len) == 0;
		};

		if(match("help"))
		{
			serial::debugprintf("no help!");
		}
		else
		{
			serial::debugprintf("unknown command '%.*s'", cmd_len, cmd);
		}

		serial::debugprint('\n');
	}












	static void rx_handler(uint8_t c)
	{
		if(c == '\r')
			c = '\n';

		if(currentMode == MODE_RUNNING && c == BREAK_CHAR)
			startDebugger();

		inputBuffer.write(c);
		lastInputTimestamp = scheduler::getElapsedNanoseconds();
	}

	static void worker()
	{
		constexpr char CTRL_A       = '\x01';
		constexpr char CTRL_C       = '\x03';
		constexpr char CTRL_D       = '\x04';
		constexpr char CTRL_E       = '\x05';
		constexpr char CTRL_H       = '\x08';
		constexpr char CTRL_K       = '\x0B';
		constexpr char ENTER        = '\x0A';
		constexpr char BACKSPACE    = '\x7F';


		// returns true if the line was edited in the middle.
		auto append = [](char ch) -> bool {

			// if we're full, ding.
			if(line.length >= MAX_BUFFER_SIZE)
			{
				serial::debugprint('\a');
				return false;
			}

			bool ret = false;
			// cursor points "past-the-end".
			auto tailN = line.length - line.cursor;
			if(tailN > 0)
				(ret = true), memmove(line.buffer + line.cursor + 1, line.buffer + line.cursor, tailN);

			line.buffer[line.cursor++] = ch;
			line.length++;

			return ret;
		};

		auto refresh_line = []() {
			// just clear the whole line and print everything again.
			serial::debugprint("\x1b[2K");
			serial::debugprint("\x1b[1G");

			// print the rest of the line
			serial::debugprint(line.buffer, line.length);

			// move the cursor to the right place
			serial::debugprintf("\x1b[%zuG", line.cursor + 1);
		};

		auto erase_left = [&refresh_line]() {
			if(line.cursor == 0)
				return;

			memmove(line.buffer + line.cursor - 1, line.buffer + line.cursor, line.length - line.cursor);
			line.cursor--;
			line.length--;

			refresh_line();
		};

		auto erase_right = [&refresh_line]() {
			if(line.cursor == line.length)
				return;

			memmove(line.buffer + line.cursor, line.buffer + line.cursor + 1, line.length - line.cursor);
			line.length--;

			refresh_line();
		};


		struct {
			void home()
			{
				line.cursor = 0;
				serial::debugprint("\x1b[0G");
			}
			void end()
			{
				line.cursor = line.length;
				serial::debugprint("\x1b[999G");
			}
			void left()
			{
				if(line.cursor == 0)
					return;

				line.cursor--;
				serial::debugprint("\x1b[1D");
			}
			void right()
			{
				if(line.cursor == line.length)
					return;

				line.cursor++;
				serial::debugprint("\x1b[1C");
			}
		} cursor;


		auto blocking_read = []() -> uint8_t {
			while(inputBuffer.size() == 0)
				;

			return inputBuffer.read();
		};

		while(true)
		{
			while(inputBuffer.size() > 0)
			{
				// we must peek, because there's no guarantee we'll get all the bytes of a multibyte
				// sequence in the buffer at once (eg. escape sequences), so we need to be able
				// to "abort" and wait for more bytes.
				auto x = inputBuffer.peek();

				// escape sequence.
				if(x == 0x1B)
				{
					// there should be at least 2 more chars
					if(inputBuffer.size() > 2)
					{
						// pop the esc
						inputBuffer.read();

						char a = inputBuffer.read();
						char b = inputBuffer.read();

						if(a == '[')
						{
							char thing = 0;
							if(b >= '0' && b <= '9')
							{
								int n = 0;

								// ok now time to read until a ~
								char x = b;
								while(x >= '0' && x <= '9')
								{
									n = (10 * n) + (x - '0');
									x = blocking_read();
								}

								// we have a modifier -- for now, ignore them.
								if(x == ';')
								{
									// advance it.
									x = blocking_read();

									int mods = 0;
									while(x >= '0' && x <= '9')
									{
										mods = (10 * mods) + (x - '0');
										x = blocking_read();
									}

									thing = x;
								}
								else
								{
									thing = static_cast<char>(n);
								}
							}
							else
							{
								thing = b;
							}

							switch(thing)
							{
								case 3: erase_right(); break;

								case 1: cursor.home(); break;
								case 7: cursor.home(); break;

								case 4: cursor.end(); break;
								case 8: cursor.end(); break;

								case 'C': cursor.right(); break;
								case 'D': cursor.left(); break;
								case 'H': cursor.home(); break;
								case 'F': cursor.end(); break;

								case 'A': {
								} break;

								case 'B': {
								} break;
							}
						}
						else if(a == 'O')
						{
							if(b == 'H')        cursor.home();
							else if(b == 'F')   cursor.end();
						}
					}
					else
					{
						// see if we are past the timeout (because pressing the escape key only sends ESC, duh)
						if(scheduler::getElapsedNanoseconds() > lastInputTimestamp + ESCAPE_TIMEOUT)
						{
							inputBuffer.read();

							// this was an escape
							// for now... ignore it i guess?
							serial::debugprintf("<esc>");
						}
						else
						{
							// keep waiting.
							break;
						}
					}
				}
				else
				{
					inputBuffer.pop();

					if(x == ENTER)
					{
						// clear the useless bits in the buffer first
						memset(line.buffer + line.length, 0, MAX_BUFFER_SIZE - line.length);
						do_command(line.buffer, line.length);

						line.length = 0;
						line.cursor = 0;
					}
					else if(x == CTRL_H || x == BACKSPACE)
					{
						erase_left();
					}
					else if(x == CTRL_A)
					{
						cursor.home();
					}
					else if(x == CTRL_E)
					{
						cursor.end();
					}
					else if(x == CTRL_D)
					{
						erase_right();
					}
					else if(x == CTRL_C)
					{
						line.length = 0;
						line.cursor = 0;
						refresh_line();
					}
					else if(x == CTRL_K)
					{
						line.length = line.cursor;
						refresh_line();
					}
					else if(x >= 32 && x < 127)
					{
						// just add this to the buffer.
						bool needClr = append((char) x);

						// the good thing is, we can usually rely on the client end having a proper terminal emulator as well,
						// so we can just echo the received byte directly, and it'll handle all the escape sequences etc.
						// our problem is that we need to handle line-editing on our line buffer.

						// we need to refresh the line, in case we edited stuff in the middle.
						if(needClr) refresh_line();
						else        serial::debugprint((char) x);
					}
				}
			}
		}
	}


	void init()
	{
		serial::initReceive(rx_handler);
		memset(line.buffer, 0, MAX_BUFFER_SIZE);

		inputBuffer = krt::circularbuf<uint8_t, _allocator, _aborter>(MAX_BUFFER_SIZE);
		scheduler::addThread(scheduler::createThread(scheduler::getKernelProcess(), worker));
	}
}
}
