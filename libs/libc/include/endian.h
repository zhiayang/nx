// endian.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#ifdef __cplusplus
extern "C" {
#endif

#define __bswap_16(x) __builtin_bswap16(x)
#define __bswap_32(x) __builtin_bswap32(x)
#define __bswap_64(x) __builtin_bswap64(x)

/* The compiler provides the type constants. */
#define __LITTLE_ENDIAN __ORDER_LITTLE_ENDIAN__
#define __PDP_ENDIAN __ORDER_PDP_ENDIAN__
#define __BIG_ENDIAN __ORDER_BIG_ENDIAN__

/* The compiler also provides the local endianness as one of the above. */
#define __BYTE_ORDER __BYTE_ORDER__
#define __FLOAT_WORD_ORDER __FLOAT_WORD_ORDER__

/* Declare conversion macros to allow easy endian conversion. */

#if __BYTE_ORDER == __LITTLE_ENDIAN

	#define __htobe16(x) __bswap_16(x)
	#define __htole16(x) (x)
	#define __be16toh(x) __bswap_16(x)
	#define __le16toh(x) (x)

	#define __htobe32(x) __bswap_32(x)
	#define __htole32(x) (x)
	#define __be32toh(x) __bswap_32(x)
	#define __le32toh(x) (x)

	#define __htobe64(x) __bswap_64(x)
	#define __htole64(x) (x)
	#define __be64toh(x) __bswap_64(x)
	#define __le64toh(x) (x)

#elif __BYTE_ORDER == __BIG_ENDIAN

	#define __htobe16(x) (x)
	#define __htole16(x) __bswap_16(x)
	#define __be16toh(x) (x)
	#define __le16toh(x) __bswap_16(x)

	#define __htobe32(x) (x)
	#define __htole32(x) __bswap_32(x)
	#define __be32toh(x) (x)
	#define __le32toh(x) __bswap_32(x)

	#define __htobe64(x) (x)
	#define __htole64(x) __bswap_64(x)
	#define __be64toh(x) (x)
	#define __le64toh(x) __bswap_64(x)

#else

	#error Your processor uses a weird endian, please add support.

#endif








/* Constans for each kind of known endian. */
#define LITTLE_ENDIAN __LITTLE_ENDIAN
#define PDP_ENDIAN __PDP_ENDIAN
#define BIG_ENDIAN __BIG_ENDIAN

/* Easy access to the current endian. */
#define BYTE_ORDER __BYTE_ORDER
#define FLOAT_WORD_ORDER __FLOAT_WORD_ORDER

/* Easy conversion of 16-bit integers. */
#define htobe16(x) __htobe16(x)
#define htole16(x) __htole16(x)
#define be16toh(x) __be16toh(x)
#define le16toh(x) __le16toh(x)

/* Easy conversion of 32-bit integers. */
#define htobe32(x) __htobe32(x)
#define htole32(x) __htole32(x)
#define be32toh(x) __be32toh(x)
#define le32toh(x) __le32toh(x)

/* Easy conversion of 64-bit integers. */
#define htobe64(x) __htobe64(x)
#define htole64(x) __htole64(x)
#define be64toh(x) __be64toh(x)
#define le64toh(x) __le64toh(x)


#ifdef __cplusplus
}
#endif



