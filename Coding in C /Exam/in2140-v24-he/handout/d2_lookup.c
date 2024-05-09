/* ======================================================================
 * YOU ARE EXPECTED TO MODIFY THIS FILE.
 * ====================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "d2_lookup.h"

/* Definerer en minimum ID verdi */
#define ID_MIN_VALUE 1000 

/* Definerer MAX_PACKET_SIZE som skal holde paa max 1024 bytes (Som i D1) */
#define MAX_PACKET_SIZE 1024 

/* Definerer MAX_NETNODES_IN_BUFFER som holder paa max 5 netnodes*/
#define MAX_NETNODES_IN_BUFFER 5 // One buffer can contain up to 5 NetNodes.


/* Hjelpefunksjon som skal printe treet med DFS */
void d2_print_node_recursively(struct NetNode* node, struct LocalTreeStore* nodes, int depth, int* visited) {

    /* If-sjekk hvis vi ikke faar inn en node */
    if (node == NULL) {
        return ;
    }
    
    /* Sjekker om vi allerede har besoekt en node for aa detektere en cycle */
    if (visited[node->id]) {
        printf("Cycle detected at node with id %u\n", node->id);
        return ;
    }

    /* Marker den naavaerende noden som besoekt */
    visited[node->id] = 1;
    
    /* Printer trestruktur basert paa dybden */
    for (int i = 0; i < depth; ++i) {
        printf("-- ");
    }
    
    /* Printer informasjon om naavaerende node */
    printf("id %u value %u children %u\n", node->id, node->value, node->num_children);

    /* Printer ut hvert barn rekursivt */
    for (uint32_t i = 0; i < node->num_children; ++i) {
        uint32_t childId = node->child_id[i];
        if (childId < (uint32_t)nodes->number_of_nodes) {
            d2_print_node_recursively(&nodes->netNodes[childId], nodes, depth + 1, visited);
        }
    }
    
    /* Markerer bort den naavaerende noden som besoekt foer vi returnerer */
    visited[node->id] = 0;
}


D2Client* d2_client_create( const char* server_name, uint16_t server_port )
{
    /* Oppretter en D1Peer instans */
    D1Peer* peer = d1_create_client( );
    if (peer == NULL) {
        return NULL;
    }

    /* Faar server ingormasjon og lagrer det i D1Peer ved funksjon fra D1 */
    d1_get_peer_info(peer, server_name, server_port) ;

    /* Oppretter D2Client og allokerer minne */
    D2Client* client = malloc(sizeof(D2Client));

    /* Hvis client er NULL, feilmelding blir sendt */
    if (client == NULL) {
        fprintf(stderr, "Memory allocation for D2Client failed\n");
        //free(peer); 
        return NULL;
    }

    /* Lagrer peer i D2Client objekt */
    client->peer = peer;

    /* Returnerer klienten */
    return client;
    
}

D2Client* d2_client_delete( D2Client* client )
{
    /* Sletter D1Peer instans */
    if (client->peer != NULL) {

        /* Bruker d1_delete for aa slette en client */
        d1_delete(client->peer);
    }

    /* Freer saa D2Client objektet */
    free(client);

    /* Returner alltid NULL */
    return NULL;
    
}

int d2_send_request( D2Client* client, uint32_t id )
{
    /* Sjekk om ID er riktig */
    if (id <= ID_MIN_VALUE) {
            fprintf(stderr, "Invalid ID. ID must be greater than %u.\n", ID_MIN_VALUE);
        return -1;
    }

    /* Oppretter PacketRequest og fyller den ut + konverterer til network byte order */
    PacketRequest* request = malloc(sizeof(PacketRequest));
    if (!request) {
        fprintf(stderr, "Malloc failed\n");
        return -1;
    }

    /* Fully initialize the memory to zero */
    memset(request, 0, sizeof(PacketRequest));

    /* Hoster til network byte order */
    request->type = htons(TYPE_REQUEST);
    request->id = htonl(id); 

    /* Sender packeten */
    size_t packet_size = sizeof(PacketRequest);
    ssize_t bytes_sent = d1_send_data(client->peer, (char*)request, packet_size);


    /* Hvis packet ikke kunne bli sendt */
    if (bytes_sent <= 0) {
        fprintf(stderr, "Packet sending failure\n");
        return -1;
    }

    /* Huske aa frigjoere */
    free(request);

    return 1;

}

