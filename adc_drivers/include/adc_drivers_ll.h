
#pragma once
#include "soc/sens_struct.h"
#include <stdbool.h>
#include "hal/misc.h"
#include "soc/adc_periph.h"
#include "soc/sens_reg.h"
#include "soc/rtc_io_struct.h"
#include "soc/syscon_struct.h"
#include "soc/rtc_cntl_struct.h"


static inline void adc1_ll_set_formato_salida(adc_ancho_resolucion_t BITS_RESOLUCION)
{
	SENS.sar_start_force.sar1_bit_width = BITS_RESOLUCION;
	SENS.sar_read_ctrl.sar1_sample_bit = BITS_RESOLUCION;
}

static inline void adc2_ll_set_formato_salida(adc_ancho_resolucion_t BITS_RESOLUCION)
{
	SENS.sar_start_force.sar2_bit_width = BITS_RESOLUCION;
	SENS.sar_read_ctrl2.sar2_sample_bit = BITS_RESOLUCION;
}

static inline void adc1_rtc_ll_inversion_salida(bool habilitar_inversion)
{
	SENS.sar_read_ctrl.sar1_data_inv = habilitar_inversion;
}

static inline void adc2_rtc_ll_inversion_salida(bool habilitar_inversion)
{
	SENS.sar_read_ctrl2.sar2_data_inv = habilitar_inversion;
}

static inline void adc1_ll_set_divisor_frecuencia(uint32_t division)
{
	HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_read_ctrl, sar1_clk_div, division);
}

static inline void adc2_ll_set_divisor_frecuencia(uint32_t division)
{
	HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_read_ctrl2, sar2_clk_div, division);
}

static inline void adc_ll_deshabilitar_hall(void)
{
    RTCIO.hall_sens.xpd_hall = 0;
}

static inline void adc_ll_deshabilitar_modulo_amp(void)
{
    //channel is set in the  convert function
    SENS.sar_meas_wait2.force_xpd_amp = SENS_FORCE_XPD_AMP_PD;
    //disable FSM, it's only used by the LNA.
    SENS.sar_meas_ctrl.amp_rst_fb_fsm = 0;
    SENS.sar_meas_ctrl.amp_short_ref_fsm = 0;
    SENS.sar_meas_ctrl.amp_short_ref_gnd_fsm = 0;
    HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_meas_wait1, sar_amp_wait1, 1);
    HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_meas_wait1, sar_amp_wait2, 1);
    HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_meas_wait2, sar_amp_wait3, 1);
}

static inline void adc1_ll_set_atenuacion(adc1_canal_t CANAL, adc_atenuacion_t ATENUACION)
{
	SENS.sar_atten1 = ( SENS.sar_atten1 & ~(0x3 << (CANAL * 2)) ) | ((ATENUACION & 0x3) << (CANAL * 2));
}

static inline void adc2_ll_set_atenuacion(adc1_canal_t CANAL, adc_atenuacion_t ATENUACION)
{
	SENS.sar_atten2 = ( SENS.sar_atten2 & ~(0x3 << (CANAL * 2)) ) | ((ATENUACION & 0x3) << (CANAL * 2));
}

static void adc_ll_set_gestion_energia(adc_ll_energia_t gestion)
{
    if (gestion == ADC_ENERGIA_SW_ON) {
        SENS.sar_meas_wait2.force_xpd_sar = SENS_FORCE_XPD_SAR_PU;
    } else if (gestion == ADC_ENERGIA_POR_FSM) {
        SENS.sar_meas_wait2.force_xpd_sar = SENS_FORCE_XPD_SAR_FSM;
    } else if (gestion == ADC_ENERGIA_SW_OFF) {
        SENS.sar_meas_wait2.force_xpd_sar = SENS_FORCE_XPD_SAR_PD;
    }
}

static inline void adc1_ll_set_controlador(adc_ll_controlador_t CONTROLADOR)
{
	switch ( CONTROLADOR ) {
	case ADC_LL_CONTROL_RTC :
		SENS.sar_read_ctrl.sar1_dig_force       = 0;    // 1: Select digital control;       0: Select RTC control.
		SENS.sar_meas_start1.meas1_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
		SENS.sar_meas_start1.sar1_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
		SENS.sar_touch_ctrl1.xpd_hall_force     = 1;    // 1: SW control HALL power;        0: ULP FSM control HALL power.
		SENS.sar_touch_ctrl1.hall_phase_force   = 1;    // 1: SW control HALL phase;        0: ULP FSM control HALL phase.
		break;
	case ADC_LL_CONTROL_ULP:
		SENS.sar_read_ctrl.sar1_dig_force       = 0;    // 1: Select digital control;       0: Select RTC control.
		SENS.sar_meas_start1.meas1_start_force  = 0;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
		SENS.sar_meas_start1.sar1_en_pad_force  = 0;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
		SENS.sar_touch_ctrl1.xpd_hall_force     = 0;    // 1: SW control HALL power;        0: ULP FSM control HALL power.
		SENS.sar_touch_ctrl1.hall_phase_force   = 0;    // 1: SW control HALL phase;        0: ULP FSM control HALL phase.
		break;
	case ADC_LL_CONTROL_DIG:
		SENS.sar_read_ctrl.sar1_dig_force       = 1;    // 1: Select digital control;       0: Select RTC control.
		SENS.sar_meas_start1.meas1_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
		SENS.sar_meas_start1.sar1_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
		SENS.sar_touch_ctrl1.xpd_hall_force     = 1;    // 1: SW control HALL power;        0: ULP FSM control HALL power.
		SENS.sar_touch_ctrl1.hall_phase_force   = 1;    // 1: SW control HALL phase;        0: ULP FSM control HALL phase.
		break;
	default:
		break;
	}
}

