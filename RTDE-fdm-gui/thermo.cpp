
float	Se = 0, 
		Ve = 0,
		pe = 0;

float	Kp = 0,
		Ki = 0,
		Kd = 0;


float PID(float consigne, float mesure) {

	float e = consigne - mesure;
	Se += e;
	Ve = e - pe;
	float cmd = Kp * Se + Kd * Ve;
	pe = e;
	

	return cmd;
}