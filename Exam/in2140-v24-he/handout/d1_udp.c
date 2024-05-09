/* ======================================================================
 * YOU ARE EXPECTED TO MODIFY THIS FILE.
 * ====================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "d1_udp.h"

/* Definerer MAX_PACKET_SIZE som skal holde paa max 1024 bytes */
#define MAX_PACKET_SIZE 1024

/* Hjelpefunksjon for aa sjekke for feil */
int check_error(int result, char* message) {
    if (result == -1) {
        perror(message);
        exit(EXIT_FAILURE);
        return -1;
    }
    else {
        return 1;
    }
}

D1Peer* d1_create_client( )
{
    /* Oppretter en D1Peer instans */
    D1Peer *client = malloc(sizeof(D1Peer)); //Med malloc blir clienten plassert paa heapen. 

    /* Sjekker og printer ut feilmelding om clienten er NULL */
    if (client == NULL) {
        fprintf(stderr, "Memory Allocation failed \n");
        return NULL;
    }

    /* Memsetter klienten sin lengde av bytes value til 0, konvertert til unsigned char, fra klienten */
    memset(client, 0, sizeof(D1Peer));

    /* Setter klienten sin next_seqno til 0 (skal initialiseres til 0 ifoelge mod) */
    client->next_seqno = 0;

    /* Setter opp UDP-en */
    client->socket = socket(AF_INET, SOCK_DGRAM, 0); //Oppretter en socket 
    check_error(client->socket, "socket"); //Sjekker om socket ble opprettet med hjelpemetoden 

    /* Sjekker om socket ble allokert */
    if (client->socket < 0) {
        //free(client); //Freer klient paa d1_delete()
        fprintf(stderr, "Socket creation failed \n");
        return NULL;
    }

    return client;
}

D1Peer* d1_delete( D1Peer* peer )
{
    /* Hvis peer struct ikke er NULL, maa vi lukke og frigjoere */
    if (peer != NULL) {
        close(peer->socket); //Lukker socketen 
        free(peer); //Frigjoer minne fra server structen 
    }
    return NULL;
}

int d1_get_peer_info(struct D1Peer* peer, const char* servername, uint16_t server_port)
{
    /* Oppretter en int readcount */
    int rc;

    /* Oppretter en struct in_addr (bibliotek i C) */
    struct in_addr ipaddresse;

    /* Hvis servername er "localhost", bytt ut med riktig IP addresse */
    if (strcmp(servername, "localhost") == 0) {
        servername = "127.0.0.1";
    }

    /* Konvertere IP adresse fra streng til struct in_addr */
    rc = inet_pton(AF_INET, servername, &ipaddresse.s_addr);
    check_error(rc, "inet_pton");
    if (rc == -1) {
        fprintf(stderr, "IP address not valid\n"); //WARNING
        return -1;
    }

    peer->addr.sin_family = AF_INET; /* Angir adressefamilie, IPv4 */
    peer->addr.sin_port = htons(server_port); /* Angir postnummer */
    peer->addr.sin_addr = ipaddresse; /* Setter IP-adressen til localhost */

    return 1;
}

