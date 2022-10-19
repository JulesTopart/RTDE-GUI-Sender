#pragma once
#include <ur_rtde/rtde_control_interface.h>
#include <ur_rtde/rtde_io_interface.h>

#include "console.h"

using namespace ur_rtde;

#define TRY_RTDE if(isConnected())
#define CATCH_RTDE else

class UrRobot {
public:

	static bool connect(std::string);
	static void disconnect();

	static inline bool isConnected() { return _connected; };

	static void checkNetwork();

	static bool isRunning();

	static void setStandardDigitalOut(int pin, bool state);
	static void setAnalogOutputVoltage(int pin, double value);
	static void moveJ(std::vector<double> axis, double speed, double accel, int x1=0, int x2=0);
	static void moveL(std::vector<double> axis, double speed, double accel);
	static void moveL(std::vector<double> axis, double speed, double accel, double blend, double extrusion, bool async);
	static void moveP(std::vector<double> path);
	static void servoC(std::vector<double> axis, double speed, double accel, int x1 = 0, int x2 = 0);
	//static void moveP()

private:
	static RTDEControlInterface* rtde_control;
	static RTDEIOInterface* rtde_io;
	static bool _connected;

	static std::vector<std::vector<double>> pathBuffer;
};