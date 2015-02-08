/**
  ******************************************************************************
  * @file    stm32f30x_tim.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    23-October-2012
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the TIM peripheral:
  *            + TimeBase management
  *            + Output Compare management
  *            + Input Capture management
  *            + Advanced-control timers (TIM1 and TIM8) specific features  
  *            + Interrupts, DMA and flags management
  *            + Clocks management
  *            + Synchronization management
  *            + Specific interface management
  *            + Specific remapping management      
  *              
  @verbatim
    
  ==============================================================================
                       ##### How to use this driver #####
  ==============================================================================
   [..] This driver provides functions to configure and program the TIM 
        of all stm32f30x devices.
        These functions are split in 9 groups: 
     
        (#) TIM TimeBase management: this group includes all needed functions 
            to configure the TM Timebase unit:
                 (++) Set/Get Prescaler
                 (++) Set/Get Autoreload  
                 (++) Counter modes configuration
                 (++) Set Clock division  
                 (++) Select the One Pulse mode
                 (++) Update Request Configuration
                 (++) Update Disable Configuration
                 (++) Auto-Preload Configuration 
                 (++) Enable/Disable the counter     
                   
       (#) TIM Output Compare management: this group includes all needed 
           functions to configure the Capture/Compare unit used in Output 
           compare mode: 
                 (++) Configure each channel, independently, in Output Compare mode
                 (++) Select the output compare modes
                 (++) Select the Polarities of each channel
                 (++) Set/Get the Capture/Compare register values
                 (++) Select the Output Compare Fast mode 
                 (++) Select the Output Compare Forced mode  
                 (++) Output Compare-Preload Configuration 
                 (++) Clear Output Compare Reference
                 (++) Select the OCREF Clear signal
                 (++) Enable/Disable the Capture/Compare Channels    
                    
        (#) TIM Input Capture management: this group includes all needed 
            functions to configure the Capture/Compare unit used in 
            Input Capture mode:
                 (++) Configure each channel in input capture mode
                 (++) Configure Channel1/2 in PWM Input mode
                 (++) Set the Input Capture Prescaler
                 (++) Get the Capture/Compare values      
                     
        (#) Advanced-control timers (TIM1 and TIM8) specific features
                 (++) Configures the Break input, dead time, Lock level, the OSSI,
                      the OSSR State and the AOE(automatic output enable)
                 (++) Enable/Disable the TIM peripheral Main Outputs
                 (++) Select the Commutation event
                 (++) Set/Reset the Capture Compare Preload Control bit
                               
        (#) TIM interrupts, DMA and flags management
                 (++) Enable/Disable interrupt sources
                 (++) Get flags status
                 (++) Clear flags/ Pending bits
                 (++) Enable/Disable DMA requests 
                 (++) Configure DMA burst mode
                 (++) Select CaptureCompare DMA request  
                
        (#) TIM clocks management: this group includes all needed functions 
            to configure the clock controller unit:
                 (++) Select internal/External clock
                 (++) Select the external clock mode: ETR(Mode1/Mode2), TIx or ITRx
         
        (#) TIM synchronization management: this group includes all needed 
            functions to configure the Synchronization unit:
                 (++) Select Input Trigger  
                 (++) Select Output Trigger  
                 (++) Select Master Slave Mode 
                 (++) ETR Configuration when used as external trigger   
       
        (#) TIM specific interface management, this group includes all 
            needed functions to use the specific TIM interface:
                 (++) Encoder Interface Configuration
                 (++) Select Hall Sensor   
           
        (#) TIM specific remapping management includes the Remapping 
            configuration of specific timers               
     
  @endverbatim
      
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
#include "stm32f30x_tim.h"
#include "stm32f30x_rcc.h"

/** @addtogroup STM32F30x_StdPeriph_Driver
  * @{
  */

/** @defgroup TIM 
  * @brief TIM driver modules
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* ---------------------- TIM registers bit mask ------------------------ */
#define SMCR_ETR_MASK      ((uint16_t)0x00FF)
#define CCMR_OFFSET        ((uint16_t)0x0018)
#define CCER_CCE_SET       ((uint16_t)0x0001)
#define	CCER_CCNE_SET      ((uint16_t)0x0004)
#define CCMR_OC13M_MASK    ((uint32_t)0xFFFEFF8F)
#define CCMR_OC24M_MASK    ((uint32_t)0xFEFF8FFF)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void TI1_Config(TIM_TypeDef * TIMx, uint16_t TIM_ICPolarity,
		       uint16_t TIM_ICSelection, uint16_t TIM_ICFilter);
static void TI2_Config(TIM_TypeDef * TIMx, uint16_t TIM_ICPolarity,
		       uint16_t TIM_ICSelection, uint16_t TIM_ICFilter);
static void TI3_Config(TIM_TypeDef * TIMx, uint16_t TIM_ICPolarity,
		       uint16_t TIM_ICSelection, uint16_t TIM_ICFilter);
static void TI4_Config(TIM_TypeDef * TIMx, uint16_t TIM_ICPolarity,
		       uint16_t TIM_ICSelection, uint16_t TIM_ICFilter);

/* Private functions ---------------------------------------------------------*/

/** @defgroup TIM_Private_Functions
  * @{
  */

/** @defgroup TIM_Group1 TimeBase management functions
 *  @brief   TimeBase management functions 
 *
@verbatim   
 ===============================================================================
                   ##### TimeBase management functions #####
 ===============================================================================  
  
             
    *** TIM Driver: how to use it in Timing(Time base) Mode ***
    ============================================================ 
    [..]
    To use the Timer in Timing(Time base) mode, the following steps are mandatory:
       
    (#) Enable TIM clock using 
        RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE) function               
    (#) Fill the TIM_TimeBaseInitStruct with the desired parameters.   
    (#) Call TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStruct) to configure 
        the Time Base unit
        with the corresponding configuration        
    (#) Enable the NVIC if you need to generate the update interrupt.        
    (#) Enable the corresponding interrupt using the function 
        TIM_ITConfig(TIMx, TIM_IT_Update)      
    (#) Call the TIM_Cmd(ENABLE) function to enable the TIM counter.
    [..]                  
    (@) All other functions can be used separately to modify, if needed,
        a specific feature of the Timer. 

@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes the TIMx peripheral registers to their default reset values.
  * @param  TIMx: where x can be 1, 2, 3, 4, 6 ,7 ,8, 15, 16 or 17 to select the TIM peripheral.
  * @retval None

  */
