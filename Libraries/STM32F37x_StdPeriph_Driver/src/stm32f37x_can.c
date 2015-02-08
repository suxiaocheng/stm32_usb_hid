/**
  ******************************************************************************
  * @file    stm32f37x_can.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    20-September-2012
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Controller area network (CAN) peripheral:           
  *           + Initialization and Configuration 
  *           + CAN Frames Transmission 
  *           + CAN Frames Reception    
  *           + Operation modes switch  
  *           + Error management          
  *           + Interrupts and flags        
  *         
  *  @verbatim
================================================================================
                      ##### How to use this driver #####
================================================================================
    [..]
    (#) Enable the CAN controller interface clock using 
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); for CAN1 

    (#) CAN pins configuration
       (++) Enable the clock for the CAN GPIOs using the following function:
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);   
       (++) Connect the involved CAN pins to AF9 using the following function 
            GPIO_PinAFConfig(GPIOx, GPIO_PinSourcex, GPIO_AF_CANx); 
       (++) Configure these CAN pins in alternate function mode by calling
            the function  GPIO_Init();   
    (#) Initialise and configure the CAN using CAN_Init() and 
        CAN_FilterInit() functions.   

    (#) Transmit the desired CAN frame using CAN_Transmit() function.

    (#) Check the transmission of a CAN frame using CAN_TransmitStatus()
        function.

    (#) Cancel the transmission of a CAN frame using CAN_CancelTransmit()
        function.  

    (#) Receive a CAN frame using CAN_Recieve() function.

    (#) Release the receive FIFOs using CAN_FIFORelease() function.

    (#) Return the number of pending received frames using 
        CAN_MessagePending() function.            

    (#) To control CAN events you can use one of the following two methods:
       (++) Check on CAN flags using the CAN_GetFlagStatus() function.  
       (++) Use CAN interrupts through the function CAN_ITConfig() at 
            initialization phase and CAN_GetITStatus() function into 
            interrupt routines to check if the event has occurred or not.
            After checking on a flag you should clear it using CAN_ClearFlag()
            function. And after checking on an interrupt event you should 
            clear it using CAN_ClearITPendingBit() function.            

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
#include "stm32f37x_can.h"
#include "stm32f37x_rcc.h"

/** @addtogroup STM32F37x_StdPeriph_Driver
  * @{
  */

/** @defgroup CAN 
  * @brief CAN driver modules
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* CAN Master Control Register bits */
#define MCR_DBF           ((uint32_t)0x00010000)	/* software master reset */

/* CAN Mailbox Transmit Request */
#define TMIDxR_TXRQ       ((uint32_t)0x00000001)	/* Transmit mailbox request */

/* CAN Filter Master Register bits */
#define FMR_FINIT         ((uint32_t)0x00000001)	/* Filter init mode */

/* Time out for INAK bit */
#define INAK_TIMEOUT      ((uint32_t)0x0000FFFF)
/* Time out for SLAK bit */
#define SLAK_TIMEOUT      ((uint32_t)0x0000FFFF)

/* Flags in TSR register */
#define CAN_FLAGS_TSR     ((uint32_t)0x08000000)
/* Flags in RF1R register */
#define CAN_FLAGS_RF1R    ((uint32_t)0x04000000)
/* Flags in RF0R register */
#define CAN_FLAGS_RF0R    ((uint32_t)0x02000000)
/* Flags in MSR register */
#define CAN_FLAGS_MSR     ((uint32_t)0x01000000)
/* Flags in ESR register */
#define CAN_FLAGS_ESR     ((uint32_t)0x00F00000)

/* Mailboxes definition */
#define CAN_TXMAILBOX_0   ((uint8_t)0x00)
#define CAN_TXMAILBOX_1   ((uint8_t)0x01)
#define CAN_TXMAILBOX_2   ((uint8_t)0x02)

#define CAN_MODE_MASK     ((uint32_t) 0x00000003)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static ITStatus CheckITStatus(uint32_t CAN_Reg, uint32_t It_Bit);

/** @defgroup CAN_Private_Functions
  * @{
  */

/** @defgroup CAN_Group1 Initialization and Configuration functions
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
 ===============================================================================
          ##### Initialization and Configuration functions #####
 ===============================================================================
    [..] This section provides functions allowing to 
         (#) Initialize the CAN peripherals : Prescaler, operating mode, the maximum number 
             of time quanta to perform resynchronization, the number of time quanta in
             Bit Segment 1 and 2 and many other modes. 
         (#) Configures the CAN reception filter.
         (#) Select the start bank filter for slave CAN.
         (#) Enables or disables the Debug Freeze mode for CAN
         (#) Enables or disables the CAN Time Trigger Operation communication mode
   
@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes the CAN peripheral registers to their default reset values.
  * @param  CANx: where x can be 1 to select the CAN1 peripheral.
  * @retval None
  */