int d1_recv_data( struct D1Peer* peer, char* buffer, size_t sz )
{
    /* En if-sjekk om sz vi tar inn er mindre enn D1Header */
    if (sz < sizeof(D1Header)) {
        fprintf(stderr, "Buffer too small for header\n");
        return -1;  // Buffer too big for header
    }

    /* Oppretter en packet definert MAX_PACKET_SIZE */
    char packet[MAX_PACKET_SIZE]; //ENDRET

    /* Memsetter packet til 0 */
    memset(packet, 0, MAX_PACKET_SIZE);

    /* Bruker recv for aa motta riktig packet */
    ssize_t recvBytes = recv(
        peer->socket,
        packet, 
        sizeof(packet),
        0
    );
    
    /* Haandtering av feil */
    check_error(recvBytes, "recv");
    if(recvBytes < (ssize_t)sizeof(D1Header)) {
        fprintf(stderr, "Incomplete header received\n");
        return -1;
    }

    /* Caster packeten til D1Header struct */
    D1Header* header = (D1Header*)packet;  

    /* Network to host byte order */
    header->checksum = ntohs(header->checksum); 
    header->flags = ntohs(header->flags);
    header->size = ntohl(header->size); 

    /* En if-sjekk for aa se om vi sender en data packet */
    if (!(header->flags & FLAG_DATA)) {
        fprintf(stderr, "This is not a data packet");
        return -1;
    }

    /* Setter en variabel chcksm som skal haandtere checksum, og setter den til 0 */
    uint16_t chcksm = 0;
   
    /* Setter en variabel sizeCombined til sz + D1Header */
    uint16_t sizeCombined = (sz + sizeof(D1Header));

    /* Setter Headeren sin size til aa vaere stoerrelsen av bufferet + Headeren */
    header->size = sizeCombined;

    /* XOR paa flags */
    chcksm = chcksm ^ header->flags;

    /* XOR paa foerste del av size */
    chcksm = chcksm ^ ((header->size >> 16) & 0xFFFF);

    /* XOR paa foerste resterende del av size */
    chcksm = chcksm ^ (header->size & 0xFFFF);
    
    /* XOR paa buffer/data */
    for (size_t i = 0; i < sz; i+=2) {
        uint16_t chunk = ((unsigned char)buffer[i] << 8) | (unsigned char)buffer[i+1];
        chcksm = chcksm ^ chunk;
    }
    
    /* Normal check hvis nummer av bytes er oddetall (padding??) */
    if (sz % 2 != 0) {
        chcksm = chcksm ^ (uint16_t)buffer[sz - 1] >> 8; //Pad with zero for computation if size is odd
    }

    /* Setter checksum lik chcksm */
    header->checksum = chcksm; 

    /* Hvis size ikke er lik riktig size, printer feilmelding og returnerer feil */
    if ((uint32_t)header->size != (uint32_t)sizeCombined) {
        fprintf(stderr, "Invalid checksum or size\n");

        /* Printtester for aa sjekke hva som faktisk er ulikt/feil */
        printf("Stoerrelse header->checksum: %u\n",header->checksum); //1008
        printf("Stoerrelse chcksm : %hu\n",chcksm);
        printf("Stoerrelse Size : %zu\n",sz+ recvBytes); //1012
        printf("Stoerrelse Receive bytes: %zu\n",recvBytes); //12
        printf("Stoerrelse Receive bytes: %zu\n",sz); //1000
        printf("Stoerrelse header->size: %u\n",htonl(header->size)); //1008

        return -1;

    } 
    /* Hvis alt er likt, sendes en ack packet med riktig peer og seqno */
    d1_send_ack(peer, (header->flags & SEQNO) >> 7);

    /* Memcopyer minneomraade */
    memcpy(buffer, packet + sizeof(D1Header), recvBytes - sizeof(D1Header)); //sizeCombined??
        
    /* Returnerer riktig antall bytes received */
    return recvBytes - sizeof(D1Header);

}

int d1_wait_ack( D1Peer* peer, char* buffer, size_t sz )
{
    /* This is meant as a helper function for d1_send_data.
     * When D1 data has send a packet, this one should wait for the suitable ACK.
     * If the arriving ACK is wrong, it resends the packet and waits again.
     *
     * Implementation is optional.
     */

    /* Velger aa ikke bruke parametere fra wait_ack */
    (void)buffer;
    (void)sz;

    /* Oppretter en packet */
    char packet[MAX_PACKET_SIZE];

    /* Bruker recv for aa motta riktig packet */
    ssize_t recvBytes = recv(
        peer->socket,
        packet, 
        sizeof(D1Header),
        0
    );
    /* Sjekker eventuelle feil */
    check_error(recvBytes, "recv");
    if(recvBytes < (ssize_t)sizeof(D1Header)) {
        fprintf(stderr, "Incomplete header received\n");
        return -1;
    }

    /* Caster packeten til D1Header struct */
    D1Header* header = (D1Header*)packet;

    //memcpy(buffer, packet + sizeof(D1Header), recvBytes - sizeof(D1Header)); ?

    /* Network to host byte order paa header: checksum, flags, size */
    header->checksum = ntohs(header->checksum); 
    header->flags = ntohs(header->flags);
    header->size = ntohl(header->size); 

    /* Hvis header->flags er i FLAG_ACK (AND operator), saa er peer->next_seqno ikke peer_>next_seqno */
    if (header->flags & FLAG_ACK) {
        peer->next_seqno = !peer->next_seqno;
        return 1;
    }

    return -1;


}