void TIM_DeInit(TIM_TypeDef * TIMx)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_PERIPH(TIMx));

	if (TIMx == TIM1) {
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1, ENABLE);
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1, DISABLE);
	} else if (TIMx == TIM2) {
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2, ENABLE);
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2, DISABLE);
	} else if (TIMx == TIM3) {
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, ENABLE);
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, DISABLE);
	} else if (TIMx == TIM4) {
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM4, ENABLE);
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM4, DISABLE);
	} else if (TIMx == TIM6) {
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM6, ENABLE);
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM6, DISABLE);
	} else if (TIMx == TIM7) {
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM7, ENABLE);
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM7, DISABLE);
	} else if (TIMx == TIM8) {
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM8, ENABLE);
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM8, DISABLE);
	} else if (TIMx == TIM15) {
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM15, ENABLE);
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM15, DISABLE);
	} else if (TIMx == TIM16) {
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM16, ENABLE);
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM16, DISABLE);
	} else {
		if (TIMx == TIM17) {
			RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM17, ENABLE);
			RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM17, DISABLE);
		}
	}
}

/**
  * @brief  Initializes the TIMx Time Base Unit peripheral according to 
  *         the specified parameters in the TIM_TimeBaseInitStruct.
  * @param  TIMx: where x can be  1, 2, 3, 4, 6 ,7 ,8, 15, 16 or 17 to select the TIM peripheral.
  * @param  TIM_TimeBaseInitStruct: pointer to a TIM_TimeBaseInitTypeDef structure
  *         that contains the configuration information for the specified TIM peripheral.
  * @retval None
  */
void TIM_TimeBaseInit(TIM_TypeDef * TIMx,
		      TIM_TimeBaseInitTypeDef * TIM_TimeBaseInitStruct)
{
	uint16_t tmpcr1 = 0;

	/* Check the parameters */
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	assert_param(IS_TIM_COUNTER_MODE
		     (TIM_TimeBaseInitStruct->TIM_CounterMode));
	assert_param(IS_TIM_CKD_DIV(TIM_TimeBaseInitStruct->TIM_ClockDivision));

	tmpcr1 = TIMx->CR1;

	if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM2) ||
	    (TIMx == TIM3) || (TIMx == TIM4)) {
		/* Select the Counter Mode */
		tmpcr1 &= (uint16_t) (~(TIM_CR1_DIR | TIM_CR1_CMS));
		tmpcr1 |= (uint32_t) TIM_TimeBaseInitStruct->TIM_CounterMode;
	}

	if ((TIMx != TIM6) && (TIMx != TIM7)) {
		/* Set the clock division */
		tmpcr1 &= (uint16_t) (~TIM_CR1_CKD);
		tmpcr1 |= (uint32_t) TIM_TimeBaseInitStruct->TIM_ClockDivision;
	}

	TIMx->CR1 = tmpcr1;

	/* Set the Autoreload value */
	TIMx->ARR = TIM_TimeBaseInitStruct->TIM_Period;

	/* Set the Prescaler value */
	TIMx->PSC = TIM_TimeBaseInitStruct->TIM_Prescaler;

	if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM15) ||
	    (TIMx == TIM16) || (TIMx == TIM17)) {
		/* Set the Repetition Counter value */
		TIMx->RCR = TIM_TimeBaseInitStruct->TIM_RepetitionCounter;
	}

	/* Generate an update event to reload the Prescaler 
	   and the repetition counter(only for TIM1 and TIM8) value immediatly */
	TIMx->EGR = TIM_PSCReloadMode_Immediate;
}

/**
  * @brief  Fills each TIM_TimeBaseInitStruct member with its default value.
  * @param  TIM_TimeBaseInitStruct : pointer to a TIM_TimeBaseInitTypeDef
  *         structure which will be initialized.
  * @retval None
  */
void TIM_TimeBaseStructInit(TIM_TimeBaseInitTypeDef * TIM_TimeBaseInitStruct)
{
	/* Set the default configuration */
	TIM_TimeBaseInitStruct->TIM_Period = 0xFFFFFFFF;
	TIM_TimeBaseInitStruct->TIM_Prescaler = 0x0000;
	TIM_TimeBaseInitStruct->TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct->TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct->TIM_RepetitionCounter = 0x0000;
}

/**
  * @brief  Configures the TIMx Prescaler.
  * @param  TIMx: where x can be  1, 2, 3, 4, 8, 15, 16 or 17 to select the TIM peripheral.
  * @param  Prescaler: specifies the Prescaler Register value
  * @param  TIM_PSCReloadMode: specifies the TIM Prescaler Reload mode
  *          This parameter can be one of the following values:
  *            @arg TIM_PSCReloadMode_Update: The Prescaler is loaded at the update event.
  *            @arg TIM_PSCReloadMode_Immediate: The Prescaler is loaded immediatly.
  * @retval None
  */
void TIM_PrescalerConfig(TIM_TypeDef * TIMx, uint16_t Prescaler,
			 uint16_t TIM_PSCReloadMode)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	assert_param(IS_TIM_PRESCALER_RELOAD(TIM_PSCReloadMode));
	/* Set the Prescaler value */
	TIMx->PSC = Prescaler;
	/* Set or reset the UG Bit */
	TIMx->EGR = TIM_PSCReloadMode;
}