void CAN_DeInit(CAN_TypeDef * CANx)
{
	/* Check the parameters */
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	if (CANx == CAN1) {
		/* Enable CAN1 reset state */
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, ENABLE);
		/* Release CAN1 from reset state */
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, DISABLE);
	}
}

/**
  * @brief  Initializes the CAN peripheral according to the specified
  *         parameters in the CAN_InitStruct.
  * @param  CANx: where x can be 1 to select the CAN1 peripheral.
  * @param  CAN_InitStruct: pointer to a CAN_InitTypeDef structure that contains
  *         the configuration information for the CAN peripheral.
  * @retval Constant indicates initialization succeed which will be 
  *         CAN_InitStatus_Failed or CAN_InitStatus_Success.
  */
uint8_t CAN_Init(CAN_TypeDef * CANx, CAN_InitTypeDef * CAN_InitStruct)
{
	uint8_t InitStatus = CAN_InitStatus_Failed;
	uint32_t wait_ack = 0x00000000;
	/* Check the parameters */
	assert_param(IS_CAN_ALL_PERIPH(CANx));
	assert_param(IS_FUNCTIONAL_STATE(CAN_InitStruct->CAN_TTCM));
	assert_param(IS_FUNCTIONAL_STATE(CAN_InitStruct->CAN_ABOM));
	assert_param(IS_FUNCTIONAL_STATE(CAN_InitStruct->CAN_AWUM));
	assert_param(IS_FUNCTIONAL_STATE(CAN_InitStruct->CAN_NART));
	assert_param(IS_FUNCTIONAL_STATE(CAN_InitStruct->CAN_RFLM));
	assert_param(IS_FUNCTIONAL_STATE(CAN_InitStruct->CAN_TXFP));
	assert_param(IS_CAN_MODE(CAN_InitStruct->CAN_Mode));
	assert_param(IS_CAN_SJW(CAN_InitStruct->CAN_SJW));
	assert_param(IS_CAN_BS1(CAN_InitStruct->CAN_BS1));
	assert_param(IS_CAN_BS2(CAN_InitStruct->CAN_BS2));
	assert_param(IS_CAN_PRESCALER(CAN_InitStruct->CAN_Prescaler));

	/* Exit from sleep mode */
	CANx->MCR &= (~(uint32_t) CAN_MCR_SLEEP);

	/* Request initialisation */
	CANx->MCR |= CAN_MCR_INRQ;

	/* Wait the acknowledge */
	while (((CANx->MSR & CAN_MSR_INAK) != CAN_MSR_INAK)
	       && (wait_ack != INAK_TIMEOUT)) {
		wait_ack++;
	}

	/* Check acknowledge */
	if ((CANx->MSR & CAN_MSR_INAK) != CAN_MSR_INAK) {
		InitStatus = CAN_InitStatus_Failed;
	} else {
		/* Set the time triggered communication mode */
		if (CAN_InitStruct->CAN_TTCM == ENABLE) {
			CANx->MCR |= CAN_MCR_TTCM;
		} else {
			CANx->MCR &= ~(uint32_t) CAN_MCR_TTCM;
		}

		/* Set the automatic bus-off management */
		if (CAN_InitStruct->CAN_ABOM == ENABLE) {
			CANx->MCR |= CAN_MCR_ABOM;
		} else {
			CANx->MCR &= ~(uint32_t) CAN_MCR_ABOM;
		}

		/* Set the automatic wake-up mode */
		if (CAN_InitStruct->CAN_AWUM == ENABLE) {
			CANx->MCR |= CAN_MCR_AWUM;
		} else {
			CANx->MCR &= ~(uint32_t) CAN_MCR_AWUM;
		}

		/* Set the no automatic retransmission */
		if (CAN_InitStruct->CAN_NART == ENABLE) {
			CANx->MCR |= CAN_MCR_NART;
		} else {
			CANx->MCR &= ~(uint32_t) CAN_MCR_NART;
		}

		/* Set the receive FIFO locked mode */
		if (CAN_InitStruct->CAN_RFLM == ENABLE) {
			CANx->MCR |= CAN_MCR_RFLM;
		} else {
			CANx->MCR &= ~(uint32_t) CAN_MCR_RFLM;
		}

		/* Set the transmit FIFO priority */
		if (CAN_InitStruct->CAN_TXFP == ENABLE) {
			CANx->MCR |= CAN_MCR_TXFP;
		} else {
			CANx->MCR &= ~(uint32_t) CAN_MCR_TXFP;
		}

		/* Set the bit timing register */
		CANx->BTR =
		    (uint32_t) ((uint32_t) CAN_InitStruct->
				CAN_Mode << 30) | ((uint32_t) CAN_InitStruct->
						   CAN_SJW << 24) | ((uint32_t)
								     CAN_InitStruct->
								     CAN_BS1 <<
								     16) |
		    ((uint32_t) CAN_InitStruct->
		     CAN_BS2 << 20) | ((uint32_t) CAN_InitStruct->
				       CAN_Prescaler - 1);

		/* Request leave initialisation */
		CANx->MCR &= ~(uint32_t) CAN_MCR_INRQ;

		/* Wait the acknowledge */
		wait_ack = 0;

		while (((CANx->MSR & CAN_MSR_INAK) == CAN_MSR_INAK)
		       && (wait_ack != INAK_TIMEOUT)) {
			wait_ack++;
		}

		/* ...and check acknowledged */
		if ((CANx->MSR & CAN_MSR_INAK) == CAN_MSR_INAK) {
			InitStatus = CAN_InitStatus_Failed;
		} else {
			InitStatus = CAN_InitStatus_Success;
		}
	}

	/* At this step, return the status of initialization */
	return InitStatus;
}

