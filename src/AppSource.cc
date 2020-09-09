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

#include "AppSource.h"
#include "AppJob_m.h"

namespace thesis{

Define_Module(AppSource);

AppSource::AppSource()
{
    sendMessageEvent = NULL;
}

AppSource::~AppSource()
{
    cancelAndDelete(sendMessageEvent);
}

void AppSource::initialize()
{
    maxInterval = par("maxInterval").doubleValue();
    sendMessageEvent = new cMessage("sendEvent");
    scheduleAt(par("sendInterval"), sendMessageEvent);
}

void AppSource::handleMessage(cMessage *msg)
{
    ASSERT(msg == sendMessageEvent);
    simtime_t t,departure_time;

    // create new message
    AppJob *job = new AppJob(getJobName());
    job->setAppId(par("appId"));
    send(job, "out");

    // schedule next message
    departure_time = par("sendInterval").doubleValue();
    if (maxInterval > 0 && departure_time > maxInterval){
        t = simTime() + maxInterval;
    } else {
        t = simTime() + departure_time;
    }
    scheduleAt(t, sendMessageEvent);
}

const char *AppSource::getJobName(){
    return "job";
}

};