/**
  * @brief  Specifies the TIMx Counter Mode to be used.
  * @param  TIMx: where x can be  1, 2, 3, 4 or 8 to select the TIM peripheral.
  * @param  TIM_CounterMode: specifies the Counter Mode to be used
  *          This parameter can be one of the following values:
  *            @arg TIM_CounterMode_Up: TIM Up Counting Mode
  *            @arg TIM_CounterMode_Down: TIM Down Counting Mode
  *            @arg TIM_CounterMode_CenterAligned1: TIM Center Aligned Mode1
  *            @arg TIM_CounterMode_CenterAligned2: TIM Center Aligned Mode2
  *            @arg TIM_CounterMode_CenterAligned3: TIM Center Aligned Mode3
  * @retval None
  */
void TIM_CounterModeConfig(TIM_TypeDef * TIMx, uint16_t TIM_CounterMode)
{
	uint16_t tmpcr1 = 0;

	/* Check the parameters */
	assert_param(IS_TIM_LIST3_PERIPH(TIMx));
	assert_param(IS_TIM_COUNTER_MODE(TIM_CounterMode));

	tmpcr1 = TIMx->CR1;

	/* Reset the CMS and DIR Bits */
	tmpcr1 &= (uint16_t) ~ (TIM_CR1_DIR | TIM_CR1_CMS);

	/* Set the Counter Mode */
	tmpcr1 |= TIM_CounterMode;

	/* Write to TIMx CR1 register */
	TIMx->CR1 = tmpcr1;
}

/**
  * @brief  Sets the TIMx Counter Register value
  * @param  TIMx: where x can be 1, 2, 3, 4, 6 ,7 ,8, 15, 16 or 17 to select the TIM peripheral.
  * @param  Counter: specifies the Counter register new value.
  * @retval None
  */
void TIM_SetCounter(TIM_TypeDef * TIMx, uint32_t Counter)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_PERIPH(TIMx));

	/* Set the Counter Register value */
	TIMx->CNT = Counter;
}

/**
  * @brief  Sets the TIMx Autoreload Register value
  * @param  TIMx: where x can be 1, 2, 3, 4, 6 ,7 ,8, 15, 16 or 17 to select the TIM peripheral.
  * @param  Autoreload: specifies the Autoreload register new value.
  * @retval None
  */
void TIM_SetAutoreload(TIM_TypeDef * TIMx, uint32_t Autoreload)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_PERIPH(TIMx));

	/* Set the Autoreload Register value */
	TIMx->ARR = Autoreload;
}

/**
  * @brief  Gets the TIMx Counter value.
  * @param  TIMx: where x can be 1, 2, 3, 4, 6 ,7 ,8, 15, 16 or 17 to select the TIM peripheral.
  * @retval Counter Register value
  */
uint32_t TIM_GetCounter(TIM_TypeDef * TIMx)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_PERIPH(TIMx));

	/* Get the Counter Register value */
	return TIMx->CNT;
}

/**
  * @brief  Gets the TIMx Prescaler value.
  * @param  TIMx: where x can be 1, 2, 3, 4, 6 ,7 ,8, 15, 16 or 17 to select the TIM peripheral.
  * @retval Prescaler Register value.
  */
uint16_t TIM_GetPrescaler(TIM_TypeDef * TIMx)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_PERIPH(TIMx));

	/* Get the Prescaler Register value */
	return TIMx->PSC;
}

/**
  * @brief  Enables or Disables the TIMx Update event.
  * @param  TIMx: where x can be 1, 2, 3, 4, 6 ,7 ,8, 15, 16 or 17 to select the TIM peripheral.
  * @param  NewState: new state of the TIMx UDIS bit
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_UpdateDisableConfig(TIM_TypeDef * TIMx, FunctionalState NewState)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	assert_param(IS_FUNCTIONAL_STATE(NewState));

	if (NewState != DISABLE) {
		/* Set the Update Disable Bit */
		TIMx->CR1 |= TIM_CR1_UDIS;
	} else {
		/* Reset the Update Disable Bit */
		TIMx->CR1 &= (uint16_t) ~ TIM_CR1_UDIS;
	}
}

/**
  * @brief  Configures the TIMx Update Request Interrupt source.
  * @param  TIMx: where x can be 1, 2, 3, 4, 6 ,7 ,8, 15, 16 or 17 to select the TIM peripheral.
  * @param  TIM_UpdateSource: specifies the Update source.
  *          This parameter can be one of the following values:
  *            @arg TIM_UpdateSource_Regular: Source of update is the counter
  *                 overflow/underflow or the setting of UG bit, or an update
  *                 generation through the slave mode controller.
  *            @arg TIM_UpdateSource_Global: Source of update is counter overflow/underflow.
  * @retval None
  */
void TIM_UpdateRequestConfig(TIM_TypeDef * TIMx, uint16_t TIM_UpdateSource)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	assert_param(IS_TIM_UPDATE_SOURCE(TIM_UpdateSource));

	if (TIM_UpdateSource != TIM_UpdateSource_Global) {
		/* Set the URS Bit */
		TIMx->CR1 |= TIM_CR1_URS;
	} else {
		/* Reset the URS Bit */
		TIMx->CR1 &= (uint16_t) ~ TIM_CR1_URS;
	}
}

/**
  * @brief  Sets or resets the update interrupt flag (UIF)status bit Remapping.
  *         when sets, reading TIMx_CNT register returns UIF bit instead of CNT[31]  
  * @param  TIMx: where x can be 1, 2, 3, 4, 6 ,7 ,8, 15, 16 or 17 to select the TIM peripheral.
  * @param  NewState: new state of the UIFREMAP bit.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_UIFRemap(TIM_TypeDef * TIMx, FunctionalState NewState)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	assert_param(IS_FUNCTIONAL_STATE(NewState));

	if (NewState != DISABLE) {
		/* Enable the TIM Counter */
		TIMx->CR1 |= TIM_CR1_UIFREMAP;
	} else {
		/* Disable the TIM Counter */
		TIMx->CR1 &= (uint16_t) ~ TIM_CR1_UIFREMAP;
	}
}

