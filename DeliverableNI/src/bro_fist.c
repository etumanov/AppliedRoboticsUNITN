#include "headers/bro_fist.h"
#include "headers/bro_comm.h"
#include "headers/bro_opts.h"

int 
main (int argc, char *argv[])
{
    //int bro_server_socket = -1;
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
    
    //bro_start_server (&bro_server_socket, &bro_client_socket);   

	char asd [2];
	printf("Enter something to start (press RUN before): ");
	scanf ("%1s",asd); 
	
	for (int velocita = 30; velocita < 71 ; velocita++){
		FILE *f;
		char logFileName [50] = "";
		strcat(logFileName, "p_");
		char tmp[15];
		sprintf(tmp, "%d", velocita);
		strcat(logFileName, tmp);
		strcat(logFileName, "_st_2_b_1000.csv");
		f = fopen(logFileName, "w");
		if (f == NULL)
		{
			perror("logFile");
			return -1;
		}

		int packet_no_total = 1;
		int packet_no = 0;
		float previousCounter = 0;
		long previousTime = 0;
		long initialTime = 0;
		long currentTime[packet_no_total * BUFFER_SIZE];
		double speed[packet_no_total * BUFFER_SIZE];
		float input[packet_no_total * BUFFER_SIZE];
		float counter[packet_no_total * BUFFER_SIZE];

		for(int i = 0 ; i < BUFFER_SIZE; i++){
			if(i < 3 || i ==BUFFER_SIZE-1)
					input[i] = 0;
				else
					input[i] = velocita;
		}

		do {
			packet_no++;
			for (int i = 0; i < BUFFER_SIZE; i++){
				in_packet[i].operation = SET_POWER;
				in_packet[i].port = MOTOR_B;
				in_packet[i].data = input[i];
				//printf("pacchetto %d: %d - %d - %f\n", i, in_packet[i].operation, in_packet[i].port, in_packet[i].data);
			}

			comm_res = bro_server_fist(in_packet, out_packet, bro_client_socket, bro_spam_socket);

			for (int i = 0; i < BUFFER_SIZE; i++) {
				if(i == 0)
					initialTime = out_packet[i].time;
				currentTime[i] = out_packet[i].time - initialTime;
				//printf("Istruzione %i:\n", i+1);
				//printf("\tOperazione: %i\n", in_packet[i].operation);
				//printf("\tPorta: %i\n", in_packet[i].port);
				//printf("\tDatetime: %li\n", currentTime[i]);
				//printf("\tDati: %.2f\n", in_packet[i].data);
				//printf("\tDati settati: %.2f\n", out_packet[i].data);
				
				if(currentTime[i] - previousTime == 0)
					speed[i] = 0;
				else
					speed[i] = (out_packet[i].data - previousCounter) / (double)(currentTime[i] - previousTime);
				counter[i]=out_packet[i].data;
				//printf("\tSpeed: %f\n", speed[i]);
				previousCounter = out_packet[i].data;
				previousTime = currentTime[i];
			}
		} while ((in_packet[0].operation != BRO_END_COMMUNICATION) && (comm_res>= 0) && (packet_no < packet_no_total));
		
		printf("Velocita corrente %i!\n", velocita);

		for (int j = 0; j < BUFFER_SIZE-1 ; j++)
			fprintf(f, "%li,%f,%f,%f\n", currentTime[j], input[j],counter[j], speed[j]);
		fclose(f);
	}

	bro_bt_close_connection(bro_spam_socket);
    
	//bro_stop_server(bro_server_socket, bro_client_socket);
    
    return 0;
}
