#pragma once

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
	~Message();

	int getPackedSize();

	void pack(uint8_t *buf);
	static const Message *unpack(const uint8_t *buf);

	const void print() const;

public:
	bool operator==(const Message &msg) const;

private:
	uint32_t mLen, mType;
	uint8_t *mBuf = nullptr;
};

class NodeHandler {
public:
	inline NodeHandler() {}
	inline virtual ~NodeHandler() {}
	inline virtual void onConnected(Node *node, void *userdata) = 0;
	inline virtual void onDisconnected(Node *node, void *userdata) = 0;
	inline virtual void onMsgRecv(Node *node,
				      const Message *msg,
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
	bool mIsLoopExternal;
	bool mIsSpinning;
};
} // namespace nexus
