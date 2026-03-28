#include <nexus.hpp>
#include <log.h>

namespace nexus {

Message::Message(uint32_t len, uint32_t type, uint8_t *data)
	: mLen(len), mType(type), mBuf((uint8_t *)malloc(mLen))
{
	if (mBuf != nullptr)
		memcpy(mBuf, data, mLen);
}

Message::~Message()
{
	free(mBuf);
}

int Message::getPackedSize()
{
	return mLen + sizeof(mLen) + sizeof(mType);
}

void Message::pack(uint8_t *buf)
{
	int offset = 0;
	memcpy(buf + offset, (uint8_t *)&mType, sizeof(uint32_t));
	offset += sizeof(mType);

	memcpy(buf + offset, (uint8_t *)&mLen, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buf + offset, mBuf, mLen);
}

const Message *Message::unpack(const uint8_t *buf)
{

	int offset = 0;
	uint32_t type, len;
	memcpy((uint8_t *)&type, buf + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy((uint8_t *)&len, buf + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	const Message *msg = new Message(len, type, (uint8_t *)malloc(len));

	memcpy(msg->mBuf, buf + offset, msg->mLen);

	return msg;
}

const void Message::print() const
{
	LOGI("Type: %u, Size: %u", mType, mLen);
	for (int i = 0; i < mLen; i++)
		printf("%u, ", mBuf[i]);
	printf("\n");
}

bool Message::operator==(const Message &msg) const
{
	bool eq = msg.mLen == this->mLen;
	eq = eq && (msg.mType == this->mType);

	eq = eq && (memcmp(this->mBuf, msg.mBuf, this->mLen) == 0);
	return eq;
}

} // namespace nexus
