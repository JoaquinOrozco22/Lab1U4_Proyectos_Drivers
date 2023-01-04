#include <stdio.h>
#include "adc_drivers.h"
#include "driver/adc.h"
#include "C:/Espressif/frameworks/esp-idf-v4.4.3/workspace/PROYECTO_ADC/components/adc_drivers/include/adc_drivers_hal.h"
#include "C:/Espressif/frameworks/esp-idf-v4.4.3/components/driver/adc_common.c"


const int adc1_maping_canales_ent_sal[MAX_CANTIDAD_CANALES_ADC1] = {ADC1_CANAL_0_GPIO_NUMERO, ADC1_CANAL_1_GPIO_NUMERO, ADC1_CANAL_2_GPIO_NUMERO, ADC1_CANAL_3_GPIO_NUMERO, ADC1_CANAL_4_GPIO_NUMERO,
																	ADC1_CANAL_5_GPIO_NUMERO, ADC1_CANAL_6_GPIO_NUMERO, ADC1_CANAL_7_GPIO_NUMERO};

const int adc2_maping_canales_ent_sal[MAX_CANTIDAD_CANALES_ADC2] = {ADC2_CANAL_0_GPIO_NUMERO, ADC2_CANAL_1_GPIO_NUMERO, ADC2_CANAL_2_GPIO_NUMERO, ADC2_CANAL_3_GPIO_NUMERO, ADC2_CANAL_4_GPIO_NUMERO,
	     	 	 	 	 	 	 	 	 	 	 	 	 	 	 	ADC2_CANAL_5_GPIO_NUMERO, ADC2_CANAL_6_GPIO_NUMERO, ADC2_CANAL_7_GPIO_NUMERO, ADC2_CANAL_8_GPIO_NUMERO, ADC2_CANAL_9_GPIO_NUMERO};

esp_err_t ADC_SECUENCIA_INIT(adc_modulo_t MODULO, adc_ancho_resolucion_t RESOLUCION,adc_atenuacion_t ATENUACION, adc_canal_t CANAL_INICIO, int NUM_CANALES)
{
	adc1_canal_t ADC1_CANAL_INICIO = 0;
	adc1_canal_t ADC1_CANAL_FINAL = 0;
	adc2_canal_t ADC2_CANAL_INICIO = 0;
	adc2_canal_t ADC2_CANAL_FINAL = 0;

	if(MODULO == ADC_MODULO_1)
	{
		ADC1_CANAL_INICIO = CANAL_INICIO;
		ADC1_CANAL_FINAL = (ADC1_CANAL_INICIO+NUM_CANALES)-1;

		if(ADC1_CANAL_FINAL >= ADC1_CANAL_MAX)
		{
			printf("ERROR_ADC_SECUENCIA_INIT_MODULO1, Canal invalido para el modulo seleccionado.\n");
			return ESP_FAIL;
		}

		for(adc1_canal_t i = ADC1_CANAL_INICIO; i <= ADC1_CANAL_FINAL; i++)
		{
			ADC1_CONFIGURACION_CANAL(RESOLUCION,ATENUACION, i);
		}
	}
	if(MODULO == ADC_MODULO_2)
	{
		ADC2_CANAL_INICIO = CANAL_INICIO;
		ADC2_CANAL_FINAL = (ADC2_CANAL_INICIO+NUM_CANALES)-1;

		if(ADC2_CANAL_FINAL >= ADC2_CANAL_MAX)
		{
			printf("ERROR_ADC_SECUENCIA_INIT_MODULO2, Canal invalido para el modulo seleccionado.\n");
			return ESP_FAIL;
		}

		for(adc2_canal_t i = ADC2_CANAL_INICIO; i <= ADC2_CANAL_FINAL; i++)
		{
			ADC2_CONFIGURACION_CANAL(RESOLUCION,ATENUACION, i);
		}
	}
	return ESP_OK;
}

