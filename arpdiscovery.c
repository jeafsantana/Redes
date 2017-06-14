#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>

#define MAC_ADDR_LEN 6
#define BUFFER_SIZE 1600
#define MAX_DATA_SIZE 1500



 struct arp
    {
      unsigned char target_ethernet_address[6];
	  unsigned char source_ethernet_address[6];
	  unsigned short ethernet_type;
      short int hw_type;
      short int pro_type;
      char hlen;
      char plen;
      short int operation;
      unsigned char sender_ha [6];
      unsigned char sender_ip [4]; 
      unsigned char target_ha [6];
      unsigned char target_ip[4];  
    };


pthread_t thread[2];
char ifname[IFNAMSIZ];


void *ARPrequest()
{

    struct arp arp_dados;
	int arp_socket,optval,ifreq_socket;
    struct ifreq ifr;
   	struct sockaddr sa;

 /* Cria um descritor de socket do tipo RAW */
	if ((ifreq_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1)
	{
		perror("Erro ao tentar criar o socket!");
		return (void *)-1;
	}

	/* Obtem o endereco MAC da interface local */
	strcpy(ifr.ifr_name, ifname);
	if (ioctl(ifreq_socket, SIOCGIFHWADDR, &ifr) < 0)
	{
		perror("Erro ao tentar obter MAC de origem!");
		return (void *)-1;
	}

	/* Obtem o endereco IP da interface local */
	if (ioctl(ifreq_socket, SIOCGIFADDR, &ifr) < 0)
	{
		perror("ERROR ao tentar obter IP de origem!");
		return (void *)-1;
	}

    memcpy(&arp_dados.source_ethernet_address, &ifr.ifr_hwaddr.sa_data, 6);
	memcpy(&arp_dados.sender_ha, &ifr.ifr_hwaddr.sa_data, 6);
	memcpy(&arp_dados.sender_ip, &ifr.ifr_hwaddr.sa_data[2], 4);

    close(ifreq_socket);

     /* ARP */
    
     arp_dados.ethernet_type = htons(0x806);
     arp_dados.hw_type = htons(1);
	 arp_dados.pro_type = htons(0x0800);
	 arp_dados.operation = htons(1);
	 arp_dados.hlen = 6;
	 arp_dados.plen = 4;
	 memset(&arp_dados.target_ha, 0x00, 6);
	 memcpy(&arp_dados.target_ip, &arp_dados.sender_ip, 3);



        int i = 1;
	    while (i < 255)
    	{
	    	/** address variation **/
	    	arp_dados.target_ip[3] = i;
    
	    	/** socket arp Internet Protocol v4 **/
	    	if ((arp_socket = socket(AF_INET, SOCK_PACKET, htons(0x806))) < 0)
	    	{
	    		perror("ERROR ao abrir o socket");
	    		return (void *)-1;
	    	}
    
	    	/** options on socket arp **/
	    	/** SO_BROADCAST mensagem broadcast. **/
	    	if (setsockopt(arp_socket, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) < 0)
	    	{
	    		perror("ERROR ao abrir o socket");
	    		return (void *)-1;
	    	}

	    	memset(&sa, 0x00, sizeof(sa));
	    	strcpy(sa.sa_data, ifname);
    
	    	/** sending data **/
	    	if (sendto(arp_socket, &arp_dados, sizeof(arp_dados), 0, (struct sockaddr *)&sa, sizeof(sa)) < 0)
	    	{
	    		perror("ERROR ao enviar o pacote");
	    		return (void *)-1;
	    	}
    
	    	i++;

	    	close(arp_socket); /** fim da conexão **/
	    }
}


void *ARPreplies() 
{

	int s, n, i;
	struct arp arp_dados;
	struct sockaddr sa;
	unsigned char source_hardware_address[6];
	unsigned char source_protocol_address[4];


    if ((s = socket(AF_INET, SOCK_PACKET, htons(0x806))) < 0)
	{
		perror("ERROR ao abrir o socket");
		return (void *)-1;
	}

	i = 0;
	while(1)
	{
		memset(&sa, 0x00, sizeof(sa));
		memset(&arp_dados, 0x00, sizeof(arp_dados));
		n = sizeof(sa);
		
		/** buffer address recinving **/
		
		if (recvfrom(s, &arp_dados, sizeof(arp_dados), 0, (struct sockaddr *)&sa, &n) < 0)
		{
			perror("ERROR ao receber o pacote");
			return (void *)-1;
		}
		if ((ntohs(arp_dados.operation) == ARPOP_REPLY))
		{
			i++;

			memcpy(source_hardware_address, &arp_dados.sender_ha, 6);
			memcpy(source_protocol_address, &arp_dados.sender_ip, 4);

			printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
				   source_hardware_address[0],
				   source_hardware_address[1],
				   source_hardware_address[2],
				   source_hardware_address[3],
				   source_hardware_address[4],
				   source_hardware_address[5]);

			printf("IP: %u.%u.%u.%u\n",
				   source_protocol_address[0],
				   source_protocol_address[1],
				   source_protocol_address[2],
				   source_protocol_address[3]);
		}

    }

}



int main(int argc, char *argv[])
{
	
    if (argc != 2)
	    {
	    	printf("IP: %s iface\n", argv[0]);
	    	return 1;
	    }
    	strcpy(ifname, argv[1]);

	    int err;
	    err = pthread_create(&(thread[0]), NULL, &ARPrequest, NULL);
	    if (err != 0)
	    	printf("\n error thread :[%s]", strerror(err));
	    else
	    	printf("\n--Sending Packet--\n");
    
	    err = pthread_create(&(thread[1]), NULL, &ARPreplies, NULL);
	    if (err != 0)
	    	printf("\n error thread create :[%s]", strerror(err));
	    else
	    	printf("\n--Receiving packets--\n");
    
	    sleep(30);

	
	
}
