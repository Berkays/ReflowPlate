#include "analog.h"

HAL_StatusTypeDef Add_ADC_Channel(uint32_t channel, uint32_t samplingTime)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    sConfig.Channel = channel;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    sConfig.SamplingTime = samplingTime;
    return HAL_ADC_ConfigChannel(&hadc, &sConfig);
}

uint32_t Read_ADC(uint32_t channel, uint32_t samplingTime)
{
    hadc.Instance->CHSELR = 0;
    Add_ADC_Channel(channel, samplingTime);

    HAL_ADC_Start(&hadc);
    HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);
    uint32_t adcValue = HAL_ADC_GetValue(&hadc) * ADC_CORRECTION;
    HAL_ADC_Stop(&hadc);
    hadc.Instance->CHSELR = 0;

    return adcValue;
}