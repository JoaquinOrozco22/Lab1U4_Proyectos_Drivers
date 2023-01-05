
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "driver/gpio.h"

#include "C:/Espressif/frameworks/esp-idf-v4.4.3/workspace/PROYECTO_ADC/components/adc_drivers/include/adc_drivers.h"
#include "C:/Espressif/frameworks/esp-idf-v4.4.3/components/soc/esp32/include/soc/sens_struct.h"

#define MODULO1 ADC_MODULO_1
#define MODULO2 ADC_MODULO_2

#define CANAL_0 ADC_CANAL_0
#define CANAL_1 ADC_CANAL_1
#define CANAL_2 ADC_CANAL_2
#define CANAL_3 ADC_CANAL_3
#define CANAL_4 ADC_CANAL_4
#define CANAL_5 ADC_CANAL_5
#define CANAL_6 ADC_CANAL_6
#define CANAL_7 ADC_CANAL_7
#define CANAL_8 ADC_CANAL_8
#define CANAL_9 ADC_CANAL_9

#define RESOLUCION_9BITS ADC_9_BITS_RESOLUCION
#define RESOLUCION_9BITS_VAL_DECIMAL 511
#define RESOLUCION_10BITS ADC_10_BITS_RESOLUCION
#define RESOLUCION_10BITS_VAL_DECIMAL 1023
#define RESOLUCION_11BITS ADC_11_BITS_RESOLUCION
#define RESOLUCION_11BITS_VAL_DECIMAL 2047
#define RESOLUCION_12BITS ADC_12_BITS_RESOLUCION
#define RESOLUCION_12BITS_VAL_DECIMAL 4095

#define ATENUACION_0dB ADC_ATENUACION_0_Db
#define ATENUACION_2_5dB ADC_ATENUACION_2_5_Db
#define ATENUACION_6dB ADC_ATENUACION_6_Db
#define ATENUACION_11dB ADC_ATENUACION_11_Db

#define VOLTAJE 3.28
#define VOLTAJE2 5.00

#define AUMENTAR GPIO_NUM_21
#define DISMINUIR GPIO_NUM_22

#define LED1 GPIO_NUM_16
#define LED2 GPIO_NUM_19

#define VENTILADORES GPIO_NUM_23

#define CANTIDAD_MUESTRAS 64

#define RANGO_CANALES 5

void app_main(void)
{
	esp_err_t flag;

	uint32_t lectura_adc1_canal_6=0;

	float v0=0;
	float v1=0;
	float v2=0;
	float v3=0;
	float v4=0;

	float temperatura=0;
	int Estado_boton_aumentar;
	int Estado_boton_disminuir;
	int setpoint=25;
	int activar=0;

	uint32_t lectura[RANGO_CANALES];

	gpio_reset_pin(AUMENTAR);
	gpio_reset_pin(DISMINUIR);
	gpio_reset_pin(LED1);
	gpio_reset_pin(LED2);
	gpio_reset_pin(VENTILADORES);

	gpio_set_direction(AUMENTAR, GPIO_MODE_INPUT);
	gpio_set_direction(DISMINUIR, GPIO_MODE_INPUT);
	gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
	gpio_set_direction(VENTILADORES, GPIO_MODE_OUTPUT);

	gpio_set_pull_mode(AUMENTAR, GPIO_PULLUP_ONLY);
	gpio_set_pull_mode(DISMINUIR, GPIO_PULLUP_ONLY);

	flag = ADC_SECUENCIA_INIT(MODULO2, RESOLUCION_12BITS, ATENUACION_11dB, CANAL_0, RANGO_CANALES);
	if(flag == ESP_FAIL)
	{
		printf("Ha ocurrido un error en la configuracion del modulo y canal ADC.\n");
		exit(1);
	}

	flag=ADC_INIT(MODULO1, RESOLUCION_12BITS, ATENUACION_11dB, CANAL_6);
	if(flag == ESP_FAIL)
	{
		printf("Ha ocurrido un error en la configuracion del modulo y canal ADC.\n");
		exit(1);
	}

    while (true) {

    	ADC_SECUENCIA_READ(MODULO2, RESOLUCION_12BITS, CANAL_0, RANGO_CANALES, lectura, CANTIDAD_MUESTRAS);
    	lectura_adc1_canal_6=ADC1_READ(CANAL_6, CANTIDAD_MUESTRAS);

		temperatura=(((lectura_adc1_canal_6*VOLTAJE2)/RESOLUCION_12BITS_VAL_DECIMAL)*100)-338;

		v0 = (lectura[0]*VOLTAJE)/RESOLUCION_12BITS_VAL_DECIMAL;
		v1 = (lectura[1]*VOLTAJE)/RESOLUCION_12BITS_VAL_DECIMAL;
      	v2 = (lectura[2]*VOLTAJE)/RESOLUCION_12BITS_VAL_DECIMAL;
		v3 = (lectura[3]*VOLTAJE)/RESOLUCION_12BITS_VAL_DECIMAL;
		v4 = (lectura[4]*VOLTAJE)/RESOLUCION_12BITS_VAL_DECIMAL;
		/*v5 = (lectura[5]*VOLTAJE)/RESOLUCION_12BITS_VAL_DECIMAL;
		v6 = (lectura[6]*VOLTAJE)/RESOLUCION_12BITS_VAL_DECIMAL;
		v7 = (lectura[7]*VOLTAJE)/RESOLUCION_12BITS_VAL_DECIMAL;
		v8 = (lectura[8]*VOLTAJE)/RESOLUCION_12BITS_VAL_DECIMAL;
		v9 = (lectura[9]*VOLTAJE)/RESOLUCION_12BITS_VAL_DECIMAL;*/


    	printf("\nDireccion de SENS: %p\n", &SENS);
		printf("\nSetpoint: %d\n", setpoint);
		printf("Temperatura Canal 8 ADC2: %.2f Grados\n",temperatura);


		printf("Voltaje Canal 0 ADC2: %.2f V\n", v0);
		printf("Voltaje Canal 1 ADC2: %.2f V\n", v1);
		printf("Voltaje Canal 2 ADC2: %.2f V\n", v2);
		printf("Voltaje Canal 3 ADC2: %.2f V\n", v3);
		printf("Voltaje Canal 4 ADC2: %.2f V\n", v4);



		vTaskDelay(pdMS_TO_TICKS(1000));

    	Estado_boton_aumentar = gpio_get_level(AUMENTAR);
    	if(Estado_boton_aumentar == 0)
    	{
    		setpoint++;
    	}
    	Estado_boton_disminuir = gpio_get_level(DISMINUIR);
    	if(Estado_boton_disminuir == 0)
    	{
    		setpoint--;
    	}
    	if(temperatura <= setpoint)
    	{
    		activar=0;
    	}
    	else
    	{
    		activar=1;
    	}
		if(activar==0)
		{
   	    	gpio_set_level(VENTILADORES, 1);
   	    	gpio_set_level(LED1, 1);
   	    	gpio_set_level(LED2, 0);
		}
		else
		{
   	    	gpio_set_level(VENTILADORES, 0);
   	    	gpio_set_level(LED1, 0);
   	    	gpio_set_level(LED2, 1);
		}
    }
}

