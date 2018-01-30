
#ifndef PID_CONTROLLER_H_
#define PID_CONTROLLER_H_

#define SCALE			1.0
#define PID_MAX			255.0
#define PID_SUMMMAX		255.0

typedef struct{
	int16_t *pid_Kp;
	int16_t *pid_Ki;
	int16_t *pid_Kd;
	float pid_P;
	float pid_I;
	float pid_D;
	float pid_error;
	uint8_t start;
	}PID_para;

void pid_init(PID_para *para, int16_t *Kp, int16_t *Ki, int16_t *Kd);
void pid_reset(PID_para *para);
uint8_t pid_update(PID_para *para ,float sollwert, float istwert);
uint8_t pid_2_pwm(float erg);

#endif //PID_CONTROLLER_H_
