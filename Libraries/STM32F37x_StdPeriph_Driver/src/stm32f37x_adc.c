/**
  ******************************************************************************
  * @file    stm32f37x_adc.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    20-September-2012
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Analog to Digital Converter (ADC) peripheral:
  *           + Initialization and Configuration
  *           + Analog Watchdog configuration
  *           + Temperature Sensor, Vrefint (Internal Reference Voltage)
  *             and VBAT (Voltage battery) management
  *           + Regular Channels Configuration
  *           + Regular Channels DMA Configuration
  *           + Injected channels Configuration
  *           + Interrupts and flags management
  *         
  *  @verbatim
================================================================================
                      ##### How to use this driver #####
================================================================================
    [..]
    (#) Enable the ADC interface clock using 
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    (#) ADC pins configuration
       (++) Enable the clock for the ADC GPIOs using the following function:
            RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOx, ENABLE);
       (++) Configure these ADC pins in analog mode using GPIO_Init();
    (#) Configure the data alignment using the ADC_Init() function.
    (#) Activate the ADC peripheral using ADC_Cmd() function.

    *** Regular channels group configuration ***
    ============================================
    [..] 
    (+) To configure the ADC regular channels group features, use 
        ADC_Init() and ADC_RegularChannelConfig() functions.
    (+) To activate the continuous mode, use the ADC_ContinuousModeCmd()
        function.
    (+) To configure and activate the Discontinuous mode, use the
        ADC_DiscModeChannelCountConfig() and ADC_DiscModeCmd() functions.
    (+) To read the ADC converted values, use the ADC_GetConversionValue()
        function.

    *** DMA for Regular channels group features configuration ***
    =============================================================
    [..]
    (+) To enable the DMA mode for regular channels group, use the 
        ADC_DMACmd() function.
             
    *** Injected channels group configuration ***
    =============================================
    [..]    
    (+) To configure the ADC Injected channels group features, use 
        ADC_InjectedChannelConfig() function.
    (+) To activate the Injected Discontinuous mode, use the 
        ADC_InjectedDiscModeCmd() function.  
    (+) To activate the AutoInjected mode, use the ADC_AutoInjectedConvCmd() 
        function.
    (+) To read the ADC converted values, use the ADC_GetInjectedConversionValue() function.

  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f37x_adc.h"
#include "stm32f37x_rcc.h"

/** @addtogroup STM32F37x_StdPeriph_Driver
  * @{
  */

/** @defgroup ADC 
  * @brief ADC driver modules
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* CR2 register Mask */
#define ADC_CR2_CLEAR_MASK         ((uint32_t)0xFFF1F7FD)
/* ADC SQRx mask */
#define ADC_SQR_SQ_SET             ((uint32_t)0x0000001F)
/* ADC JSQRx mask */
#define ADC_JSQR_JSQ_SET           ((uint32_t)0x0000001F)
/* ADC SMPRx mask */
#define ADC_SMPR_SMP_SET           ((uint32_t)0x00000007)
/* ADC JDRx registers offset */
#define ADC_JDR_OFFSET             ((uint8_t)0x28)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup ADC_Private_Functions
  * @{
  */

/** @defgroup ADC_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions 
 *
@verbatim
 ===============================================================================
          ##### Initialization and Configuration functions #####
 ===============================================================================  
    [..] This section provides functions allowing to:
        (+) Scan Conversion Mode (multichannels or one channel) for regular group
        (+) ADC Continuous Conversion Mode (Continuous or Single conversion) for
            regular group
        (+) External trigger Edge and source of regular group, 
        (+) Converted data alignment (left or right)
        (+) The number of ADC conversions that will be done using the sequencer
            for regular channel group
        (+) Enable or disable the ADC peripheral
        (+) Start/Reset the calibration
   
@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes the ADCx peripheral registers to their default reset values.
  * @param  ADCx: where x can be 1 to select the ADC peripheral.
  * @retval None
  */
void ADC_DeInit(ADC_TypeDef * ADCx)
{
	/* Check the parameters */
	assert_param(IS_ADC_ALL_PERIPH(ADCx));

	/* Enable ADC1 reset state */
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);
	/* Release ADC1 from reset state */
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE);
}

