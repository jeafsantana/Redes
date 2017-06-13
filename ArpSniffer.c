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
	
	 struct arp
    {
      short int hw_type;
      short int pro_type;
      char hlen;
      char plen;
      short int operation;
      unsigned char sender_ha [6];
      unsigned char sender_ip [4] 
      unsigned char target_ha [6];
      unsigned char target_ip[4];  
    };
	
	int fd;
	unsigned char buffer[BUFFER_SIZE];
	unsigned char *data;
	struct ifreq ifr;
	struct arp arp_dados;
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
		unsigned char mac_dst[6];
		unsigned char mac_src[6];
		short int ethertype;
		int distancia;

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
		
		distancia = buffer+sizeof(mac_dst)+sizeof(mac_src)+sizeof(ethertype);
		
		memcpy(&arp_dados.hw_type, distancia, sizeof(arp_dados.hw_type));
		arp_dados.hw_type = ntohs(arp_dados.hw_type);

		distancia = distancia + sizeof(arp_dados.hw_type);
		memcpy(&arp_dados.pro_type, distancia, sizeof(arp_dados.pro_type));
		arp_dados.pro_type = ntohs(arp_dados.pro_type);
		
		distancia = distancia + sizeof(arp_dados.pro_type);
		memcpy(arp_dados.hlen, distancia, sizeof(arp_dados.hlen));
		
		distancia = distancia + sizeof(arp_dados.hlen);
		memcpy(arp_dados.plen, distancia, sizeof(arp_dados.plen));
		
		distancia = distancia + sizeof(arp_dados.plen);
		memcpy(&arp_dados.operation, distancia, sizeof(arp_dados.operation));
		arp_dados.operation = ntohs(arp_dados.operation);

		distancia = distancia + sizeof(arp_dados.operation);
		memcpy(arp_dados.sender_ha, distancia, sizeof(arp_dados.sender_ha));
		
		distancia = distancia + sizeof(arp_dados.sender_ha);
		memcpy(arp_dados.sender_ip, distancia, sizeof(arp_dados.sender_ip));
		
		distancia = distancia + sizeof(arp_dados.sender_ip);
		memcpy(arp_dados.target_ha, distancia, sizeof(arp_dados.target_ha));
		
		distancia = distancia + sizeof(arp_dados.target_ha);
		memcpy(arp_dados.target_ip, distancia, sizeof(arp_dados.target_ip));
		
		distancia = distancia + sizeof(arp_dados.target_ip);
		data = (distancia);

		if (ethertype == ETHERTYPE) {
			printf("MAC destino: %02x:%02x:%02x:%02x:%02x:%02x\n", 
                        mac_dst[0], mac_dst[1], mac_dst[2], mac_dst[3], mac_dst[4], mac_dst[5]);
			printf("MAC origem:  %02x:%02x:%02x:%02x:%02x:%02x\n", 
                        mac_src[0], mac_src[1], mac_src[2], mac_src[3], mac_src[4], mac_src[5]);
			printf("EtherType: 0x%04x\n", ethertype);
			
			printf("CABEÇALHO ARP");
			
			printf("Hardware Type: %d", arp_dados.hw_type);
			printf("Protocol Type: %d", arp_dados.pro_type);
			printf("HLEN: %c", arp_dados.hlen);
			printf("PLEN: %c", arp_dados.plen);
			printf("Operation: %d", arp_dados.operation);
			printf("Hardware Sender: %02x:%02x:%02x:%02x:%02x:%02x\n",
			arp_dados.sender_ha[0], arp_dados.sender_ha[1], arp_dados.sender_ha[2], arp_dados.sender_ha[3], arp_dados.sender_ha[4], arp_dados.sender_ha[5]);
			printf("IP Sender:  %02x:%02x:%02x:%02x",
			arp_dados.sender_ip[0], arp_dados.sender_ip[1], arp_dados.sender_ip[2], arp_dados.sender_ip[3],);
			printf("Hardware Target: %02x:%02x:%02x:%02x:%02x:%02x\n", 
			arp_dados.target_ha[0], arp_dados.target_ha[1],arp_dados.target_ha[2],arp_dados.target_ha[3],arp_dados.target_ha[4],arp_dados.target_ha[5]);
			printf("IP Target:  %02x:%02x:%02x:%02x",
			arp_dados.target_ip[0], arp_dados.target_ip[1], arp_dados.target_ip[2], arp_dados.target_ip[3],);
			
			printf("Dado: %s\n", data);
			printf("\n");
		}
	}

	close(fd);
	return 0;
}
