#ifndef INC_MESSAGEHANDLER_H_
#define INC_MESSAGEHANDLER_H_

class MessageHandler {
public:

	virtual void onMessage(const char* message) = 0;
};

#endif

