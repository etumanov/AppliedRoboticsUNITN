#include "headers/BRO_spam_fists.h"

double sT = 0.002; // samplingTime
double r = 0.028; // radious
double ratio = 2046.278; // ratio between 1 meter and 1 degree rotation of the wheel, used to calculate the number of degree given a distance in meters

//controller 1
static double u_0 = 0.0, u_1 = 0.0;
static double y_0 = 0.0, y_1 = 0.0;

static double u_01 = 0.0, u_11 = 0.0;
static double y_01 = 0.0, y_11 = 0.0;

//controller 2
int setSpeedCounter = 0;
double alpha = 0.075;
const double PI = 3.141592653589;

static void
decode_bro_port (U8 in_port, U8 * out_port)
{
    switch (in_port) {
        case MOTOR_A:
            *out_port = NXT_PORT_A;
            break;
        case MOTOR_B:
            *out_port = NXT_PORT_B;
            break;
        case MOTOR_C:
            *out_port = NXT_PORT_C;
            break;
        
        case PORT_1:
            *out_port = NXT_PORT_S1;
            break;
        case PORT_2:
            *out_port = NXT_PORT_S2;
            break;
        case PORT_3:
            *out_port = NXT_PORT_S3;
            break;
        case PORT_4:
            *out_port = NXT_PORT_S4;
            break;
        default:    
            break;
    };
}

//  :3 Listen, here we will write the function that will decode only ONE order and will return some data, IF it's a sensors packet...
static void
decode_bro_input (const bro_fist_t * input_packet, bro_fist_t * output_packet, engines_t * motors)
{
    U8 temp_port;
    
    output_packet->port = input_packet->port;
    output_packet->operation = input_packet->operation;

    switch (input_packet->operation) {
        case LIGHT_SENSOR:
            decode_bro_port (input_packet->port, &temp_port);
            output_packet->data = (float) ecrobot_get_light_sensor(temp_port);
            break;
            
        case TOUCH_SENSOR:
            decode_bro_port (input_packet->port, &temp_port);
            output_packet->data = (float) ecrobot_get_touch_sensor(temp_port);
            break;
            
        case SOUND_SENSOR:
            decode_bro_port (input_packet->port, &temp_port);
            output_packet->data = (float) ecrobot_get_sound_sensor(temp_port);
            break;
            
        case RADAR_SENSOR:
            decode_bro_port (input_packet->port, &temp_port);
            output_packet->data = (float) ecrobot_get_sonar_sensor(temp_port);
            break;
            
        case TACHO_COUNT:
            decode_bro_port (input_packet->port, &temp_port);
            output_packet->data = (float) nxt_motor_get_count(temp_port);
            break;
 
       case SET_POWER:
            decode_bro_port (input_packet->port, &temp_port);
            switch (temp_port) {
                case NXT_PORT_A:
                    motors->first.speed_control_type = RAW_POWER;
                    motors->first.powers[0] = input_packet->data;
					output_packet->data = (float) nxt_motor_get_count(temp_port);
					output_packet->time = (long) ecrobot_get_systick_ms();
                break;
                
                case NXT_PORT_B:
                    motors->second.speed_control_type = RAW_POWER;
                    motors->second.powers[0] = input_packet->data;
					output_packet->data = (float) nxt_motor_get_count(temp_port);
					output_packet->time = (long) ecrobot_get_systick_ms();
                break;
                
                case NXT_PORT_C:
                    motors->third.speed_control_type = RAW_POWER;
                    motors->third.powers[0] = input_packet->data;
					output_packet->data = (float) nxt_motor_get_count(temp_port);
					output_packet->time = (long) ecrobot_get_systick_ms();
                break;
            };
			
            break;
		case SET_SPEED:
            motors->first.speed_control_type = PID_CONTROLLED;
			motors->second.speed_control_type = PID_CONTROLLED;
            motors->first.speed_ref[setSpeedCounter] = input_packet->data;
			motors->second.speed_ref[setSpeedCounter] = input_packet->data;
			if(setSpeedCounter == 0){
				motors->first.distance_ref[setSpeedCounter] = input_packet->data2;
				motors->second.distance_ref[setSpeedCounter] = input_packet->data2;
			}else{
				motors->first.distance_ref[setSpeedCounter] = motors->first.distance_ref[setSpeedCounter -1] + input_packet->data2;
				motors->second.distance_ref[setSpeedCounter] = motors->second.distance_ref[setSpeedCounter -1] + input_packet->data2;
			}
			setSpeedCounter++;
            break;
        default:
            break;
    };
}


//  Function to decode a LIST of FISTS and do things accordingly... :3 
void decode_bro_fists (bro_fist_t * orders, bro_fist_t * response, engines_t * motors)
{
	//manipulatePackets(&orders);
			
	setSpeedCounter = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        decode_bro_input (&orders[i], &response[i], motors);    
    };
};

void manipulatePackets(bro_fist_t * orders){
	bro_fist_t * input_packet = &orders[0];
	input_packet->operation = SET_SPEED;
	input_packet->data = 5;
	input_packet->data2 = 1;
	input_packet->port = NXT_PORT_B;
	
	input_packet = &orders[1];
	input_packet->operation = SET_SPEED;
	input_packet->data = 2;
	input_packet->data2 = 1;
	input_packet->port = NXT_PORT_B;
	
	input_packet = &orders[2];
	input_packet->operation = SET_SPEED;
	input_packet->data = 5;
	input_packet->data2 = 1;
	input_packet->port = NXT_PORT_B;
	
	input_packet = &orders[3];
	input_packet->operation = SET_SPEED;
	input_packet->data = 0;
	input_packet->data2 = 0;
	input_packet->port = NXT_PORT_B;
	
	input_packet = &orders[4];
	input_packet->operation = SET_SPEED;
	input_packet->data = 0;
	input_packet->data2 = 0;
	input_packet->port = NXT_PORT_B;
}

