
#pragma once
#include "esp_err.h"

typedef enum {
    ADC_MODULO_NUM_1 = 0,          /*!< SAR ADC 1 */
    ADC_MODULO_NUM_2 = 1,          /*!< SAR ADC 2 */
    ADC_MODULO_NUM_MAX,
} adc_ll_modulo_num_t;

typedef enum {
    ADC_ENERGIA_POR_FSM,   /*!< ADC XPD controlled by FSM. Used for polling mode */
    ADC_ENERGIA_SW_ON,    /*!< ADC XPD controlled by SW. power on. Used for DMA mode */
    ADC_ENERGIA_SW_OFF,   /*!< ADC XPD controlled by SW. power off. */
    ADC_ENERGIA_MAX,      /*!< For parameter check. */
} adc_ll_energia_t;

typedef enum {
    ADC_LL_CONTROL_RTC   = 0,    ///< For ADC1 and ADC2. Select RTC controller.
    ADC_LL_CONTROL_ULP   = 1,    ///< For ADC1 and ADC2. Select ULP controller.
    ADC_LL_CONTROL_DIG   = 2,    ///< For ADC1 and ADC2. Select DIG controller.
    ADC_LL_CONTROL_PWDET = 3,    ///< For ADC2. Select PWDET controller.
} adc_ll_controlador_t;

typedef enum {
    ADC_MODULO_1 = 1,          /*!< SAR ADC 1. */
    ADC_MODULO_2 = 2,          /*!< SAR ADC 2. */
	ADC_MAX_MODULO,
} adc_modulo_t;

typedef enum {
    ADC_9_BITS_RESOLUCION  = 0, /*!< ADC capture width is 9Bit. */
	ADC_10_BITS_RESOLUCION = 1, /*!< ADC capture width is 10Bit. */
	ADC_11_BITS_RESOLUCION = 2, /*!< ADC capture width is 11Bit. */
	ADC_12_BITS_RESOLUCION = 3, /*!< ADC capture width is 12Bit. */
	ADC_MAX_RESOLUCION,
} adc_ancho_resolucion_t;

typedef enum {
    ADC_ATENUACION_0_Db   = 0,  /*!<No input attenumation, ADC can measure up to approx. 800 mV. */
    ADC_ATENUACION_2_5_Db = 1,  /*!<The input voltage of ADC will be attenuated extending the range of measurement by about 2.5 dB (1.33 x) */
    ADC_ATENUACION_6_Db   = 2,  /*!<The input voltage of ADC will be attenuated extending the range of measurement by about 6 dB (2 x) */
    ADC_ATENUACION_11_Db  = 3,  /*!<The input voltage of ADC will be attenuated extending the range of measurement by about 11 dB (3.55 x) */
    ADC_ATENUACION_MAX,
} adc_atenuacion_t;

typedef enum {
    ADC_CANAL_0 = 0, 	   /*!< ADC channel */
    ADC_CANAL_1 = 1,     /*!< ADC channel */
    ADC_CANAL_2 = 2,     /*!< ADC channel */
    ADC_CANAL_3 = 3,     /*!< ADC channel */
    ADC_CANAL_4 = 4,     /*!< ADC channel */
    ADC_CANAL_5 = 5,     /*!< ADC channel */
    ADC_CANAL_6 = 6,     /*!< ADC channel */
    ADC_CANAL_7 = 7,     /*!< ADC channel */
    ADC_CANAL_8 = 8,     /*!< ADC channel */
    ADC_CANAL_9 = 9,     /*!< ADC channel */
    ADC_CANAL_MAX,
} adc_canal_t;

typedef enum {
    ADC1_CANAL_0 = 0,  	  /*!< ADC1 channel 0 is GPIO36 */
    ADC1_CANAL_1 = 1,     /*!< ADC1 channel 1 is GPIO37 */
    ADC1_CANAL_2 = 2,     /*!< ADC1 channel 2 is GPIO38 */
    ADC1_CANAL_3 = 3,     /*!< ADC1 channel 3 is GPIO39 */
    ADC1_CANAL_4 = 4,     /*!< ADC1 channel 4 is GPIO32 */
    ADC1_CANAL_5 = 5,     /*!< ADC1 channel 5 is GPIO33 */
    ADC1_CANAL_6 = 6,     /*!< ADC1 channel 6 is GPIO34 */
    ADC1_CANAL_7 = 7,     /*!< ADC1 channel 7 is GPIO35 */
    ADC1_CANAL_MAX,
} adc1_canal_t;

