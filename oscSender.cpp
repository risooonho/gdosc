#include "oscSender.h"
#include "osc/OscOutboundPacketStream.h"

bool gdOscSender::init(std::string host, int port, int buffersize) {
    settings.host = host;
    settings.port = port;
    settings.buffersize = buffersize;
    return setup(settings);
}

bool gdOscSender::setup(gdOscSenderSettings &settings) {
    UdpTransmitSocket *socket = nullptr;
    this->settings = settings;
    try {
        IpEndpointName name = IpEndpointName(settings.host.c_str(), settings.port);
        socket = new UdpTransmitSocket(name);
        sendSocket.reset(socket);
        std::cout << "socket should be reset now" << std::endl;
    } catch (std::exception &e) {
        std::cout << "failed creating sendSocket because: " << e.what() << std::endl;
        sendSocket.reset();
        return false;
    }
    return true;
}

void gdOscSender::clear() {
    sendSocket.reset();
}

void gdOscSender::sendMessage(gdOscMessage &message) {
    if (!sendSocket) {
        std::cout << "trying to send with empty socket" << std::endl;
        return;
    }

    // setting this much larger as it gets trimmed down to the size its using before being sent.
    // TODO: find a way to make it dynamic?
    // static const int OUTPUT_BUFFER_SIZE = 327680;
    char buffer[settings.buffersize];
    osc::OutboundPacketStream p(buffer, settings.buffersize);

    p << ::osc::BeginBundleImmediate;
    appendMessage(message, p);
    p << ::osc::EndBundle;

    sendSocket->Send(p.Data(), p.Size());
}

void gdOscSender::appendMessage(gdOscMessage &message, osc::OutboundPacketStream &p) {
    p << ::osc::BeginMessage(message.getAddress().c_str());
    for (int i = 0; i < message.getNumArgs(); ++i) {
        if (message.getArgType(i) == TYPE_INT32) {
            p << message.getArgAsInt32(i);
        } else if (message.getArgType(i) == TYPE_FLOAT) {
            p << message.getArgAsFloat(i);
        } else if (message.getArgType(i) == TYPE_STRING) {
            p << message.getArgAsString(i).c_str();
        } else {
            throw OscExcInvalidArgumentType();
        }
    }
    p << ::osc::EndMessage;
}
