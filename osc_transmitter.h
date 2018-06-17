/* osc_client.h */
#include "scene/3d/spatial.h"
#include "scene/3d/position_3d.h"
#include "scene/main/node.h"
#include "node_path.h"
#include "print_string.h"
#include <cstdlib>
#include <iostream>
#include <memory>

#include "oscSender.h"

#ifndef GDOSC_H
#define GDOSC_H

class OSCtransmitter : public Node {
    GDCLASS(OSCtransmitter, Node);

protected:
    static void _bind_methods();

public:

    OSCtransmitter();
    OSCtransmitter(String host, int port);
    ~OSCtransmitter();
    void init(String host, int port, int buffersize);
    void testSend();
    void setAddress(String a);
    void sendMessage();
    void appendInt(int m);
    void appendFloat(float m);
    void appendString(String m);
    void reset();

private:

    gdOscSender* osc_sender;
    gdOscMessage msg;

};

#endif