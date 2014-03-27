#include <sys/socket.h> // socket header files
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <netdb.h>
#include <stropts.h>
#include <poll.h>

#define SIZE 	          21000
#define MY_PORT           1400
#define BUFF_SIZE         1024
#define MY_UNSERVED       20
#define BAUDRATE          B19200

//#define DEBUG

struct sockaddr_in current, clients[5];	
int second_socket[5];
int fd;
// set the client pointer to 0
int i = 0;

int first_socket,int_in;

// This procedure will handle the IO recieved from the serial port
int isr_complete = 0;
void serial_ISR(int status)
{
	int l;
	unsigned char fd_buf[BUFF_SIZE];
	int n = read (fd, fd_buf, sizeof fd_buf);  // read up to BUFF_SIZE characters if ready to read
	printf("%s", fd_buf);
	#ifdef DEBUG
	 printf("Recieved %s\n", fd_buf);
	#endif
	for( l = 0; l < i; l++)
	{
		if(sendto(second_socket[l], fd_buf, n, 0, (struct sockaddr *)&clients[l], sizeof(clients[l]))<0)
		{
			perror("Cannot send the data\n");
		}
	}
	close(fd);
	fd = open(portname, O_RDWR | O_NOCTTY | O_NONBLOCK);

}

void RegisterClient(sockaddr_in *address)
{
	clients[i] = *address;
	second_socket[i] = first_socket;
	#ifdef DEBUG
	printf("OK Recieved\n");
	//i++; //i increments the through clients.
	unsigned char *msg = (unsigned char*)"OK";
	int er = sendto(first_socket, msg, sizeof(msg), 0, (struct sockaddr *)&address, sizeof(address));
	#endif

}

int main(void)
{
	struct sockaddr_in in;
	struct termios oldtio,newtio; 
	struct pollfd fds[2];

	static bool registered[5] = {false, false, false, false, false};
	
	// Set up the Socket variables for UDP and bind to the port
	
	if((first_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("Cannot create socket\n");
		return 0;
	}

	memset((char *) &current, 0, sizeof(current));
	current.sin_family = AF_INET;
	current.sin_port = htons(MY_PORT);
	current.sin_addr.s_addr = INADDR_ANY;
        socklen_t len = sizeof(struct sockaddr);
	int kl = bind(first_socket, (struct sockaddr *)&current, sizeof(current));
	if ( kl < 0)
	{
		perror("bind failed");
		return 0;
    }
	
	// here the non blocking flag needs to be set so that polling
	// can be used instead of blocking read calls
	int socket_flags = fcntl( first_socket, F_GETFL, 0);
	socket_flags = socket_flags | O_NONBLOCK;
	fcntl( first_socket , F_SETFL, socket_flags);
	
	
	
	char *portname = "/dev/ttyO1";
	struct sigaction saio;
	fd = open(portname, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0)
	{
		perror("Serial port failed");
			return 1;
	}
	
	saio.sa_handler = serial_ISR;
	//saio.sa_mask = 0;
	saio.sa_flags = 0;
	saio.sa_restorer = NULL;
	sigaction(SIGIO, &saio,NULL);

	fcntl(fd, F_SETOWN, getpid());
	fcntl(fd, F_SETFL, FASYNC);
	socklen_t inlength;
	tcgetattr(fd,&oldtio); /* save current port settings */                    
    /* set new port settings for canonical input processing */                 
    newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;               
    newtio.c_iflag = IGNPAR | ICRNL;                                           
    newtio.c_oflag = 0;                                                    
    newtio.c_lflag = ICANON;                                                   
    newtio.c_cc[VMIN]=1;                                                       
    newtio.c_cc[VTIME]=0;                                                      
    tcflush(fd, TCIFLUSH);                                                     
    tcsetattr(fd,TCSANOW,&newtio);    
	
	fds[0].fd = first_socket;
	fds[0].events = POLLIN;

	//main loop 
	while(1)
	{	
		if(-1 == poll(fds, 1, 0))
		{
			perror("Poll");
		}


		if( fds[0].revents & POLLIN)
		{
			unsigned char buf[BUFF_SIZE];

			socklen_t inlength = sizeof(in);	
			int recvlen = recvfrom(first_socket, buf, BUFF_SIZE, 0, (struct sockaddr *)&in, &inlength);
			#ifdef DEBUG
			printf("%s %d \n", buf, recvlen);
			#endif
			if(buf[recvlen-1] == '\n')
			{
				recvlen--;
			}

			if(recvlen == -1)
			{
				printf("There was an error receiving\n");
			}

			//If it is a new client then publish OK to enter the client list OR UN to remove
			if(recvlen == 2)
			{
				//Add the ip to the list
				if( (buf[0] == 'O') && (buf[1] == 'K') && !registered[i])
				{
					printf("Registered!\n");
					registered[i] = true;
					RegisterClient(&in);
				}
				//Find the ip addr and remove it from the udp list
				else if ( (buf[0] == 'U') && (buf[1] == 'N') && registered[i])
				{
					int j;
					for( j = 0; j < i; j++)
					{
						if(clients[j].sin_addr.s_addr == in.sin_addr.s_addr)
						{
							int k;
							//remove client[j] from the list
							for(k = (j+1); k < i; k++)
							{
								clients[k-1] = clients[k];
							}
							i--;
							break;
						}
					}


				}
				else
				{
					write(fd,buf,2);
				}

			} // if (recvlen == 2)
			else
			{	
				#ifdef DEBUG
				printf("Starting sending\n");
				#endif
				if(recvlen != 0)
				{

					//char buffer [50];

					if(-1 == write(fd, buf, recvlen))
					{
						printf("Error = %i\n", errno);
					}
					else
					{
						printf("Write to XBees succeful\n");
					}

					close(fd);
					fd = open(portname, O_RDWR | O_NOCTTY | O_NONBLOCK);

				}
				else
				{
					printf("Nothing to write\n");
				}
			}
			#ifdef DEBUG
			printf("The loop is over \n");
			#endif
		}
	}

}