typedef enum {
    ADC2_CANAL_0 = 0,	  /*!< ADC2 channel 0 is GPIO4 */
    ADC2_CANAL_1 = 1,     /*!< ADC2 channel 1 is GPIO0 */
    ADC2_CANAL_2 = 2,     /*!< ADC2 channel 2 is GPIO2 */
    ADC2_CANAL_3 = 3,     /*!< ADC2 channel 3 is GPIO15 */
    ADC2_CANAL_4 = 4,     /*!< ADC2 channel 4 is GPIO13 */
    ADC2_CANAL_5 = 5,     /*!< ADC2 channel 5 is GPIO12 */
    ADC2_CANAL_6 = 6,     /*!< ADC2 channel 6 is GPIO14 */
    ADC2_CANAL_7 = 7,     /*!< ADC2 channel 7 is GPIO27 */
    ADC2_CANAL_8 = 8,     /*!< ADC2 channel 6 is GPIO25 */
    ADC2_CANAL_9 = 9,     /*!< ADC2 channel 7 is GPIO26 */
    ADC2_CANAL_MAX,
} adc2_canal_t;

typedef enum {
    GPIO_PIN_NUM_SIN_CONECCION = -1,    /*!< Use to signal not connected to S/W */
    GPIO_PIN_NUM_0 = 0,     /*!< GPIO0, input and output */
    GPIO_PIN_NUM_1 = 1,     /*!< GPIO1, input and output */
    GPIO_PIN_NUM_2 = 2,     /*!< GPIO2, input and output */
    GPIO_PIN_NUM_3 = 3,     /*!< GPIO3, input and output */
    GPIO_PIN_NUM_4 = 4,     /*!< GPIO4, input and output */
    GPIO_PIN_NUM_5 = 5,     /*!< GPIO5, input and output */
    GPIO_PIN_NUM_6 = 6,     /*!< GPIO6, input and output */
    GPIO_PIN_NUM_7 = 7,     /*!< GPIO7, input and output */
    GPIO_PIN_NUM_8 = 8,     /*!< GPIO8, input and output */
    GPIO_PIN_NUM_9 = 9,     /*!< GPIO9, input and output */
    GPIO_PIN_NUM_10 = 10,   /*!< GPIO10, input and output */
    GPIO_PIN_NUM_11 = 11,   /*!< GPIO11, input and output */
    GPIO_PIN_NUM_12 = 12,   /*!< GPIO12, input and output */
    GPIO_PIN_NUM_13 = 13,   /*!< GPIO13, input and output */
    GPIO_PIN_NUM_14 = 14,   /*!< GPIO14, input and output */
    GPIO_PIN_NUM_15 = 15,   /*!< GPIO15, input and output */
    GPIO_PIN_NUM_16 = 16,   /*!< GPIO16, input and output */
    GPIO_PIN_NUM_17 = 17,   /*!< GPIO17, input and output */
    GPIO_PIN_NUM_18 = 18,   /*!< GPIO18, input and output */
    GPIO_PIN_NUM_19 = 19,   /*!< GPIO19, input and output */
    GPIO_PIN_NUM_20 = 20,   /*!< GPIO20, input and output */
    GPIO_PIN_NUM_21 = 21,   /*!< GPIO21, input and output */
    GPIO_PIN_NUM_22 = 22,   /*!< GPIO22, input and output */
    GPIO_PIN_NUM_23 = 23,   /*!< GPIO23, input and output */
    GPIO_PIN_NUM_25 = 25,   /*!< GPIO25, input and output */
    GPIO_PIN_NUM_26 = 26,   /*!< GPIO26, input and output */
    GPIO_PIN_NUM_27 = 27,   /*!< GPIO27, input and output */
    GPIO_PIN_NUM_28 = 28,   /*!< GPIO28, input and output */
    GPIO_PIN_NUM_29 = 29,   /*!< GPIO29, input and output */
    GPIO_PIN_NUM_30 = 30,   /*!< GPIO30, input and output */
    GPIO_PIN_NUM_31 = 31,   /*!< GPIO31, input and output */
    GPIO_PIN_NUM_32 = 32,   /*!< GPIO32, input and output */
    GPIO_PIN_NUM_33 = 33,   /*!< GPIO33, input and output */
    GPIO_PIN_NUM_34 = 34,   /*!< GPIO34, input mode only */
    GPIO_PIN_NUM_35 = 35,   /*!< GPIO35, input mode only */
    GPIO_PIN_NUM_36 = 36,   /*!< GPIO36, input mode only */
    GPIO_PIN_NUM_37 = 37,   /*!< GPIO37, input mode only */
    GPIO_PIN_NUM_38 = 38,   /*!< GPIO38, input mode only */
    GPIO_PIN_NUM_39 = 39,   /*!< GPIO39, input mode only */
    GPIO_PIN_NUM_MAX,
} pin_gpio_num_t;

#define ADC1_GPIO36_CANAL     ADC1_CANAL_0
#define ADC1_CANAL_0_GPIO_NUMERO 36

#define ADC1_GPIO37_CANAL     ADC1_CANAL_1
#define ADC1_CANAL_1_GPIO_NUMERO 37

