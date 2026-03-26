#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <nexus.h>

struct nexus_message {
	uint32_t type;
	uint8_t *data;
	uint32_t len;
};

#endif // !_MESSAGE_H_
