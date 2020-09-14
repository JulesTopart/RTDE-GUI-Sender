#include "irtde.h"
#include "core.h"

using namespace ur_rtde;

UrRobot::UrRobot() {
	rtde_control = nullptr;
};

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
	Console::log("Connected sucessfully !", "RTDE");
	_connected = true;
	return true;
}

void UrRobot::disconnect(){

	_connected = false;
}

void UrRobot::checkNetwork() {
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

void UrRobot::testCircle(){
	TRY_RTDE{
		double velocity = 0.25;
		double acceleration = 1.2;
		double blend = 0.1;
		std::vector<double> waypoint_1 = { -0.300, -0.300, 0.100, -2.695, 1.605, -0.036 };
		std::vector<double> waypoint_2 = { -0.399, -0.199, 0.099, -2.694, 1.606, -0.037 };
		std::vector<double> waypoint_3 = { -0.500, -0.299, 0.099, -2.695, 1.606, -0.038 };
		std::vector<double> waypoint_4 = { -0.399, -0.400, 0.100, -2.695, 1.605, -0.038 };
		std::vector<double> waypoint_5 = { -0.300, -0.300, 0.100, -2.696, 1.605, -0.036 };

		// Move to init pose
		rtde_control->moveL({ -0.300, -0.300, 0.100, -2.695, 1.605, -0.036 });

		// Perform circular motion
		for (unsigned int i = 0; i < 5; i++)
		{
			rtde_control->moveP(waypoint_1, velocity, acceleration, blend);
			rtde_control->moveC(waypoint_2, waypoint_3, velocity, acceleration, blend);
			rtde_control->moveC(waypoint_4, waypoint_5, velocity, acceleration, blend);
		}
		//rtde_control->stop;
	}CATCH_RTDE{
		ASSERT("RTDE is offline");
	}
}

void UrRobot::setStandardDigitalOut(int pin, bool state){
	TRY_RTDE rtde_io->setStandardDigitalOut(pin, state);
}

void UrRobot::setAnalogOutputCurrent(int pin, double value){
	TRY_RTDE rtde_io->setAnalogOutputCurrent(pin, value);
}

void UrRobot::moveJ(std::vector<double> axis, double accel, double speed, int x1, int x2){
	TRY_RTDE rtde_control->moveJ({ axis[0], axis[1], axis[2], axis[3], axis[4], axis[5] }, accel, speed);
}

void UrRobot::moveL(std::vector<double> axis, double accel, double speed, int x1, int x2){
	TRY_RTDE rtde_control->moveL({ axis[0], axis[1], axis[2], axis[3], axis[4], axis[5] }, accel, speed);
}

