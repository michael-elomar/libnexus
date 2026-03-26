#include <message.h>
#include <nexus.hpp>
#include <log.h>

namespace nexus {

Message::Message(uint32_t len, uint32_t type, uint8_t *data)
{
	mMsg = nexus_message_init(len, type, data);
}
Message::Message(struct nexus_message *msg)
{
	mMsg = msg;
}

Message::~Message()
{
	nexus_message_destroy(mMsg);
}

int Message::getPackedSize()
{
	return nexus_message_get_packed_size(mMsg);
}

void Message::pack(uint8_t *buf, uint32_t buflen)
{
	nexus_message_pack(mMsg, buf, buflen);
}

const Message *Message::unpack(uint8_t *buf, uint32_t buflen)
{
	const Message *msg = new Message(nexus_message_unpack(buf, buflen));
	return msg;
}

void Message::print()
{
	LOGI("Type: %u, Size: %u", mType, mLen);
	for (int i = 0; i < mLen; i++)
		printf("%u, ", mBuf[i]);
}

} // namespace nexus
