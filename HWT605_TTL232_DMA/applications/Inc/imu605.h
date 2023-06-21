//=====================================================================================================
// imu605.h
//=====================================================================================================
// See: https://github.com/tungchiahui
//
// Date : 28/05/2023		             
//
// Organization : ���紴��ѧ�� Vinci�����˶�
//
// Author : Tung Chia-hui
//
// Notes : Initial release
//
//=====================================================================================================


#ifndef __IMU605_H_
#define __IMU605_H_

#ifdef __cpluscplus
extern "C"
{
#endif

#include "main.h"
#include "struct_typedef.h"

typedef struct
{
	__packed struct
	{
		fp32 X;
		fp32 Y;
		fp32 Z;
	}Accel;

	__packed struct
	{
		fp32 X;
		fp32 Y;
		fp32 Z;
	}Gyro;
	
	__packed struct
	{
		fp32 X;
		fp32 Y;
		fp32 Z;
	}Magnet;
	
	__packed struct
	{
		fp32 yaw;    //ƫ����  ǰ����ƫ��
		fp32 pitch;  //������	 ǰ������°ڶ�
		fp32 roll;   //������  ���ҵ����°ڶ�
	}Euler;//ŷ����
	
	__packed struct
	{
		fp32 quat[4];
	}Quaternion;
	
	__packed struct
	{
		fp32 temp;
	}Other_Data;
	
}IMU_t;

#define	Accel_Type  		  0x51
#define	Gyro_Type  			0x52
#define	Magnet_Type  		0x54
#define	Euler_Type  		 	0x53
#define	Quaternion_Type 0x59

#define IMU_OK 1
#define IMU_Error 0

bool_t IMU_Read_Euler(uint8_t *rx_buffer);
uint8_t __SUMCRC(uint8_t *puchMsg, uint16_t usDataLen);

#ifdef __cpluscplus
}
#endif

#endif