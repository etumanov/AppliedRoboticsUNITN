#include <stdio.h>
#include "headers/BRO_spam_fists.h"
#include "headers/BRO_spam_client.h"

/*--------------------------------------------------------------------------*/
/* OSEK declarations                                                        */
/*--------------------------------------------------------------------------*/
DeclareCounter(SysTimerCnt);
DeclareResource(lcd);
DeclareTask(BRO_Comm);
DeclareTask(PID_Controller);
DeclareTask(DisplayTask);

engines_t engines;
int offset;
const double PI = 3.141592653589;


/*--------------------------------------------------------------------------*/
/* LEJOS OSEK hooks                                                         */
/*--------------------------------------------------------------------------*/


void ecrobot_device_initialize()
{
    ecrobot_init_bt_slave("1234");
    
    memset(&engines, 0, sizeof(engines_t));

    engines.first.port = NXT_PORT_A;
    engines.first.speed_control_type = NOT_USING;

    engines.second.port = NXT_PORT_B;
    engines.second.speed_control_type = NOT_USING;
    engines.second.speed_ref[0] = 0;

    engines.third.port = NXT_PORT_C;
    engines.third.speed_control_type = NOT_USING;
    engines.third.speed_ref[0] = 0;

    if (CONN_SONAR) {
        ecrobot_init_sonar_sensor(SONAR_PORT);
    };
    if (CONN_LIGHT) {
        ecrobot_set_light_sensor_active(LIGHT_PORT);
    };
}


void ecrobot_device_terminate()
{
  
    memset(&engines, 0, sizeof(engines_t));

    nxt_motor_set_speed(NXT_PORT_A, 0, 1);
    nxt_motor_set_speed(NXT_PORT_B, 0, 1);
    nxt_motor_set_speed(NXT_PORT_C, 0, 1);
        
    ecrobot_set_light_sensor_inactive(LIGHT_PORT);
    ecrobot_term_sonar_sensor(SONAR_PORT);

    bt_reset();

    ecrobot_term_bt_connection();
}

/*--------------------------------------------------------------------------*/
/* Function to be invoked from a category 2 interrupt                       */
/*--------------------------------------------------------------------------*/
void user_1ms_isr_type2(void)
{
    StatusType ercd;

    /*
     *  Increment OSEK Alarm System Timer Count
    */
    ercd = SignalCounter( SysTimerCnt );
    if ( ercd != E_OK ) {
        ShutdownOS( ercd );
    }
}

/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : PID_Controller                                                 */
/* Priority: 3                                                              */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: preemptive                                                     */
/*--------------------------------------------------------------------------*/
TASK(PID_Controller)
{
    updateMotors(&engines);
    TerminateTask();
}

TASK(BRO_Comm)
{

			
    U32 connect_status = 0;
    
    /*  Declaring two buffers for communication */
    bro_fist_t in_packet[BUFFER_SIZE];
    bro_fist_t out_packet[BUFFER_SIZE];
    
    memset (in_packet, 0, sizeof(bro_fist_t) * BUFFER_SIZE);
    memset (out_packet, 0, sizeof(bro_fist_t) * BUFFER_SIZE);

    /*  As you might know we have a problem here... :3
     *  That problem is that the BT device installed on the AT91SAM7 seems to
     *  have some speed problems with the receiving for the first data via BT.
     *  It needs ~40ms to get every kind of data (Even a uint32) so we will have
     *  to use some kind of pooling (not really pooling, because it would lock
     *  the NXT, mind you ;) ) and work on the data received only when some
     *  data, usually all of it (I won't tell you to read the device drivers
     *  for BlueTooth written for nxtOSEK, but the ecrobot_read_bt_packet checks
     *  if all the data declared in the first byte is present in the device's
     *  buffer.
     *
     *  No problem, with our drill we will pierce the Heavens!
     *  (And also with our BROFists, right?)
     */
	
    //connect_status = ecrobot_read_bt(in_packet, 0, sizeof(bro_fist_t)*BUFFER_SIZE);
	connect_status = ecrobot_read_bt_packet((U8*) in_packet, sizeof(bro_fist_t)*BUFFER_SIZE);
		
    // Se sono arrivati dei dati...
    if (connect_status > 0) {
		offset = offset + connect_status;
        // Decodifica ed elabora i pacchetti ricevuti
        decode_bro_fists (in_packet, out_packet, &engines);
        // Invia la risposta
        bt_send((U8*)out_packet, sizeof(bro_fist_t)*BUFFER_SIZE);
    }

    TerminateTask();
}

TASK(DisplayTask)
{
	display_clear(0);
	display_goto_xy(0,1);
	display_string("Spd(rad/s):");
	display_goto_xy(10,1);
    display_int(engines.first.speeds[0], 3);
    display_goto_xy(13,1);
    display_int(engines.second.speeds[0], 3);
	display_goto_xy(0,2);
	display_string("Dist(dm):");
	double distanceFirst = ((engines.first.revolutions[0]) * PI/180) * 0.028 * 10;
	double distanceSecond = ((engines.second.revolutions[0]) * PI/180) * 0.028 * 10;
	display_goto_xy(10,2);
	display_int(distanceFirst, 3);
	display_goto_xy(13,2);
	display_int(distanceSecond, 3);

	display_goto_xy(0,4);
	display_string("Total(dm):");
	double distance = (((engines.first.revolutions[0] + engines.second.revolutions[0])/2.0) * PI/180) * 0.028 * 10;
	display_goto_xy(10,4);
	display_int(distance, 3);
	display_goto_xy(0,5);
	display_string("w:");
	double omega = ((engines.first.revolutions[0] - engines.second.revolutions[0])/0.18) * 0.028;
	display_goto_xy(10,5);
	display_int(omega, 3);
	display_update();
    TerminateTask();
}
