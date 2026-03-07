#include <nexus.h>

struct nexus_message {
	uint32_t type;
	uint8_t *data;
	uint32_t len;
};
