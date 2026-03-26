#include <message.h>
#include <stdlib.h>
#include <log.h>
#include <string.h>

struct nexus_message *nexus_message_init(uint32_t len,
					 uint32_t type,
					 uint8_t *data)
{
	struct nexus_message *msg =
		(struct nexus_message *)calloc(1, sizeof(struct nexus_message));

	if (!msg) {
		LOG_ERRNO("Failed to allocate memory for nexus message");
		return NULL;
	}

	msg->type = type;
	msg->len = len;

	msg->data = (uint8_t *)malloc(msg->len);
	if (!msg->data) {
		LOG_ERRNO(
			"Failed to allocate memory for data payload inside message");
		return NULL;
	}

	if (data)
		memcpy(msg->data, data, msg->len);

	return msg;
}

void nexus_message_destroy(struct nexus_message *msg)
{
	if (msg) {
		if (msg->data) {
			free(msg->data);
			msg->data = NULL;
		}
		free(msg);
		msg = NULL;
	}
}

void nexus_message_print(struct nexus_message *msg)
{
	LOGI("Message Type: %u", msg->type);
	LOGI("Message Payload Length: %u", msg->len);
	LOGI("Message Payload:");
	for (int i = 0; i < msg->len; i++) {
		printf("%u,", msg->data[i]);
	}
	printf("\n");
}

int nexus_message_get_packed_size(struct nexus_message *msg)
{
	return msg->len + sizeof(msg->len) + sizeof(msg->type);
}

void nexus_message_pack(struct nexus_message *msg,
			uint8_t *buf,
			uint32_t buflen)
{
	int offset = 0;
	memcpy(buf + offset, (uint8_t *)&msg->type, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buf + offset, (uint8_t *)&msg->len, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buf + offset, msg->data, msg->len);
}

struct nexus_message *nexus_message_unpack(uint8_t *buf, uint32_t buflen)
{
	struct nexus_message *msg =
		(struct nexus_message *)calloc(1, sizeof(struct nexus_message));

	if (!msg) {
		LOG_ERRNO("Failed to allocate memory for nexus message");
		return NULL;
	}

	int offset = 0;
	memcpy((uint8_t *)&msg->type, buf + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy((uint8_t *)&msg->len, buf + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	msg->data = (uint8_t *)malloc(msg->len);
	if (!msg->data) {
		LOG_ERRNO(
			"Failed to allocate memory for data payload inside message");
		free(msg);
		return NULL;
	}

	memcpy(msg->data, buf + offset, msg->len);
	return msg;
}
