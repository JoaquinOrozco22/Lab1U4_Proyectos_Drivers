#include "adc_drivers.h"

#include <stdio.h>
#include "include/adc_drivers.h"
#include <esp_types.h>
#include <stdlib.h>
#include <ctype.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "soc/rtc.h"
#include "driver/rtc_io.h"
#include "sys/lock.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "hal/adc_types.h"
#include "hal/adc_hal.h"
#include "hal/adc_hal_conf.h"

#if SOC_DAC_SUPPORTED
#include "driver/dac.h"
#include "hal/dac_hal.h"
#endif

#include "adc_drivers_hal.h"

#define ADC_CHECK_RET(fun_ret) ({                  \
    if (fun_ret != ESP_OK) {                                \
        ESP_LOGE(ADC_TAG,"%s:%d\n",__FUNCTION__,__LINE__);  \
        return ESP_FAIL;                                    \
    }                                                       \
})

#define ADC_CHECK(a, str, ret_val) ({                                               \
    if (!(a)) {                                                                     \
        ESP_LOGE(ADC_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str);                \
        return (ret_val);                                                           \
    }                                                                               \
})

#define ADC_CHANNEL_CHECK(periph, channel) ADC_CHECK(channel < SOC_ADC_CHANNEL_NUM(periph), "ADC"#periph" channel error", ESP_ERR_INVALID_ARG)

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.

#define RTC_ENTER_CRITICAL()    portENTER_CRITICAL(&rtc_spinlock)
#define RTC_EXIT_CRITICAL()     portEXIT_CRITICAL(&rtc_spinlock)

#define ADC_POWER_ENTER()       RTC_ENTER_CRITICAL()
#define ADC_POWER_EXIT()        RTC_EXIT_CRITICAL()

#define SARADC1_ENTER()         RTC_ENTER_CRITICAL()
#define SARADC1_EXIT()          RTC_EXIT_CRITICAL()

#define SARADC2_ENTER()         RTC_ENTER_CRITICAL()
#define SARADC2_EXIT()          RTC_EXIT_CRITICAL()

static _lock_t adc2_wifi_lock;
#define SARADC2_ACQUIRE()       _lock_acquire( &adc2_wifi_lock )
#define SARADC2_RELEASE()       _lock_release( &adc2_wifi_lock )
#define SARADC2_TRY_ACQUIRE()   _lock_try_acquire( &adc2_wifi_lock )
#define SARADC2_LOCK_CHECK()    ((uint32_t *)adc2_wifi_lock != NULL)

static const char *ADC_TAG = "ADC";

//TODO: IDF-3610
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
//prevent ADC1 being used by I2S dma and other tasks at the same time.
static _lock_t adc1_dma_lock;
#define SARADC1_ACQUIRE() _lock_acquire( &adc1_dma_lock )
#define SARADC1_RELEASE() _lock_release( &adc1_dma_lock )
#endif


static int s_adc_power_on_cnt;

const int adc1_maping_canales_ent_sal[MAX_CANTIDAD_CANALES_ADC1] = {ADC1_CANAL_0_GPIO_NUMERO, ADC1_CANAL_1_GPIO_NUMERO, ADC1_CANAL_2_GPIO_NUMERO, ADC1_CANAL_3_GPIO_NUMERO,
																	ADC1_CANAL_4_GPIO_NUMERO, ADC1_CANAL_5_GPIO_NUMERO, ADC1_CANAL_6_GPIO_NUMERO, ADC1_CANAL_7_GPIO_NUMERO};

const int adc2_maping_canales_ent_sal[MAX_CANTIDAD_CANALES_ADC2] = {ADC2_CANAL_0_GPIO_NUMERO, ADC2_CANAL_1_GPIO_NUMERO, ADC2_CANAL_2_GPIO_NUMERO, ADC2_CANAL_3_GPIO_NUMERO,
																	ADC2_CANAL_4_GPIO_NUMERO, ADC2_CANAL_5_GPIO_NUMERO, ADC2_CANAL_6_GPIO_NUMERO, ADC2_CANAL_7_GPIO_NUMERO,
																	ADC2_CANAL_8_GPIO_NUMERO, ADC2_CANAL_9_GPIO_NUMERO};

static inline void adc2_dac_disable( adc2_canal_t channel)
{
#if SOC_DAC_SUPPORTED
#ifdef CONFIG_IDF_TARGET_ESP32
    if ( channel == ADC2_CANAL_8 ) { // the same as DAC channel 1
        dac_output_disable(DAC_CHANNEL_1);
    } else if ( channel == ADC2_CANAL_9 ) {
        dac_output_disable(DAC_CHANNEL_2);
    }
#else
    if ( channel == ADC2_CHANNEL_6 ) { // the same as DAC channel 1
        dac_output_disable(DAC_CHANNEL_1);
    } else if ( channel == ADC2_CHANNEL_7 ) {
        dac_output_disable(DAC_CHANNEL_2);
    }
#endif
#endif // SOC_DAC_SUPPORTED
}

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
