#include <nexus-cc/foo.pb.h>
#include <nexus.hpp>
#include <log.h>
#include <unistd.h>

#define TEST_ADDRESS "unix:@test_node"

class TestHandler : public nexus::NodeHandler {
public:
	inline virtual ~TestHandler() override {}
	inline virtual void onConnected(nexus::Node *node,
					void *userdata) override
	{
		LOGI("Node connected");
	}
	inline virtual void onDisconnected(nexus::Node *node,
					   void *userdata) override
	{
		LOGI("Node disconnected");
	}
	inline virtual void onMsgRecv(nexus::Node *node,
				      const nexus::Message *msg,
				      void *userdata) override
	{
		LOGI("Message received");
		assert(msg->type() == 1);

		nexus::Foo recover;
		recover.ParseFromArray(msg->buf(), msg->len());

		assert(recover.x() == 10.0);
		assert(recover.y() == -13);
		assert(recover.s() == "FOO");
		node->stop();
	}
};

void test_node()
{
	if (fork()) {
		nexus::Node *server = new nexus::Node(new TestHandler());
		server->listen(TEST_ADDRESS);
		server->spin();
		delete server;
		LOGI("%s passed", __func__);
	} else {
		nexus::Node *client = new nexus::Node();
		client->connect(TEST_ADDRESS);
		nexus::Foo msg;
		msg.set_x(10);
		msg.set_y(-13);
		msg.set_s("FOO");
		client->sendProtoMessage(1, &msg);
		delete client;
	}
}

int main(int argc, char **argv)
{
	test_node();
}
