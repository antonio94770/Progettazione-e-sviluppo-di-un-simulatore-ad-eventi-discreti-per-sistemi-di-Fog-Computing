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

#ifndef __TESI_TOPOLOGYBUILDER_H_
#define __TESI_TOPOLOGYBUILDER_H_

#include <omnetpp.h>
#include "databaseutils.h"

using namespace omnetpp;

typedef std::map<long,cModule *> SourceId2ModuleMap;
typedef std::map<long,cModule *> FogId2ModuleMap;
typedef std::map<long,cModule *> SinkId2ModuleMap;

namespace thesis {

/**
 * TODO - Generated class
 */
class TopologyBuilder : public cSimpleModule
{
  protected:
    virtual void initialize();
    void buildSource(SourceId2ModuleMap &SourceId2Mod, cModule *parentmod, std::vector<format_node> Source, std::vector<format_xy> XYsource, double scartoX, double scartoY);
    void buildFog(FogId2ModuleMap &FogId2Mod, cModule *parentmod, std::vector<format_node> Fog, std::vector<format_xy> XYfog, double scartoX, double scartoY);
    void buildSink(FogId2ModuleMap &SinkId2Mod, cModule *parentmod, std::vector<format_node> Sink, std::vector<format_xy> XYsink, double scartoX, double scartoY);
    void buildLinkSourceFog(SourceId2ModuleMap SourceId2Mod, FogId2ModuleMap FogId2Mod, std::vector<format_link> LinkSourceFog);
    void buildLinkFogSink(FogId2ModuleMap FogId2Mod, SinkId2ModuleMap SinkId2Mod, std::vector<format_link> LinkFogSink);
    void buildMap(const char *networkName, const char *parentModule);
    virtual void handleMessage(cMessage *msg);
};

}

#endif
