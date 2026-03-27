#include <nexus.hpp>
#include <log.h>

void test_pack_unpack()
{
	uint8_t foo[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	uint32_t type = 10;

	nexus::Message *msg = new nexus::Message(10, type, foo);

	uint8_t *buffer = (uint8_t *)malloc(msg->getPackedSize());
	msg->pack(buffer);

	const nexus::Message *recovered_msg = nexus::Message::unpack(buffer);

	assert(recovered_msg == msg);
	LOGI("Test has passed");
	delete recovered_msg;
	delete msg;
}

int main(int argc, char **argv)
{
	test_pack_unpack();
}
