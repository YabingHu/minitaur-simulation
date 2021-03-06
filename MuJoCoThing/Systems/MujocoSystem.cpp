#include "MujocoSystem.h"
#include "../System.h"
#include "../render.h"

MujocoSystem::MujocoSystem(mjModel * m, mjData * d)
{
	mj_copyModel(model, m);
	mj_copyData(data, model, d);
}

MujocoSystem::MujocoSystem(char * file)
{
	char error[1000];
	this->model = mj_loadXML(file, NULL, error, sizeof(error));
	if (!this->model) {
		mju_error_s("Couldn't load model: %s", error);
		printf("%s\n", error);
	}
	else {
		this->data = mj_makeData(model);
	}

	this->realTime = true;
	this->lastTime = 0;
	this->data = mj_makeData(model);
}

MujocoSystem::~MujocoSystem()
{
	mj_deleteModel(model);
	mj_deleteData(data);
	if (render) render->close();
}

void MujocoSystem::setRealTime(bool b)
{
	this->realTime = b;
}

void MujocoSystem::setGraphics(bool b)
{
	if (b) { //starting graphics
		if(!render) 
			render = new mjRender(model);

		if(!render->isOpen())
			render->init("Minitaur", 500, 500);
	}
	else if (render && render->isOpen()) //ending graphics
		render->close();
}

void MujocoSystem::reset(){
	this->data = mj_makeData(model);
	this->lastTime = data->time;
	this->lastRender = data->time;
}

double MujocoSystem::step()
{
	if(!model) return 0;

	//read actuator data
	for (int i = 0; i < model->nu; i++) {
		string name = mj_id2name(model, mjtObj::mjOBJ_ACTUATOR, i);
		data->ctrl[i] = (*actuatorRef)[name];
	}

	mj_step2(model, data);

	//render
	if (shouldRender()) {
		render->render(data);
		lastRender = data->time;
		lastRealTime = clock();
	}

	mj_step1(model, data);
	int offset = 0;
	for (int i = 0; i < model->nsensor; i++) {
		std::vector<double> temp;
		int j = 0;
		while(j < model->sensor_dim[i]){
			temp.push_back(data->sensordata[offset+i+j]);
			j++;
		}
		offset += j-1;

		string name = mj_id2name(model, mjtObj::mjOBJ_SENSOR, i);
		(*sensorRef)[name] = temp;
	}
	//timekeeping
	double dt = data->time - lastTime;
	lastTime = data->time;
	return dt;
}

bool MujocoSystem::shouldRender()
{
	if (!render) return false;

	if (realTime)
		return data->time - lastRender > 1.0 / 60.0;
	else
		return (clock() - lastRealTime) / (double)CLOCKS_PER_SEC > 1.0 / 60.0;
}