int d1_send_data( D1Peer* peer, char* buffer, size_t sz )
{
    /* Soerger for at sz pluss stoerrelse paa D1Header ikke exceeder 1024 bytes */
    if (sz > MAX_PACKET_SIZE - sizeof(D1Header)) {
        return -1; //Error
    }

    /* Oppretter en header som skal jobbes med */
    D1Header* header = malloc(sizeof(D1Header)); 

    /* If sjekk hvis minne allocation feiler */
    if (!header) {
        fprintf(stderr, "Memory allocation for D1Header failed\n");
        return -1;
    }

    /* Innisialiserer header til zeros */
    memset(header, 0, sizeof(*header)); 

    /* Oppretter packeten som skal sendes */
    char packet[MAX_PACKET_SIZE];

    /* Naa skal jeg sette FLAG_DATA og SEQNO som enten blir 0 eller 1 (sequence logic) */
    uint16_t flgs = FLAG_DATA | (peer->next_seqno ? SEQNO : 0);

    /* Setter en variabel chcksm som skal haandtere checksum, og setter den til 0 */
    uint16_t chcksm = 0;

    /* Setter Headeren sin flag til flgs */
    header->flags = flgs;

    /* Setter Headeren sin size til aa vaere stoerrelsen av bufferet + Headeren */
    header->size = (sz + sizeof(D1Header));

    /* Setter checksum lik 0 foer jeg utfoerer regning med XOR */
    header->checksum = 0;

    /* XOR paa flags */
    chcksm = chcksm ^ header->flags;

    /* XOR paa foerste del av size */
    chcksm = chcksm ^ ((header->size >> 16) & 0xFFFF);

    /* XOR paa foerste resterende del av size */
    chcksm = chcksm ^ (header->size & 0xFFFF);
    
    /* XOR paa buffer/data */
    for (size_t i = 0; i < sz; i+=2) {
        uint16_t chunk = ((unsigned char)buffer[i] << 8) | (unsigned char)buffer[i+1];
        chcksm = chcksm ^ chunk;
    }
    
    /* Normal check hvis nummer av bytes er oddetall (Padding??) */
    if (sz % 2 != 0) {
        chcksm = chcksm ^ (uint16_t)buffer[sz - 1] >> 8; //Pad with zero for computation if size is odd
    }

    /* Setter verdiene etter beregningen og konverterer til network byte order */
    header->checksum = htons(chcksm); 
    header->flags = htons(flgs);
    header->size = htonl(sz + sizeof(D1Header)); 

    /* Memcpyer packet */
    memcpy(packet, header, sizeof(D1Header));
    memcpy(packet + sizeof(D1Header), buffer, sz);

    /* Send packet, bruker sszite_t fordi det er en signed integer type, for aa kunne returnere et tall eller en error kode */
    ssize_t sentBytes = sendto(
        peer->socket,
        packet,
        sz + sizeof(D1Header),
        0,
        (struct sockaddr*)&peer->addr, /* Addressen vi skal sende til */
        sizeof(struct sockaddr_in) /* Lengden paa addressen vi skal sende til */
    );

    /* Sjekker for eventuelle feil */
    check_error(sentBytes, "sendto");

    /* Frigjoer header som jeg mallocerte paa starten av funksjonen */
    free(header);


    /* Sjekker for d1_wait_ack() */
    if (d1_wait_ack(peer, buffer, sz) == 1) {

        return sentBytes - sizeof(D1Header);

    } else {

        return -1;
    }

    /* Print sjekk for aa se om vi sendte riktig */
    printf("Sendte %s!\n", buffer);



    /* Returnerer antall bytes sent (ikke inkludert header) */
    return sentBytes - sizeof(D1Header);

}

void d1_send_ack(struct D1Peer* peer, int seqno) {

    /* Oppretter en D1Header */
    D1Header header;

    /* Memsetter header til 0 */
    memset(&header, 0, sizeof(D1Header));

    /* Hoster til network byte order */
    header.flags = htons(FLAG_ACK | (seqno ? 1 : 0)); //Setter seqno til ACKNOs seqno i FLAG
    header.size = htonl(sizeof(D1Header)); //Lik stoerrelse paa D1Header

    /* Bruker sendto for aa sende ack */
    int wc = sendto(
        peer->socket,
        &header,
        sizeof(D1Header),
        0,
        (struct sockaddr*)&peer->addr,
        sizeof(struct sockaddr_in)
    );

    /* Sjekker for eventuelle feil */
    check_error(wc, "sendto");

    //printf("Sent ACK packet with seqno: %d\n", seqno);
}

