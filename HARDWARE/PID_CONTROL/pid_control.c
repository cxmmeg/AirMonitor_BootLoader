#ifndef __PID_CONTROL_C
#define __PID_CONTROL_C
#include "include.h"
//�ⲿȫ�ֱ���������Remote_Sour[2];
static float Setpoint = 0.0000;    //Ŀ���,PID������Ҫ�����ص�ֵ
extern float Pitch,Roll,Yaw;      //X Y Z
extern float 	Remote_Sour[2];    //ң�ؽ������黺�������ⲿ���顣ȫ������
float Remote_Prac[2];     //�ڲ�����
float PID1_P=45,PID1_I=10,PID1_D=15;  //PID����
//float PID2_P=45,PID2_I=13,PID2_D=15;
//e�ǽǶ�ƫ�KP,KI��KDʹ����PID��������Ҫ�Ĳ�������Ҫ���ǲ�ͬ��������յó�ʹС���ܹ�վ���ĺ�����ֵ
float PID1(float e,float kp,float ki,float kd)
{
    static float es=0,sum=0;
    float r;
    sum+=e;
	  if(sum>8)
			sum=8;
		if(sum<-8)
			sum=-8;
    r = kp*e+ki*sum+kd*(e-es);
    es=e;
    return r; 
}
float PID2(float e,float kp,float ki,float kd)
{
    static float es=0,sum=0;
    float r;
    sum+=e;
	  if(sum>8)
			sum=8;
		if(sum<-8)
			sum=-8;
    r = kp*e+ki*sum+kd*(e-es);
    es=e;
    return r; 
}
void main_control(void)
{
	 float PID1_out,PID2_out,Pwm1,Pwm2;
	 static float Bias;
   Get_Acc_Gyro_values();//�õ��������ݲ������ں�
	 Bias=Pitch-Setpoint;  //�Ƕ�ƫ��ֵ
	 PID1_out=PID1(Bias,PID1_P,PID1_I,PID1_D);
	 PID2_out=PID2(Bias,PID1_P,PID1_I,PID1_D);
	 if(Bias>30||Bias<-30)
		 Moto_PWM(0,0);
	 else
	 {
		 Pwm1=PID1_out;    //�������ŵ�ֵ
		 Pwm2=PID2_out;
	   Moto_PWM(Pwm1,Pwm2);    //��λ����
	 }
}
#endif