/**
  * @brief  Enables or disables TIMx peripheral Preload register on ARR.
  * @param  TIMx: where x can be 1, 2, 3, 4, 6 ,7 ,8, 15, 16 or 17 to select the TIM peripheral.
  * @param  NewState: new state of the TIMx peripheral Preload register
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_ARRPreloadConfig(TIM_TypeDef * TIMx, FunctionalState NewState)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	assert_param(IS_FUNCTIONAL_STATE(NewState));

	if (NewState != DISABLE) {
		/* Set the ARR Preload Bit */
		TIMx->CR1 |= TIM_CR1_ARPE;
	} else {
		/* Reset the ARR Preload Bit */
		TIMx->CR1 &= (uint16_t) ~ TIM_CR1_ARPE;
	}
}

/**
  * @brief  Selects the TIMx's One Pulse Mode.
  * @param  TIMx: where x can be 1, 2, 3, 4, 6 ,7 ,8, 15, 16 or 17 to select the TIM peripheral.
  * @param  TIM_OPMode: specifies the OPM Mode to be used.
  *          This parameter can be one of the following values:
  *            @arg TIM_OPMode_Single
  *            @arg TIM_OPMode_Repetitive
  * @retval None
  */
void TIM_SelectOnePulseMode(TIM_TypeDef * TIMx, uint16_t TIM_OPMode)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	assert_param(IS_TIM_OPM_MODE(TIM_OPMode));

	/* Reset the OPM Bit */
	TIMx->CR1 &= (uint16_t) ~ TIM_CR1_OPM;

	/* Configure the OPM Mode */
	TIMx->CR1 |= TIM_OPMode;
}

/**
  * @brief  Sets the TIMx Clock Division value.
  * @param  TIMx: where x can be 1, 2, 3, 4, 8, 15, 16 or 17, to select the TIM peripheral.
  * @param  TIM_CKD: specifies the clock division value.
  *          This parameter can be one of the following value:
  *            @arg TIM_CKD_DIV1: TDTS = Tck_tim
  *            @arg TIM_CKD_DIV2: TDTS = 2*Tck_tim
  *            @arg TIM_CKD_DIV4: TDTS = 4*Tck_tim
  * @retval None
  */
void TIM_SetClockDivision(TIM_TypeDef * TIMx, uint16_t TIM_CKD)
{
	/* Check the parameters */
	assert_param(IS_TIM_LIST1_PERIPH(TIMx));
	assert_param(IS_TIM_CKD_DIV(TIM_CKD));

	/* Reset the CKD Bits */
	TIMx->CR1 &= (uint16_t) (~TIM_CR1_CKD);

	/* Set the CKD value */
	TIMx->CR1 |= TIM_CKD;
}

/**
  * @brief  Enables or disables the specified TIM peripheral.
  * @param  TIMx: where x can be 1, 2, 3, 4, 6, 7, 8, 15, 16 or 17 to select 
  *        the TIMx peripheral.
  * @param  NewState: new state of the TIMx peripheral.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_Cmd(TIM_TypeDef * TIMx, FunctionalState NewState)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	assert_param(IS_FUNCTIONAL_STATE(NewState));

	if (NewState != DISABLE) {
		/* Enable the TIM Counter */
		TIMx->CR1 |= TIM_CR1_CEN;
	} else {
		/* Disable the TIM Counter */
		TIMx->CR1 &= (uint16_t) ~ TIM_CR1_CEN;
	}
}

/**
  * @}
  */

/** @defgroup TIM_Group2 Output Compare management functions
 *  @brief    Output Compare management functions 
 *
@verbatim   
 ===============================================================================
                ##### Output Compare management functions #####
 ===============================================================================  
       
  *** TIM Driver: how to use it in Output Compare Mode ***
  ======================================================== 
  [..] 
  To use the Timer in Output Compare mode, the following steps are mandatory:
       
       (#) Enable TIM clock using RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE) function
       
       (#) Configure the TIM pins by configuring the corresponding GPIO pins
       
       (#) Configure the Time base unit as described in the first part of this driver, 
           if needed, else the Timer will run with the default configuration:
           (++) Autoreload value = 0xFFFF
           (++) Prescaler value = 0x0000
           (++) Counter mode = Up counting
           (++) Clock Division = TIM_CKD_DIV1   
       (#) Fill the TIM_OCInitStruct with the desired parameters including:
           (++) The TIM Output Compare mode: TIM_OCMode
           (++) TIM Output State: TIM_OutputState
           (++) TIM Pulse value: TIM_Pulse
           (++) TIM Output Compare Polarity : TIM_OCPolarity
       
       (#) Call TIM_OCxInit(TIMx, &TIM_OCInitStruct) to configure the desired channel with the 
           corresponding configuration
       
       (#) Call the TIM_Cmd(ENABLE) function to enable the TIM counter.
 [..]      
       (@) All other functions can be used separately to modify, if needed,
           a specific feature of the Timer. 
          
       (@) In case of PWM mode, this function is mandatory:
           TIM_OCxPreloadConfig(TIMx, TIM_OCPreload_ENABLE); 
              
       (@) If the corresponding interrupt or DMA request are needed, the user should:
                (#@) Enable the NVIC (or the DMA) to use the TIM interrupts (or DMA requests). 
                (#@) Enable the corresponding interrupt (or DMA request) using the function 
                     TIM_ITConfig(TIMx, TIM_IT_CCx) (or TIM_DMA_Cmd(TIMx, TIM_DMA_CCx))   

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the TIMx Channel1 according to the specified parameters in
  *         the TIM_OCInitStruct.
  * @param  TIMx: where x can be 1, 2, 3, 4, 8, 15, 16 or 17, to select the TIM peripheral.
  * @param  TIM_OCInitStruct: pointer to a TIM_OCInitTypeDef structure that contains
  *         the configuration information for the specified TIM peripheral.
  * @retval None
  */
