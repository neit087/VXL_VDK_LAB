#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

extern int timer_flag[10];
void setTimer(int index, int duration);
void timer_run(void);

#endif /* INC_SOFTWARE_TIMER_H_ */
