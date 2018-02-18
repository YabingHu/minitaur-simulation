#ifndef MTCTRL_H
#define MTCTRL_H

#include "ModelController.h"
#include "AutoPID.h"

class MinitaurLegController : public ModelController {
public:
	MinitaurLegController(const char *);
	void step() override;
	void keyboardCallback(GLFWwindow *, int, int, int, int) override;
private:
	pid ctrlR, ctrlT;
	int sensor1, sensor2, motor1, motor2;
	double setR;
	double setT;
};

#endif