/**
  * @brief  Initializes the ADCx peripheral according to the specified parameters
  *         in the ADC_InitStruct.
  * @param  ADCx: where x can be 1 to select the ADC peripheral.
  * @param  ADC_InitStruct: pointer to an ADC_InitTypeDef structure that contains
  *         the configuration information for the specified ADC peripheral.
  * @retval None
  */
void ADC_Init(ADC_TypeDef * ADCx, ADC_InitTypeDef * ADC_InitStruct)
{
	uint32_t tmpreg1 = 0;
	uint8_t tmpreg2 = 0;
	/* Check the parameters */
	assert_param(IS_ADC_ALL_PERIPH(ADCx));
	assert_param(IS_FUNCTIONAL_STATE(ADC_InitStruct->ADC_ScanConvMode));
	assert_param(IS_FUNCTIONAL_STATE
		     (ADC_InitStruct->ADC_ContinuousConvMode));
	assert_param(IS_ADC_EXT_TRIG(ADC_InitStruct->ADC_ExternalTrigConv));
	assert_param(IS_ADC_DATA_ALIGN(ADC_InitStruct->ADC_DataAlign));
	assert_param(IS_ADC_REGULAR_LENGTH(ADC_InitStruct->ADC_NbrOfChannel));

  /*---------------------------- ADCx CR1 Configuration -----------------*/
	/* Get the ADCx CR1 value */
	tmpreg1 = ADCx->CR1;
	/* Clear SCAN bit */
	tmpreg1 &= (uint32_t) (~ADC_CR1_SCAN);
	/* Configure ADCx: scan conversion mode */
	/* Set SCAN bit according to ADC_ScanConvMode value */
	tmpreg1 |=
	    (uint32_t) ((uint32_t) ADC_InitStruct->ADC_ScanConvMode << 8);
	/* Write to ADCx CR1 */
	ADCx->CR1 = tmpreg1;

  /*---------------------------- ADCx CR2 Configuration -----------------*/
	/* Get the ADCx CR2 value */
	tmpreg1 = ADCx->CR2;
	/* Clear CONT, ALIGN and EXTSEL bits */
	tmpreg1 &= ADC_CR2_CLEAR_MASK;
	/* Configure ADCx: external trigger event and continuous conversion mode */
	/* Set ALIGN bit according to ADC_DataAlign value */
	/* Set EXTSEL bits according to ADC_ExternalTrigConv value */
	/* Set CONT bit according to ADC_ContinuousConvMode value */
	tmpreg1 |=
	    (uint32_t) (ADC_InitStruct->
			ADC_DataAlign | ADC_InitStruct->ADC_ExternalTrigConv |
			((uint32_t) ADC_InitStruct->ADC_ContinuousConvMode <<
			 1));
	/* Write to ADCx CR2 */
	ADCx->CR2 = tmpreg1;

  /*---------------------------- ADCx SQR1 Configuration -----------------*/
	/* Get the ADCx SQR1 value */
	tmpreg1 = ADCx->SQR1;
	/* Clear L bits */
	tmpreg1 &= (uint32_t) (~ADC_SQR1_L);
	/* Configure ADCx: regular channel sequence length */
	/* Set L bits according to ADC_NbrOfChannel value */
	tmpreg2 |= (uint8_t) (ADC_InitStruct->ADC_NbrOfChannel - (uint8_t) 1);
	tmpreg1 |= (uint32_t) tmpreg2 << 20;
	/* Write to ADCx SQR1 */
	ADCx->SQR1 = tmpreg1;
}

/**
  * @brief  Fills each ADC_InitStruct member with its default value.
  * @param  ADC_InitStruct : pointer to an ADC_InitTypeDef structure which will be initialized.
  * @retval None
  */
