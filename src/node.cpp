#include <algorithm>
#include <node.h>
#include <log.h>
#include <nexus.hpp>

namespace nexus {

Node::Node(NodeHandler *handler, neutron::Loop *loop)
	: mHandler(handler), mContext(new neutron::Context(this, loop))

{
	if (loop != nullptr)
		mIsLoopExternal = true;
	mLoop = mContext->getLoop();
}

Node::~Node()
{
	delete mContext;
	delete mHandler;
	if (mIsLoopExternal)
		delete mLoop;
}

int Node::listen(const std::string &address)
{
	int ret = 0;
	neutron::Address addr = neutron::Address(address);
	if (!addr.isValid()) {
		LOGE("Address %s is invalid", address.c_str());
		return EINVAL;
	}
	ret = mContext->listen(addr);
	if (ret) {
		LOGE("Context failed to listen");
		return ret;
	}
	return 0;
}

int Node::connect(const std::string &address)
{
	int ret = 0;
	neutron::Address addr = neutron::Address(address);
	if (!addr.isValid()) {
		LOGE("Address %s is invalid", address.c_str());
		return EINVAL;
	}
	ret = mContext->connect(addr);
	if (ret) {
		LOGE("Context failed to connect");
		return ret;
	}
	return 0;
}

int Node::spin()
{
	int ret = 0;
	if (!mIsSpinning)
		mIsSpinning = true;

	while (mIsSpinning) {
		ret = mLoop->spin();
	}
	return ret;
}

void Node::stop()
{
	if (!mIsSpinning)
		return;

	mIsSpinning = false;
	mLoop->wakeup();
}

int Node::sendMessage(Message *msg)
{
	int ret = 0;
	uint32_t packlen = msg->getPackedSize();
	uint8_t *packbuf = (uint8_t *)malloc(packlen);
	if (!packbuf) {
		LOGE("Cannot allocate memory for pack buffer");
		return ENOMEM;
	}

	msg->pack(packbuf);
	ret = mContext->send(packbuf, packlen);
	if (ret)
		LOG_ERRNO("mContext::send");

	free(packbuf);
	return ret;
}

int Node::sendProtoMessage(uint32_t type, google::protobuf::Message *msg)
{
	int ret = 0;
	size_t protobuflen = msg->ByteSizeLong();
	uint8_t *packprotobuf = (uint8_t *)malloc(protobuflen);
	if (!packprotobuf) {
		LOG_ERRNO(
			"Failed to allocate memory for packing proto message");
		return ENOMEM;
	}

	if (!msg->SerializeToArray(packprotobuf, protobuflen)) {
		LOGE("Failed to serialize message");
	}

	Message *nexus_msg = new Message(protobuflen, type, packprotobuf);
	ret = sendMessage(nexus_msg);
	delete nexus_msg;
	return ret;
}

/* definitions for private neutron Context callbacks */

inline void Node::onConnected(neutron::Context *ctx, neutron::Connection *conn)
{
	mHandler->onConnected(this, nullptr);
}

inline void Node::onDisconnected(neutron::Context *ctx,
				 neutron::Connection *conn)
{
	mHandler->onDisconnected(this, nullptr);
}

inline void Node::onFdCreated(neutron::Context *ctx, int fd)
{
	LOGI("Fd Created %d", fd);
}

inline void Node::recvData(neutron::Context *ctx,
			   neutron::Connection *conn,
			   const std::vector<uint8_t> &buf)
{
	const Message *msg = Message::unpack(buf.data());
	mHandler->onMsgRecv(this, msg, nullptr);
}

} // namespace nexus
