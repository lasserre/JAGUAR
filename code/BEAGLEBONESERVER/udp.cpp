#include <sys/socket.h> // socket header files
#include <netinet/in.h>
#include <netinet/udp.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define SIZE 	          21000
#define MY_PORT           1400
#define BUFF_SIZE         1024
#define MY_UNSERVED       20

struct sockaddr_in current, clients[5];	
int second_socket[5];
unsigned char buf[BUFF_SIZE];
int fd;
// set the client pointer to 0
int i = 0;

// This procedure will handle the IO recieved from the serial port
int isr_complete = 0;
void serial_ISR(int status)
{
	int n = read (fd, buf, sizeof buf);  // read up to 100 characters if ready to read
	for(int l = 0; l < i; l++)
	{
		if(sendto(second_socket[l], buf, sizeof(buf), 0, (struct sockaddr *)&clients[l], sizeof(clients[l]))<0)
		{
			perror("Cannot send the data\n");
		}
	}
}

int main(void)
{
	struct sockaddr_in current, clients[5], in;	
	int first_socket,int_in;
    int second_socket[5];
	int bind;
	
	if((first_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("Cannot create socket\n");
		return 0;
	}
	
	current.sin_family = AF_INET;
	current.sin_port = htons(MY_PORT);
	current.sin_addr.s_addr = INADDR_ANY;
	int kl = bind(first_socket, (struct sockaddr *)&current, sizeof(struct sockaddr_in));
	if ( kl < 0) 
	{
		perror("bind failed");
		return 0;
    }
	
	char *portname = "/dev/ttyO1";
	struct sigaction saio;
	fd = open(portname, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0)
	{
		perror("Serial port cannot be opened");
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
	
	//main loop 
	while(1)
	{
	
		int recvlen = recvfrom(int_in, buf, BUFF_SIZE, 0, (struct sockaddr *)&in, &inlength);
		//If it is a new client then publish OK to enter the client list OR UN to remove
		if(recvlen == 2)
		{
			//Add the ip to the list
			if( (buf[0] == 'O') &&(buf[1] == 'K'))
			{
				clients[i] = in;
				second_socket[i] = int_in;
				i++;
				break;
			}
			//Find the ip addr and remove it from the udp list
			if( (buf[0] == 'U') &&(buf[1] == 'N'))
			{
				for(int j = 0; j < i; j++)
				{
					if(clients[j].sin_addr.s_addr == in.sin_addr.s_addr)
					{
						//remove client[j] from the list
						for(int k = (j+1); k < i; k++)
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
				break;
			}
		}
		else
		{
			write(fd,buf,sizeof(buf));
		}
	}
}