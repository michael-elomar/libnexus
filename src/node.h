#ifndef _NODE_H_
#define _NODE_H_

#include <stdbool.h>
#include <nexus.h>

struct nexus_node {
	struct neutron_loop *loop;
	struct neutron_ctx *ctx;
	bool is_publisher;
	bool is_spinning;
	const char *address;
	struct nexus_node_cbs *cbs;
};

#endif
