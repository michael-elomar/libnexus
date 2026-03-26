#include <nexus.h>
#include <neutron.hpp>
#include <google/protobuf/message.h>

namespace nexus {

class Message;
class Node;
class NodeHandler;

class Message {
public:
	Message(uint32_t len, uint32_t type, uint8_t *data);
	Message(struct nexus_message *msg);
	~Message();

	int getPackedSize();

	void pack(uint8_t *buf, uint32_t buflen);
	static const Message *unpack(uint8_t *buf, uint32_t buflen);

	void print();

private:
	uint32_t mLen, mType;
	uint8_t *mBuf;
	struct nexus_message *mMsg;
};

class NodeHandler {
public:
	inline NodeHandler() {}
	inline virtual ~NodeHandler() {}
	inline virtual void onConnected(Node *node, void *userdata) = 0;
	inline virtual void onDisconnected(Node *node, void *userdata) = 0;
	inline virtual void onMsgRecv(Node *node,
				      Message *msg,
				      void *userdata) = 0;
};

class Node : public neutron::Context::Handler {
public:
	Node(NodeHandler *handler, neutron::Loop *loop = nullptr);
	~Node();

	int listen(const std::string &address);
	int connect(const std::string &address);
	int spin();
	void stop();

	int sendMessage(Message *msg);
	int sendProtoMessage(uint32_t type, google::protobuf::Message *msg);

private:
	/* private callbacks for the neutron::Context  */
	inline virtual void onConnected(neutron::Context *ctx,
					neutron::Connection *conn) override;
	inline virtual void onDisconnected(neutron::Context *ctx,
					   neutron::Connection *conn) override;

	inline virtual void onFdCreated(neutron::Context *ctx, int fd) override;
	inline virtual void recvData(neutron::Context *ctx,
				     neutron::Connection *conn,
				     const std::vector<uint8_t> &buf) override;

private:
	neutron::Loop *mLoop;
	neutron::Context *mContext;
	NodeHandler *mHandler;
	std::string mAddress;
	bool is_publisher;
	bool mIsSpinning;
};
} // namespace nexus
