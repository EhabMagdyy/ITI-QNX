#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "msg_def.h"
#include <sys/neutrino.h>
#include <errno.h>

int main(void) {
	int chid;
	rcvid_t rcvid;
	calculator_t msg;
	result_t reply_msg;

	chid = ChannelCreate(0);
	printf("PID: %d\n", getpid());
	printf("CHID: %d\n", chid);

	while(1){
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);

		switch(msg.op){
		case '+':	reply_msg.result = msg.num1 + msg.num2;	break;
		case '-':	reply_msg.result = msg.num1 - msg.num2;	break;
		case '*':	reply_msg.result = msg.num1 * msg.num2;	break;
		case '/':   reply_msg.result = msg.num2 == 0? 0 : (msg.num1 / msg.num2);	break;
		default:	break;
		}

		MsgReply(rcvid, EOK, &reply_msg, sizeof(reply_msg));
	}

	return EXIT_SUCCESS;
}