void ADC_StructInit(ADC_InitTypeDef * ADC_InitStruct)
{
	/* Reset ADC init structure parameters values */
	/* initialize the ADC_ScanConvMode member */
	ADC_InitStruct->ADC_ScanConvMode = DISABLE;
	/* Initialize the ADC_ContinuousConvMode member */
	ADC_InitStruct->ADC_ContinuousConvMode = DISABLE;
	/* Initialize the ADC_ExternalTrigConv member */
	ADC_InitStruct->ADC_ExternalTrigConv = ADC_ExternalTrigConv_T19_TRGO;
	/* Initialize the ADC_DataAlign member */
	ADC_InitStruct->ADC_DataAlign = ADC_DataAlign_Right;
	/* Initialize the ADC_NbrOfChannel member */
	ADC_InitStruct->ADC_NbrOfChannel = 1;
}

/**
  * @brief  Enables or disables the specified ADC peripheral.
  * @param  ADCx: where x can be 1 to select the ADC peripheral.
  * @param  NewState: new state of the ADCx peripheral.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_Cmd(ADC_TypeDef * ADCx, FunctionalState NewState)
{
	/* Check the parameters */
	assert_param(IS_ADC_ALL_PERIPH(ADCx));
	assert_param(IS_FUNCTIONAL_STATE(NewState));
	if (NewState != DISABLE) {
		/* Set the ADON bit to wake up the ADC from power down mode */
		ADCx->CR2 |= ADC_CR2_ADON;
	} else {
		/* Disable the selected ADC peripheral */
		ADCx->CR2 &= (uint32_t) (~ADC_CR2_ADON);
	}
}

/**
  * @brief  Starts the selected ADC calibration process.
  * @param  ADCx: where x can be 1 to select the ADC peripheral.
  * @retval None
  */
void ADC_StartCalibration(ADC_TypeDef * ADCx)
{
	/* Check the parameters */
	assert_param(IS_ADC_ALL_PERIPH(ADCx));
	/* Enable the selected ADC calibration process */
	ADCx->CR2 |= ADC_CR2_CAL;
}

/**
  * @brief  Resets the selected ADC calibration registers.
  * @param  ADCx: where x can be 1 to select the ADC peripheral.
  * @retval None
  */
void ADC_ResetCalibration(ADC_TypeDef * ADCx)
{
	/* Check the parameters */
	assert_param(IS_ADC_ALL_PERIPH(ADCx));
	/* Resets the selected ADC calibration registers */
	ADCx->CR2 |= ADC_CR2_RSTCAL;
}

/**
  * @}
  */

/** @defgroup ADC_Group2 Analog Watchdog configuration functions
 *  @brief   Analog Watchdog configuration functions 
 *
@verbatim   
 ===============================================================================
          ##### Analog Watchdog configuration functions ##### 
 ===============================================================================  

    [..] This section provides functions allowing to configure the Analog Watchdog
         (AWD) feature in the ADC.
  
    [..] A typical configuration Analog Watchdog is done following these steps :
         (#) The ADC guarded channel(s) is (are) selected using the 
             ADC_AnalogWatchdogSingleChannelConfig() function.
         (#) The Analog watchdog lower and higher threshold are configured using
             the ADC_AnalogWatchdogThresholdsConfig() function.
         (#) The Analog watchdog is enabled and configured to enable the check,
             on one or more channels, using the  ADC_AnalogWatchdogCmd() function.

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the analog watchdog on single/all regular
  *         or injected channels
  * @param  ADCx: where x can be 1 to select the ADC peripheral.
  * @param  ADC_AnalogWatchdog: the ADC analog watchdog configuration.
  *          This parameter can be one of the following values:
  *            @arg ADC_AnalogWatchdog_SingleRegEnable: Analog watchdog on a single regular channel
  *            @arg ADC_AnalogWatchdog_SingleInjecEnable: Analog watchdog on a single injected channel
  *            @arg ADC_AnalogWatchdog_SingleRegOrInjecEnable: Analog watchdog on a single regular or injected channel
  *            @arg ADC_AnalogWatchdog_AllRegEnable: Analog watchdog on  all regular channel
  *            @arg ADC_AnalogWatchdog_AllInjecEnable: Analog watchdog on  all injected channel
  *            @arg ADC_AnalogWatchdog_AllRegAllInjecEnable: Analog watchdog on all regular and injected channels
  *            @arg ADC_AnalogWatchdog_None: No channel guarded by the analog watchdog
  * @retval None	  
  */
