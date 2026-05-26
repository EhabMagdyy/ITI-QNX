#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "rpi_gpio.h"

int main(void) {

	printf("Setting up GPIO\n");
	rpi_gpio_setup(GPIO17, GPIO_OUT);

	while(1){
		rpi_gpio_output(GPIO17, GPIO_HIGH);
		sleep(1);
		rpi_gpio_output(GPIO17, GPIO_LOW);
		sleep(1);
	}

	return EXIT_SUCCESS;
}