void TIM_OC1Init(TIM_TypeDef * TIMx, TIM_OCInitTypeDef * TIM_OCInitStruct)
{
	uint32_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

	/* Check the parameters */
	assert_param(IS_TIM_LIST1_PERIPH(TIMx));
	assert_param(IS_TIM_OC_MODE(TIM_OCInitStruct->TIM_OCMode));
	assert_param(IS_TIM_OUTPUT_STATE(TIM_OCInitStruct->TIM_OutputState));
	assert_param(IS_TIM_OC_POLARITY(TIM_OCInitStruct->TIM_OCPolarity));

	/* Disable the Channel 1: Reset the CC1E Bit */
	TIMx->CCER &= (uint32_t) ~ TIM_CCER_CC1E;

	/* Get the TIMx CCER register value */
	tmpccer = TIMx->CCER;
	/* Get the TIMx CR2 register value */
	tmpcr2 = TIMx->CR2;

	/* Get the TIMx CCMR1 register value */
	tmpccmrx = TIMx->CCMR1;

	/* Reset the Output Compare Mode Bits */
	tmpccmrx &= (uint32_t) ~ TIM_CCMR1_OC1M;
	tmpccmrx &= (uint32_t) ~ TIM_CCMR1_CC1S;
	/* Select the Output Compare Mode */
	tmpccmrx |= TIM_OCInitStruct->TIM_OCMode;

	/* Reset the Output Polarity level */
	tmpccer &= (uint32_t) ~ TIM_CCER_CC1P;
	/* Set the Output Compare Polarity */
	tmpccer |= TIM_OCInitStruct->TIM_OCPolarity;

	/* Set the Output State */
	tmpccer |= TIM_OCInitStruct->TIM_OutputState;

	if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM15)
	    || (TIMx == TIM16) || (TIMx == TIM17)) {
		assert_param(IS_TIM_OUTPUTN_STATE
			     (TIM_OCInitStruct->TIM_OutputNState));
		assert_param(IS_TIM_OCN_POLARITY
			     (TIM_OCInitStruct->TIM_OCNPolarity));
		assert_param(IS_TIM_OCNIDLE_STATE
			     (TIM_OCInitStruct->TIM_OCNIdleState));
		assert_param(IS_TIM_OCIDLE_STATE
			     (TIM_OCInitStruct->TIM_OCIdleState));

		/* Reset the Output N Polarity level */
		tmpccer &= (uint32_t) ~ TIM_CCER_CC1NP;
		/* Set the Output N Polarity */
		tmpccer |= TIM_OCInitStruct->TIM_OCNPolarity;
		/* Reset the Output N State */
		tmpccer &= (uint32_t) ~ TIM_CCER_CC1NE;

		/* Set the Output N State */
		tmpccer |= TIM_OCInitStruct->TIM_OutputNState;
		/* Reset the Output Compare and Output Compare N IDLE State */
		tmpcr2 &= (uint32_t) ~ TIM_CR2_OIS1;
		tmpcr2 &= (uint32_t) ~ TIM_CR2_OIS1N;
		/* Set the Output Idle state */
		tmpcr2 |= TIM_OCInitStruct->TIM_OCIdleState;
		/* Set the Output N Idle state */
		tmpcr2 |= TIM_OCInitStruct->TIM_OCNIdleState;
	}
	/* Write to TIMx CR2 */
	TIMx->CR2 = tmpcr2;

	/* Write to TIMx CCMR1 */
	TIMx->CCMR1 = tmpccmrx;

	/* Set the Capture Compare Register value */
	TIMx->CCR1 = TIM_OCInitStruct->TIM_Pulse;

	/* Write to TIMx CCER */
	TIMx->CCER = tmpccer;
}

/**
  * @brief  Initializes the TIMx Channel2 according to the specified parameters 
  *         in the TIM_OCInitStruct.
  * @param  TIMx: where x can be 1, 2, 3, 4, 8 or 15 to select the TIM peripheral.
  * @param  TIM_OCInitStruct: pointer to a TIM_OCInitTypeDef structure that contains
  *         the configuration information for the specified TIM peripheral.
  * @retval None
  */
