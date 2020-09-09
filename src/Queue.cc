//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Queue.h"
#include "AppJob_m.h"

namespace thesis {

Define_Module(Queue);

Queue::Queue()
{
    endServiceEvent=NULL;
}
Queue::~Queue() {
    cancelAndDelete(endServiceEvent);
}

void Queue::initialize()
{
    endServiceEvent=new cMessage("endService");
}

void Queue::handleMessage(cMessage *msg)
{
    AppJob *job;

    if(msg == endServiceEvent)
    {
        //get message from queue
        job = (AppJob*)buffer.pop();

        send(job,"out");

        if (!buffer.isEmpty()) {
            service_time = par("serviceTime");
            scheduleAt(simTime() + service_time, endServiceEvent);

        }
    }
    else
    {
        buffer.insert(msg);
        if (!endServiceEvent->isScheduled())
        {
            service_time=par("serviceTime");
            scheduleAt(simTime() + service_time, endServiceEvent);
        }


    }
}
};
