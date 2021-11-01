/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : App/p2p_server_app.c
 * Description        : P2P Server Application
 ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_common.h"
#include "dbg_trace.h"
#include "ble.h"
#include "p2p_server_app.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "vl53l0x_api.h"
#include "main.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern I2C_HandleTypeDef hi2c1;
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern RTC_HandleTypeDef hrtc;
/* USER CODE END PTD */

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
//Data transmitt containers
uint8_t str[64] = {0,};
char Message[64] = {0,};
uint8_t MessageLen;


//ToF sensor handler
VL53L0X_Dev_t Device[32] = {0,};
//ToF Sensors debugging data
VL53L0X_RangingMeasurementData_t RangingData[32] = {0,};
//Service variables
VL53L0X_Error status;
uint32_t refSpadCount	;
uint8_t isApertureSpads;
uint8_t VhvSettings;
uint8_t PhaseCal;
VL53L0X_DeviceInfo_t DevInfo[32] = {0,};
uint16_t  Ranged[32] = {0,};
uint16_t Measurement_counter = 0;
uint16_t Measurement_DayLimit = 5;

extern UART_HandleTypeDef huart1;
///Vbat Variables
extern uint16_t V_supply;
extern float battery_voltage;
extern float mcu_voltage;
extern uint8_t DMA_flag;
extern uint32_t ADC_buffer[2];
uint8_t ADC_Error = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void P2PS_Send_Notification( void );
void Run_Dev(VL53L0X_DEV Device);
void Measure(GPIO_TypeDef* Port, uint16_t Pin, uint8_t Index);
void SHUT(GPIO_TypeDef* Port, uint16_t Pin);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void P2PS_STM_App_Notification(P2PS_STM_App_Notification_evt_t *pNotification)
{
/* USER CODE BEGIN P2PS_STM_App_Notification_1 */

/* USER CODE END P2PS_STM_App_Notification_1 */
  switch(pNotification->P2P_Evt_Opcode)
  {
/* USER CODE BEGIN P2PS_STM_App_Notification_P2P_Evt_Opcode */

/* USER CODE END P2PS_STM_App_Notification_P2P_Evt_Opcode */

    case P2PS_STM__NOTIFY_ENABLED_EVT:
/* USER CODE BEGIN P2PS_STM__NOTIFY_ENABLED_EVT */

/* USER CODE END P2PS_STM__NOTIFY_ENABLED_EVT */
      break;

    case P2PS_STM_NOTIFY_DISABLED_EVT:
/* USER CODE BEGIN P2PS_STM_NOTIFY_DISABLED_EVT */

/* USER CODE END P2PS_STM_NOTIFY_DISABLED_EVT */
      break;

    case P2PS_STM_WRITE_EVT:
/* USER CODE BEGIN P2PS_STM_WRITE_EVT */

/* USER CODE END P2PS_STM_WRITE_EVT */
      break;

    default:
/* USER CODE BEGIN P2PS_STM_App_Notification_default */

/* USER CODE END P2PS_STM_App_Notification_default */
      break;
  }
/* USER CODE BEGIN P2PS_STM_App_Notification_2 */

/* USER CODE END P2PS_STM_App_Notification_2 */
  return;
}

void P2PS_APP_Notification(P2PS_APP_ConnHandle_Not_evt_t *pNotification)
{
/* USER CODE BEGIN P2PS_APP_Notification_1 */

/* USER CODE END P2PS_APP_Notification_1 */
  switch(pNotification->P2P_Evt_Opcode)
  {
/* USER CODE BEGIN P2PS_APP_Notification_P2P_Evt_Opcode */

/* USER CODE END P2PS_APP_Notification_P2P_Evt_Opcode */
  case PEER_CONN_HANDLE_EVT :
/* USER CODE BEGIN PEER_CONN_HANDLE_EVT */

/* USER CODE END PEER_CONN_HANDLE_EVT */
    break;

    case PEER_DISCON_HANDLE_EVT :
/* USER CODE BEGIN PEER_DISCON_HANDLE_EVT */

/* USER CODE END PEER_DISCON_HANDLE_EVT */
    break;

    default:
/* USER CODE BEGIN P2PS_APP_Notification_default */

/* USER CODE END P2PS_APP_Notification_default */
      break;
  }
/* USER CODE BEGIN P2PS_APP_Notification_2 */

/* USER CODE END P2PS_APP_Notification_2 */
  return;
}

void P2PS_APP_Init(void)
{
/* USER CODE BEGIN P2PS_APP_Init */
  UTIL_SEQ_RegTask( 1<< TIM1_ID, UTIL_SEQ_RFU, P2PS_Send_Notification );
/* USER CODE END P2PS_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
void P2PS_Send_Notification(void)

{
  /* const uint16_t test_data[20] = {0x123, 0x4567, 0x89AB, 0xCDEF, 0x1A1A, 0x2B2B, 0x3C3C, 0x4D4D,
  0x5E5E, 0x6F6F, 0x7A7A, 0x8B8B, 0x9C9C, 0xADAD, 0xBEBE, 0xCFCF, 0xDADA, 0xEBEB, 0xFCFC, 0xFFFF};
   APP_DBG_MSG("� P2P APPLICATION SERVER  : INFORM CLIENT BUTTON 1 PUSHED \n\r");*/
//  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

  Measure(XSHUT1_GPIO_Port, XSHUT1_Pin, 1);
  Measure(XSHUT2_GPIO_Port, XSHUT2_Pin, 2);

//  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  HAL_Delay(1000);

  /* P2PS_STM_App_Update_Int16_Pro(P2P_NOTIFY_CHAR_UUID, (uint16_t *) test_data, 10);*/

  P2PS_STM_App_Update_Char(P2P_NOTIFY_CHAR_UUID, (uint8_t*) 0x0D);
  return;

}


