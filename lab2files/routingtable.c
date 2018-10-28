#include "ne.h"
#include "router.h"

struct route_entry routingTable[MAX_ROUTERS];
int NumRoutes;

/* Routine Name    : InitRoutingTbl
 * INPUT ARGUMENTS : 1. (struct pkt_INIT_RESPONSE *) - The INIT_RESPONSE from Network Emulator
 *                   2. int - My router's id received from command line argument.
 * RETURN VALUE    : void
 * USAGE           : This routine is called after receiving the INIT_RESPONSE message from the Network Emulator. 
 *                   It initializes the routing table with the bootstrap neighbor information in INIT_RESPONSE.  
 *                   Also sets up a route to itself (self-route) with next_hop as itself and cost as 0.
 */
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

/* Routine Name    : UpdateRoutes
 * INPUT ARGUMENTS : 1. (struct pkt_RT_UPDATE *) - The Route Update message from one of the neighbors of the router.
 *                   2. int - The direct cost to the neighbor who sent the update. 
 *                   3. int - My router's id received from command line argument.
 * RETURN VALUE    : int - Return 1 : if the routing table has changed on running the function.
 *                         Return 0 : Otherwise.
 * USAGE           : This routine is called after receiving the route update from any neighbor. 
 *                   The routing table is then updated after running the distance vector protocol. 
 *                   It installs any new route received, that is previously unknown. For known routes, 
 *                   it finds the shortest path using current cost and received cost. 
 *                   It also implements the forced update and split horizon rules. My router's id
 *                   that is passed as argument may be useful in applying split horizon rule.
 */
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
					//If new cost is smaller and neighbor's next hop is not me
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
