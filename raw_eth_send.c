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
	struct ifreq if_idx;
	struct ifreq if_mac;
	struct sockaddr_ll socket_address;
	struct arp arp_dados;
	char ifname[IFNAMSIZ];
	int frame_len = 0;
	char buffer[BUFFER_SIZE];
	char data[MAX_DATA_SIZE];
	char dest_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //broadcast
	short int ethertype = htons(0x0FFF);

	if (argc != 2) {
		printf("Usage: %s iface\n", argv[0]);
		return 1;
	}
	strcpy(ifname, argv[1]);

	/* Cria um descritor de socket do tipo RAW */
	if ((fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
		perror("socket");
		exit(1);
	}

	/* Obtem o indice da interface de rede */
	memset(&if_idx, 0, sizeof (struct ifreq));
	strncpy(if_idx.ifr_name, ifname, IFNAMSIZ - 1);
	if (ioctl(fd, SIOCGIFINDEX, &if_idx) < 0) {
		perror("SIOCGIFINDEX");
		exit(1);
	}

	/* Obtem o endereco MAC da interface local */
	memset(&if_mac, 0, sizeof (struct ifreq));
	strncpy(if_mac.ifr_name, ifname, IFNAMSIZ - 1);
	if (ioctl(fd, SIOCGIFHWADDR, &if_mac) < 0) {
		perror("SIOCGIFHWADDR");
		exit(1);
	}

	/* Indice da interface de rede */
	socket_address.sll_ifindex = if_idx.ifr_ifindex;

	/* Tamanho do endereco (ETH_ALEN = 6) */
	socket_address.sll_halen = ETH_ALEN;

	/* Endereco MAC de destino */
	memcpy(socket_address.sll_addr, dest_mac, MAC_ADDR_LEN);
	
	/* ARP */
     arp_dados.hw_type = htons(1);
	 arp_dados.pro_type = htons(0x0800);
	 arp_dados.operation = htons(1);
	 arp_dados.hlen = htons(6);
	 arp_dados.plen = htons(4);
	 

	/* Preenche o buffer com 0s */
	memset(buffer, 0, BUFFER_SIZE);

	/* Monta o cabecalho Ethernet */

	/* Preenche o campo de endereco MAC de destino */	
	memcpy(buffer, dest_mac, MAC_ADDR_LEN);
	frame_len += MAC_ADDR_LEN;

	/* Preenche o campo de endereco MAC de origem */
	memcpy(buffer + frame_len, if_mac.ifr_hwaddr.sa_data, MAC_ADDR_LEN);
	frame_len += MAC_ADDR_LEN;

	/* Preenche o campo EtherType */
	memcpy(buffer + frame_len, &ethertype, sizeof(ethertype));
	frame_len += sizeof(ethertype);
	
	/* Monta o cabecalho ARP */

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

	
	
	
	

	/* Obtem uma mensagem do usuario */
	printf("Digite a mensagem: ");
	scanf("%[^\n]s", data);

	/* Preenche o campo de dados */
	memcpy(buffer + frame_len, data, strlen(data));
	frame_len += strlen(data) + 1;

	/* Envia pacote */
	if (sendto(fd, buffer, frame_len, 0, (struct sockaddr *) &socket_address, sizeof (struct sockaddr_ll)) < 0) {
		perror("send");
		close(fd);
		exit(1);
	}

	printf("Pacote enviado.\n");

	close(fd);
	return 0;
}
