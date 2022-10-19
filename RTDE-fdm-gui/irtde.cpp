#include "irtde.h"
#include "core.h"

using namespace ur_rtde;

RTDEControlInterface* UrRobot::rtde_control = nullptr;
RTDEIOInterface* UrRobot::rtde_io = nullptr;
bool UrRobot::_connected = false;
std::vector<std::vector<double>> UrRobot::pathBuffer;

bool UrRobot::connect(std::string ip){
	try{
		rtde_control = new RTDEControlInterface(ip);
		rtde_io = new RTDEIOInterface(ip);
	}
	catch (const std::exception&){
		Console::error("Connection failed.", "RTDE");
		rtde_control = nullptr;
		rtde_io = nullptr;
		return false;
	}
	Console::success("Connected sucessfully !", "RTDE");
	_connected = true;
	return true;
}

void UrRobot::disconnect(){
	delete rtde_control;
	delete rtde_io;

	rtde_control = nullptr;
	rtde_io = nullptr;

	_connected = false;
}

void UrRobot::checkNetwork() {
	if(isConnected())
		if (rtde_control->isConnected()) {
			_connected = true;
		}
		else {
			_connected = false;
		}
}

bool UrRobot::isRunning(){
	bool state = false;
	TRY_RTDE state = rtde_control->isProgramRunning();
	return state;
}

void UrRobot::setStandardDigitalOut(int pin, bool state){
	TRY_RTDE rtde_io->setStandardDigitalOut(pin, state);
}

void UrRobot::setAnalogOutputVoltage(int pin, double value){
	TRY_RTDE rtde_io->setAnalogOutputVoltage(pin, value);
}

void UrRobot::moveJ(std::vector<double> axis, double speed, double accel, int x1, int x2){
	TRY_RTDE rtde_control->moveJ({ axis[0], axis[1], axis[2], axis[3], axis[4], axis[5] }, speed, accel);
}

void UrRobot::moveL(std::vector<double> axis, double speed, double accel){
	TRY_RTDE rtde_control->moveL({ axis[0], axis[1], axis[2], axis[3], axis[4], axis[5] }, speed, accel);
}

void UrRobot::moveL(std::vector<double> axis, double speed, double accel, double blend, double extrusion, bool async) {
	
	if (async) {
		std::vector<double> path;
		path = axis;
		path.push_back(speed);
		path.push_back(accel);
		path.push_back(blend);
		path.push_back(extrusion);

		pathBuffer.push_back(path);
	}else {
		std::vector<double> path;
		path = axis;
		path.push_back(speed);
		path.push_back(accel);
		path.push_back(blend);
		path.push_back(extrusion);

		pathBuffer.push_back(path);
		TRY_RTDE rtde_control->moveL(pathBuffer);
		pathBuffer.clear();
	}
}

void UrRobot::moveP(std::vector<double>path) {
	//TRY_RTDE rtde_control->movePath;
}


void UrRobot::servoC(std::vector<double> axis, double speed, double accel, int x1, int x2) {
	TRY_RTDE rtde_control->servoC({ axis[0], axis[1], axis[2], axis[3], axis[4], axis[5] }, speed, accel);
}