int d2_recv_response_size(D2Client* client) {

    /* Hvis clienten ikke finnes, returner feil */
    if (client == NULL) {
        return -1;
    }

    /* Oppretter en PacketResponseSize */
    PacketResponseSize* responseSize = malloc(sizeof(PacketResponseSize));
    if (!responseSize) {
        fprintf(stderr, "Malloc failed\n");
        return -1;
    }

    /* Oppretter en buffer med MAX_PACKET_SIZE */
    char buffer[MAX_PACKET_SIZE]; 

    /* Receive data ved aa bruke d1_recv_data funksjonen */
    size_t result = d1_recv_data(client->peer, buffer, 1000);
    /* If-sjekk paa muligens feil */
    if (result < sizeof(PacketResponseSize)) {
        fprintf(stderr, "Incomplete PacketResponseSize received\n");
        return -1;
    }

    /* Copy the received data into responseSize */
    memcpy(responseSize, buffer, sizeof(PacketResponseSize));

    /* Network to host byte order paa type og size */
    responseSize->type = ntohs(TYPE_RESPONSE_SIZE);
    responseSize->size = ntohs(responseSize->size);

    /* Sjekker om responsesize sin type ikke er lik TYPE_RESPONSE_SIZE */
    if (ntohs(responseSize->type) != TYPE_RESPONSE_SIZE) {
        fprintf(stderr, "Unexpected packet type: %u\n", responseSize->type);
        printf("TYPE_RESPONSE_SIZE:%d\n", TYPE_RESPONSE_SIZE);
        return -1;
    }
    
    /* Oppretter en int sizeToReturn som holder paa responsSize sin stoerrelse, dette gjoer at jeg lagrer stoerelsen foer jeg skal fri minnet */
    int sizeToReturn = responseSize->size; 
    /* Frier responseSize */
    free(responseSize);

    return sizeToReturn;

}

int d2_recv_response(D2Client* client, char* buffer, size_t sz) {

    /* If-sjekk hvis client, buffer og sz har feil verdier input */
    if (client == NULL || buffer == NULL || sz < MAX_PACKET_SIZE) {
        return -1;
    }

    /* Receive data ved aa bruke d1_recv_data funksjonen */
    int result = d1_recv_data(client->peer, buffer, sizeof(buffer));
    /* En if-sjekk for aa si ifra om feil hvis result ikke gaar gjennom */
    if (result <= -1) {
        fprintf(stderr, "Incomplete PacketResponse received\n");
        return -1;
    }

    /* Caster PacketResponse til bufferet */
    PacketResponse* response = (PacketResponse*)buffer;

    /* Oppretter en type fra response og utfoerer network to host byte order */
    uint16_t type = ntohs(response->type); 

    /* If-sjekk om type av PacketRespons blir behandlet riktig */
    if (type == TYPE_RESPONSE) {
        //printf("%d: D2 received a TYPE_RESPONSE packet.\n", getpid());
        printf("\n");
    } else if (type == TYPE_LAST_RESPONSE) {
        //printf("%d: D2 received a TYPE_LAST_RESPONSE packet.\n", getpid());
        printf("\n");
    } else {
        printf("%d: D2 received an unexpected packet type.\n", getpid());
        return -1; 
    }

    return result;

}

LocalTreeStore* d2_alloc_local_tree(int num_nodes) {

    /* Oppretter en LocalTreeStore og mallocer structen */
    LocalTreeStore* treeStore = malloc(sizeof(LocalTreeStore));

    /* If sjekk om malloc ikke gikk */
    if (!treeStore) {
        fprintf(stderr, "Malloc failed\n");
        return NULL;
    }
  
    /* Oppretter treeStore sin NetNodes og mallocer structen */
    treeStore->netNodes = malloc(sizeof(NetNode) * num_nodes);

    /* If sjekk om malloc ikke gikk */
    if (!treeStore->netNodes) {
        fprintf(stderr, "Malloc failed\n");
        free(treeStore);
        return NULL;
    }
  
    /* Setter treeStore sin number_of_nodes til aa vaere num_nodes */
    treeStore->number_of_nodes = num_nodes;

    /* Returnerer LocalTreeStoren som jeg opprettet */
    return treeStore;
}

