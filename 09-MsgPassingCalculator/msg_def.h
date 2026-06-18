/*
 * msg_def.h
 *
 *  Created on: Jun 16, 2026
 *      Author: ehab
 */

#ifndef MSG_DEF_H_
#define MSG_DEF_H_

#include "sys/iomsg.h"

typedef struct{
	char op;
	int8_t num1;
	int8_t num2;
} calculator_t;

typedef struct{
	int16_t result;
} result_t;

#endif /* MSG_DEF_H_ */
