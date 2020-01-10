// ps2.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include <stddef.h>

namespace port
{
	static inline uint8_t read1b(uint16_t port)
	{
		uint8_t ret;
		asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
		return ret;
	}

	static inline void write1b(uint16_t port, uint8_t value)
	{
		asm volatile("outb %0, %1" :: "a"(value), "Nd"(port));
	}
}

namespace ps2
{
	constexpr uint8_t DATA_PORT         = 0x60;
	constexpr uint8_t COMMAND_PORT      = 0x64;
	constexpr uint64_t TIMEOUT_COUNT    = 10000;

	static inline void wait_output_buf()
	{
		uint64_t x = TIMEOUT_COUNT;
		while(--x && port::read1b(COMMAND_PORT) & 0x1)
			;
	}

	static inline void wait_input_buf()
	{
		uint64_t x = TIMEOUT_COUNT;
		while(--x && port::read1b(COMMAND_PORT) & 0x2)
			;
	}

	static inline void send_cmd(uint8_t cmd)
	{
		port::write1b(COMMAND_PORT, cmd);
	}

	static inline void send_port1(uint8_t data)
	{
		wait_input_buf();
		port::write1b(DATA_PORT, data);
	}

	static inline void send_port2(uint8_t data)
	{
		send_cmd(0xD4);

		wait_input_buf();
		port::write1b(DATA_PORT, data);
	}

	static inline uint8_t read_data()
	{
		wait_output_buf();
		return port::read1b(DATA_PORT);
	}

	static inline uint8_t read_data_immediate()
	{
		// don't wait for the output bus -- irq handler uses this.
		return port::read1b(DATA_PORT);
	}

	static inline uint8_t get_cmd_response(uint8_t cmd)
	{
		send_cmd(cmd);
		return read_data();
	}

	static inline uint8_t read_config()
	{
		return get_cmd_response(0x20);
	}

	static inline void write_config(uint8_t byte)
	{
		port::write1b(COMMAND_PORT, 0x60);
		wait_input_buf();

		port::write1b(DATA_PORT, byte);
	}

	// ports are 1 and 2!
	bool send_data(int port, uint8_t data);
	bool send_data(int port, uint8_t data, uint8_t data2);
}









