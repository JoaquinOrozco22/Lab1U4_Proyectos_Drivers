/*
 *  04/01/2023
 * Archivo: adc_drivers_hal.h
 *
 * Desarrollados por:
 * - Sebastian De la Cruz Delgado 18060707
 * - Joaquin Bernardo Orozco Lopez 18060729
 *
 * Todos los derechos de autor reservados.
 * El mal uso de estos Drivers no es responsabilidad de los desarrolladores.
 */


#include "adc_drivers_ll.h"

#define adc1_hal_set_formato_salida(BITS_RESOLUCION) adc1_ll_set_formato_salida(BITS_RESOLUCION)

#define adc2_hal_set_formato_salida(BITS_RESOLUCION) adc2_ll_set_formato_salida(BITS_RESOLUCION)

#define adc1_rtc_hal_inversion_salida(habilitar_inversion) adc1_rtc_ll_inversion_salida(habilitar_inversion)

#define adc2_rtc_hal_inversion_salida(habilitar_inversion) adc2_rtc_ll_inversion_salida(habilitar_inversion)

#define adc1_hal_set_atenuacion(CANAL, ATENUACION) adc1_ll_set_atenuacion(CANAL, ATENUACION)

#define adc2_hal_set_atenuacion(CANAL, ATENUACION) adc2_ll_set_atenuacion(CANAL, ATENUACION)

#define adc_hal_set_gestion_energia(gestion) adc_ll_set_gestion_energia(gestion)

#define adc1_hal_set_controlador(CONTROLADOR) adc1_ll_set_controlador(CONTROLADOR)

#define adc2_hal_set_controlador(CONTROLADOR) adc2_ll_set_controlador(CONTROLADOR)
