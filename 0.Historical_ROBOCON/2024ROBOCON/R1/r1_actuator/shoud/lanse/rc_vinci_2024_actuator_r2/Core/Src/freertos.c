/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
extern uint8_t nrf_key[20];
extern int target_gimbal ;
extern uint8_t cmd_speed_buf[1];
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
int cnt_T = 0;
extern uint8_t cmd_speed_buf[1];
extern int target_grab_2006;
extern int left_qv_miao_2006[3];
extern int target_grab_3508;
extern int target_grab_2006;
extern int right_qv_miao_2006[3];

extern  uint8_t step_left_qvmiao ;
extern uint8_t step_right_qvmiao ;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
extern pid_type_def  pid_p_can1_2006[8];
extern motor_measure_t motor_can2[8];
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId led_taskHandle;
osThreadId pid_cacla_TaskHandle;
osThreadId shoot_taskHandle;
osThreadId grab_taskHandle;
osThreadId gimbal_taskHandle;
osThreadId left_qvmiaoHandle;
osThreadId right_qvmiaoHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const *argument);
extern void LedTask(void const *argument);
extern void PidCaclaTask(void const *argument);
extern void ShootTask(void const *argument);
extern void GrabActionTask(void const *argument);
extern void GimbalPostionTask(void const *argument);
extern void left_qvmiao_Task(void const *argument);
extern void right_qvmiao_Task(void const *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of led_task */
  osThreadDef(led_task, LedTask, osPriorityIdle, 0, 128);
  led_taskHandle = osThreadCreate(osThread(led_task), NULL);

  /* definition and creation of pid_cacla_Task */
  osThreadDef(pid_cacla_Task, PidCaclaTask, osPriorityIdle, 0, 128);
  pid_cacla_TaskHandle = osThreadCreate(osThread(pid_cacla_Task), NULL);

  /* definition and creation of shoot_task */
  osThreadDef(shoot_task, ShootTask, osPriorityIdle, 0, 128);
  shoot_taskHandle = osThreadCreate(osThread(shoot_task), NULL);

  /* definition and creation of grab_task */
  osThreadDef(grab_task, GrabActionTask, osPriorityIdle, 0, 128);
  grab_taskHandle = osThreadCreate(osThread(grab_task), NULL);

  /* definition and creation of gimbal_task */
  osThreadDef(gimbal_task, GimbalPostionTask, osPriorityIdle, 0, 128);
  gimbal_taskHandle = osThreadCreate(osThread(gimbal_task), NULL);

  /* definition and creation of left_qvmiao */
  osThreadDef(left_qvmiao, left_qvmiao_Task, osPriorityIdle, 0, 128);
  left_qvmiaoHandle = osThreadCreate(osThread(left_qvmiao), NULL);

  /* definition and creation of right_qvmiao */
  osThreadDef(right_qvmiao, right_qvmiao_Task, osPriorityIdle, 0, 128);
  right_qvmiaoHandle = osThreadCreate(osThread(right_qvmiao), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  vTaskSuspend(grab_taskHandle);
  vTaskSuspend(shoot_taskHandle);
  vTaskSuspend(left_qvmiaoHandle);
  vTaskSuspend(right_qvmiaoHandle);
  osDelay(3000);

  target_grab_2006 = -19000;
  // cmd_speed_buf[0]=0;
  target_grab_3508 = 1000;
  // vTaskSuspend(gimbal_taskHandle);
  /* Infinite loop */
  for (;;)
  {

    if (nrf_key[0])
    {
      osDelay(20);
      while (nrf_key[0])
      {
        osDelay(1);
      }
      osDelay(20);
      vTaskResume(grab_taskHandle);
    }
    else if (nrf_key[1])
    {
      osDelay(20);
      while (nrf_key[1])
      {
        osDelay(1);
      }
      osDelay(20);
      vTaskResume(shoot_taskHandle);
    }
    else if (nrf_key[2])
    {
      osDelay(20);
      while (nrf_key[2])
      {
        osDelay(1);
      }
      osDelay(20);
      vTaskResume(left_qvmiaoHandle);
    }
    else if (nrf_key[3])
    {
      osDelay(20);
      while (nrf_key[3])
      {
        osDelay(1);
      }
      osDelay(20);
      vTaskResume(right_qvmiaoHandle);
    }
    else if (nrf_key[4])
    {
      osDelay(20);
      while (nrf_key[4])
      {
        osDelay(1);
      }
      osDelay(20);
      static uint8_t step111 = 0;
      if (!step111)
      {
        step111++;
        for (int i = 0; i < 3; i++)
        {
          left_qv_miao_2006[i] = 30000;
          right_qv_miao_2006[i] = 30000;
        }
      }
      else
      {
        step111--;
        target_grab_3508 = -300;
        target_grab_2006 = -27500;
      }
    }
    else if(nrf_key[7])
    {
	    
	osDelay(20);
      while (nrf_key[7])
      {
        osDelay(1);
      }
	//osDelay(20);    
	    
	    
	    
	    
      vTaskSuspend(right_qvmiaoHandle);
    osDelay(2);
	   for(int i =0;i<3;i++)
	   {
	   right_qv_miao_2006[i] = 70000;
	   }		   
	   step_right_qvmiao=0;
    
    
    }
        else if(nrf_key[6])
    {
	    
	osDelay(20);
      while (nrf_key[6])
      {
        osDelay(1);
      }
	//osDelay(20);    
	    
	    
	    
	    
      vTaskSuspend(left_qvmiaoHandle);
    osDelay(2);
	   for(int i =0;i<3;i++)
	   {
	   left_qv_miao_2006[i] = 70000;
	   }		   
	   step_left_qvmiao=0;
    
    
    }
    
    
    
    //����
     if(nrf_key[19])
    {
	if(nrf_key[8])
	{
	    
		osDelay(20);
		while (nrf_key[8])
		{
			osDelay(1);
		}
		target_gimbal=-660000;
			cmd_speed_buf[0]=50;
		   vTaskResume(shoot_taskHandle);
	
	}
	else if(nrf_key[9])
	{
	    
		osDelay(20);
		while (nrf_key[9])
		{
			osDelay(1);
		}
		target_gimbal=660000;
			cmd_speed_buf[0]=50;osDelay(20);
	   vTaskResume(shoot_taskHandle);
	}
	else  if(nrf_key[10])
	{
	    
		osDelay(20);
		while (nrf_key[10])
		{
			osDelay(1);
		}
		target_gimbal=650000;
			cmd_speed_buf[0]=50;osDelay(20);
	   vTaskResume(shoot_taskHandle);
	}
	else  if(nrf_key[11])
	{
	    
		osDelay(20);
		while (nrf_key[11])
		{
			osDelay(1);
		}
		target_gimbal=650000;
			cmd_speed_buf[0]=50;osDelay(20);
	   vTaskResume(shoot_taskHandle);
	}
	
	
		else  if(nrf_key[12])
	{
	    
		osDelay(20);
		while (nrf_key[12])
		{
			osDelay(1);
		}
			cmd_speed_buf[0]=50;
		target_gimbal=667000;osDelay(20);
	   vTaskResume(shoot_taskHandle);
	}
		else  if(nrf_key[13])
	{
	    
		osDelay(20);
		while (nrf_key[13])
		{
			osDelay(1);
		}
		target_gimbal=659000;
		cmd_speed_buf[0]=50;osDelay(20);
	   vTaskResume(shoot_taskHandle);
	}
		
	
	
	
	
	
	
    }
    else 
    {
	if(nrf_key[8])
	{
	    
		osDelay(20);
		while (nrf_key[8])
		{
			osDelay(1);
		}
		target_gimbal=-51000;
			cmd_speed_buf[0]=30;osDelay(20);
		   vTaskResume(shoot_taskHandle);
	
	}
	else if(nrf_key[9])
	{
	    
		osDelay(20);
		while (nrf_key[9])
		{
			osDelay(1);
		}
		target_gimbal=-69000;
			cmd_speed_buf[0]=30;osDelay(20);
	   vTaskResume(shoot_taskHandle);
	}
	else  if(nrf_key[10])
	{
	    
		osDelay(20);
		while (nrf_key[10])
		{
			osDelay(1);
		}
		target_gimbal=-66000;
			cmd_speed_buf[0]=30;osDelay(20);
	   vTaskResume(shoot_taskHandle);
	}
	else  if(nrf_key[11])
	{
	    
		osDelay(20);
		while (nrf_key[11])
		{
			osDelay(1);
		}
		target_gimbal=-87000;
			cmd_speed_buf[0]=30;osDelay(20);
	   vTaskResume(shoot_taskHandle);
	}
	
	
		else  if(nrf_key[12])
	{
	    
		osDelay(20);
		while (nrf_key[12])
		{
			osDelay(1);
		}
			cmd_speed_buf[0]=30;
		target_gimbal=-88000;osDelay(20);
	   vTaskResume(shoot_taskHandle);
	}
		else  if(nrf_key[13])
	{
	    
		osDelay(20);
		while (nrf_key[13])
		{
			osDelay(1);
		}
		target_gimbal=-87000;
		cmd_speed_buf[0]=30;
	osDelay(20);
	   vTaskResume(shoot_taskHandle);
	}
		
	
    
    
    }
    osDelay(5);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
