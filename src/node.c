#include <node.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <log.h>

#define ADDRESS_PREFIX "unix:"

static void recv_cb(struct neutron_ctx *ctx,
		    struct neutron_conn *conn,
		    uint8_t *buf,
		    uint32_t buflen,
		    void *userdata)
{
	struct nexus_message *msg = nexus_message_unpack(buf, buflen);
	struct nexus_node *node = (struct nexus_node *)userdata;

	/* no bother checking the event if no callbacks are defined */
	if (!node->cbs)
		return;

	if (node->cbs->msg_recv_cb) {
		(*node->cbs->msg_recv_cb)(node, msg, NULL);
	}
}

static void fd_event_cb(struct neutron_ctx *ctx,
			enum neutron_event event,
			struct neutron_conn *conn,
			void *userdata)
{
	struct nexus_node *node = (struct nexus_node *)userdata;

	/* no bother checking the event if no callbacks are defined */
	if (!node->cbs)
		return;

	switch (event) {
	case NEUTRON_EVENT_CONNECTED:
		if (node->cbs->connected_cb)
			(*node->cbs->connected_cb)(node, NULL);
		break;
	case NEUTRON_EVENT_DISCONNECTED:
		if (node->cbs->disconnected_cb)
			(*node->cbs->disconnected_cb)(node, NULL);
		break;
	default:
		break;
	}
}

struct nexus_node *nexus_node_create(struct nexus_node_cbs *cbs)
{
	struct nexus_node *node =
		(struct nexus_node *)calloc(1, sizeof(struct nexus_node));

	if (!node) {
		LOG_ERRNO("Failure: cannot allocate memory for nexus node");
		goto clean;
	}

	if (cbs)
		node->cbs = cbs;
	else
		LOGW("Node callbacks are undefined");

	node->loop = neutron_loop_create();
	if (!node->loop) {
		LOGE("Failure: cannot create neutron loop");
		goto clean;
	}

	node->ctx = neutron_ctx_create_with_loop(
		fd_event_cb, node->loop, (void *)node);
	if (!node->loop) {
		LOGE("Failure: cannot create neutron ctx");
		goto clean;
	}

	int ret = neutron_ctx_set_socket_data_cb(node->ctx, recv_cb);
	if (ret) {
		LOG_ERRNO("neutron_ctx_set_socket_data_cb");
		goto clean;
	}

	return node;

clean:
	nexus_node_destroy(node);
	return NULL;
}

void nexus_node_destroy(struct nexus_node *node)
{
	if (node) {
		neutron_ctx_destroy(node->ctx);
		neutron_loop_destroy(node->loop);
		free(node);
		node = NULL;
	}
}

int nexus_node_listen(struct nexus_node *node, const char *address)
{
	int ret = 0;
	struct neutron_addr *addr = neutron_addr_parse(address);
	if (ret) {
		LOG_ERRNO("neutron_addr_parse");
		return ret;
	}

	ret = neutron_ctx_listen(node->ctx, addr);
	if (ret) {
		LOG_ERRNO("neutron_ctx_listen");
		return ret;
	}
	return 0;
}

int nexus_node_connect(struct nexus_node *node, const char *address)
{
	int ret = 0;

	struct neutron_addr *addr = neutron_addr_parse(address);
	if (!addr) {
		LOG_ERRNO("neutron_addr_parse");
		return errno;
	}

	ret = neutron_ctx_connect(node->ctx, addr);
	if (ret) {
		LOG_ERRNO("neutron_ctx_connect");
		return ret;
	}
	return 0;
}

int nexus_node_spin(struct nexus_node *node)
{
	int ret = 0;

	if (!node->is_spinning)
		node->is_spinning = true;

	while (node->is_spinning) {
		ret = neutron_loop_spin(node->loop);
	}
	return ret;
}

void nexus_node_stop(struct nexus_node *node)
{
	if (!node->is_spinning)
		return;

	node->is_spinning = false;
	neutron_loop_wakeup(node->loop);
}

int nexus_node_send_message(struct nexus_node *node, struct nexus_message *msg)
{
	int ret = 0;
	if (!node || !msg)
		return EINVAL;

	uint32_t packlen = nexus_message_get_packed_size(msg);
	uint8_t *packbuf = (uint8_t *)malloc(packlen);
	if (!packbuf) {
		LOGE("Cannot allocate memory for packbuf");
		return ENOMEM;
	}

	nexus_message_pack(msg, packbuf, packlen);

	ret = neutron_ctx_send(node->ctx, packbuf, packlen);
	if (ret) {
		LOG_ERRNO("neutron_ctx_send");
		free(packbuf);
		return ret;
	}

	free(packbuf);
	return 0;
}

int nexus_node_send_protobuf(struct nexus_node *node,
			     ProtobufCMessage *proto_msg,
			     uint32_t type)
{
	int ret = 0;
	size_t protobuflen = protobuf_c_message_get_packed_size(proto_msg);
	uint8_t *pack_protobuf = (uint8_t *)malloc(protobuflen);
	if (!pack_protobuf) {
		LOG_ERRNO(
			"Failed to allocate memory for packing proto message");
		return ENOMEM;
	}

	size_t packedbuflen = protobuf_c_message_pack(proto_msg, pack_protobuf);
	if (packedbuflen != protobuflen) {
		LOGE("Packed size inconsistency");
		free(pack_protobuf);
		return EINVAL;
	}

	struct nexus_message *msg =
		nexus_message_init(packedbuflen, type, pack_protobuf);
	if (!msg) {
		LOGE("Cannot create message");
		return ENOMEM;
	}

	ret = nexus_node_send_message(node, msg);
	nexus_message_destroy(msg);
	return ret;
}