esp_err_t ADC1_CONFIGURACION_CANAL(adc_ancho_resolucion_t RESOLUCION,adc_atenuacion_t ATENUACION, adc1_canal_t CANAL)
{
    ADC_CHECK(RESOLUCION < ADC_MAX_RESOLUCION, "WIDTH ERR: ESP32 support 9 ~ 12 bit width", ESP_ERR_INVALID_ARG);
    SARADC1_ENTER();
	adc1_hal_set_formato_salida(RESOLUCION);
    SARADC1_EXIT();

    ADC_CHANNEL_CHECK(ADC_MODULO_NUM_1, CANAL);
    ADC_CHECK(ATENUACION < ADC_ATENUACION_MAX, "ADC Atten Err", ESP_ERR_INVALID_ARG);
	adc1_inicializar_gpio(CANAL);
    SARADC1_ENTER();

#if SOC_DAC_SUPPORTED
    dac_hal_rtc_sync_by_adc(false);
#endif

	adc1_rtc_hal_inversion_salida(SOC_ADC1_INVERSION_DEFAULT_DATOS);
	adc1_ll_set_divisor_frecuencia(SOC_ADC_SAR_CLK_DIV_DEFAULT(ADC_MODULO_NUM_1));
#ifdef CONFIG_IDF_TARGET_ESP32
	adc_ll_deshabilitar_hall();
	adc_ll_deshabilitar_modulo_amp();
#endif
	adc1_hal_set_atenuacion(CANAL, ATENUACION);
    SARADC1_EXIT();
    return ESP_OK;
}

esp_err_t ADC2_CONFIGURACION_CANAL(adc_ancho_resolucion_t RESOLUCION,adc_atenuacion_t ATENUACION, adc2_canal_t CANAL)
{

		ADC_CHANNEL_CHECK(ADC_MODULO_NUM_2, CANAL);
		ADC_CHECK(ATENUACION < ADC_ATENUACION_MAX, "ADC Atten Err", ESP_ERR_INVALID_ARG);

		adc2_inicializar_gpio(CANAL);

		if ( SARADC2_TRY_ACQUIRE() == -1 ) {

			return ESP_ERR_TIMEOUT;
		}

		SARADC2_ENTER();

#if SOC_DAC_SUPPORTED
		dac_hal_rtc_sync_by_adc(false);

#endif
		adc2_rtc_hal_inversion_salida(SOC_ADC1_INVERSION_DEFAULT_DATOS);
		adc2_ll_set_divisor_frecuencia(SOC_ADC_SAR_CLK_DIV_DEFAULT(ADC_MODULO_NUM_2));
#ifdef CONFIG_IDF_TARGET_ESP32
		adc_ll_deshabilitar_hall();
		adc_ll_deshabilitar_modulo_amp();
#endif
		adc2_hal_set_atenuacion(CANAL, ATENUACION);
		SARADC2_EXIT();

		SARADC2_RELEASE();
  return ESP_OK;
}

esp_err_t ADC_INIT(adc_modulo_t MODULO, adc_ancho_resolucion_t RESOLUCION,adc_atenuacion_t ATENUACION, adc_canal_t CANAL)
{
	adc1_canal_t ADC1_CANAL = 0;
	adc2_canal_t ADC2_CANAL = 0;

	if(MODULO == ADC_MODULO_1)
	{
		ADC1_CANAL = CANAL;

		if(ADC1_CANAL >= ADC1_CANAL_MAX)
		{
			printf("ERROR, Canal invalido para el modulo seleccionado.\n");
			return ESP_FAIL;
		}
		ADC1_CONFIGURACION_CANAL(RESOLUCION,ATENUACION, ADC1_CANAL);
	}
	if(MODULO == ADC_MODULO_2)
	{
		ADC2_CANAL = CANAL;
		if(ADC2_CANAL >= ADC2_CANAL_MAX)
		{
			printf("ERROR, Canal invalido para el modulo seleccionado.\n");
			return ESP_FAIL;
		}
		ADC2_CONFIGURACION_CANAL(RESOLUCION,ATENUACION, ADC2_CANAL);
	}
	return ESP_OK;
}

