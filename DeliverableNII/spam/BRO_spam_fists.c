#include "headers/BRO_spam_fists.h"

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
        default:    //  Only for future upgrades
            break;
    };
}

//  :3 Listen, here we will write the function that will decode only ONE order and will return some data, IF it's a sensors packet...
static void decode_bro_input (bro_fist_t * input_packet, engines_t * motors )
{
    U8 temp_port;
    
    output_packet->port = input_packet->port;
    output_packet->operation = input_packet->operation;
    
    switch (input_packet->operation) {
        case LIGHT_SENSOR:
            decode_bro_port (input_packet->port, &temp_port);
            input_packet->data = (float) ecrobot_get_light_sensor(temp_port);
            break;
            
        case TOUCH_SENSOR:
            decode_bro_port (input_packet->port, &temp_port);
            input_packet->data = (float) ecrobot_get_touch_sensor(temp_port);
            break;
            
        case SOUND_SENSOR:
            decode_bro_port (input_packet->port, &temp_port);
            input_packet->data = (float) ecrobot_get_sound_sensor(temp_port);
            break;
            
        case RADAR_SENSOR:
            decode_bro_port (input_packet->port, &temp_port);
            input_packet->data = (float) ecrobot_get_sonar_sensor(temp_port);
            break;
            
        case TACHO_COUNT:
            decode_bro_port (input_packet->port, &temp_port);
            input_packet->data = (float) nxt_motor_get_count(temp_port);
            break;
 
       case SET_POWER:
            decode_bro_port (input_packet->port, &temp_port);
            switch (temp_port) {
                case NXT_PORT_A:
                    motors->first.speed_control_type = RAW_POWER;
                    motors->first.powers[0] = input_packet->data;
					input_packet->data = (float) nxt_motor_get_count(temp_port);
					input_packet->time = (long) ecrobot_get_systick_ms();
                break;
                
                case NXT_PORT_B:
                    motors->second.speed_control_type = RAW_POWER;
                    motors->second.powers[0] = input_packet->data;
					input_packet->data = (float) nxt_motor_get_count(temp_port);
					input_packet->time = (long) ecrobot_get_systick_ms();
                break;
                
                case NXT_PORT_C:
                    motors->third.speed_control_type = RAW_POWER;
                    motors->third.powers[0] = input_packet->data;
					input_packet->data = (float) nxt_motor_get_count(temp_port);
					input_packet->time = (long) ecrobot_get_systick_ms();
                break;
            };
			
            break;
		case SET_SPEED:
            motors->first.speed_control_type = PID_CONTROLLED;
            motors->first.speed_ref = input_packet->data;
            break;
        default:
            //Nothing HERE
            break;
    };
}


//  Function to decode a LIST of FISTS and do things accordingly... :3 
void decode_bro_fists (bro_fist_t * orders, engines_t * motors)
{
    for (int i = 0; i < BUFFER_SIZE; i ++) {
        decode_bro_input (&orders[i], motors);    
    };
};

double controller (double u_2){
	double gain = 13;
	double y_2 = 1/(-31 * sT -2) * (-4 * y_11 +(-31 * sT + 2) * y_01 + gain * (u_2*(-128*sT*sT - 32*sT -2)+u_11*(4-256*sT*sT)+u_01*(-128*sT*sT+32*sT-2)));
	u_01 = u_11;
	u_11 = u_2;
	y_01 = y_11;
	y_11 = y_2;
	return y_2;
}

double evaluate_speed(motor_t * motor, double previousSpeed) {
	double space_rad = (motor->revolutions[0] - motor->revolutions[1]) * (PI / 180.0);
	double speed_rad = space_rad/ (0.002);
	speed_rad = speed_rad * alpha + (1-alpha) * previousSpeed;
	return speed_rad;
}

void updateMotors(engines_t * motors){
	motor_t *motor;
    motor_t *otherMotor;
	motor = &(motors->first); 
	otherMotor = &(motors->second);
	
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
		double error = motor->speed_ref[0] - motor->speeds[0];
		motor->powers[0] = controller(error);
	}
	
	nxt_motor_set_speed(motor->port, motor->powers[0], 1);
}