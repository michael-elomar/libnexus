#include <nexus-cc/foo.pb.h>
#include <nexus.hpp>
#include <log.h>

void test_pack_unpack()
{
	uint8_t foo[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	uint32_t type = 10;

	nexus::Message *msg = new nexus::Message(10, type, foo);

	uint8_t *buffer = (uint8_t *)malloc(msg->getPackedSize());
	msg->pack(buffer);

	const nexus::Message *recoverd_msg = nexus::Message::unpack(buffer);
	assert(*recoverd_msg == *msg);
	LOGI("%s passed", __func__);

	free(buffer);
	delete msg;
	delete recoverd_msg;
}

void test_proto()
{
	nexus::Foo foo_msg;
	foo_msg.set_x(10);
	foo_msg.set_y(-13);
	foo_msg.set_s("FOO");

	size_t protobuflen = foo_msg.ByteSizeLong();
	uint8_t *packprotobuf = (uint8_t *)malloc(protobuflen);
	if (!packprotobuf) {
		LOG_ERRNO(
			"Failed to allocate memory for packing proto message");
		return;
	}

	if (!foo_msg.SerializeToArray(packprotobuf, protobuflen)) {
		free(packprotobuf);
	}

	nexus::Foo recover;
	recover.ParseFromArray(packprotobuf, protobuflen);

	assert(foo_msg.x() == recover.x());
	assert(foo_msg.y() == recover.y());
	assert(foo_msg.s() == recover.s());
	LOGI("%s passed", __func__);
}

int main(int argc, char **argv)
{
	test_pack_unpack();
	test_proto();
}
