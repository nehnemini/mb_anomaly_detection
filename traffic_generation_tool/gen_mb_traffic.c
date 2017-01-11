/* National Autonomous University of Mexico */
/* Paulo Contreras Flores  */
/* paulo.contreras.flores@gmail.com  */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>       // for size in data file
#include <netinet/in.h>
#include <netdb.h> 		    // for hostent structure and gethostbyname function       
#include <netinet/ip.h>
#include <time.h>
#include <string.h>


/* use first command: ip addr add saddr dev eth0
to spoof IP address */

/* ./gen_mb_traffic saddr daddr sport func_code unit_id mb_data_file */



/* struct for the Modbus ADU, Modbus Application Data Unit or
better know as Modbus TCP/IP Message. 
See Modbus messaging on TCP/IP implementation guide v1.0b */
typedef struct _modbus_adu modbus_adu;
struct _modbus_adu {
  uint16_t transid;     // Transaction Identifier - 2 bytes
  uint16_t protoid;     // Protocol Identifier - 2 bytes
  uint16_t lenfield;    // Length Field - 2 bytes
  uint8_t  unitid;      // Unit Identifier - 1 byte
  uint8_t  funcode;	// Function Code - 1 bytes
  uint8_t  data[255];	// Function Data - n bytes
};


/* struct for source and destination IP address, and ports */
typedef struct _args args;
struct _args {
  uint32_t saddr; 
  struct   hostent *daddr;     // structure for server information
  uint16_t sport;
  uint16_t dport;
};

#define verbose 1 

#define MAXDATASIZE 100   



