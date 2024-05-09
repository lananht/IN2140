/* ======================================================================
 * YOU CAN MODIFY THIS FILE.
 * ====================================================================== */

#ifndef D2_LOOKUP_MOD_H
#define D2_LOOKUP_MOD_H

#include "d1_udp.h"

/* Inkluderer ogsaa d2_lookup headeren */
//#include "d2_lookup.h"

struct D2Client
{
    D1Peer* peer;
};

typedef struct D2Client D2Client;


struct LocalTreeStore
{
    int number_of_nodes;
    struct NetNode* netNodes; /* Legger til struct NetNode for aa kunne jobbe med LocalTreeStore m NetNodes */
};

typedef struct LocalTreeStore LocalTreeStore;


// /* Legger til hjelpefunksjoner */
void d2_print_node_recursively(struct NetNode* node, struct LocalTreeStore* nodes, int depth, int* visited);


#endif /* D2_LOOKUP_MOD_H */

