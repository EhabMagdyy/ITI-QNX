#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "msg_def.h"

#include <sys/neutrino.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

// Read one line from stdin into buf, strip the newline
static int readline(const char *prompt, char *buf, int size) {
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(buf, size, stdin) == NULL) return -1;
    /* Strip trailing newline */
    int len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
    return 0;
}

int main(void){
    int coid;
    calculator_t msg;
    result_t reply_msg;

    int server_pid, server_chid;
    char line[64];

    // Connection setup
    if (readline("Enter server PID: ", line, sizeof(line)) < 0) return EXIT_FAILURE;
    sscanf(line, "%d", &server_pid);

    if (readline("Enter server Channel ID: ", line, sizeof(line)) < 0) return EXIT_FAILURE;
    sscanf(line, "%d", &server_chid);

    coid = ConnectAttach(0, server_pid, server_chid, _NTO_SIDE_CHANNEL, 0);
    if(coid == -1) {
        perror("ConnectAttach failed");
        return EXIT_FAILURE;
    }
    printf("Connected successfully on coid: %d\n", coid);

    while(1){
		// Read operands and operator
		int tmp1, tmp2;

		if (readline("Enter num1: ", line, sizeof(line)) < 0) return EXIT_FAILURE;
		sscanf(line, "%d", &tmp1);
		msg.num1 = (int8_t)tmp1;

		if (readline("Enter num2: ", line, sizeof(line)) < 0) return EXIT_FAILURE;
		sscanf(line, "%d", &tmp2);
		msg.num2 = (int8_t)tmp2;

		if (readline("Enter operator (+, -, *, /): ", line, sizeof(line)) < 0) return EXIT_FAILURE;
		msg.op = line[0];

		// Send message and wait for reply
		if(MsgSend(coid, &msg, sizeof(msg), &reply_msg, sizeof(reply_msg)) == -1){
			perror("MsgSend failed");
			ConnectDetach(coid);
			return EXIT_FAILURE;
		}

		printf("Result: %d %c %d = %d\n", msg.num1, msg.op, msg.num2, reply_msg.result);
	}

    ConnectDetach(coid);

    return EXIT_SUCCESS;
}