void TIM_OC2Init(TIM_TypeDef * TIMx, TIM_OCInitTypeDef * TIM_OCInitStruct)
{
	uint32_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

	/* Check the parameters */
	assert_param(IS_TIM_LIST2_PERIPH(TIMx));
	assert_param(IS_TIM_OC_MODE(TIM_OCInitStruct->TIM_OCMode));
	assert_param(IS_TIM_OUTPUT_STATE(TIM_OCInitStruct->TIM_OutputState));
	assert_param(IS_TIM_OC_POLARITY(TIM_OCInitStruct->TIM_OCPolarity));

	/* Disable the Channel 2: Reset the CC2E Bit */
	TIMx->CCER &= (uint32_t) ~ TIM_CCER_CC2E;

	/* Get the TIMx CCER register value */
	tmpccer = TIMx->CCER;
	/* Get the TIMx CR2 register value */
	tmpcr2 = TIMx->CR2;

	/* Get the TIMx CCMR1 register value */
	tmpccmrx = TIMx->CCMR1;

	/* Reset the Output Compare mode and Capture/Compare selection Bits */
	tmpccmrx &= (uint32_t) ~ TIM_CCMR1_OC2M;
	tmpccmrx &= (uint32_t) ~ TIM_CCMR1_CC2S;

	/* Select the Output Compare Mode */
	tmpccmrx |= (uint32_t) (TIM_OCInitStruct->TIM_OCMode << 8);

	/* Reset the Output Polarity level */
	tmpccer &= (uint32_t) ~ TIM_CCER_CC2P;
	/* Set the Output Compare Polarity */
	tmpccer |=
	    (uint32_t) ((uint32_t) TIM_OCInitStruct->TIM_OCPolarity << 4);

	/* Set the Output State */
	tmpccer |=
	    (uint32_t) ((uint32_t) TIM_OCInitStruct->TIM_OutputState << 4);

	if ((TIMx == TIM1) || (TIMx == TIM8)) {
		assert_param(IS_TIM_OUTPUTN_STATE
			     (TIM_OCInitStruct->TIM_OutputNState));
		assert_param(IS_TIM_OCN_POLARITY
			     (TIM_OCInitStruct->TIM_OCNPolarity));
		assert_param(IS_TIM_OCNIDLE_STATE
			     (TIM_OCInitStruct->TIM_OCNIdleState));
		assert_param(IS_TIM_OCIDLE_STATE
			     (TIM_OCInitStruct->TIM_OCIdleState));

		/* Reset the Output N Polarity level */
		tmpccer &= (uint32_t) ~ TIM_CCER_CC2NP;
		/* Set the Output N Polarity */
		tmpccer |=
		    (uint32_t) ((uint32_t) TIM_OCInitStruct->
				TIM_OCNPolarity << 4);
		/* Reset the Output N State */
		tmpccer &= (uint32_t) ~ TIM_CCER_CC2NE;

		/* Set the Output N State */
		tmpccer |=
		    (uint32_t) ((uint32_t) TIM_OCInitStruct->
				TIM_OutputNState << 4);
		/* Reset the Output Compare and Output Compare N IDLE State */
		tmpcr2 &= (uint32_t) ~ TIM_CR2_OIS2;
		tmpcr2 &= (uint32_t) ~ TIM_CR2_OIS2N;
		/* Set the Output Idle state */
		tmpcr2 |=
		    (uint32_t) ((uint32_t) TIM_OCInitStruct->
				TIM_OCIdleState << 2);
		/* Set the Output N Idle state */
		tmpcr2 |=
		    (uint32_t) ((uint32_t) TIM_OCInitStruct->
				TIM_OCNIdleState << 2);
	}
	/* Write to TIMx CR2 */
	TIMx->CR2 = tmpcr2;

	/* Write to TIMx CCMR1 */
	TIMx->CCMR1 = tmpccmrx;

	/* Set the Capture Compare Register value */
	TIMx->CCR2 = TIM_OCInitStruct->TIM_Pulse;

	/* Write to TIMx CCER */
	TIMx->CCER = tmpccer;
}

/**
  * @brief  Initializes the TIMx Channel3 according to the specified parameters
  *         in the TIM_OCInitStruct.
  * @param  TIMx: where x can be 1, 2, 3, 4 or 8 to select the TIM peripheral.
  * @param  TIM_OCInitStruct: pointer to a TIM_OCInitTypeDef structure that contains
  *         the configuration information for the specified TIM peripheral.
  * @retval None
  */
void TIM_OC3Init(TIM_TypeDef * TIMx, TIM_OCInitTypeDef * TIM_OCInitStruct)
{
	uint32_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

	/* Check the parameters */
	assert_param(IS_TIM_LIST3_PERIPH(TIMx));
	assert_param(IS_TIM_OC_MODE(TIM_OCInitStruct->TIM_OCMode));
	assert_param(IS_TIM_OUTPUT_STATE(TIM_OCInitStruct->TIM_OutputState));
	assert_param(IS_TIM_OC_POLARITY(TIM_OCInitStruct->TIM_OCPolarity));

	/* Disable the Channel 3: Reset the CC2E Bit */
	TIMx->CCER &= (uint32_t) ~ TIM_CCER_CC3E;

	/* Get the TIMx CCER register value */
	tmpccer = TIMx->CCER;
	/* Get the TIMx CR2 register value */
	tmpcr2 = TIMx->CR2;

	/* Get the TIMx CCMR2 register value */
	tmpccmrx = TIMx->CCMR2;

	/* Reset the Output Compare mode and Capture/Compare selection Bits */
	tmpccmrx &= (uint32_t) ~ TIM_CCMR2_OC3M;
	tmpccmrx &= (uint32_t) ~ TIM_CCMR2_CC3S;
	/* Select the Output Compare Mode */
	tmpccmrx |= TIM_OCInitStruct->TIM_OCMode;

	/* Reset the Output Polarity level */
	tmpccer &= (uint32_t) ~ TIM_CCER_CC3P;
	/* Set the Output Compare Polarity */
	tmpccer |=
	    (uint32_t) ((uint32_t) TIM_OCInitStruct->TIM_OCPolarity << 8);

	/* Set the Output State */
	tmpccer |=
	    (uint32_t) ((uint32_t) TIM_OCInitStruct->TIM_OutputState << 8);

	if ((TIMx == TIM1) || (TIMx == TIM8)) {
		assert_param(IS_TIM_OUTPUTN_STATE
			     (TIM_OCInitStruct->TIM_OutputNState));
		assert_param(IS_TIM_OCN_POLARITY
			     (TIM_OCInitStruct->TIM_OCNPolarity));
		assert_param(IS_TIM_OCNIDLE_STATE
			     (TIM_OCInitStruct->TIM_OCNIdleState));
		assert_param(IS_TIM_OCIDLE_STATE
			     (TIM_OCInitStruct->TIM_OCIdleState));

		/* Reset the Output N Polarity level */
		tmpccer &= (uint32_t) ~ TIM_CCER_CC3NP;
		/* Set the Output N Polarity */
		tmpccer |=
		    (uint32_t) ((uint32_t) TIM_OCInitStruct->
				TIM_OCNPolarity << 8);
		/* Reset the Output N State */
		tmpccer &= (uint32_t) ~ TIM_CCER_CC3NE;

		/* Set the Output N State */
		tmpccer |=
		    (uint32_t) ((uint32_t) TIM_OCInitStruct->
				TIM_OutputNState << 8);
		/* Reset the Output Compare and Output Compare N IDLE State */
		tmpcr2 &= (uint32_t) ~ TIM_CR2_OIS3;
		tmpcr2 &= (uint32_t) ~ TIM_CR2_OIS3N;
		/* Set the Output Idle state */
		tmpcr2 |=
		    (uint32_t) ((uint32_t) TIM_OCInitStruct->
				TIM_OCIdleState << 4);
		/* Set the Output N Idle state */
		tmpcr2 |=
		    (uint32_t) ((uint32_t) TIM_OCInitStruct->
				TIM_OCNIdleState << 4);
	}
	/* Write to TIMx CR2 */
	TIMx->CR2 = tmpcr2;

	/* Write to TIMx CCMR2 */
	TIMx->CCMR2 = tmpccmrx;

	/* Set the Capture Compare Register value */
	TIMx->CCR3 = TIM_OCInitStruct->TIM_Pulse;

	/* Write to TIMx CCER */
	TIMx->CCER = tmpccer;
}

