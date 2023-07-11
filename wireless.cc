#include <omnetpp.h>
#include <map>
#include "packet_m.h"

using namespace omnetpp;
using namespace std;


double src_x = 0.0;
double src_y = 0.0;
double sink_x = 0.0;
double sink_y = 0.0;
map <int, double> node_xloc;
map <int, double> node_yloc;
double transmission_rng = 0.0;
int flag = 1;
int numNodes = 0;


class MySource : public cSimpleModule
{
public:
	cModule *wireless;

    virtual Packet *generateMessage();
    virtual void initialize();
    virtual void handleMessage(cMessage *msgSend);
};

Define_Module(MySource);

void MySource::initialize()
{
   src_x = par("x").doubleValue();
   src_y = par("y").doubleValue();
   transmission_rng = par("transmission_range").doubleValue();
   numNodes = par("num_nodes").intValue();
   wireless = getModuleByPath("wireless");
   scheduleAt(simTime(), new Packet);
}

void MySource::handleMessage(cMessage *msgSend)
{	
    if(flag == 1){
	    Packet  *msg = generateMessage();
	    msg->setX(src_x);
	    msg->setY(src_y);
            sendDirect(msg, wireless->gate("in"));
    }
}


class MyNode : public cSimpleModule
{
public:
	int key;
	cModule *sink;
	cModule *wireless;

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

Define_Module(MyNode);

void MyNode::initialize()
{
	key = par("key").intValue();
	node_xloc[key] = par("x").doubleValue();
	node_yloc[key] = par("y").doubleValue();
	sink = getModuleByPath("sink");
	wireless= getModuleByPath("wireless");
}

void MyNode::finish()
{

}

void MyNode::handleMessage(cMessage *msg)
{
	Packet *recv = check_and_cast<Packet *>(msg);
	double loc_x = node_xloc[key];
	double loc_y = node_yloc[key];
	double sink_dist = sqrt(pow(loc_x - sink_x, 2) + pow(loc_y - sink_y, 2));
	if(sink_dist <= transmission_rng){
		recv->setX(loc_x);
                recv->setY(loc_y);
		sendDirect(recv, sink->gate("sink_in"));
	}
	else{
	       recv->setX(loc_x);
	       recv->setY(loc_y);
	       sendDirect(recv, wireless->gate("in"));
        }
}

class MyWireless : public cSimpleModule
{
public:
        cModule *sink;
        cModule *node[100];

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

Define_Module(MyWireless);

void MyWireless::initialize()
{
        sink = getModuleByPath("sink");
	for (int i = 0; i < numNodes; i++)
	{
		char nodename[20];
		sprintf(nodename, "node[%d]", i);
		node[i] = getModuleByPath(nodename);
	}
}

void MyWireless::finish()
{

}

void MyWireless::handleMessage(cMessage *msg)
{
        Packet *recv = check_and_cast<Packet *>(msg);
        double loc_x = (double) recv->getX();
        double loc_y = (double) recv->getY();
        double sink_dist = sqrt(pow(loc_x - sink_x, 2) + pow(loc_y - sink_y, 2));
	double node_sink = 1000.0;
	int node_key = -1;
        if(sink_dist <= transmission_rng){
                sendDirect(recv, sink->gate("sink_in"));
        }
        else{
		for (int i = 0; i < numNodes; i++)
		{
			double current_sink = sqrt(pow(node_xloc[i] - sink_x, 2) + pow(node_yloc[i] - sink_y, 2));
			double current_node = sqrt(pow(loc_x - node_xloc[i], 2) + pow(loc_y - node_yloc[i], 2));
			if ((current_node <= transmission_rng) && (current_sink <= node_sink)){
				node_sink = current_sink;
				node_key = i;
			}
		}
		double prev_trans = sqrt(pow(loc_x - node_xloc[node_key], 2) + pow(loc_y - node_yloc[node_key], 2));
		if (prev_trans > 0){
			sendDirect(recv, node[node_key]->gate("node_in"));
		}
		else{
			flag = 0;
			delete msg;
			EV << "Sink is unreachable\n";
		}
        }
}


class MySink : public cSimpleModule
{

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

Define_Module(MySink);

void MySink::initialize()
{
	sink_x = par("x").doubleValue();
	sink_y = par("y").doubleValue();
}

void MySink::finish()
{

}

void MySink::handleMessage(cMessage *msg)
{
	   EV << "Sink recieved a new message\n";
           simtime_t d = simTime() - msg->getCreationTime();
	   EV << "Message lifetime is:" << d <<"\n";
           flag = 0;
           delete msg;
}

Packet *MySource::generateMessage()
{
        Packet *msg = new Packet();
        msg->setX(0.0);
        msg->setY(0.0);
        return msg;
}


