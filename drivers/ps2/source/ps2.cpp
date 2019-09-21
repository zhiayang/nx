// ps2.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <ipc.h>
#include <svr/tty.h>
#include <syscall.h>

#include "ps2.h"

static void vlog(int lvl, const char* fmt, va_list ap)
{
	char* buf = 0;
	int len = vasprintf(&buf, fmt, ap);

	char sys[5] = { 0 };
	strcpy(sys, "ps/2");

	syscall::kernel_log(lvl, sys, 4, buf, len);
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

		// test the ports.
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

		printf("**\n\n\n\ninitialised\n");
	}


	bool send_data(int port, uint8_t data)
	{
		send_fns[port - 1](data);
		uint8_t resp = read_data();

		while(resp == 0xFE)
		{
			send_fns[port - 1](data);
			resp = read_data();
		}

		if(resp != 0xFA)
		{
			warn("port %d did not ACK, aborting", port);
			return false;
		}

		return true;
	}
}


extern "C" int main()
{
	ps2::init_controller();

	while(1)
		;

	return 0;
}








