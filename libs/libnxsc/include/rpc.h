// rpc.h
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <memory>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#include <nx/rpc_message.h>
#include <zst/result.h>

namespace nx {

	rpc::message_t rpc_call(uint64_t connId, const rpc::message_t& params);
	uint32_t rpc_call_procedure(uint64_t connId, const rpc::message_t& params);
	void rpc_return(uint64_t connId, const rpc::message_t& result);

	rpc::message_t rpc_wait_call(uint64_t connId);
	rpc::message_t rpc_wait_any_call(uint64_t* connId);

	uint64_t rpc_open(uint64_t target);
	void rpc_close(uint64_t connId);



	/*
		how the template RPC interface works: each "RPC function" should be a struct, like so:
		struct SomeFunction
		{
			SomeFunction(...) : field(...) {  }

			using return_type = <return_type>;
			using error_type = <error_type>;
			static constexpr uint64_t function_number = ...;

		private:
			<fields>
		};

		and sizeof(SomeFunction) <= 96;


		in terms of usage, the client is always the one to open connections. the server should
		only be instantiated once the connection id is known. for now (2020-09-23), there is no
		mechanism for a server to know when a client has opened a connection to it.

		the "recommended" way to write a server is to use rpc_wait_any_call() and maintain some
		sort of hashmap to a Server instance. then, call reply() on the server with the appropriate
		type.
	*/

	namespace rpc
	{
		// i'd rather not pull in the entirety of type_traits just for is_void...
		namespace __detail
		{
			template <typename A>
			struct is_void { static constexpr bool value = false; };

			template <>
			struct is_void<void> { static constexpr bool value = true; };
		}

		struct Client
		{
			Client(pid_t targetPid) { this->connId = rpc_open(targetPid); }
			~Client() { this->close(); }

			bool valid() const { return this->connId != 0; }
			void close() { if(this->connId != 0) rpc_close(this->connId); this->connId = 0; }


			template <typename Func,
				typename ReturnType = typename Func::return_type,
				typename ErrorType = typename Func::error_type,
				typename... Args
			>
			zst::Result<ReturnType, ErrorType> call(Args&&... args)
			{
				using Ret = zst::Result<ReturnType, ErrorType>;

				static_assert(sizeof(Func) <= rpc::message_t::MAX_SIZE);
				static_assert(sizeof(ReturnType) <= rpc::message_t::MAX_SIZE);

				rpc::message_t params = { };
				params.function = Func::function_number;

				new (reinterpret_cast<Func*>(&params.bytes[0])) Func(static_cast<Args&&>(args)...);

				{
					auto ret = rpc_call(this->connId, params);
					if(ret.status == RPC_OK)
					{
						if constexpr (__detail::is_void<ReturnType>::value)
							return Ret::of_value();

						else
							return Ret::of_value(*reinterpret_cast<ReturnType*>(ret.bytes));
					}
					else
					{
						return Ret::of_error(*reinterpret_cast<ErrorType*>(ret.bytes));
					}
				}
			}


		private:
			uint64_t connId;
		};


		struct Server
		{
			Server(uint64_t connId) { this->connId = connId;}

			bool valid() const { return this->connId != 0; }
			void close() { if(this->connId != 0) rpc_close(this->connId); this->connId = 0; }


			template <typename Func, typename... Args>
			void reply(Args&&... args)
			{
				using return_type = typename Func::return_type;

				static_assert(sizeof(Func) <= rpc::message_t::MAX_SIZE);
				static_assert(sizeof(return_type) <= rpc::message_t::MAX_SIZE);

				rpc::message_t result = { };
				result.status = rpc::RPC_OK;
				result.function = Func::function_number;

				if constexpr (!__detail::is_void<return_type>::value)
					new (reinterpret_cast<return_type*>(&result.bytes[0])) return_type(static_cast<Args&&>(args)...);

				rpc_return(this->connId, result);
			}

			template <typename Func, typename ExtraData = void, typename... Args>
			void error(uint64_t errorcode, Args&&... args)
			{
				static_assert(sizeof(ExtraData) <= rpc::message_t::MAX_SIZE - sizeof(uint64_t));

				rpc::message_t result = { };
				result.status   = rpc::RPC_ERR_CALL;
				result.function = Func::function_number;
				result.args[0]  = errorcode;

				if constexpr (!__detail::is_void<ExtraData>::value)
				{
					new (reinterpret_cast<ExtraData*>(&result.bytes[sizeof(uint64_t)]))
						ExtraData(static_cast<Args&&>(args)...);
				}

				rpc_return(this->connId, result);
			}


		private:
			uint64_t connId;
		};
	}
}
