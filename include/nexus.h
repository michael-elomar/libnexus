#include <stdint.h>
#include <protobuf-c/protobuf-c.h>

/* Opaque structures for declarations */
struct nexus_node;
struct nexus_message;

struct nexus_node_cbs {
	void (*connected_cb)(struct nexus_node *node, void *userdata);
	void (*disconnected_cb)(struct nexus_node *node, void *userdata);
	void (*msg_recv_cb)(struct nexus_node *node,
			    struct nexus_message *msg,
			    void *userdata);
};

/* Message API */

struct nexus_message *nexus_message_init(uint32_t len,
					 uint32_t type,
					 uint8_t *buf);

int nexus_message_get_packed_size(struct nexus_message *msg);

void nexus_message_pack(struct nexus_message *msg,
			uint8_t *buf,
			uint32_t buflen);

struct nexus_message *nexus_message_unpack(uint8_t *buf, uint32_t buflen);

void nexus_message_destroy(struct nexus_message *msg);

void nexus_message_print(struct nexus_message *msg);

/* Node API */

struct nexus_node *nexus_node_create(struct nexus_node_cbs *cbs);

void nexus_node_destroy(struct nexus_node *node);

int nexus_node_listen(struct nexus_node *node, const char *address);

int nexus_node_connect(struct nexus_node *node, const char *address);

int nexus_node_send_message(struct nexus_node *node, struct nexus_message *msg);

int nexus_node_send_protobuf(struct nexus_node *node,
			     ProtobufCMessage *proto_msg,
			     uint32_t type);

int nexus_node_spin(struct nexus_node *node);

void nexus_node_stop(struct nexus_node *node);
