/**
  ******************************************************************************
  * @file    usb_lib.h
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    28-August-2012
  * @brief   USB library include files
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_LIB_H
#define __USB_LIB_H

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "usb_type.h"
#include "usb_regs.h"
#include "usb_def.h"
#include "usb_core.h"
#include "usb_init.h"
#include "usb_sil.h"
#include "usb_mem.h"
#include "usb_int.h"

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#ifndef DEBUG
#undef USB_DEBUG
#endif

#ifdef USB_DEBUG
int stm_printf(const char *fmt, ...);
#define usb_printf	stm_printf
#else
#define usb_printf(fmt, ...)
#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* External variables --------------------------------------------------------*/

#endif /* __USB_LIB_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