/**
  * @brief  Initializes the TIMx Channel4 according to the specified parameters
  *         in the TIM_OCInitStruct.
  * @param  TIMx: where x can be 1, 2, 3, 4 or 8 to select the TIM peripheral.
  * @param  TIM_OCInitStruct: pointer to a TIM_OCInitTypeDef structure that contains
  *         the configuration information for the specified TIM peripheral.
  * @retval None
  */
void TIM_OC4Init(TIM_TypeDef * TIMx, TIM_OCInitTypeDef * TIM_OCInitStruct)
{
	uint32_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

	/* Check the parameters */
	assert_param(IS_TIM_LIST3_PERIPH(TIMx));
	assert_param(IS_TIM_OC_MODE(TIM_OCInitStruct->TIM_OCMode));
	assert_param(IS_TIM_OUTPUT_STATE(TIM_OCInitStruct->TIM_OutputState));
	assert_param(IS_TIM_OC_POLARITY(TIM_OCInitStruct->TIM_OCPolarity));

	/* Disable the Channel 4: Reset the CC4E Bit */
	TIMx->CCER &= (uint32_t) ~ TIM_CCER_CC4E;

	/* Get the TIMx CCER register value */
	tmpccer = TIMx->CCER;
	/* Get the TIMx CR2 register value */
	tmpcr2 = TIMx->CR2;

	/* Get the TIMx CCMR2 register value */
	tmpccmrx = TIMx->CCMR2;

	/* Reset the Output Compare mode and Capture/Compare selection Bits */
	tmpccmrx &= (uint32_t) ~ TIM_CCMR2_OC4M;
	tmpccmrx &= (uint32_t) ~ TIM_CCMR2_CC4S;

	/* Select the Output Compare Mode */
	tmpccmrx |= (uint32_t) (TIM_OCInitStruct->TIM_OCMode << 8);

	/* Reset the Output Polarity level */
	tmpccer &= (uint32_t) ~ TIM_CCER_CC4P;
	/* Set the Output Compare Polarity */
	tmpccer |=
	    (uint32_t) ((uint32_t) TIM_OCInitStruct->TIM_OCPolarity << 12);

	/* Set the Output State */
	tmpccer |=
	    (uint32_t) ((uint32_t) TIM_OCInitStruct->TIM_OutputState << 12);

	if ((TIMx == TIM1) || (TIMx == TIM8)) {
		assert_param(IS_TIM_OCIDLE_STATE
			     (TIM_OCInitStruct->TIM_OCIdleState));
		/* Reset the Output Compare IDLE State */
		tmpcr2 &= (uint32_t) ~ TIM_CR2_OIS4;
		/* Set the Output Idle state */
		tmpcr2 |=
		    (uint32_t) ((uint32_t) TIM_OCInitStruct->
				TIM_OCIdleState << 6);
	}
	/* Write to TIMx CR2 */
	TIMx->CR2 = tmpcr2;

	/* Write to TIMx CCMR2 */
	TIMx->CCMR2 = tmpccmrx;

	/* Set the Capture Compare Register value */
	TIMx->CCR4 = TIM_OCInitStruct->TIM_Pulse;

	/* Write to TIMx CCER */
	TIMx->CCER = tmpccer;
}

/**
  * @brief  Initializes the TIMx Channel5 according to the specified parameters
  *         in the TIM_OCInitStruct.
  * @param  TIMx: where x can be 1 or 8 to select the TIM peripheral.
  * @param  TIM_OCInitStruct: pointer to a TIM_OCInitTypeDef structure that contains
  *         the configuration information for the specified TIM peripheral.
  * @retval None
  */
void TIM_OC5Init(TIM_TypeDef * TIMx, TIM_OCInitTypeDef * TIM_OCInitStruct)
{
	uint32_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

	/* Check the parameters */
	assert_param(IS_TIM_LIST4_PERIPH(TIMx));
	assert_param(IS_TIM_OC_MODE(TIM_OCInitStruct->TIM_OCMode));
	assert_param(IS_TIM_OUTPUT_STATE(TIM_OCInitStruct->TIM_OutputState));
	assert_param(IS_TIM_OC_POLARITY(TIM_OCInitStruct->TIM_OCPolarity));

	/* Disable the Channel 5: Reset the CC5E Bit */
	TIMx->CCER &= (uint32_t) ~ TIM_CCER_CC5E;	/* to be verified */

	/* Get the TIMx CCER register value */
	tmpccer = TIMx->CCER;
	/* Get the TIMx CR2 register value */
	tmpcr2 = TIMx->CR2;

	/* Get the TIMx CCMR3 register value */
	tmpccmrx = TIMx->CCMR3;

	/* Reset the Output Compare mode and Capture/Compare selection Bits */
	tmpccmrx &= (uint32_t) ~ TIM_CCMR3_OC5M;

	/* Select the Output Compare Mode */
	tmpccmrx |= (uint32_t) (TIM_OCInitStruct->TIM_OCMode);

	/* Reset the Output Polarity level */
	tmpccer &= (uint32_t) ~ TIM_CCER_CC5P;
	/* Set the Output Compare Polarity */
	tmpccer |=
	    (uint32_t) ((uint32_t) TIM_OCInitStruct->TIM_OCPolarity << 16);

	/* Set the Output State */
	tmpccer |=
	    (uint32_t) ((uint32_t) TIM_OCInitStruct->TIM_OutputState << 16);

	if ((TIMx == TIM1) || (TIMx == TIM8)) {
		assert_param(IS_TIM_OCIDLE_STATE
			     (TIM_OCInitStruct->TIM_OCIdleState));
		/* Reset the Output Compare IDLE State */
		tmpcr2 &= (uint32_t) ~ TIM_CR2_OIS5;
		/* Set the Output Idle state */
		tmpcr2 |=
		    (uint32_t) ((uint32_t) TIM_OCInitStruct->
				TIM_OCIdleState << 16);
	}
	/* Write to TIMx CR2 */
	TIMx->CR2 = tmpcr2;

	/* Write to TIMx CCMR2 */
	TIMx->CCMR3 = tmpccmrx;

	/* Set the Capture Compare Register value */
	TIMx->CCR5 = TIM_OCInitStruct->TIM_Pulse;

	/* Write to TIMx CCER */
	TIMx->CCER = tmpccer;
}