#define ADC1_GPIO38_CANAL     ADC1_CANAL_2
#define ADC1_CANAL_2_GPIO_NUMERO 38

#define ADC1_GPIO39_CANAL     ADC1_CANAL_3
#define ADC1_CANAL_3_GPIO_NUMERO 39

#define ADC1_GPIO32_CANAL     ADC1_CANAL_4
#define ADC1_CANAL_4_GPIO_NUMERO 32

#define ADC1_GPIO33_CANAL     ADC1_CANAL_5
#define ADC1_CANAL_5_GPIO_NUMERO 33

#define ADC1_GPIO34_CANAL     ADC1_CANAL_6
#define ADC1_CANAL_6_GPIO_NUMERO 34

#define ADC1_GPIO35_CANAL     ADC1_CANAL_7
#define ADC1_CANAL_7_GPIO_NUMERO 35

#define ADC2_GPIO4_CANAL      ADC2_CANAL_0
#define ADC2_CANAL_0_GPIO_NUMERO 4

#define ADC2_GPIO0_CANAL      ADC2_CANAL_1
#define ADC2_CANAL_1_GPIO_NUMERO 0

#define ADC2_GPIO2_CANAL      ADC2_CANAL_2
#define ADC2_CANAL_2_GPIO_NUMERO 2

#define ADC2_GPIO15_CANAL     ADC2_CANAL_3
#define ADC2_CANAL_3_GPIO_NUMERO 15

#define ADC2_GPIO13_CANAL     ADC2_CANAL_4
#define ADC2_CANAL_4_GPIO_NUMERO 13

#define ADC2_GPIO12_CANAL     ADC2_CANAL_5
#define ADC2_CANAL_5_GPIO_NUMERO 12

#define ADC2_GPIO14_CANAL     ADC2_CANAL_6
#define ADC2_CANAL_6_GPIO_NUMERO 14

#define ADC2_GPIO27_CANAL     ADC2_CANAL_7
#define ADC2_CANAL_7_GPIO_NUMERO 27

#define ADC2_GPIO25_CANAL     ADC2_CANAL_8
#define ADC2_CANAL_8_GPIO_NUMERO 25

#define ADC2_GPIO26_CANAL     ADC2_CANAL_9
#define ADC2_CANAL_9_GPIO_NUMERO 26


#define MAX_CANTIDAD_CANALES_ADC1 (8)
#define MAX_CANTIDAD_CANALES_ADC2 (10)

#define SOC_ADC1_INVERSION_DEFAULT_DATOS (1)
#define SOC_ADC2_INVERSION_DEFAULT_DATOS (1)

#define OBTENER_ADC1_GPIO_NUM_PIN(canal) (adc1_maping_canales_ent_sal[canal])
#define OBTENER_ADC2_GPIO_NUM_PIN(canal) (adc2_maping_canales_ent_sal[canal])


extern esp_err_t ADC_SECUENCIA_INIT(adc_modulo_t MODULO, adc_ancho_resolucion_t RESOLUCION,adc_atenuacion_t ATENUACION, adc_canal_t CANAL_INICIO, int NUM_CANALES);

extern esp_err_t ADC1_CONFIGURACION_CANAL(adc_ancho_resolucion_t RESOLUCION,adc_atenuacion_t ATENUACION, adc1_canal_t CANAL);

extern esp_err_t ADC2_CONFIGURACION_CANAL(adc_ancho_resolucion_t RESOLUCION,adc_atenuacion_t ATENUACION, adc2_canal_t CANAL);

extern esp_err_t ADC_INIT(adc_modulo_t MODULO, adc_ancho_resolucion_t RESOLUCION,adc_atenuacion_t ATENUACION, adc_canal_t CANAL);

extern esp_err_t ADC_SECUENCIA_READ(adc_modulo_t MODULO, adc_ancho_resolucion_t RESOLUCION,adc_canal_t CANAL_INICIO, int NUM_CANALES, uint32_t *lectura, int NUM_MUESTRAS);

extern uint32_t ADC1_TOMAR_LECTURA(adc1_canal_t ADC1_CANAL);

extern uint32_t ADC1_READ(adc_canal_t CANAL, int NUM_MUESTRAS);

extern uint32_t ADC2_TOMAR_LECTURA(adc2_canal_t ADC2_CANAL, adc_ancho_resolucion_t RESOLUCION);

extern uint32_t ADC2_READ(adc_canal_t CANAL, adc_ancho_resolucion_t RESOLUCION, int NUM_MUESTRAS);

extern esp_err_t adc1_inicializar_gpio(adc1_canal_t ADC_CANAL);

extern esp_err_t adc2_inicializar_gpio(adc2_canal_t ADC_CANAL);

extern void adc_liberar_energia(void);

extern void adc_adquirir_energia(void);