void  d2_free_local_tree( LocalTreeStore* nodes )
{
    /* Frigjoer all minne som har blitt allokert for the local tree struct */
    if (nodes) {
        free(nodes->netNodes);
        free(nodes);
    }
 
  
}

int d2_add_to_local_tree( LocalTreeStore* nodes, int node_idx, char* buffer, int buflen )
{
    /* En ifsjekk for aa se om riktig parametere blir sendt inn */
    if (!nodes || !buffer || buflen < 12) { // Minimum stoerrelse for aa ha en node length
        fprintf(stderr, "Error: Invalid parameters.\n");
        return -1;
    }

    /* Oppretter en char som defineres som buffer */
    char* node_buffer = buffer;

    /* Saa lenge node_buffer + 12 bytes er mindre eller like buffer + buflen */
    while (node_buffer + 12 <= buffer + buflen) { 
        /* Hvis node_idx er stoerre eller lik 100, saa er treet fullt. */
        if (node_idx >= 100) {
            fprintf(stderr, "Error: LocalTreeStore is full.\n");
            return -1;
        }

        /* Oppretter en NetNode node og setter det til LocalTree sin netNodes med node_idx */
        NetNode* node = &nodes->netNodes[node_idx];

        /* Utfoerer network to host byte order paa id, value og num_children */
        node->id = ntohl(*(uint32_t*)node_buffer);
        node->value = ntohl(*(uint32_t*)(node_buffer + 4));
        node->num_children = ntohl(*(uint32_t*)(node_buffer + 8));

        /* Setter node_buffer til aa oeke pekeren til bufferten forbi den faste stoerrelsesdelen av noden */
        node_buffer += 12; 

        /* If-sjekk om bufferet har nok plass for barnas ID */
        if (node_buffer + node->num_children * 4 > buffer + buflen) {
            fprintf(stderr, "Error: Buffer overrun while accessing children of node %u.\n", node->id);
            return -1;
        }

        /* Itererer gjennom hvert barn til den gjeldene noden */
        for (uint32_t j = 0; j < node->num_children; j++) {
            /* Henter ID til hver barnenode */
            node->child_id[j] = ntohl(*(uint32_t*)(node_buffer + j * 4));
        }
        /* Oeker node_buffer med antall barn, multiplisert med 4. */
        node_buffer += node->num_children * 4; // Dette flytter pekeren forbi ID-ene til alle barna og forbereder den til aa behandle neste node i bufferet

        /* Oeker node_idz for aa gaa til neste node_idx */
        node_idx++; 

    }

    /* Returnerer neste ledige indeks i LocalTreeStore */
    return node_idx; 

}

void d2_print_tree(struct LocalTreeStore* nodes) {

    /* En if-sjekk som sjekker om treet er tomt eller ikke initialisert riktig */
    if (nodes == NULL || nodes->number_of_nodes == 0 || nodes->netNodes == NULL) {
        printf("The tree is empty or not initialized properly.\n");
        return;
    }
    
    /* Allokerer et arrat for aa tracke besoekte noder */
    int* visited = (int*)malloc(nodes->number_of_nodes * sizeof(int));
    if (visited == NULL) {
        printf("Failed to allocate memory for visited tracker.\n");
        return;
    }

    /* Initialiserer besoekt array til 0 */
    memset(visited, 0, nodes->number_of_nodes * sizeof(int));

    /* Kaller paa hjelpefunksjonen, som skal printe node rekursivt med DFS */
    d2_print_node_recursively(&nodes->netNodes[0], nodes, 0, visited);
    
    /* Huske aa frigjoere visited */
    free(visited);
}


