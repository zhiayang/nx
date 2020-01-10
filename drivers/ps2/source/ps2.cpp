// ps2.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <ipc.h>
#include <svr/tty.h>
#include <syscall.h>

#include "ps2.h"
#include "keyboard.h"

static void vlog(int lvl, const char* fmt, va_list ap)
{
	char* buf = 0;
	int len = vasprintf(&buf, fmt, ap);

	syscall::kernel_log(lvl, "ps/2", 4, buf, len);
	free(buf);
}

static void log(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vlog(0, fmt, ap);
	va_end(ap);
}

static void warn(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vlog(1, fmt, ap);
	va_end(ap);
}

static void error(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vlog(2, fmt, ap);
	va_end(ap);
}





namespace ps2
{
	static bool Port1Working = true;
	static bool Port2Working = true;

	static int KeyboardPort = -1;

	static Keyboard* kb = 0;

	// i love c.
	static constexpr void (*send_fns[2])(uint8_t data) = {
		send_port1,
		send_port2
	};

	static void init_controller()
	{
		// setup the ps/2 controller robustly. note that we can support both ports on
		// a dual-channel controller, but we only support keyboards for now.

		{
			// disable both devices
			send_cmd(0xAD);  // disables device 1
			send_cmd(0xA7);  // disables device 2

			// flush the buffer.
			port::read1b(DATA_PORT);

			uint8_t config = read_config();

			// disable translation, irq1 and irq12
			config &= ~((1 << 6) | (1 << 0) | (1 << 1));
			write_config(config);
		}


		// controller self-test. send 0xAA, wait for response
		{
			auto response = get_cmd_response(0xAA);
			if(response != 0x55)
			{
				error("controller failed self-test (response = %02x)", response);
				return;
			}
			else
			{
				log("controller passed self-test");
			}
		}

		// enable the second port, if any
		{
			send_cmd(0xA8);
			auto config = read_config();

			if(config & (1 << 5))
			{
				// this is a single channel.
				Port2Working = false;
			}
			else
			{
				Port2Working = true;
			}

			// disable it again.
			send_cmd(0xA7);
		}

		// test the ports. (this takes time)
		if constexpr (false)
		{
			log("testing ports...");

			int working_ports = 2;
			Port1Working = true;

			if(get_cmd_response(0xAB) != 0)
				warn("first port failed"), Port1Working = false, working_ports -= 1;

			if(Port2Working && (get_cmd_response(0xA9) != 0))
				warn("second port failed"), Port2Working = false, working_ports -= 1;

			if(working_ports == 0)
			{
				error("all ports failed, aborting");
				return;
			}
		}

		bool workingPorts[2] = {
			Port1Working,
			Port2Working
		};

		// check the first port to see what is plugged in.
		// we use a do-while so we can break out.
		do {
			for(int i = 0; i < 2; i++)
			{
				if(!workingPorts[i])
					continue;

				// disable scanning first.
				if(!send_data(i + 1, 0xF5))
					break;

				// send identify.
				if(!send_data(i + 1, 0xF2))
					break;

				// wait for input.
				auto r1 = read_data();
				if(r1 == 0xAB)
				{
					auto r2 = read_data();
					if(r2 == 0x41 || r2 == 0xC1)
						log("port%d: mf2 keyboard (w/ translation)", i + 1), KeyboardPort = i + 1;

					else if(r2 == 0x83)
						log("port%d: mf2 keyboard", i + 1), KeyboardPort = i + 1;

					else
						warn("port%d: unknown device (%02x, %02x)", i + 1, r1, r2);
				}
				else
				{
					switch(r1)
					{
						case 0: log("port%d: ps/2 mouse", i + 1); break;
						case 3: log("port%d: mouse w/ scroll", i + 1); break;
						case 4: log("port%d: mouse w/ 5 buttons", i + 1); break;
						default: warn("port%d: unknown device (%02x)", i + 1, r1); break;
					}
				}
			}

		} while(false);

		// we probably want mouse support at some point.
		if(KeyboardPort == -1)
		{
			error("no ps/2 keyboard detected, aborting");
			return;
		}

		// enable interrupts for the keyboard port.
		if(KeyboardPort == 1)       send_cmd(0xAE), write_config(read_config() | 0x1);
		else if(KeyboardPort == 2)  send_cmd(0xA8), write_config(read_config() | 0x2);

		// enable scanning.
		send_data(KeyboardPort, 0xF4);

		log("controller initialised");
	}


	bool send_data(int port, uint8_t data)
	{
		send_fns[port - 1](data);
		uint8_t resp = read_data();

		int tries = 5;
		while(resp == 0xFE && tries > 0)
		{
			send_fns[port - 1](data);
			resp = read_data();

			tries -= 1;
		}

		if(resp != 0xFA)
		{
			warn("port %d did not ACK, aborting", port);
			return false;
		}

		return true;
	}

	bool send_data(int port, uint8_t d1, uint8_t d2)
	{
		send_fns[port - 1](d1);
		send_fns[port - 1](d2);

		uint8_t resp = read_data();

		int tries = 5;
		while(resp == 0xFE && tries > 0)
		{
			send_fns[port - 1](d1);
			send_fns[port - 1](d2);

			resp = read_data();

			tries -= 1;
		}

		if(resp != 0xFA)
		{
			warn("port %d did not ACK, aborting", port);
			return false;
		}

		return true;
	}


	static void square(uint32_t c)
	{
		uint32_t* fb = (uint32_t*) 0xFF'0000'0000;
		for(int y = 40; y < 60; y++)
		{
			for(int x = 1420; x < 1440; x++)
			{
				*(fb + y * 1440 + x) = 0xff'000000 | c;
			}
		}
	}


	// interrupt handler.
	static size_t cnt = 0;
	static uint64_t interrupt_handler(uint64_t sender, uint64_t sigType, uint64_t a, uint64_t b, uint64_t c)
	{
		// the kernel gives us the IRQ number in 'a'
		if(sigType != nx::ipc::SIGNAL_DEVICE_IRQ)
			warn("expected %lu, got %lu", nx::ipc::SIGNAL_DEVICE_IRQ, sigType);

		if(a == 1)
		{
			// auto x = read_data_immediate();
			uint8_t x = (uint8_t) b;

			if(KeyboardPort == 1)
				kb->addByte(x);


			// log("rx byte %02x %zu", x, cnt++);
			square(((uint8_t) cnt) << 16 | ((uint8_t) cnt) << 8 | (uint8_t) cnt);
			// else            square(0x00ff00);
		}
		else if(a == 12)
		{
			auto x = read_data_immediate();
			// if(KeyboardPort == 2)
			// 	kb->addByte(x);
		}
		else
		{
			error("unknown irq #%lu", a);
		}

		/*
			tofix:

			doing syscalls in signal handlers is apparently still a bad idea. we get all the telltale signs of stack
			corruption; mostly executing/loading random addresses that lead to page faults...

			need to figure out why. this is probably impossible to step through with a debugger ):
		*/

		return nx::ipc::SIGNAL_NO_PROPOGATE;
	}
}


int main()
{
	ps2::init_controller();
	ps2::kb = new ps2::Keyboard();

	// set scancode 2 for the keyboard.
	if(auto port = ps2::KeyboardPort; port != -1)
		ps2::send_data(port, 0xF0, 2);

	nx::ipc::install_intr_signal_handler(nx::ipc::SIGNAL_DEVICE_IRQ, &ps2::interrupt_handler);

	while(true)
		;
}








