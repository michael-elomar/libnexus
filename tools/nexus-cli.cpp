#include <math.h>
#include <nexus.hpp>
#include <neutron.hpp>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <log.h>
#include <string.h>
#include <signal.h>

class CLIHandler : public nexus::NodeHandler {
public:
	inline virtual ~CLIHandler() override {}
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
		msg->print();
	}
};

class App {
public:
	App(CLIHandler *handler)
	{
		App::sInstance = this;
		mNode = new nexus::Node(handler);
	}

	~App()
	{
		delete mNode;
	}

	static void sigHandler(int signum)
	{
		sInstance->stop();
	}

	int run()
	{
		if (mIsServer) {
			mNode->listen(mAddress);
			return mNode->spin();
		} else {
			return mNode->connect(mAddress);
		}
	}

	void stop()
	{
		mNode->stop();
	}

	void setServer(bool isServer)
	{
		mIsServer = isServer;
	}

	void setAddress(const std::string &address)
	{
		mAddress = address;
	}

	bool isServer()
	{
		return mIsServer;
	}

	void sendMsg(nexus::Message *msg)
	{
		mNode->sendMessage(msg);
	}

private:
	inline static App *sInstance;
	nexus::Node *mNode;
	std::string mAddress;
	bool mIsServer;
};

static void usage(const char *progname)
{
	printf("usage: %s [<options>]\n"
	       "  -h --help: print this help message and exit.\n"
	       "  -p --publish: use cli to publish message to the given address.\n"
	       "  -s --subscribe: subscribe to the given address to receive mesasge.\n"
	       "  -m --message: the value of the message to be sent while using the publish option.\n"
	       "  -l --list: integer whose multiples will be replaced by the value given to str2.\n",
	       progname);
}

int main(int argc, char **argv)
{
	/* exit on SIGINT & SIGTERM */
	signal(SIGINT, &App::sigHandler);
	signal(SIGTERM, &App::sigHandler);

	/* ignore SIGPIPE */
	signal(SIGPIPE, SIG_IGN);

	int ret = 0;

	std::string raw_message;
	App app(new CLIHandler());

	const struct option long_options[] = {
		{"help", no_argument, NULL, 'h'},
		{"publish", required_argument, NULL, 'p'},
		{"subscribe", required_argument, NULL, 's'},
		{"message", required_argument, NULL, 'm'},
		{"list", required_argument, NULL, 'l'},
		{NULL, 0, NULL, 0},
	};

	const char short_options[] = "hp:s:ml";
	char c = 0;
	int optidx = 0;
	while ((c = getopt_long(
			argc, argv, short_options, long_options, &optidx))
	       != -1) {
		switch (c) {
		case 'h':
			usage(argv[0]);
			exit(EXIT_SUCCESS);
			break;
		case 'p':
			app.setServer(true);
			app.setAddress(optarg);
			break;
		case 's':
			app.setServer(false);
			app.setAddress(optarg);
			break;
		case 'm':
			if (!app.isServer()) {
				LOGE("Message option must only be used with a publisher ctx");
				usage(argv[0]);
				exit(EXIT_FAILURE);
			}
			raw_message = optarg;
		case 'l':
			LOGI("Received list option");
			break;
		default:
			LOGW("Unsupported option: '%c'", c);
			usage(argv[0]);
			exit(EXIT_FAILURE);
			break;
		}
	}

	app.run();
	if (!app.isServer()) {
		uint8_t buf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		nexus::Message *msg = new nexus::Message(10, 3, buf);
		app.sendMsg(msg);
		delete msg;
	}
}
