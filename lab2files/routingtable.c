#include "ne.h"
#include "router.h"

struct route_entry routingTable[MAX_ROUTERS];
int NumRoutes;


void InitRoutingTbl(struct pkt_INIT_RESPONSE * InitResponse, int myID) {

    int i;
    struct nbr_cost costTemp;

    for (i = 0; i < InitResponse -> no_nbr; i++) {
		
		costTemp = InitResponse -> nbrcost[i];
        routingTable[i].cost = costTemp.cost;
        routingTable[i].next_hop = costTemp.nbr;
        routingTable[i].dest_id = costTemp.nbr;
    }

    routingTable[i].dest_id = myID;
    routingTable[i].next_hop = myID;
    routingTable[i].cost = 0;

    NumRoutes = i + 1;
}

int UpdateRoutes(struct pkt_RT_UPDATE * RecvdUpdatePacket, int costToNbr, int myID) {

    struct route_entry * tableEntry;
    struct route_entry updateEntry;
	int i;
    int j;
    int hasChanged = 0;
    int entryFound;

    for (i = 0; i < RecvdUpdatePacket -> no_routes; i++) {
        entryFound = 0;
        updateEntry = RecvdUpdatePacket -> route[i];
        updateCost = updateEntry.cost + costToNbr;
		
		for (j = 0; j < NumRoutes; j++) {
            tableEntry = & routingTable[j];

            if (tableEntry -> dest_id == updateEntry.dest_id) {
            	entryFound = 1;
				if (updateCost < routingTable[j].cost && updateEntry.next_hop != myID){	
					routingTable[j].next_hop = RecvdUpdatePacket -> sender_id;
					routingTable[j].cost = updateCost;
					hasChanged = 1;
				}else if(routingTable[j].next_hop == RecvdUpdatePacket -> sender_id && routingTable[j].cost != updateCost){
					routingTable[j].cost = updateCost;
					hasChanged = 1;
				}
				break;	
            }
        }

        if (entryFound == 0) {
            NumRoutes++;
            routingTable[NumRoutes].dest_id = updateEntry.dest_id;
            routingTable[NumRoutes].next_hop = RecvdUpdatePacket -> sender_id;
            routingTable[NumRoutes].cost = updateEntry.cost + costToNbr;
            hasChanged = 1;
        }
    }

    return hasChanged;
}

/* Routine Name    : ConvertTabletoPkt
 * INPUT ARGUMENTS : 1. (struct pkt_RT_UPDATE *) - An empty pkt_RT_UPDATE structure
 *                   2. int - My router's id received from command line argument.
 * RETURN VALUE    : void
 * USAGE           : This routine fills the routing table into the empty struct pkt_RT_UPDATE. 
 *                   My router's id  is copied to the sender_id in pkt_RT_UPDATE. 
 *                   Note that the dest_id is not filled in this function. When this update message 
 *                   is sent to all neighbors of the router, the dest_id is filled.
 */
void ConvertTabletoPkt(struct pkt_RT_UPDATE *UpdatePacketToSend, int myID);



/* Routine Name    : PrintRoutes
 * INPUT ARGUMENTS : 1. (FILE *) - Pointer to the log file created in router.c, with a filename that uses MyRouter's id.
 *                   2. int - My router's id received from command line argument.
 * RETURN VALUE    : void
 * USAGE           : This routine prints the routing table to the log file 
 *                   according to the format and rules specified in the Handout.
 */
void PrintRoutes (FILE* Logfile, int myID);



/* Routine Name    : UninstallRoutesOnNbrDeath
 * INPUT ARGUMENTS : 1. int - The id of the inactive neighbor 
 *                   (one who didn't send Route Update for FAILURE_DETECTION seconds).
 *                   
 * RETURN VALUE    : void
 * USAGE           : This function is invoked when a nbr is found to be dead. The function checks all routes that
 *                   use this nbr as next hop, and changes the cost to INFINITY.
 */
void UninstallRoutesOnNbrDeath(int DeadNbr);