int main(int argc, char *argv[])
{

  /* Check number of arguments */
  if (argc != 7) { 
    printf("Use: %s saddr daddr func_code unit_id data_file \n",argv[0]);
    exit(-1);
  }


  /* Check function code argument */ 
  int src_port = strtol(argv[3], NULL, 10);
  if(src_port < 0 || src_port > 65535 ){
    printf("Error: Source Port must be beetwen 0 and 65535\n");
    exit(-1);
  }


  /* Check function code argument */ 
  int funcode = strtol(argv[4], NULL, 10);
  if(funcode < 0 || funcode > 255 ){
    printf("Error: Function code must be beetwen 0 and 255\n");
    exit(-1);
  }


  /* Check unit id argument */ 
  //int unitid = 2;
  int unitid = strtol(argv[5], NULL, 10);
  if(unitid < 0 || unitid > 255 ){
    printf("Error: Unit identifier must be beetwen 0 and 255\n");
    exit(-1);
  }


  /* Check and open data file argument */
  FILE *fd;
  if((fd = fopen(argv[6],"rb")) == NULL){  // read in binary format
    printf("Error: Can't open input file %s\n",argv[6]);
    exit(-1);
  }

  /* Get size of data file for function code data */
  struct stat st;
  stat("func_data.bin", &st);
  unsigned char data[st.st_size];




  unsigned int socketfd,    // socket descriptor
               rxbytes,     // number of bytes received
               txbytes,     // number of bytes transmited  
               i, j = 0;        
              

  args a;     // struct for source and destination ip, and port

  struct sockaddr_in clientaddr;   // sockaddr structure for Modbus client (or master)
  struct sockaddr_in servaddr;     // sockaddr structure for Modbus server (or slave)


  char saddr[INET_ADDRSTRLEN];    // just for print saddr




  /* Convert the saddr from argv[1] and put on clientdaddr.sin_addr.s_addr
  in binary format */
  if((inet_pton(AF_INET, argv[1], &(clientaddr.sin_addr.s_addr))) < 1){
    printf("Error: IP address %s is invalid\n", argv[1]);
    exit(-1);
  }

  /* Get the daddr returned from gethostbyname function */
  if ((a.daddr=gethostbyname(argv[2]))==NULL){       
    printf("Error: IP address %s is invalid\n", argv[2]);
    exit(-1);
  }

  /* Assign ports */
  // a.sport = 0;     // any local port
  a.sport = src_port;  // port to spoof
  a.dport = 502;   // standard port for Modbus servaddr (or slave)




  /* Create socket descriptor */
  if ((socketfd=socket(AF_INET, SOCK_STREAM, 0))==-1){  
      printf("Error: unable to create socket\n");
      exit(-1);
  }

  /* Bind to a specific network interface (this is unusual, the os normally
  assign the saddr associated to the network interface, and assign a "random" 
  port, but for this program is necesary spoof the saddr and maybe put 
  manually the sport */
  clientaddr.sin_family = AF_INET;
  /* clientaddr.sin_addr.s_addr = inet_addr("192.168.1.65"); */
  clientaddr.sin_port = htons(a.sport);         // convert to network byte order (big endian)
  bind(socketfd, (struct sockaddr *)&clientaddr, sizeof(clientaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(a.dport);           // convert to network byte order (big endian)
  servaddr.sin_addr = *((struct in_addr *)a.daddr->h_addr); // cast hostent structure to in_addr structure
  bzero(&(servaddr.sin_zero),8);

  /* Connect to server */
  if(connect(socketfd, (struct sockaddr *)&servaddr,
    sizeof(struct sockaddr))==-1){ 
    printf("Error: unable to connect to server\n");
    exit(-1);
  }




  unsigned char *buf_request = (unsigned char *)malloc(65536);
  modbus_adu *mdhdr_request = (struct modbus_adu *)buf_request; // cast buf_request to modbus_adu



  /* Modbus ADU  = Header + PDU (Protocol Data Unit) */

  /* Prepare Modbus PDU */
  fread(data,st.st_size,1,fd);      // read n bytes for data file
  for(i = 0; i<st.st_size; i++){
    mdhdr_request->data[i] = data[i];}

  mdhdr_request->funcode = funcode;


  /* Prepare Modbus Header */  
  srand(time(NULL));
  int r = rand() % 1024;
  mdhdr_request->transid = htons(r + j);
  mdhdr_request->protoid = htons(0x0000);     // Set 0, for potencial future use
  mdhdr_request->lenfield = htons(0x0001 + 0x0001 + st.st_size);  // unitid + funcode + data
  //mdhdr_request->unitid = 0xFF;   // for Modbus TCP/IP this value is set to 0xFF
  mdhdr_request->unitid = unitid;   // for spoof the PLC




  for (j = 1; j<3300; j++ ){

    if((txbytes = send(socketfd, mdhdr_request, 6 + ntohs(mdhdr_request->lenfield), 0)) == -1){  
      printf("Error in send() \n");
      exit(-1);
    }

    if(verbose){
      inet_ntop(AF_INET, &(clientaddr.sin_addr.s_addr), saddr, INET_ADDRSTRLEN);
      printf("Source IP: %s\n", saddr);
      printf("Modbus ADU bytes transmited: %i\n", txbytes); 
      printf("transid protoid lenfield unitid funcode data(%i bytes)\n", st.st_size);		
      printf("%04x    %04x    %04x     %02x     %02x      ", 
		ntohs(mdhdr_request->transid), 
		ntohs(mdhdr_request->protoid), 
		ntohs(mdhdr_request->lenfield), 
		mdhdr_request->unitid,
		mdhdr_request->funcode);
      for(i = 0; i<(ntohs(mdhdr_request->lenfield) - 2); i++)
        printf("%02x", mdhdr_request->data[i]);
      printf("\n-----------------------------------------------------\n"); 
    } 



    unsigned char *buf_response = (unsigned char *)malloc(65536);
    modbus_adu *mdhdr_response = (struct modbus_adu *)buf_response; // cast buf_response to modbus_adu

    if((rxbytes=recv(socketfd,buf_response,MAXDATASIZE,0)) == -1){  
      printf("Error en recv() \n");
      exit(-1);
     }

    if(verbose){  
      printf("Modbus ADU bytes received: %i\n",rxbytes); 
      printf("transid protoid lenfield unitid funcode data(%i bytes)\n", sizeof(&mdhdr_response->lenfield));		
      printf("%04x    %04x    %04x     %02x     %02x      ", 
		ntohs(mdhdr_response->transid), 
		ntohs(mdhdr_response->protoid), 
		ntohs(mdhdr_response->lenfield), 
		mdhdr_response->unitid,
		mdhdr_response->funcode); 
      for(i = 0; i<(ntohs(mdhdr_response->lenfield) - 2); i++)
        printf("%02x", mdhdr_response->data[i]);
      //printf("\n-----------------------------------------------------\n"); 
    }

    /* Check and increases de transaction id */
    if(j == 65535){j = 0;}
    mdhdr_request->transid = htons(r + j);

    /* This avoid the flooding and related variables are less predictable, 
       avoid super variables. This analysis is for not detect flooding attacks, 
       is for other types of attacks. For flooding use sleep(0) */
    int secs = rand() % 2;
    //printf ("\n\nsleep %i seconds", secs);
    printf("\n-----------------------------------------------------\n"); 
    sleep(secs);
  }
   close(socketfd);

}