void updateMotors(engines_t * motors){
	motor_t *motor;
    motor_t *otherMotor;
	for(int m = 0; m < 2; m++){
		switch (m){
			case 0: motor = &(motors->first); otherMotor = &(motors->second); break;
			case 1: motor = &(motors->second); otherMotor = &(motors->first); break;
		}
		for (int i = SAMP_NUM; i > 0; i--){
			motor->revolutions[i] = motor->revolutions[i-1];
			motor->speeds[i] = motor->speeds[i-1];
		}
		motor->revolutions[0] = nxt_motor_get_count(motor->port);
		motor->times[0] = ecrobot_get_systick_ms();
		motor->speeds[0] = evaluate_speed(motor, motor->speeds[1]);
		
		if(motor->speed_control_type == NOT_USING)
			continue;
		
		if(motor->speed_control_type == PID_CONTROLLED){

			double distance = (((motor->revolutions[0] + otherMotor->revolutions[0])/2.0) * PI/180) * 0.028;

			if(abs(distance) >= motor->distance_ref[0]){
				for (int i = 0; i < BUFFER_SIZE - 1; i++){
					motor->speed_ref[i] = motor->speed_ref[i+1];
					motor->distance_ref[i] = motor->distance_ref[i+1];
					otherMotor->speed_ref[i] = otherMotor->speed_ref[i+1];
					otherMotor->distance_ref[i] = otherMotor->distance_ref[i+1];
				}
				motor->speed_ref[BUFFER_SIZE-1] = 0;
				motor->distance_ref[BUFFER_SIZE-1] = 0;
				otherMotor->speed_ref[BUFFER_SIZE-1] = 0;
				otherMotor->distance_ref[BUFFER_SIZE-1] = 0;
			}
			
			if(distance < motor->distance_ref[0]){
				double error = motor->speed_ref[0] - motor->speeds[0];
				double rotationError = rotationController(motors);

				if(m == 0) {
					error = error - rotationError/2;
					motor->powers[0] = controller(error);

				}
				if(m == 1) {
					error = error + rotationError/2;
					motor->powers[0] = controller2(error);
				}

			}
		}
		
		nxt_motor_set_speed(motor->port, motor->powers[0], 1);
	}
}

double rotationController(engines_t * motors){
	double rotationError = motors->first.speeds[0] - motors->second.speeds[0];
	return rotationError;
}

double controller (double u_2){
	double gain = 13;
	double y_2 = 1/(-31 * sT -2) * (-4 * y_1 +(-31 * sT + 2) * y_0 + gain * (u_2*(-128*sT*sT - 32*sT -2)+u_1*(4-256*sT*sT)+u_0*(-128*sT*sT+32*sT-2)));
	u_0 = u_1;
	u_1 = u_2;
	y_0 = y_1;
	y_1 = y_2;
	return y_2;
};


double controller2 (double u_2){
	double gain = 13;
	double y_2 = 1/(-31 * sT -2) * (-4 * y_11 +(-31 * sT + 2) * y_01 + gain * (u_2*(-128*sT*sT - 32*sT -2)+u_11*(4-256*sT*sT)+u_01*(-128*sT*sT+32*sT-2)));
	u_01 = u_11;
	u_11 = u_2;
	y_01 = y_11;
	y_11 = y_2;
	return y_2;
};

double evaluate_speed(motor_t * motor, double previousSpeed) {
	double space_rad = (motor->revolutions[0] - motor->revolutions[1]) * (PI / 180.0);
	double speed_rad = space_rad/ (0.002);
	speed_rad = speed_rad * alpha + (1-alpha) * previousSpeed;
	return speed_rad;
}

void initialize_motors(engines_t * motors){
	nxt_motor_set_count(NXT_PORT_A, 1000);
    for(int i = 0; i < SAMP_NUM; i++){
    	motors->first.speeds[i] = 0;
    	motors->first.revolutions[i] = nxt_motor_get_count(NXT_PORT_A);
    	motors->first.times[i] = (long) ecrobot_get_systick_ms();
    }
    for(int j = 0; j < BUFFER_SIZE; j++){
    	motors->first.speed_ref[j] = 0;
    	motors->first.distance_ref[j] = 0;
    }
    
    nxt_motor_set_count(NXT_PORT_B, 1000);
    for(int i = 0; i < SAMP_NUM; i++){
    	motors->second.speeds[i] = 0;
    	motors->second.revolutions[i] = nxt_motor_get_count(NXT_PORT_B);
    	motors->second.times[i] = (long) ecrobot_get_systick_ms();
    }
    for(int j = 0; j < BUFFER_SIZE; j++){
    	motors->first.speed_ref[j] = 0;
    	motors->first.distance_ref[j] = 0;
    }
    
    nxt_motor_set_count(NXT_PORT_C, 1000);
    for(int i = 0; i < SAMP_NUM; i++){
    	motors->third.speeds[i] = 0;
    	motors->third.revolutions[i] = nxt_motor_get_count(NXT_PORT_C);
    	motors->third.times[i] = (long) ecrobot_get_systick_ms();
    }
    for(int j = 0; j < BUFFER_SIZE; j++){
    	motors->third.speed_ref[j] = 0;
    	motors->third.distance_ref[j] = 0;
    }
}
