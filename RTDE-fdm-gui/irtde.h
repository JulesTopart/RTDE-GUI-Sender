#pragma once
#include <ur_rtde/rtde_control_interface.h>
#include <ur_rtde/rtde_io_interface.h>

#include "console.h"

using namespace ur_rtde;

#define TRY_RTDE if(isConnected())
#define CATCH_RTDE else

class UrRobot {
public:
	UrRobot();

	bool connect(std::string);
	void disconnect();

	inline bool isConnected() { return _connected; };

	void checkNetwork();

	bool isRunning();

	void setStandardDigitalOut(int pin, bool state);
	void setAnalogOutputCurrent(int pin, double value);
	void moveJ(std::vector<double> axis, double accel, double speed, int x1=0, int x2=0);
	void moveL(std::vector<double> axis, double accel, double speed, int x1=0, int x2=0);

	void testCircle();

private:
	RTDEControlInterface* rtde_control;
	RTDEIOInterface* rtde_io;

	bool _connected = false;

	//RTDEReceiveInterface* rtde_receive;
};