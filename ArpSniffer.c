#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>

#define BUFFER_SIZE 1600
#define ETHERTYPE 0x0806







int main(int argc, char *argv[])
{
	
	
    int fd;
	unsigned char buffer[BUFFER_SIZE];
	unsigned char *data;
	struct ifreq ifr;
	char ifname[IFNAMSIZ];

	if (argc != 2) {
		printf("Usage: %s iface\n", argv[0]);
		return 1;
	}
	strcpy(ifname, argv[1]);

	/* Cria um descritor de socket do tipo RAW */
	fd = socket(PF_PACKET,SOCK_RAW, htons(ETH_P_ALL));
	if(fd < 0) {
		perror("socket");
		exit(1);
	}

	/* Obtem o indice da interface de rede */
	strcpy(ifr.ifr_name, ifname);
	if(ioctl(fd, SIOCGIFINDEX, &ifr) < 0) {
		perror("ioctl");
		exit(1);
	}

	/* Obtem as flags da interface */
	if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0){
		perror("ioctl");
		exit(1);
	}

	/* Coloca a interface em modo promiscuo */
	ifr.ifr_flags |= IFF_PROMISC;
	if(ioctl(fd, SIOCSIFFLAGS, &ifr) < 0) {
		perror("ioctl");
		exit(1);
	}

	printf("Esperando pacotes ... \n");
	while (1) {

        unsigned short int hw_type;
        unsigned short int pro_type;
        char hlen;
        char plen;
        short int operation;
        unsigned char sender_ha [6];
        unsigned char sender_ip [4]; 
        unsigned char target_ha [6];
        unsigned char target_ip[4];  


		unsigned char mac_dst[6];
		unsigned char mac_src[6];
		short int ethertype;


		/* Recebe pacotes */
		if (recv(fd,(char *) &buffer, BUFFER_SIZE, 0) < 0) {
			perror("recv");
			close(fd);
			exit(1);
		}
        
		/* Copia o conteudo do cabecalho Ethernet */
		memcpy(mac_dst, buffer, sizeof(mac_dst));
		memcpy(mac_src, buffer+sizeof(mac_dst), sizeof(mac_src));
		memcpy(&ethertype, buffer+sizeof(mac_dst)+sizeof(mac_src), sizeof(ethertype));
		ethertype = ntohs(ethertype);
		
		/* Copia o conteudo do cabecalho ARP */
		

		
		memcpy(&hw_type, buffer+sizeof(mac_dst)+sizeof(mac_src)+sizeof(ethertype), sizeof(hw_type));
		hw_type = ntohs(hw_type);


		memcpy(&pro_type, buffer+sizeof(mac_dst)+sizeof(mac_src)+sizeof(ethertype)+ sizeof(hw_type), sizeof(pro_type));
		pro_type = ntohs(pro_type);
		

		memcpy(&hlen, buffer+sizeof(mac_dst)+sizeof(mac_src)+sizeof(ethertype)+ sizeof(hw_type)+ sizeof(pro_type), sizeof(hlen));
		

		memcpy(&plen, buffer+sizeof(mac_dst)+sizeof(mac_src)+sizeof(ethertype)+ sizeof(hw_type)+ sizeof(pro_type)+ sizeof(hlen), sizeof(plen));
		

		memcpy(&operation, buffer+sizeof(mac_dst)+sizeof(mac_src)+sizeof(ethertype)+ sizeof(hw_type)+ sizeof(pro_type)+ sizeof(hlen)+ sizeof(plen), sizeof(operation));
		operation = ntohs(operation);


		memcpy(sender_ha, buffer+sizeof(mac_dst)+sizeof(mac_src)+sizeof(ethertype)+ sizeof(hw_type)+ sizeof(pro_type)+ sizeof(hlen)+ sizeof(plen)+ sizeof(operation), sizeof(sender_ha));
		

		memcpy(sender_ip, buffer+sizeof(mac_dst)+sizeof(mac_src)+sizeof(ethertype)+ sizeof(hw_type)+ sizeof(pro_type)+ sizeof(hlen)+ sizeof(plen)+ sizeof(operation)+ sizeof(sender_ha), sizeof(sender_ip));
		

		memcpy(target_ha, buffer+sizeof(mac_dst)+sizeof(mac_src)+sizeof(ethertype)+ sizeof(hw_type)+ sizeof(pro_type)+ sizeof(hlen)+ sizeof(plen)+ sizeof(operation)+ sizeof(sender_ha)+ sizeof(sender_ip), sizeof(target_ha));
		

		memcpy(target_ip, buffer+sizeof(mac_dst)+sizeof(mac_src)+sizeof(ethertype)+ sizeof(hw_type)+ sizeof(pro_type)+ sizeof(hlen)+ sizeof(plen)+ sizeof(operation)+ sizeof(sender_ha)+ sizeof(sender_ip)+ sizeof(target_ha), sizeof(target_ip));
		

		data = (buffer+sizeof(mac_dst)+sizeof(mac_src)+sizeof(ethertype)+ sizeof(hw_type)+ sizeof(pro_type)+ sizeof(hlen)+ sizeof(plen)+ sizeof(operation)+ sizeof(sender_ha)+ sizeof(sender_ip)+ sizeof(target_ha)+ sizeof(target_ip));

		if (ethertype == ETHERTYPE) {
			printf("MAC destino: %02x:%02x:%02x:%02x:%02x:%02x\n", 
                        mac_dst[0], mac_dst[1], mac_dst[2], mac_dst[3], mac_dst[4], mac_dst[5]);
			printf("MAC origem:  %02x:%02x:%02x:%02x:%02x:%02x\n", 
                        mac_src[0], mac_src[1], mac_src[2], mac_src[3], mac_src[4], mac_src[5]);
			printf("EtherType: 0x%04x\n", ethertype);
			
			printf("CABEÇALHO ARP");
			
			printf("Hardware Type: %04x", ntohs(hw_type));
			printf("Protocol Type: %04x", ntohs(pro_type));
			printf("HLEN: %02x", hlen);
			printf("PLEN: %02x", plen);
			printf("Operation: %04x", operation);
			printf("Hardware Sender: %02x:%02x:%02x:%02x:%02x:%02x\n",
			sender_ha[0], sender_ha[1], sender_ha[2], sender_ha[3], sender_ha[4], sender_ha[5]);
			printf("IP Sender:  %d.%d.%d.%d",
			sender_ip[0], sender_ip[1], sender_ip[2], sender_ip[3]);
			printf("Hardware Target: %02x:%02x:%02x:%02x:%02x:%02x\n", 
			target_ha[0], target_ha[1],target_ha[2],target_ha[3],target_ha[4],target_ha[5]);
			printf("IP Target:  %d:%d:%d:%d",
			target_ip[0], target_ip[1], target_ip[2], target_ip[3]);
			
			printf("Dado: %s\n", data);
			printf("\n");
		}
	}

	close(fd);
	return 0;
}
