/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "main.h"
#include "fdcan.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>

#include "driver_servo_pwm.h"
#include "driver_user_button.h"
#include "driver_usb.h"
#include "driver_buzzer.h"

#include "driver_ZDTEmmV50.h"

#include "lib_deehli.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */
uint8_t has_new_msg = 0;
uint8_t* serial_buffer_copy;
int USB_user_callback(uint8_t* Buf, uint32_t len)
{
  printf("[fw][usb] received %lu bytes: %.*s\r\n", (unsigned long)len, (int)len, Buf);
  has_new_msg = 1;
  serial_buffer_copy = Buf;
    return 0;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void command_success(bool is_active) /*Sound to play on success*/
{
  if (!is_active) {return;}
  buzzer_enable(); buzzer_set_midi(90); HAL_Delay(200); buzzer_disable();
  HAL_Delay(200);
  buzzer_enable(); buzzer_set_midi(90); HAL_Delay(200); buzzer_disable();
}
void command_error() /*Sound to play on error*/
{
	  buzzer_enable(); buzzer_set_midi(90); HAL_Delay(100); buzzer_disable();
	  HAL_Delay(100);
	  buzzer_enable(); buzzer_set_midi(90); HAL_Delay(100); buzzer_disable();
	  HAL_Delay(100);
	  buzzer_enable(); buzzer_set_midi(90); HAL_Delay(100); buzzer_disable();
	  HAL_Delay(100);
	  buzzer_enable(); buzzer_set_midi(90); HAL_Delay(100); buzzer_disable();
}
void command_timeout() /*Sound to play on timeout*/
{
	  buzzer_enable(); buzzer_set_midi(90); HAL_Delay(50); buzzer_disable();
	  HAL_Delay(50);
	  buzzer_enable(); buzzer_set_midi(90); HAL_Delay(50); buzzer_disable();
	  HAL_Delay(50);
	  buzzer_enable(); buzzer_set_midi(90); HAL_Delay(50); buzzer_disable();
	  HAL_Delay(50);
	  buzzer_enable(); buzzer_set_midi(90); HAL_Delay(50); buzzer_disable();
	  HAL_Delay(50);
	  buzzer_enable(); buzzer_set_midi(90); HAL_Delay(50); buzzer_disable();
}

/* command errors serial notify */
void notify_setup_success() /*Serial data to send on setup success*/
{
	char buf_dbg[] = "[fw][usb] setup success\r\n";
	USB_transmit((uint8_t*) buf_dbg, strlen(buf_dbg));
	char buf[4] = "255\n";
	USB_transmit((uint8_t*) buf, 4);
}
void notify_cmd_ack() /*Serial data to send on valid command*/
{
	char buf_dbg[] = "[fw][usb] cmd ack\r\n";
	USB_transmit((uint8_t*) buf_dbg, strlen(buf_dbg));
	char buf[4] = "250\n";
	USB_transmit((uint8_t*) buf, 4);
}
void notify_command_success() /*Serial data to send on success*/
{
	char buf_dbg[] = "[fw][usb] cmd success\r\n";
	USB_transmit((uint8_t*) buf_dbg, strlen(buf_dbg));
	char buf[4] = "249\n";
	USB_transmit((uint8_t*) buf, 4);
}
void notify_command_error(uint8_t error_code) /*Serial data to send on error*/
{
	char buf_dbg[32];
	snprintf(buf_dbg, sizeof(buf_dbg), "[fw][usb] cmd error %u\r\n", error_code);
	USB_transmit((uint8_t*) buf_dbg, strlen(buf_dbg));
	char buf[10];
	snprintf(buf, sizeof(buf), "248,%u\n", error_code);
	USB_transmit((uint8_t*) buf, strlen(buf));
}
void notify_command_timeout() /*Serial data to send on timeout*/
{
	notify_command_error(10);
}
void notify_setup_error() /*Setup went wrong for example*/
{
	char buf[4] = "254\n";
	USB_transmit((uint8_t*) buf, 4);
	buzzer_enable(); buzzer_set_midi(90); HAL_Delay(50); buzzer_disable();
	HAL_Delay(50);
	buzzer_enable(); buzzer_set_midi(90); HAL_Delay(50); buzzer_disable();
	HAL_Delay(50);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USB_DEVICE_Init();
  MX_FDCAN1_Init();
  MX_TIM12_Init();
  /* USER CODE BEGIN 2 */
  /* =========== ZDT AND CAN SETUP =========== */
  FDCAN_FilterTypeDef sFilterConfig;

  sFilterConfig.IdType = FDCAN_EXTENDED_ID;
  sFilterConfig.FilterIndex = 0;
  sFilterConfig.FilterType = FDCAN_FILTER_MASK;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  sFilterConfig.FilterID1 = 0x0000;  // Set to 0 to accept all IDs
  sFilterConfig.FilterID2 = 0x0000;  // Set to 0 to accept all IDs
  sFilterConfig.RxBufferIndex = 0;
  if (CAN_add_filter(&hfdcan1, &sFilterConfig) != HAL_OK)
  {
    Error_Handler(); /* Filter configuration Error */
  }
  
  ZDT_Handler zdt;
  ZDT_ReturnCode retz;
  zdt.hfdcan = &hfdcan1;
  zdt.mot_id = 0x01U;
  zdt.checksum = ZDT_CS0x6B;
  zdt.receive_timeout = 1000;
  zdt.send_timeout = 3000;
  /* Init ZDT handler instance and CAN */
  if(ZDT_CAN_setup(&zdt)!= ZDT_OK)
  {
    Error_Handler();
  }
  
  /* =========== SERVO SETUP =========== */
  servo_PWM servo1;
  servo1.htim = &htim1;
  servo1.channel = TIM_CHANNEL_3;
  servo1.min_angle = -150; servo1.max_angle = 150;
  servo1.min_duty = 0.025; servo1.max_duty = 0.125;
  servo_PWM_start(&servo1);

  servo_PWM servo2;
  servo2.htim = &htim1;
  servo2.channel = TIM_CHANNEL_1;
  servo2.min_angle = -150; servo2.max_angle = 150;
  servo2.min_duty = 0.025; servo2.max_duty = 0.125;
  servo_PWM_start(&servo2);

  servo_PWM servo3;
  servo3.htim = &htim2;
  servo3.channel = TIM_CHANNEL_3;
  servo3.min_angle = -150; servo3.max_angle = 150;
  servo3.min_duty = 0.025; servo3.max_duty = 0.125;
  servo_PWM_start(&servo3);

  servo_PWM servo4;
  servo4.htim = &htim2;
  servo4.channel = TIM_CHANNEL_1;
  servo4.min_angle = -150; servo4.max_angle = 150;
  servo4.min_duty = 0.025; servo4.max_duty = 0.125;
  servo_PWM_start(&servo4);

  servo_PWM* servos[4] = {&servo1, &servo2, &servo3, &servo4};
  int n_servos = 4;

  /* =========== INIT POSITIONS AND MISC =========== */
  bool CMD_SOUND_ACK = false; // Set to false to disable sound ack for end of command
  float servo_offsets[] = {-5.0f, 1.0f, -2.0f, -6.0f}; // Servo and hardware angle offsets from default position

  servo_PWM_set_angle(&servo1, 0 + servo_offsets[0]);
  servo_PWM_set_angle(&servo2, 0 + servo_offsets[1]);
  servo_PWM_set_angle(&servo3, 0 + servo_offsets[2]);
  servo_PWM_set_angle(&servo4, 0 + servo_offsets[3]);

  if (ZDT_cmd_motor_enable_control(&zdt, true, false))
  {
	  notify_setup_error();
	  Error_Handler();
  }

  /* =========== AUTO HOMING ON STARTUP =========== */
  printf("[fw] startup homing\r\n");
  retz = ZDT_cmd_trigger_homing(&zdt, ZDT_HomNear, false);
  if (retz == ZDT_TIMEOUT)
  {
	  notify_setup_error();
	  Error_Handler();
  }
  else if (retz)
  {
	  notify_setup_error();
	  Error_Handler();
  }

  retz = ZDT_receive_reached_message(&zdt, 15000);
  if (retz == ZDT_TIMEOUT)
  {
	  notify_setup_error();
	  Error_Handler();
  }
  else if (retz)
  {
	  notify_setup_error();
	  Error_Handler();
  }

  retz = ZDT_cmd_set_0_position_homing(&zdt, false);
  if (retz == ZDT_TIMEOUT)
  {
	  notify_setup_error();
	  Error_Handler();
  }
  else if (retz)
  {
	  notify_setup_error();
	  Error_Handler();
  }
   
  double dout = 0.0;
  int ret;
  int msg_type;
  char **out_strings;
  int str_count;

  /* Play sucess buzzer sound and notify through serial */
  printf("[fw] setup complete\r\n");
  command_success(true); notify_setup_success();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  if (has_new_msg == 1) { // New message received
    has_new_msg = 0;
    printf("[fw] parsing USB message\r\n");
    ret = split_string((char *)serial_buffer, &out_strings, &str_count, ",");
    if (ret) {continue;}

    ret = chars_to_double(out_strings[0], &dout); /*Retrieve message type*/
    if (ret) {continue;}
    msg_type = (int)dout;
    printf("[fw] msg_type=%d str_count=%d\r\n", msg_type, str_count);

    if (msg_type == 1) { // servo set
      printf("[fw] servo set\r\n");
      if (str_count != 3) {command_error(); notify_command_error(1); continue;} // Arg count error

      ret = chars_to_double(out_strings[1], &dout);
      if (ret) {command_error(); notify_command_error(1); continue;}
      int servo_id = ((int)dout) -1;
      if (servo_id < 0 || servo_id >= n_servos) {
    	  command_error(); notify_command_error(1); continue;
      }

      ret = chars_to_double(out_strings[2], &dout);
      if (ret) {command_error(); notify_command_error(1); continue;}
      float target_angle = (float)dout + servo_offsets[servo_id];

      notify_cmd_ack(); /* Args are ok */

      ret = servo_PWM_set_angle(servos[servo_id], target_angle);
      if (ret) {command_error(); notify_command_error(1); continue;}
      command_success(CMD_SOUND_ACK); notify_command_success();
    } else if (msg_type == 2) { // servo goto
      printf("[fw] servo goto\r\n");
      if (str_count != 4) {command_error(); notify_command_error(1); continue;} // Arg count error

      ret = chars_to_double(out_strings[1], &dout);
      if (ret) {command_error(); notify_command_error(1); continue;}
      int servo_id = ((int)dout) -1;
      if (servo_id < 0 || servo_id >= n_servos) {
    	  command_error(); notify_command_error(1); continue;
      }

      ret = chars_to_double(out_strings[2], &dout);
      if (ret) {command_error(); notify_command_error(1); continue;}
      float target_angle = (float)dout + servo_offsets[servo_id];

      ret = chars_to_double(out_strings[3], &dout);
      if (ret) {command_error(); notify_command_error(1); continue;}
      float max_angle_speed = (float)dout;

      notify_cmd_ack(); /* Args are ok */

      ret = servo_PWM_goto_angle(servos[servo_id], target_angle, max_angle_speed);
      if (ret) {command_error(); notify_command_error(1); continue;}
      command_success(CMD_SOUND_ACK); notify_command_success();
    } else if (msg_type == 3) { // servo group goto 1by1
      printf("[fw] servo group goto 1by1\r\n");
      if (str_count%3 != 1) {command_error(); notify_command_error(1); continue;} // Arg count error
      int servo_count = (str_count - 1) / 3;
      if (servo_count <= 0 || servo_count > n_servos) {command_error(); notify_command_error(1); continue;}

      servo_PWM **servos_payload;
      float *angles_deg, *max_speeds_degps;
      servos_payload = (servo_PWM **) malloc(servo_count * sizeof(servo_PWM *));
      angles_deg = (float *) malloc(servo_count * sizeof(float));
      max_speeds_degps = (float *) malloc(servo_count * sizeof(float));
      if (servos_payload == NULL || angles_deg == NULL || max_speeds_degps == NULL) {command_error(); notify_command_error(1); continue;}

      for (int i=0; i<servo_count; i++) {
        ret = chars_to_double(out_strings[3*i + 1], &dout);
        if (ret) {command_error(); notify_command_error(1); continue;}
        int this_servo_id = ((int)dout)-1;
        if (this_servo_id < 0 || this_servo_id*sizeof(servo_PWM*) > sizeof(servos)) {command_error(); notify_command_error(1); continue;}

        ret = chars_to_double(out_strings[3*i + 2], &dout);
        if (ret) {command_error(); notify_command_error(1); continue;}
        float target_angle = (float)dout + servo_offsets[this_servo_id];

        ret = chars_to_double(out_strings[3*i + 3], &dout);
        if (ret) {command_error(); notify_command_error(1); continue;}
        float max_angle_speed = (float)dout;

        servos_payload[i] = servos[this_servo_id];
        angles_deg[i] = target_angle;
        max_speeds_degps[i] = max_angle_speed;
      }
      notify_cmd_ack(); /* Args are ok. */

      for (int i=0; i<servo_count; i++) {
        ret = servo_PWM_goto_angle(servos_payload[i], angles_deg[i], max_speeds_degps[i]);
        if (ret) {command_error(); notify_command_error(1); continue;}
      }

      command_success(CMD_SOUND_ACK); notify_command_success();
    } else if (msg_type == 4) { // servo group goto sync
      printf("[fw] servo group goto sync\r\n");
      if (str_count%3 != 1) {command_error(); notify_command_error(1); continue;} // Arg count error
      int servo_count = (str_count - 1) / 3;
      if (servo_count <= 0 || servo_count > n_servos) {command_error(); notify_command_error(1); continue;}

      servo_PWM **servos_payload;
      float *angles_deg, *max_speeds_degps;
      servos_payload = (servo_PWM **) malloc(servo_count * sizeof(servo_PWM *));
      angles_deg = (float *) malloc(servo_count * sizeof(float));
      max_speeds_degps = (float *) malloc(servo_count * sizeof(float));
      if (servos_payload == NULL || angles_deg == NULL || max_speeds_degps == NULL) {command_error(); notify_command_error(1); continue;}

      for (int i=0; i<servo_count; i++) {
        ret = chars_to_double(out_strings[3*i + 1], &dout);
        if (ret) {command_error(); notify_command_error(1); continue;}
        int this_servo_id = ((int)dout)-1;
        if (this_servo_id < 0 || this_servo_id*sizeof(servo_PWM*) > sizeof(servos)) {command_error(); notify_command_error(1); continue;}

        ret = chars_to_double(out_strings[3*i + 2], &dout);
        if (ret) {command_error(); notify_command_error(1); continue;}
        float target_angle = (float)dout + servo_offsets[this_servo_id];

        ret = chars_to_double(out_strings[3*i + 3], &dout);
        if (ret) {command_error(); notify_command_error(1); continue;}
        float max_angle_speed = (float)dout;

        servos_payload[i] = servos[this_servo_id];
        angles_deg[i] = target_angle;
        max_speeds_degps[i] = max_angle_speed;
      }

      notify_cmd_ack(); /* Args are ok. */

      ret = servo_PWM_group_goto_angle(servos_payload, angles_deg, max_speeds_degps, servo_count);
      if (ret) {command_error(); notify_command_error(1); continue;}
      command_success(CMD_SOUND_ACK); notify_command_success();
    } else if (msg_type == 10) { // ZDT enable control
      printf("[fw] ZDT enable control\r\n");

      notify_cmd_ack(); /* Args are ok. */

      retz = ZDT_cmd_motor_enable_control(&zdt, true, false);
      if (retz==ZDT_TIMEOUT) {command_error(); notify_command_error(10); continue;} // if timeout
      else if (retz) {command_error(); notify_command_error(1); continue;}
      command_success(CMD_SOUND_ACK); notify_command_success();
    } else if (msg_type == 11) { // ZDT pos control
        printf("[fw] ZDT position control\r\n");
        if (str_count != 6) {command_error(); notify_command_error(1); continue;} // Arg count error

        ret = chars_to_double(out_strings[1], &dout);
        if (ret) {command_error(); notify_command_error(1); continue;}
        ZDT_Dir dir; if ((int)dout) {dir=ZDT_CW;} else {dir=ZDT_CCW;}

        ret = chars_to_double(out_strings[2], &dout);
        if (ret) {command_error(); notify_command_error(1); continue;}
        uint32_t npulses = (uint32_t)dout;

        ret = chars_to_double(out_strings[3], &dout);
        if (ret) {command_error(); notify_command_error(1); continue;}
        uint16_t speed = (uint16_t)dout;

        ret = chars_to_double(out_strings[4], &dout);
        if (ret) {command_error(); notify_command_error(1); continue;}
        uint8_t accell = (uint8_t)dout;

        ret = chars_to_double(out_strings[5], &dout);
        if (ret) {command_error(); notify_command_error(1); continue;}
        bool isrelative; if ((int)dout) {isrelative=true;} else {isrelative=false;}

        notify_cmd_ack(); /* Args are ok. */

        retz = ZDT_cmd_position_mode_control(&zdt, dir, npulses, speed, accell, isrelative, false);
        if (retz==ZDT_TIMEOUT) {command_error(); notify_command_error(10); continue;} // if timeout
        else if (retz) {command_error(); notify_command_error(1); continue;}
        command_success(CMD_SOUND_ACK); notify_command_success();
    } else if (msg_type == 18) { // ZDT trigger homing
      printf("[fw] ZDT trigger homing\r\n");
      if (str_count != 1) {command_error(); notify_command_error(1); continue;}

      notify_cmd_ack();
      retz = ZDT_cmd_trigger_homing(&zdt, ZDT_HomNear, false);
      if (retz==ZDT_TIMEOUT) {command_error(); notify_command_error(10); continue;}
      else if (retz) {command_error(); notify_command_error(1); continue;}

      retz = ZDT_receive_reached_message(&zdt, 15000);
      if (retz==ZDT_TIMEOUT) {command_error(); notify_command_error(10); continue;}
      else if (retz) {command_error(); notify_command_error(1); continue;}

      command_success(CMD_SOUND_ACK); notify_command_success();
    } else if (msg_type == 19) { // ZDT set homing 0
      printf("[fw] ZDT set homing zero\r\n");
      if (str_count != 1) {command_error(); notify_command_error(1); continue;}

      notify_cmd_ack();
      retz = ZDT_cmd_set_0_position_homing(&zdt, false);
      if (retz==ZDT_TIMEOUT) {command_error(); notify_command_error(10); continue;}
      else if (retz) {command_error(); notify_command_error(1); continue;}

      command_success(CMD_SOUND_ACK); notify_command_success();
    } else if (msg_type == 101) { // deehli drive all blocking
        printf("[fw] drive all blocking\r\n");
        if (str_count != 11) {command_error(); notify_command_error(1); continue;} // Arg count error

        /* Retrieve parameters */
        /* -Servos- */
        servo_PWM *servos_payload[4] = {&servo1, &servo2, &servo3, &servo4};
        float angles_deg[4], max_speeds_degps[4];

        for (int i=0; i<4; i++) {
          ret = chars_to_double(out_strings[i + 1], &dout);
          if (ret) {command_error(); notify_command_error(1); continue;}
          float target_angle = (float)dout;

          angles_deg[i] = target_angle + servo_offsets[i];
        }

        ret = chars_to_double(out_strings[5], &dout);
        if (ret) {command_error(); notify_command_error(1); continue;}
        float max_angle_speed = (float)dout;
        for (int i=0; i<4; i++) { max_speeds_degps[i] = max_angle_speed; }

        /* -ZDT- */
        ret = chars_to_double(out_strings[6], &dout);
        if (ret) {command_error(); notify_command_error(1); continue;}
        ZDT_Dir dir; if ((int)dout) {dir=ZDT_CW;} else {dir=ZDT_CCW;}

        ret = chars_to_double(out_strings[7], &dout);
        if (ret) {command_error(); notify_command_error(1); continue;}
        uint32_t npulses = (uint32_t)dout;

        ret = chars_to_double(out_strings[8], &dout);
        if (ret) {command_error(); notify_command_error(1); continue;}
        uint16_t speed = (uint16_t)dout;

        ret = chars_to_double(out_strings[9], &dout);
        if (ret) {command_error(); notify_command_error(1); continue;}
        uint8_t accell = (uint8_t)dout;

        ret = chars_to_double(out_strings[10], &dout);
        if (ret) {command_error(); notify_command_error(1); continue;}
        uint32_t reached_timeout = (uint32_t)dout;

        notify_cmd_ack(); /* Args are ok. */

        /* Drive ZDT */
        printf("[fw] sending ZDT move\r\n");
        retz = ZDT_cmd_position_mode_control(&zdt, dir, npulses, speed, accell, true, false);
        if (retz==ZDT_TIMEOUT) {command_error(); notify_command_error(10); continue;} // if timeout
        else if (retz) {command_error(); notify_command_error(1); continue;}

        /* Drive servos */
        printf("[fw] moving servos\r\n");
        ret = servo_PWM_group_goto_angle(servos_payload, angles_deg, max_speeds_degps, 4);
        if (ret) {command_error(); notify_command_error(1); continue;}

        /* Wait for ZDT ack */
        printf("[fw] waiting for ZDT reached message\r\n");
        retz = ZDT_receive_reached_message(&zdt, reached_timeout);
        if (retz==ZDT_TIMEOUT) {command_error(); notify_command_error(10); continue;} // if timeout
        else if (retz) {command_error(); notify_command_error(1); continue;}

        command_success(CMD_SOUND_ACK); notify_command_success();
  	} else {
      command_error(); notify_command_error(0); continue;
    }
  }
  if (user_button_read() == GPIO_PIN_RESET) { // Button pressed: reset positions
    /* Retrieve parameters */
    /* -Servos- */
    servo_PWM *servos_payload[4] = {&servo1, &servo2, &servo3, &servo4};
    float angles_deg[4] = {0 + servo_offsets[0], 0 + servo_offsets[1], 0 + servo_offsets[2], 0 + servo_offsets[3]};
    float max_speeds_degps[4] = {60, 60, 60, 60};

    /* -ZDT- */
    ZDT_Dir dir = ZDT_CCW;
    uint32_t npulses = 0;
    uint16_t speed = 1000;
    uint8_t accell = 0;
    uint32_t reached_timeout = 15000;

    /* Drive ZDT */
    retz = ZDT_cmd_position_mode_control(&zdt, dir, npulses, speed, accell, true, false);
    if (retz==ZDT_TIMEOUT) {command_error(); notify_command_error(10); continue;} // if timeout
    else if (retz) {command_error(); notify_command_error(1); continue;}

    /* Drive servos */
    ret = servo_PWM_group_goto_angle(servos_payload, angles_deg, max_speeds_degps, 4);
    if (ret) {command_error(); notify_command_error(1); continue;}
    
    /* Wait for ZDT ack */
    retz = ZDT_receive_reached_message(&zdt, reached_timeout);
    if (retz==ZDT_TIMEOUT) {command_error(); notify_command_error(10); continue;} // if timeout
    else if (retz) {command_error(); notify_command_error(1); continue;}

    command_success(true); notify_command_success();
  }
  HAL_Delay(1);
}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