/**
  * @brief  Initializes the TIMx Channel6 according to the specified parameters
  *         in the TIM_OCInitStruct.
  * @param  TIMx: where x can be 1 or 8 to select the TIM peripheral.
  * @param  TIM_OCInitStruct: pointer to a TIM_OCInitTypeDef structure that contains
  *         the configuration information for the specified TIM peripheral.
  * @retval None
  */
void TIM_OC6Init(TIM_TypeDef * TIMx, TIM_OCInitTypeDef * TIM_OCInitStruct)
{
	uint32_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

	/* Check the parameters */
	assert_param(IS_TIM_LIST4_PERIPH(TIMx));
	assert_param(IS_TIM_OC_MODE(TIM_OCInitStruct->TIM_OCMode));
	assert_param(IS_TIM_OUTPUT_STATE(TIM_OCInitStruct->TIM_OutputState));
	assert_param(IS_TIM_OC_POLARITY(TIM_OCInitStruct->TIM_OCPolarity));

	/* Disable the Channel 5: Reset the CC5E Bit */
	TIMx->CCER &= (uint32_t) ~ TIM_CCER_CC6E;	/* to be verified */

	/* Get the TIMx CCER register value */
	tmpccer = TIMx->CCER;
	/* Get the TIMx CR2 register value */
	tmpcr2 = TIMx->CR2;

	/* Get the TIMx CCMR3 register value */
	tmpccmrx = TIMx->CCMR3;

	/* Reset the Output Compare mode and Capture/Compare selection Bits */
	tmpccmrx &= (uint32_t) ~ TIM_CCMR3_OC6M;

	/* Select the Output Compare Mode */
	tmpccmrx |= (uint32_t) (TIM_OCInitStruct->TIM_OCMode << 8);

	/* Reset the Output Polarity level */
	tmpccer &= (uint32_t) ~ TIM_CCER_CC6P;
	/* Set the Output Compare Polarity */
	tmpccer |=
	    (uint32_t) ((uint32_t) TIM_OCInitStruct->TIM_OCPolarity << 20);

	/* Set the Output State */
	tmpccer |=
	    (uint32_t) ((uint32_t) TIM_OCInitStruct->TIM_OutputState << 20);

	if ((TIMx == TIM1) || (TIMx == TIM8)) {
		assert_param(IS_TIM_OCIDLE_STATE
			     (TIM_OCInitStruct->TIM_OCIdleState));
		/* Reset the Output Compare IDLE State */
		tmpcr2 &= (uint32_t) ~ TIM_CR2_OIS6;
		/* Set the Output Idle state */
		tmpcr2 |= (uint16_t) (TIM_OCInitStruct->TIM_OCIdleState << 18);
	}
	/* Write to TIMx CR2 */
	TIMx->CR2 = tmpcr2;

	/* Write to TIMx CCMR2 */
	TIMx->CCMR3 = tmpccmrx;

	/* Set the Capture Compare Register value */
	TIMx->CCR6 = TIM_OCInitStruct->TIM_Pulse;

	/* Write to TIMx CCER */
	TIMx->CCER = tmpccer;
}

/**
  * @brief  Selects the TIM Group Channel 5 and Channel 1, 
            OC1REFC is the logical AND of OC1REFC and OC5REF.
  * @param  TIMx: where x can be  1 or 8 to select the TIMx peripheral
  * @param  NewState: new state of the Commutation event.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_SelectGC5C1(TIM_TypeDef * TIMx, FunctionalState NewState)
{
	/* Check the parameters */
	assert_param(IS_TIM_LIST4_PERIPH(TIMx));
	assert_param(IS_FUNCTIONAL_STATE(NewState));

	if (NewState != DISABLE) {
		/* Set the GC5C1 Bit */
		TIMx->CCR5 |= TIM_CCR5_GC5C1;
	} else {
		/* Reset the GC5C1 Bit */
		TIMx->CCR5 &= (uint32_t) ~ TIM_CCR5_GC5C1;
	}
}

/**
  * @brief  Selects the TIM Group Channel 5 and Channel 2, 
            OC2REFC is the logical AND of OC2REFC and OC5REF.
  * @param  TIMx: where x can be  1 or 8 to select the TIMx peripheral
  * @param  NewState: new state of the Commutation event.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_SelectGC5C2(TIM_TypeDef * TIMx, FunctionalState NewState)
{
	/* Check the parameters */
	assert_param(IS_TIM_LIST4_PERIPH(TIMx));
	assert_param(IS_FUNCTIONAL_STATE(NewState));

	if (NewState != DISABLE) {
		/* Set the GC5C2 Bit */
		TIMx->CCR5 |= TIM_CCR5_GC5C2;
	} else {
		/* Reset the GC5C2 Bit */
		TIMx->CCR5 &= (uint32_t) ~ TIM_CCR5_GC5C2;
	}
}

/**
  * @brief  Selects the TIM Group Channel 5 and Channel 3, 
            OC3REFC is the logical AND of OC3REFC and OC5REF.
  * @param  TIMx: 