esp_err_t ADC_SECUENCIA_READ(adc_modulo_t MODULO, adc_ancho_resolucion_t RESOLUCION,adc_canal_t CANAL_INICIO, int NUM_CANALES, uint32_t *lectura, int NUM_MUESTRAS)
{
	adc1_canal_t ADC1_CANAL_INICIO = 0;
	adc1_canal_t ADC1_CANAL_FINAL = 0;
	adc2_canal_t ADC2_CANAL_INICIO = 0;
	adc2_canal_t ADC2_CANAL_FINAL = 0;

	if(MODULO == ADC_MODULO_1)
		{
			ADC1_CANAL_INICIO = CANAL_INICIO;
			ADC1_CANAL_FINAL = (ADC1_CANAL_INICIO+NUM_CANALES)-1;

			if(ADC1_CANAL_FINAL >= ADC1_CANAL_MAX)
			{
				printf("ERROR_ADC_SECUENCIA_READ_MODULO1, Canal invalido para el modulo seleccionado.\n");
				return ESP_FAIL;
			}

			for(adc1_canal_t i=0; i < NUM_CANALES; i++)
			{
				*(lectura+i) = ADC1_READ(CANAL_INICIO+i, NUM_MUESTRAS);
			}
		}
		if(MODULO == ADC_MODULO_2)
		{
			ADC2_CANAL_INICIO = CANAL_INICIO;
			ADC2_CANAL_FINAL = (ADC2_CANAL_INICIO+NUM_CANALES)-1;

			if(ADC2_CANAL_FINAL >= ADC2_CANAL_MAX)
			{
				printf("ERROR_ADC_SECUENCIA_READ_MODULO2, Canal invalido para el modulo seleccionado.\n");
				return ESP_FAIL;
			}

			for(adc2_canal_t i=0; i < NUM_CANALES; i++)
			{
				*(lectura+i) = ADC2_READ(CANAL_INICIO+i, RESOLUCION, NUM_MUESTRAS);
			}
		}
		return ESP_OK;
}

uint32_t ADC1_TOMAR_LECTURA(adc1_canal_t ADC1_CANAL)
{
	uint32_t lectura;

	ADC_CHANNEL_CHECK(ADC_MODULO_NUM_1, ADC1_CANAL);

	SARADC1_ACQUIRE();
	adc_adquirir_energia();
	SARADC1_ENTER();

	adc1_hal_set_controlador(ADC_LL_CONTROL_RTC);
	SARADC1_EXIT();

	SARADC1_ENTER();
	adc_ll_deshabilitar_hall();
	adc_ll_deshabilitar_modulo_amp();
	adc1_ll_set_controlador(ADC_LL_CONTROL_RTC);

	adc1_ll_habilitar_canal_rtc(ADC1_CANAL);
	adc1_ll_rtc_iniciar_conversion(ADC1_CANAL);
	while (adc1_ll_rtc_conversion_realizada() != true);
	lectura = adc1_ll_rtc_obtener_valor_convertido();

	SARADC1_EXIT();
	ADC_CHECK((uint32_t *)adc1_dma_lock != NULL, "adc1 lock release called before acquire", ESP_ERR_INVALID_STATE );

	adc_liberar_energia();
	SARADC1_RELEASE();

	return lectura;
}

uint32_t ADC1_READ(adc_canal_t CANAL, int NUM_MUESTRAS)
{
	adc1_canal_t ADC1_CANAL = 0;
	uint32_t valor_adc=0;

	ADC1_CANAL = CANAL;
	printf("%d\n",ADC1_CANAL);
	if(ADC1_CANAL >= ADC1_CANAL_MAX)
	{

		printf("ERROR_ADC1_READ, Canal invalido para el modulo seleccionado.\n");
		exit(1);
	}

	for(int i=0; i<NUM_MUESTRAS; i++)
	{
		valor_adc += ADC1_TOMAR_LECTURA(ADC1_CANAL);
	}
	valor_adc /= NUM_MUESTRAS;
	return valor_adc;

}

