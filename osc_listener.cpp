#include "osc_listener.h"

OSCListener::OSCListener() {
  _port = 18002;
  try {
  osc_rcv = new OSCReceiver(_port);
  std::cout << "OSC listener port: " << _port << std::endl;
  start();
  emit_signal("osc_listener_ready");
  }
  catch (const std::exception& e) {
    std::cout << "Constructor tried to instantiate OSCReceiver but " << e.what() << std::endl;
  }
}

OSCListener::~OSCListener() {
  if(osc_rcv) {
    delete osc_rcv;
  }
}

void OSCListener::set_port(int port) {
  delete osc_rcv;
  _port = port;
  osc_rcv = new OSCReceiver(_port);
}

bool OSCListener::setup(int port) {
  _port = port;
  try {
    osc_rcv->setup(_port);
    std::cout << "setting port: " << _port << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << "listener.setup failed: " << e.what() << std::endl;
  }
}

bool OSCListener::start() {
  listener = std::thread([this]{
      try {
        get_msg();
      }
      catch (const std::exception& e) {
        std::cout << "cannot get message: " << e.what() << std::endl;
      }
    });
  listener.detach();
  return true;
}

String OSCListener::getOscMsgAsString(gdOscMessage m) {
  std::string msg_string;
  msg_string = m.getAddress();
  msg_string += ":";
  for(int i =0; i < m.getNumArgs(); i++){
    msg_string += " " + m.getArgTypeName(i);
    msg_string += ":";
    if(m.getArgType(i) == TYPE_INT32) {
      msg_string += std::to_string(m.getArgAsInt32(i));
    }
    else if(m.getArgType(i) == TYPE_FLOAT) {
      msg_string += std::to_string(m.getArgAsFloat(i));
    }
    else if(m.getArgType(i) == TYPE_STRING) {
      msg_string += m.getArgAsString(i);
    }
    else {
      msg_string += "unknown";
    }
  }
  return String(msg_string.c_str());
}

Array OSCListener::getOscMessageAsArray(gdOscMessage m) {
  Array a;
  a.append(String(m.getAddress().c_str()));
  for(int i =0; i < m.getNumArgs(); i++){
    if(m.getArgType(i) == TYPE_INT32) {
      a.append(m.getArgAsInt32(i));
    }
    else if(m.getArgType(i) == TYPE_FLOAT) {
      a.append(m.getArgAsFloat(i));
    }
    else if(m.getArgType(i) == TYPE_STRING) {
      a.append(m.getArgAsString(i).c_str());
    }
    else {
      a.append("unknown");
    }
  }
  return a;
}

Array OSCListener::get_msg(){
  while(true){
    if(osc_rcv->hasWaitingMessages()){
      gdOscMessage message;
      osc_rcv->getNextMessage(&message);
      msg = getOscMessageAsArray(message);
      emit_signal("osc_message", msg);
      }
  }
}

bool OSCListener::active(bool a){
  // TODO: activate/desactivate OSC listening
}

void OSCListener::_bind_methods() {

  // methods
  ClassDB::bind_method(D_METHOD("setup", "port"), &OSCListener::setup);
  ClassDB::bind_method(D_METHOD("set_port", "port"), &OSCListener::set_port);
  ClassDB::bind_method(D_METHOD("get_port"), &OSCListener::get_port);
  ClassDB::bind_method(D_METHOD("get_msg"), &OSCListener::get_msg);

  // Properties

  // signals
  ADD_SIGNAL(MethodInfo("osc_message"));
  ADD_SIGNAL(MethodInfo("osc_listener_ready"));
}
