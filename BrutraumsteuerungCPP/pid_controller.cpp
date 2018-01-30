/*
 * PID_controller.cpp
 *
 * Created: 21.02.2016 15:35:36
 *  Author: Johannes
 */

#include "main.h"

void pid_init(PID_para *para, int16_t *Kp, int16_t *Ki, int16_t *Kd)
{
    para->pid_Kp = Kp;
    para->pid_Ki = Ki;
    para->pid_Kd = Kd;

    para->pid_P = 0;
    para->pid_I = 0;
    para->pid_D = 0;
	para->start = 1;
}

void pid_reset(PID_para *para)
{
    para->pid_P = 0;
    para->pid_I = 0;
    para->pid_D = 0;
}

uint8_t pid_update(PID_para *para, float sollwert, float istwert)
{
    float pid_error_last = para->pid_error;
    float retval;

    para->pid_error = sollwert - istwert;

    para->pid_P = para->pid_error * ((float)(*para->pid_Kp) / 100.0);
	if (para->pid_P > PID_MAX)
	{
		para->pid_P = (float)PID_MAX;
	}
	else if (para->pid_P < -PID_MAX)
	{
		para->pid_P = (float)-PID_MAX;
	}


    para->pid_I = para->pid_I + para->pid_error * ((float)(*para->pid_Ki) / 100.0);
	if (para->pid_I > PID_MAX)
	{
		para->pid_I = (float)PID_MAX;
	}
	else if (para->pid_I < -PID_MAX)
	{
		para->pid_I = (float)-PID_MAX;
	}

    if(para->start){
        pid_error_last = para->pid_error;
        para->start = 0;
    }

    para->pid_D = (pid_error_last - para->pid_error) * ((float)(*para->pid_Kd) / 100.0);
	if (para->pid_D > PID_MAX)
	{
		para->pid_D = (float)PID_MAX;
	}
	else if (para->pid_D < -PID_MAX)
	{
		para->pid_D = (float)-PID_MAX;
	}

    retval = ((para->pid_P + para->pid_I + para->pid_D) / SCALE);
    return pid_2_pwm(retval);
}

uint8_t pid_2_pwm(float erg)
{
  if(erg > 255.0) erg = 255.0;
  if(erg < 0) erg = 0;
    return(uint8_t)erg;
}
