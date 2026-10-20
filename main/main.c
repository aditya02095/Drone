#include "receiver.h"
#include "pid.h"
#include "motor.h"
#include <math.h>
#include <stdio.h>
#include "driver/gpio.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define motor_1_in 25;	//front right
#define motor_2_in 26;	//front left
#define motor_3_in 27;	//back left
#define motor_4_in 33;	//back right

/*	Throttle: Channel 3
*	Yaw: 4
*	Pitch: 1
*	Elevation: 2
*/
int receiver_pins[5] = {34, 35, 32, 18};

void main()
{
	float motor_1_pwm, motor_2_pwm, motor_3_pwm, motor_4_pwm;
	gpio_num_t motor_1_pin = motor_1_in;
	gpio_num_t motor_2_pin = motor_2_in;
	gpio_num_t motor_3_pin = motor_3_in;
	gpio_num_t motor_4_pin = motor_4_in;
	
	while(1)
	{
		ESP_ERROR_CHECK(pwm_reader_init(motor_1_pin, &motor_1_pwm));
		ESP_ERROR_CHECK(pwm_reader_init(motor_2_pin, &motor_2_pwm));
		ESP_ERROR_CHECK(pwm_reader_init(motor_3_pin, &motor_3_pwm));
		ESP_ERROR_CHECK(pwm_reader_init(motor_4_pin, &motor_4_pwm));

		float motor_1_pwm_set = motor_1_pwm;
		float motor_2_pwm_set = motor_2_pwm;
		float motor_3_pwm_set = motor_3_pwm;
		float motor_4_pwm_set = motor_4_pwm;
		
		ESP_ERROR_CHECK(motor_set(25, motor_1_pwm_set));
		ESP_ERROR_CHECK(motor_set(26, motor_2_pwm_set));
		ESP_ERROR_CHECK(motor_set(27, motor_3_pwm_set));
		ESP_ERROR_CHECK(motor_set(33, motor_4_pwm_set));
	}
}