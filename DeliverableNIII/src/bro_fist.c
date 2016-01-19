#include "headers/bro_fist.h"
#include "headers/bro_comm.h"
#include "headers/bro_opts.h"

int 
main (int argc, char *argv[])
{
    int bro_client_socket = -1;
    int bro_spam_socket = -1;
    bro_fist_t in_packet[BUFFER_SIZE];
    bro_fist_t out_packet[BUFFER_SIZE];
    int comm_res;
    bro_opts_t options;
    memset (in_packet, 0, sizeof(bro_fist_t) * BUFFER_SIZE);
    memset (out_packet, 0, sizeof(bro_fist_t) * BUFFER_SIZE);
    srand ( time(NULL) );
    if (bro_opts_parse (&options, argc, argv) == -1) {
        return 0;
    };
    
    bro_bt_connect_device (&bro_spam_socket, options.mac);
	
	char s [10];
	printf("PRESS RUN ON THE BRICK...");
	scanf ("%1s",s);
	
	int speed = 0, distance = 0;
	int i = 0;
	do {
		printf("Enter the speed (rad/s, 0 to stop): ");
		scanf ("%5s",s);
		speed = atoi(s);
		printf("Enter the distance (m): ");
		scanf ("%5s",s);
		distance = atoi(s);
		
		// Run the motor with the given speed, per the given distance
		in_packet[i].operation = SET_SPEED;
		in_packet[i].data = speed;
		in_packet[i].data2 = distance;
		in_packet[i].port = MOTOR_B;
		i++;
	} while ((i < BUFFER_SIZE-1) && (speed!=0));

	for (int j = i; j < BUFFER_SIZE; j++){
		in_packet[j].operation = SET_SPEED;
		in_packet[j].data = 0;
		in_packet[j].data2 = 0;
		in_packet[j].port = MOTOR_B;
	}
	 
	comm_res = bro_server_fist(in_packet, out_packet, bro_client_socket, bro_spam_socket);
	
	bro_bt_close_connection(bro_spam_socket);
    
    return 0;
}
