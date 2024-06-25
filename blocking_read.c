	/* ===================================================================================================*/
    /* Serial Port Programming in C (Serial Port Read)                                                    */
	/* Non Cannonical mode                                                                                */
	/*----------------------------------------------------------------------------------------------------*/
    /* Program reads a string from the serial port at 9600bps 8N1 format                                  */
	/* Baudrate - 9600                                                                                    */
	/* Stop bits - 1                                                                                       */
	/* No Parity                                                                                          */
    /*----------------------------------------------------------------------------------------------------*/
	
	/*-------------------------------------------------------------*/
    /* termios structure -  /usr/include/asm-generic/termbits.h    */ 
	/* use "man termios" to get more info about  termios structure */
	/*-------------------------------------------------------------*/

    	#include <stdio.h>
		#include <string.h>
    	#include <fcntl.h>   /* File Control Definitions           */
    	#include <termios.h> /* POSIX Terminal Control Definitions */
    	#include <unistd.h>  /* UNIX Standard Definitions 	   	   */ 
    	#include <errno.h>   /* ERROR Number Definitions           */
    	#include <stdlib.h>
		
	void main(void)
    	{
        	int fd;			/*File Descriptor*/
		
		printf("\n +----------------------------------+");
		printf("\n |        Serial Port Read          |");
		printf("\n +----------------------------------+");

		/*------------------------------- Opening the Serial Port -------------------------------*/

        	fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY);	/* ttyUSB0 is the FT232 based USB2SERIAL Converter   */
			   												/* O_RDWR   - Read/Write access to serial port       */
															/* O_NOCTTY - No terminal will control the process   */
															/* Open in blocking mode,read will wait              */
									
									                                        
       if(fd < 0) {											/* Error Checking */
			printf("\n  Error! in Opening ttyUSB0  ");
			exit(0);
		} 
		printf("\n  ttyUSB0 Opened Successfully ");
	
		/*------------------ Setting the Attributes of the serial port using termios structure ----------------- */
		
		struct termios SerialPortSettings;						/* Create the structure                          */
		tcgetattr(fd, &SerialPortSettings);						/* Get the current attributes of the Serial port */

		/* Setting the Baud rate */
		cfsetispeed(&SerialPortSettings,B9600); 				/* Set Read  Speed as 9600                       */
		cfsetospeed(&SerialPortSettings,B9600); 				/* Set Write Speed as 9600                       */

		/* 8N1 Mode */
		SerialPortSettings.c_cflag &= ~PARENB;   				/* Disables the Parity Enable bit(PARENB),So No Parity   */
		SerialPortSettings.c_cflag &= ~CSTOPB;   				/* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
		SerialPortSettings.c_cflag &= ~CSIZE;	 				/* Clears the mask for setting the data size             */
		SerialPortSettings.c_cflag |=  CS8;      				/* Set the data bits = 8                                 */
		SerialPortSettings.c_cflag &= ~CRTSCTS;       			/* No Hardware flow Control                         	 */
		SerialPortSettings.c_cflag |= CREAD | CLOCAL; 			/* Enable receiver,Ignore Modem Control lines       	 */ 
		SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);  /* Disable XON/XOFF flow control both i/p and o/p 	 	 */
		SerialPortSettings.c_lflag &= ~(ICANON | ECHO | ECHOE); /* Non Cannonical mode                            		 */
		SerialPortSettings.c_lflag &= ~(ISIG);
		SerialPortSettings.c_oflag &= ~OPOST;					/*No Output Processing									 */
		
		/* Setting Time outs */
		SerialPortSettings.c_cc[VMIN] = 0; 								
		SerialPortSettings.c_cc[VTIME] = 10; 							/* Wait for up to 1s (10 deciseconds), returning as soon as any data is received.*/

		if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0) 			/* Set the attributes to the termios structure*/
		    printf("\n  ERROR ! in Setting attributes");
		else
            printf("\n  BaudRate = 9600\n  StopBits = 1 \n  Parity   = none\n\n");
			
		
	        /*------------------------------- Write init message to serial port -----------------------------*/
			
			char init_msg[]={":ADR01;"};								/* Init message */
			write(fd, init_msg, sizeof(init_msg));						/* Write init message */

			 /*------------------------------- Variables for operation messages ------------------------------*/
			char *msg;
			char read_buffer[80];  
			//char *str0 = msg; 										
			//int  bytes_read = 0;    									
			//int num_bytes;

			/*--------------------------------- Write / read operation messages ------------------------------*/
			while(1){
				printf("  Type the command: ");
				scanf("%s", msg);
				write(fd, msg, sizeof(msg));
				tcflush(fd, TCIFLUSH);   												
				int num_bytes = read(fd, &read_buffer, sizeof(read_buffer));
				printf("Read %i bytes. Received message: \n%s\n", num_bytes, read_buffer);
				write(fd, init_msg, sizeof(init_msg));
			}
		
		close(fd); 														/* Close the serial port */
		}
