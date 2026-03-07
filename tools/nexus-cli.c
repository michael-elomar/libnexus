#include <math.h>
#include <nexus.h>
#include <neutron.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <log.h>
#include <string.h>
#include <signal.h>

struct ctx {
	bool is_publisher;
	const char *publish_address;
	const char *subscribe_address;

	const char *raw_message;

	struct nexus_node *node;
};

struct ctx s_ctx;

void sig_handler(int signum)
{
	nexus_node_stop(s_ctx.node);
}

static void clean_exit()
{
	if (s_ctx.node) {
		nexus_node_destroy(s_ctx.node);
	}
}

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

static void on_msg_revv(struct nexus_node *node,
			struct nexus_message *msg,
			void *userdata)
{
	nexus_message_print(msg);
}

static void on_connected(struct nexus_node *node, void *userdata)
{
	LOGI("Connected");
}

static void on_disconnected(struct nexus_node *node, void *userdata)
{
	LOGI("Disconnected");
}

struct nexus_node_cbs s_cbs = {
	.msg_recv_cb = &on_msg_revv,
	.connected_cb = NULL,
	.disconnected_cb = NULL,
};

int main(int argc, char **argv)
{
	/* exit on SIGINT & SIGTERM */
	signal(SIGINT, &sig_handler);
	signal(SIGTERM, &sig_handler);

	/* ignore SIGPIPE */
	signal(SIGPIPE, SIG_IGN);

	int ret = 0;

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
			s_ctx.is_publisher = true;
			s_ctx.publish_address = optarg;
			break;
		case 's':
			s_ctx.is_publisher = false;
			s_ctx.subscribe_address = optarg;
			break;
		case 'm':
			if (!s_ctx.is_publisher) {
				LOGE("Message option must only be used with a publisher ctx");
				usage(argv[0]);
				exit(EXIT_FAILURE);
			}
			s_ctx.raw_message = optarg;
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

	uint8_t buf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int type = 3;

	struct nexus_message *msg = nexus_message_init(10, type, buf);
	int packed_buflen = nexus_message_get_packed_size(msg);
	uint8_t *packed_buf = (uint8_t *)malloc(packed_buflen);
	nexus_message_pack(msg, packed_buf, packed_buflen);

	s_ctx.node = nexus_node_create(&s_cbs);
	if (!s_ctx.node) {
		LOGE("Failed to create node");
		goto clean;
	}

	if (s_ctx.is_publisher) {
		ret = nexus_node_listen(s_ctx.node, s_ctx.publish_address);
		if (ret) {
			LOG_ERRNO("nexus_node_advertise");
			goto clean;
		}
		nexus_node_spin(s_ctx.node);

	} else {
		ret = nexus_node_connect(s_ctx.node, s_ctx.subscribe_address);
		if (ret) {
			LOG_ERRNO("nexus_node_subscribe");
			goto clean;
		}
		nexus_node_send_message(s_ctx.node, msg);
	}

	clean_exit();
	return 0;

clean:
	clean_exit();
	return ret;
}