uint32_t ADC2_TOMAR_LECTURA(adc2_canal_t ADC2_CANAL, adc_ancho_resolucion_t RESOLUCION)
{
	uint32_t lectura;

	ADC_CHECK(&lectura != NULL, "ADC out value err", ESP_ERR_INVALID_ARG);
	ADC_CHECK(ADC2_CANAL < ADC2_CANAL_MAX, "ADC Channel Err", ESP_ERR_INVALID_ARG);
	ADC_CHECK(RESOLUCION < ADC_MAX_RESOLUCION, "WIDTH ERR: ESP32 support 9 ~ 12 bit width", ESP_ERR_INVALID_ARG);

	if ( SARADC2_TRY_ACQUIRE() == -1 ) {

		return ESP_ERR_TIMEOUT;
	}
	adc_adquirir_energia();

	SARADC2_ENTER();

#ifdef CONFIG_ADC_DISABLE_DAC
adc2_dac_disable(ADC2_CANAL);
#endif

	adc2_hal_set_formato_salida(RESOLUCION);
	adc2_ll_set_controlador(ADC_LL_CONTROL_RTC);

	adc2_ll_habilitar_canal_rtc(ADC2_CANAL);
	adc2_ll_rtc_iniciar_conversion(ADC2_CANAL);
	while (adc2_ll_rtc_conversion_realizada() != true);
	lectura = adc2_ll_rtc_obtener_valor_convertido();

	SARADC2_EXIT();
	adc_liberar_energia();
	SARADC2_RELEASE();

	return lectura;

}
uint32_t ADC2_READ(adc_canal_t CANAL, adc_ancho_resolucion_t RESOLUCION, int NUM_MUESTRAS)
{
	adc2_canal_t ADC2_CANAL = 0;
	uint32_t valor_adc = 0;

	ADC2_CANAL = CANAL;
	printf("%d\n",ADC2_CANAL);
	if(ADC2_CANAL >= ADC2_CANAL_MAX)
	{
		printf("ERROR_ADC2_READ, Canal invalido para el modulo seleccionado.\n");
		exit(1);
	}

	for(int i = 0; i<NUM_MUESTRAS; i++)
	{
		valor_adc += ADC2_TOMAR_LECTURA(ADC2_CANAL, RESOLUCION);
	}
	valor_adc /= NUM_MUESTRAS;

	return valor_adc;
}

esp_err_t adc2_inicializar_gpio(adc2_canal_t ADC_CANAL)
{
	pin_gpio_num_t numero_gpio = 0;

	numero_gpio = OBTENER_ADC2_GPIO_NUM_PIN(ADC_CANAL);
	ADC_CHECK_RET(rtc_gpio_init(numero_gpio));
	ADC_CHECK_RET(rtc_gpio_set_direction(numero_gpio, RTC_GPIO_MODE_DISABLED));
	ADC_CHECK_RET(rtc_gpio_pulldown_dis(numero_gpio));
	ADC_CHECK_RET(rtc_gpio_pullup_dis(numero_gpio));

	return ESP_OK;
}

esp_err_t adc1_inicializar_gpio(adc1_canal_t ADC_CANAL)
{
	pin_gpio_num_t numero_gpio = 0;

	numero_gpio = OBTENER_ADC1_GPIO_NUM_PIN(ADC_CANAL);
	ADC_CHECK_RET(rtc_gpio_init(numero_gpio));
	ADC_CHECK_RET(rtc_gpio_set_direction(numero_gpio, RTC_GPIO_MODE_DISABLED));
	ADC_CHECK_RET(rtc_gpio_pulldown_dis(numero_gpio));
	ADC_CHECK_RET(rtc_gpio_pullup_dis(numero_gpio));

	return ESP_OK;
}

void adc_liberar_energia(void)
{
    ADC_POWER_ENTER();
    s_adc_power_on_cnt--;

    if (s_adc_power_on_cnt < 0) {
        ADC_POWER_EXIT();
        ESP_LOGE(ADC_TAG, "%s called, but s_adc_power_on_cnt == 0", __func__);
        abort();
    } else if (s_adc_power_on_cnt == 0) {
    	adc_hal_set_gestion_energia(ADC_ENERGIA_SW_OFF);
    }
    ADC_POWER_EXIT();
}

void adc_adquirir_energia(void)
{
    ADC_POWER_ENTER();
    s_adc_power_on_cnt++;
    if (s_adc_power_on_cnt == 1) {
    	adc_hal_set_gestion_energia(ADC_ENERGIA_SW_ON);
    }
    ADC_POWER_EXIT();
}
