#include "bsp_adc.h"
#include "gd32f30x.h"
#include <string.h>

void adc_init(void)
{
		adc_rcu_config();
    /* TIMER configuration */
    timer_config();
    /* DMA configuration */
    dma_config();
    /* ADC configuration */
    adc_config();
}

void adc_rcu_config(void)
{
		rcu_periph_clock_enable(RCU_GPIOA);
		/* enable DMA clock */
		rcu_periph_clock_enable(RCU_DMA0);
		/* enable TIMER0 clock */
		rcu_periph_clock_enable(RCU_TIMER0);
		/* enable ADC0 clock */
		rcu_periph_clock_enable(RCU_ADC0);
//	�ƺ�����ƵҲ�� Ӧ�ú�ϵͳʱ��һ�� ת���ٶȻ��һЩ
//		/* config ADC clock */
//		rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);
}
//	ʱ��120M
/*!
    \brief      configure the timer peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    /* TIMER0 configuration */
    timer_initpara.prescaler         = 119;				//	120000000/120 1us
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 99;				//	100us����һ��
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    /* CH0 configuration in PWM mode0 */
    timer_channel_output_struct_para_init(&timer_ocintpara);
    timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 20);		//	ռ�ձ�20%
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    /* TIMER0 primary output enable */
    timer_primary_output_config(TIMER0, ENABLE);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);

    /* enable TIMER0 */
    timer_enable(TIMER0);
}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    /* ADC_DMA_channel configuration */
    dma_parameter_struct dma_data_parameter;
    
    /* ADC_DMA_channel deinit */
    dma_deinit(DMA0, DMA_CH0);
    
    /* initialize DMA single data mode */
    dma_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr = (uint32_t)(adc_value);
    dma_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_32BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_32BIT;
    dma_data_parameter.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number = 16;		//	16·ͨ����16������
    dma_data_parameter.priority = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_data_parameter);
  
    dma_circulation_enable(DMA0, DMA_CH0);
  
    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH0);
}

//	PA0~7: CH0 CH1 CH2 CH3 CH4 CH5 CH6 CH7
//	PB0~1: CH8 CH9
//	PC0~5: CH10 CH11 CH12 CH13 CH14 CH15
/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
		uint8_t i;
    /* configure the ADC sync mode */
    adc_mode_config(ADC_DAUL_REGULAL_PARALLEL);
    /* ADC scan mode function enable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);

    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL,16);
    /* ADC regular channel config */
		//	�źű仯��ΪѸ�٣���Ҫ�̵Ĳ���ʱ��SAMPLETIME 15��ͨ��
		for (i = 0; i < 16; i++) 
		{
				// ��ÿ��ͨ�����д���
				adc_regular_channel_config(ADC0, 0, adc_channels[i], ADC_SAMPLETIME_7POINT5);
		}

    /* ADC external trigger enable */
		//	ֻ��Ҫ������
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);
    /* ADC trigger config */
		//	��TIMER0_CH0��Ϊadc�����Ĵ����ź�
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_T0_CH0);
   
    /* enable ADC interface */
    adc_enable(ADC0);
    delay_1ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
    
    /* ADC DMA function enable */
    adc_dma_mode_enable(ADC0);
}