static inline void adc2_ll_set_controlador(adc_ll_controlador_t CONTROLADOR)
{
	switch ( CONTROLADOR ) {
	case ADC_LL_CONTROL_RTC :
		SENS.sar_meas_start2.meas2_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
		SENS.sar_meas_start2.sar2_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
		SENS.sar_read_ctrl2.sar2_dig_force      = 0;    // 1: Select digital control;       0: Select RTC control.
		SENS.sar_read_ctrl2.sar2_pwdet_force    = 0;    // 1: Select power detect control;  0: Select RTC control.
		SYSCON.saradc_ctrl.sar2_mux             = 1;    // 1: Select digital control;       0: Select power detect control.
		break;
	case ADC_LL_CONTROL_ULP :
		SENS.sar_meas_start2.meas2_start_force  = 0;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
		SENS.sar_meas_start2.sar2_en_pad_force  = 0;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
		SENS.sar_read_ctrl2.sar2_dig_force      = 0;    // 1: Select digital control;       0: Select RTC control.
		SENS.sar_read_ctrl2.sar2_pwdet_force    = 0;    // 1: Select power detect control;  0: Select RTC control.
		SYSCON.saradc_ctrl.sar2_mux             = 1;    // 1: Select digital control;       0: Select power detect control.
		break;
	case ADC_LL_CONTROL_DIG:
		SENS.sar_meas_start2.meas2_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
		SENS.sar_meas_start2.sar2_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
		SENS.sar_read_ctrl2.sar2_dig_force      = 1;    // 1: Select digital control;       0: Select RTC control.
		SENS.sar_read_ctrl2.sar2_pwdet_force    = 0;    // 1: Select power detect control;  0: Select RTC control.
		SYSCON.saradc_ctrl.sar2_mux             = 1;    // 1: Select digital control;       0: Select power detect control.
		break;
	case ADC_LL_CONTROL_PWDET:   // currently only used by Wi-Fi
		SENS.sar_meas_start2.meas2_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
		SENS.sar_meas_start2.sar2_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
		SENS.sar_read_ctrl2.sar2_dig_force      = 0;    // 1: Select digital control;       0: Select RTC control.
		SENS.sar_read_ctrl2.sar2_pwdet_force    = 1;    // 1: Select power detect control;  0: Select RTC control.
		SYSCON.saradc_ctrl.sar2_mux             = 0;    // 1: Select digital control;       0: Select power detect control.
		break;
	default:
		break;
	}
}

static inline void adc1_ll_rtc_iniciar_conversion(int CANAL)
{
	while (HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_slave_addr1, meas_status) != 0) {}
	SENS.sar_meas_start1.meas1_start_sar = 0;
	SENS.sar_meas_start1.meas1_start_sar = 1;
}

static inline void adc2_ll_rtc_iniciar_conversion(int CANAL)
{
	SENS.sar_meas_start2.meas2_start_sar = 0; //start force 0
	SENS.sar_meas_start2.meas2_start_sar = 1; //start force 1
}

static inline void adc1_ll_habilitar_canal_rtc(int CANAL)
{
	SENS.sar_meas_start1.sar1_en_pad = (1 << CANAL); //only one channel is selected.
}

static inline void adc2_ll_habilitar_canal_rtc(int CANAL)
{
	SENS.sar_meas_start2.sar2_en_pad = (1 << CANAL); //only one channel is selected.
}

static inline bool adc1_ll_rtc_conversion_realizada()
{
    bool ret = true;

	ret = (bool)SENS.sar_meas_start1.meas1_done_sar;

    return ret;
}

static inline bool adc2_ll_rtc_conversion_realizada()
{
    bool ret = true;

	ret = (bool)SENS.sar_meas_start2.meas2_done_sar;

    return ret;
}

static inline int adc1_ll_rtc_obtener_valor_convertido()
{
    int ret_val = 0;

	ret_val = HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_meas_start1, meas1_data_sar);

    return ret_val;
}

static inline int adc2_ll_rtc_obtener_valor_convertido()
{
    int ret_val = 0;

	ret_val = HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_meas_start2, meas2_data_sar);

    return ret_val;
}