/**
  * @brief  Configures the CAN reception filter according to the specified
  *         parameters in the CAN_FilterInitStruct.
  * @param  CAN_FilterInitStruct: pointer to a CAN_FilterInitTypeDef structure that
  *         contains the configuration information.
  * @retval None
  */
void CAN_FilterInit(CAN_FilterInitTypeDef * CAN_FilterInitStruct)
{
	uint32_t filter_number_bit_pos = 0;
	/* Check the parameters */
	assert_param(IS_CAN_FILTER_NUMBER
		     (CAN_FilterInitStruct->CAN_FilterNumber));
	assert_param(IS_CAN_FILTER_MODE(CAN_FilterInitStruct->CAN_FilterMode));
	assert_param(IS_CAN_FILTER_SCALE
		     (CAN_FilterInitStruct->CAN_FilterScale));
	assert_param(IS_CAN_FILTER_FIFO
		     (CAN_FilterInitStruct->CAN_FilterFIFOAssignment));
	assert_param(IS_FUNCTIONAL_STATE
		     (CAN_FilterInitStruct->CAN_FilterActivation));

	filter_number_bit_pos =
	    ((uint32_t) 1) << CAN_FilterInitStruct->CAN_FilterNumber;

	/* Initialisation mode for the filter */
	CAN1->FMR |= FMR_FINIT;

	/* Filter Deactivation */
	CAN1->FA1R &= ~(uint32_t) filter_number_bit_pos;

	/* Filter Scale */
	if (CAN_FilterInitStruct->CAN_FilterScale == CAN_FilterScale_16bit) {
		/* 16-bit scale for the filter */
		CAN1->FS1R &= ~(uint32_t) filter_number_bit_pos;

		/* First 16-bit identifier and First 16-bit mask */
		/* Or First 16-bit identifier and Second 16-bit identifier */
		CAN1->sFilterRegister[CAN_FilterInitStruct->CAN_FilterNumber].
		    FR1 =
		    ((0x0000FFFF & (uint32_t) CAN_FilterInitStruct->
		      CAN_FilterMaskIdLow) << 16) | (0x0000FFFF & (uint32_t)
						     CAN_FilterInitStruct->
						     CAN_FilterIdLow);

		/* Second 16-bit identifier and Second 16-bit mask */
		/* Or Third 16-bit identifier and Fourth 16-bit identifier */
		CAN1->sFilterRegister[CAN_FilterInitStruct->CAN_FilterNumber].
		    FR2 =
		    ((0x0000FFFF & (uint32_t) CAN_FilterInitStruct->
		      CAN_FilterMaskIdHigh) << 16) | (0x0000FFFF & (uint32_t)
						      CAN_FilterInitStruct->
						      CAN_FilterIdHigh);
	}

	if (CAN_FilterInitStruct->CAN_FilterScale == CAN_FilterScale_32bit) {
		/* 32-bit scale for the filter */
		CAN1->FS1R |= filter_number_bit_pos;
		/* 32-bit identifier or First 32-bit identifier */
		CAN1->sFilterRegister[CAN_FilterInitStruct->CAN_FilterNumber].
		    FR1 =
		    ((0x0000FFFF & (uint32_t) CAN_FilterInitStruct->
		      CAN_FilterIdHigh) << 16) | (0x0000FFFF & (uint32_t)
						  CAN_FilterInitStruct->
						  CAN_FilterIdLow);
		/* 32-bit mask or Second 32-bit identifier */
		CAN1->sFilterRegister[CAN_FilterInitStruct->CAN_FilterNumber].
		    FR2 =
		    ((0x0000FFFF & (uint32_t) CAN_FilterInitStruct->
		      CAN_FilterMaskIdHigh) << 16) | (0x0000FFFF & (uint32_t)
						      CAN_FilterInitStruct->
						      CAN_FilterMaskIdLow);
	}

	/* Filter Mode */
	if (CAN_FilterInitStruct->CAN_FilterMode == CAN_FilterMode_IdMask) {
		/*Id/Mask mode for the filter */
		CAN1->FM1R &= ~(uint32_t) filter_number_bit_pos;
