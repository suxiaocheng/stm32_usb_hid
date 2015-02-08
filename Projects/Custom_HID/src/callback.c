/**
  ******************************************************************************
  * @file    usb_istr.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   ISTR events interrupt service routines
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
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
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_pwr.h"
#include "usb_istr.h"

uint8_t control_state_string[][16] = {"WAIT_SETUP", "SETTING_UP", \
			"IN_DATA", "OUT_DATA", "LAST_IN_DATA", "LAST_OUT_DATA", \
			"Error", "WAIT_STATUS_IN", "WAIT_STATUS_OUT", "STALLED", "PAUSE"};

#ifdef CTR_CALLBACK
void CTR_Callback(void)
{
	uint8_t *str;
    /* dump endpoint transfer status */
    //usb_printf("e0_E:%x\n", _GetENDPOINT(ENDP0));
    
    if(pInformation->ControlState >= sizeof(control_state_string)/sizeof(control_state_string[0])){
		str = "NULL";
	}else{
		str = control_state_string[pInformation->ControlState];
	}
	usb_printf("c_s:%s\n", str);
}
#endif

#ifdef RESET_CALLBACK
void RESET_Callback(void)
{
    /* dump istr register */
    usb_printf("cntr:%x\n", *CNTR);
}
#endif