void ADC_AnalogWatchdogCmd(ADC_TypeDef * ADCx, uint32_t ADC_AnalogWatchdog)
{
	uint32_t tmpreg = 0;
	/* Check the parameters */
	assert_param(IS_ADC_ALL_PERIPH(ADCx));
	assert_param(IS_ADC_ANALOG_WATCHDOG(ADC_AnalogWatchdog));
	/* Get the old register value */
	tmpreg = ADCx->CR1;
	/* Clear AWDEN, AWDENJ and AWDSGL bits */
	tmpreg &=
	    (uint32_t) (~(ADC_CR1_JAWDEN | ADC_CR1_AWDEN | ADC_CR1_AWDSGL));
	/* Set the analog watchdog enable mode */
	tmpreg |= ADC_AnalogWatchdog;
	/* Store the new register value */
	ADCx->CR1 = tmpreg;
}

/**
  * @brief  Configures the high and low thresholds of the analog watchdog.
  * @param  ADCx: where x can be 1 to select the ADC peripheral.
  * @param  HighThreshold: the ADC analog watchdog High threshold value.
  *          This parameter must be a 12bit value.
  * @param  LowThreshold: the ADC analog watchdog Low threshold value.
  *          This parameter must be a 12bit value.
  * @retval None
  */
void ADC_AnalogWatchdogThresholdsConfig(ADC_TypeDef * ADCx,
					uint16_t HighThreshold,
					uint16_t LowThreshold)
{
	/* Check the parameters */
	assert_param(IS_ADC_ALL_PERIPH(ADCx));
	assert_param(IS_ADC_THRESHOLD(HighThreshold));
	assert_param(IS_ADC_THRESHOLD(LowThreshold));
	/* Set the ADCx high threshold */
	ADCx->HTR = HighThreshold;
	/* Set the ADCx low threshold */
	ADCx->LTR = LowThreshold;
}

/**
  * @brief  Configures the analog watchdog guarded single channel
  * @param  ADCx: where x can be 1 to select the ADC peripheral.
  * @param  ADC_Channel: the ADC channel to configure for the analog watchdog. 
  *          This parameter can be one of the following values:
  *            @arg ADC_Channel_0: ADC Channel0 selected
  *            @arg ADC_Channel_1: ADC Channel1 selected
  *            @arg ADC_Channel_2: ADC Channel2 selected
  *            @arg ADC_Channel_3: ADC Channel3 selected
  *            @arg ADC_Channel_4: ADC Channel4 selected
  *            @arg ADC_Channel_5: ADC Channel5 selected
  *            @arg ADC_Channel_6: ADC Channel6 selected
  *            @arg ADC_Channel_7: ADC Channel7 selected
  *            @arg ADC_Channel_8: ADC Channel8 selected
  *            @arg ADC_Channel_9: ADC Channel9 selected
  *            @arg ADC_Channel_10: ADC Channel10 selected
  *            @arg ADC_Channel_11: ADC Channel11 selected
  *            @arg ADC_Channel_12: ADC Channel12 selected
  *            @arg ADC_Channel_13: ADC Channel13 selected
  *            @arg ADC_Channel_14: ADC Channel14 selected
  *            @arg ADC_Channel_15: ADC Channel15 selected
  *            @arg ADC_Channel_16: ADC Channel16 selected
  *            @arg ADC_Channel_17: ADC Channel17 selected
  * @retval None
  */