void Run_Dev(VL53L0X_DEV Device)
{
  VL53L0X_WaitDeviceBooted( Device );
  VL53L0X_DataInit( Device );
  VL53L0X_StaticInit( Device );
  VL53L0X_PerformRefCalibration(Device, &VhvSettings, &PhaseCal);
  VL53L0X_PerformRefSpadManagement(Device, &refSpadCount, &isApertureSpads);
  VL53L0X_SetDeviceMode(Device, VL53L0X_DEVICEMODE_SINGLE_RANGING);

  // Enable/Disable Sigma and Signal check
  VL53L0X_SetLimitCheckEnable(Device, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
  VL53L0X_SetLimitCheckEnable(Device, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);
  VL53L0X_SetLimitCheckValue(Device, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, (FixPoint1616_t)(0.1*65536));
  VL53L0X_SetLimitCheckValue(Device, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, (FixPoint1616_t)(60*65536));
  VL53L0X_SetMeasurementTimingBudgetMicroSeconds(Device, 33000);
  VL53L0X_SetVcselPulsePeriod(Device, VL53L0X_VCSEL_PERIOD_PRE_RANGE, 18);
  VL53L0X_SetVcselPulsePeriod(Device, VL53L0X_VCSEL_PERIOD_FINAL_RANGE, 14);
  return;
}


void HRSAPP_Measurement(void)
{
  /* USER CODE BEGIN HRSAPP_Measurement */
  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);

  SHUT(XSHUT1_GPIO_Port, XSHUT1_Pin);
  Measure(XSHUT1_GPIO_Port, XSHUT1_Pin, 0);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *) ADC_buffer, 2);
  Measure(XSHUT2_GPIO_Port, XSHUT2_Pin, 2);
  /*	Measure(XSHUT3_GPIO_Port, XSHUT3_Pin, 2);
  	Measure(XSHUT4_GPIO_Port, XSHUT4_Pin, 3);
  	Measure(XSHUT5_GPIO_Port, XSHUT5_Pin, 4);
  	Measure(XSHUT6_GPIO_Port, XSHUT6_Pin, 5);
  	Measure(XSHUT7_GPIO_Port, XSHUT7_Pin, 6);
  	Measure(XSHUT8_GPIO_Port, XSHUT8_Pin, 7);
  	Measure(XSHUT9_GPIO_Port, XSHUT9_Pin, 8);
  	Measure(XSHUT10_GPIO_Port, XSHUT10_Pin, 9);
  	Measure(XSHUT11_GPIO_Port, XSHUT11_Pin, 10);
  	Measure(XSHUT12_GPIO_Port, XSHUT12_Pin, 11);
  	Measure(XSHUT13_GPIO_Port, XSHUT13_Pin, 12);
  	Measure(XSHUT14_GPIO_Port, XSHUT14_Pin, 13);
  	Measure(XSHUT15_GPIO_Port, XSHUT15_Pin, 14);*/

  if(DMA_flag == 1)
    {
      DMA_flag = 0;
      mcu_voltage = ADC_buffer[1];
      battery_voltage = ADC_buffer[0];
      snprintf(Message, 63, "First measure %d\n Second measure%f\n\n",
               RangingData[0].RangeDMaxMilliMeter, battery_voltage);
      HAL_UART_Transmit(&huart1, (uint8_t*)Message, strlen(Message), 1000);
      snprintf(Message, 63, "ADC[0] %d\n ADC[1] %d\n\n", ADC_buffer[0], ADC_buffer[1]);
      HAL_UART_Transmit(&huart1, (uint8_t*) Message, strlen(Message), 1000);
    }

  if(Measurement_counter == Measurement_DayLimit)
    {
      HAL_RTC_DeInit(&hrtc);
      HAL_PWREx_EnterSHUTDOWNMode();
    }
  /* USER CODE END HRSAPP_Measurement */
  return;
}


void SHUT(GPIO_TypeDef* Port, uint16_t Pin)
{
  HAL_GPIO_WritePin(Port, Pin, GPIO_PIN_RESET); // Disable XSHUT
  HAL_Delay(20);
  HAL_GPIO_WritePin(Port, Pin, GPIO_PIN_SET); // Enable XSHUT
  HAL_Delay(20);
}

void Measure(GPIO_TypeDef* Port, uint16_t Pin, uint8_t Index)
{
//  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

  HAL_Delay(5000);

  SHUT(Port, Pin);

  Device[Index].I2cHandle = &hi2c1;
  Device[Index].I2cDevAddr = 0x52;

  Run_Dev(&Device[Index]);

  VL53L0X_PerformSingleRangingMeasurement(&Device[Index], &RangingData[Index]);

  if(RangingData[Index].RangeStatus == 0)
    {
      snprintf(Message, 63,  "Measured distance x : %i\n\r", RangingData[Index].RangeMilliMeter);


      HAL_UART_Transmit_DMA(&huart1, str, sizeof(str));

      Ranged[Index] = RangingData[Index].RangeMilliMeter;

      uint32_t measurement;

      measurement = Ranged[Index];

      Measurement_counter++;

      HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

      P2PS_STM_App_Update_Int16(P2P_NOTIFY_CHAR_UUID, &Ranged[Index], 1);
    }
  HAL_GPIO_WritePin(Port, Pin, GPIO_PIN_RESET); // Enable XSHUT
}

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/* USER CODE END FD_LOCAL_FUNCTIONS*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