void ADC_AnalogWatchdogSingleChannelConfig(ADC_TypeDef * ADCx,
					   uint8_t ADC_Channel)
{
	uint32_t tmpreg = 0;
	/* Check the parameters */
	assert_param(IS_ADC_ALL_PERIPH(ADCx));
	assert_param(IS_ADC_CHANNEL(ADC_Channel));
	/* Get the old register value */
	tmpreg = ADCx->CR1;
	/* Clear the Analog watchdog channel select bits */
	tmpreg &= (uint32_t) (~ADC_CR1_AWDCH);
	/* Set the Analog watchdog channel */
	tmpreg |= ADC_Channel;
	/* Store the new register value */
	ADCx->CR1 = tmpreg;
}

/**
  * @}
  */

/** @defgroup ADC_Group3 Temperature Sensor- Vrefint (Internal Reference Voltage) and VBAT management function
 *  @brief   Temperature Sensor- Vrefint (Internal Reference Voltage) and VBAT management function
 *
@verbatim   
 ===============================================================================
 ##### Temperature Sensor, Vrefint and VBAT management function #####
 ===============================================================================  

    [..]  This section provides a function allowing to enable/ disable the internal 
          connections between the ADC and the Temperature Sensor, the Vrefint
          and the VBAT sources.

    [..] A typical configuration to get the Temperature sensor and Vrefint channels 
         voltages is done following these steps :
         (#) Enable the internal connection of Temperature sensor and Vrefint sources 
             with the ADC channels using ADC_TempSensorVrefintCmd() function.
             Enable the internal connection of VBAT using SYSCFG_VBATMonitoringCmd(ENABLE);
         (#) Select the ADC_Channel_TempSensor and/or ADC_Channel_Vrefint and/or 
            ADC_Channel_Vbat using ADC_RegularChannelConfig()
            or ADC_InjectedChannelConfig() functions 
         (#) Get the voltage values, using ADC_GetConversionValue() or  
             ADC_GetInjectedConversionValue().
 
@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the temperature sensor and Vrefint channel.
  * @param  NewState: new state of the temperature sensor.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_TempSensorVrefintCmd(FunctionalState NewState)
{
	/* Check the parameters */
	assert_param(IS_FUNCTIONAL_STATE(NewState));

	if (NewState != DISABLE) {
		/* Enable the temperature sensor and Vrefint channel */
		ADC1->CR2 |= ADC_CR2_TSVREFE;
	} else {
		/* Disable the temperature sensor and Vrefint channel */
		ADC1->CR2 &= (uint32_t) (~ADC_CR2_TSVREFE);
	}
}

/**
  * @}
  */

/** @defgroup ADC_Group4 Regular Channels Configuration functions
 *  @brief   Regular Channels Configuration functions 
 *
@verbatim   
 ===============================================================================
            ##### Regular Channels Configuration functions  #####
 ===============================================================================  

    [..] This section provides functions allowing to manage the ADC regular channels,
         it is composed of 2 sub sections : 
  
         (#) Configuration and management functions for regular channels: This subsection 
             provides functions allowing to configure the ADC regular channels :    
             (++) Configure the rank in the regular group sequencer for each channel
             (++) Configure the sampling time for each channel
             (++) select the conversion Trigger for regular channels
             (++) select the desired EOC event behavior configuration
             (++) Activate the continuous Mode  (*)
             (++) Activate the Discontinuous Mode 
             -@@- Please Note that the following features for regular channels
                  are configured using the ADC_Init() function :
                  (+@@) scan mode activation 
                  (+@@) continuous mode activation (**) 
                  (+@@) External trigger source  
                  (+@@) External trigger edge 
                  (+@@) number of conversion in the regular channels group sequencer.
     
             -@@- (*) and (**) are performing the same configuration
     
         (#) Get the conversion data: This subsection provides an important function in 
             the ADC peripheral since it returns the converted data of the current 
             regular channel. When the Conversion value is read, the EOC Flag is 
             automatically cleared.
  
@endverbatim
  * @{
  */

/**
  * @brief  Configures for the selected ADC regular channel its corresponding
  *         rank in the sequencer and its sample time.
  * @param  ADCx: where x can be 1 to select the ADC peripheral.
  * @param  ADC_Channel: the ADC channel to configure. 
  *          This parameter can 
 
