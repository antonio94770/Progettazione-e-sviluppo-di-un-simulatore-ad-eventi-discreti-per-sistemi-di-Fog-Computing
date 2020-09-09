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

#include "TopologyBuilder.h"
#include <sstream>
#include <omnetpp.h>

namespace thesis {

    Define_Module(TopologyBuilder);


    void TopologyBuilder::initialize()
    {
        buildMap(par("networkName").stringValue(), par("parentModule").stringValue());
    }

    void TopologyBuilder::handleMessage(cMessage *msg)
    {
        if (msg->isSelfMessage())
        {
            delete msg;
            buildMap(par("networkName").stringValue(), par("parentModule").stringValue());
        }
        else
            error("This modules does not process messages.");
    }

    void TopologyBuilder::buildMap(const char *networkName, const char *parentModule)
    {

        //Percentuale di un Source di essere scelto
        const char *database_name = par("subsampling_database");
        cout << "NOME DATABASE: " << database_name;

        //Grandezza "mappa" per calcolo dei punti x e y
        int width = 6371;
        int height = 6371;

        //Vettori che contengono le informazioni dei link e dei nodi ricavati dal database
        std::vector<format_node> Source;
        std::vector<format_node> Fog;
        std::vector<format_node> Sink;
        std::vector<format_link> LinkSourceFog;
        std::vector<format_link> LinkFogSink;

        std::vector<format_xy> XYsource;
        std::vector<format_xy> XYfog;
        std::vector<format_xy> XYsink;


        //Definisco le map per Source,Fog,Sink<ID,Modulo>
        SourceId2ModuleMap SourceId2Mod;
        FogId2ModuleMap FogId2Mod;
        SinkId2ModuleMap SinkId2Mod;


        //Raccolgo i dati dal database e li memorizzo nei vettori
        Functions::Database d;
        d.returnFog(Source, Fog, Sink, LinkSourceFog, LinkFogSink, database_name);


        //Prendo il modulo in cui verranno caricati i diversi nodi
        const char *modName = parentModule;
        cModule *parentmod = getParentModule()->getSubmodule(getName());


        //Converto latitudine e longitudine in punti X e Y
        double Xsource_min = width;
        double Ysource_min = height;

        for (format_node i : Source){
            double x = fmod((width*(180+(i.Longitudine*1))/360), (width +(width/2)));
            double pi = 3.14159265359;
            double latRad = (i.Latitudine*1)*pi/180;
            double mercN = log(tan((pi/4)+(latRad/2)));
            double y = (height/2)-(width*mercN/(2*pi));

            if (x < Xsource_min)
            {
                Xsource_min  = x;
            }

            if (y < Ysource_min)
            {
                Ysource_min  = y;
            }

            XYsource.push_back({x,y});
        }

        double Xfog_min = width;
        double Yfog_min = height;

        for (format_node i : Fog){
            double x = fmod((width*(180+(i.Longitudine*1))/360), (width +(width/2)));
            double pi = 3.14159265359;
            double latRad = (i.Latitudine*1)*pi/180;
            double mercN = log(tan((pi/4)+(latRad/2)));
            double y = (height/2)-(width*mercN/(2*pi));

            if (x < Xfog_min)
            {
                Xfog_min  = x;
            }

            if (y < Yfog_min)
            {
                Yfog_min  = y;
            }

            XYfog.push_back({x,y});
        }

        double Xsink_min = width;
        double Ysink_min = height;

        for (format_node i : Sink){
            double x = fmod((width*(180+(i.Longitudine*1))/360), (width +(width/2)));
            double pi = 3.14159265359;
            double latRad = (i.Latitudine*1)*pi/180;
            double mercN = log(tan((pi/4)+(latRad/2)));
            double y = (height/2)-(width*mercN/(2*pi));

            if (x < Xsink_min)
            {
                Xsink_min  = x;
            }

            if (y < Ysink_min)
            {
                Ysink_min  = y;
            }

            XYsink.push_back({x,y});
        }

        //Prendo il valore minimo così da non avere numeri negativi
        double temp = min(Xsource_min, Xfog_min);
        double scartoX = min(temp, Xsink_min) - 1;

        temp = min(Ysource_min, Yfog_min);
        double scartoY = min(temp, Ysink_min) - 1;

        cout << "Scarto X: " << scartoX <<endl;
        cout << "Scarto Y: " << scartoY <<endl;



        //Costruisco la mappa
        buildSource(SourceId2Mod,parentmod,Source,XYsource,scartoX,scartoY);
        buildFog(FogId2Mod,parentmod,Fog,XYfog,scartoX,scartoY);
        buildSink(SinkId2Mod,parentmod,Sink,XYsink,scartoX,scartoY);
        buildLinkSourceFog(SourceId2Mod,FogId2Mod,LinkSourceFog);
        buildLinkFogSink(FogId2Mod,SinkId2Mod,LinkFogSink);


        //Applico le modifiche

        std::map<long,cModule *>::iterator it;

        for (it=SourceId2Mod.begin(); it != SourceId2Mod.end(); ++it){
            cModule *mod = it->second;
            mod->finalizeParameters();
            mod->buildInside();
        }

        for (it=FogId2Mod.begin(); it != FogId2Mod.end(); ++it){
            cModule *mod = it->second;

            mod->finalizeParameters();
            mod->buildInside();
        }

        for (it=SinkId2Mod.begin(); it != SinkId2Mod.end(); ++it){
            cModule *mod = it->second;
            mod->finalizeParameters();
            mod->buildInside();
        }




    }


    void TopologyBuilder::buildSource(SourceId2ModuleMap &SourceId2Mod, cModule *parentmod, std::vector<format_node> Source, std::vector<format_xy> XYsource, double scartoX, double scartoY)
    {
        cModuleType* moduleType = cModuleType::get("thesis.src.AppSource");

        int count = 0;
        for (format_node i : Source){

            std::ostringstream os;

            const std::string& indirizzo = i.Indirizzo;
            const char* c_indirizzo = indirizzo.c_str();

            //Creo il nodo
            cModule *mod = moduleType->create(c_indirizzo, parentmod);

            //Inserisco il nodo Source nella map<ID,Modulo>
            SourceId2Mod[i.ID] = mod;

            //Inserisco l'id per ogni nodo
            cPar& p = mod->par("appId").setIntValue(i.ID);

            //Modifico il @display del nodo
            cDisplayString& displayString = mod->getDisplayString();
            os << "p=" << (XYsource.at(count).x - scartoX)<< "," << (XYsource.at(count).y - scartoY) << ";i=block/source;is=vs";

            cout << "X: " << (XYsource.at(count).x - scartoX)<< "Y: " << (XYsource.at(count).y - scartoY) << "\n";

            const std::string& resultsstr = os.str();
            const char* cstr1 = resultsstr.c_str();

            displayString.parse(cstr1);

            count++;
       }
    }


    void TopologyBuilder::buildFog(FogId2ModuleMap &FogId2Mod, cModule *parentmod, std::vector<format_node> Fog, std::vector<format_xy> XYfog, double scartoX, double scartoY)
    {
        cModuleType* moduleType = cModuleType::get("thesis.src.Queue");

        int count = 0;
        for (format_node i : Fog){

            std::ostringstream os;

            const std::string& indirizzo = i.Indirizzo;
            const char* c_indirizzo = indirizzo.c_str();

            //Creo il nodo
            cModule *mod = moduleType->create(c_indirizzo, parentmod);

            //Inserisco il nodo Fog nella map<ID,Modulo>
            FogId2Mod[i.ID] = mod;


            //Inserisco l'id per ogni nodo
            cPar& p = mod->par("queueId").setIntValue(i.ID);

            //Modifico il @display del nodo
            cDisplayString& displayString = mod->getDisplayString();
            os << "p=" << (XYfog.at(count).x - scartoX)<< "," << (XYfog.at(count).y - scartoY)<< ";i=block/queue;is=s";

            //cout << "X: " << x << "Y: " << y << "\n";

            const std::string& resultsstr = os.str();
            const char* cstr1 = resultsstr.c_str();

            displayString.parse(cstr1);

            count++;
        }
    }

    void TopologyBuilder::buildSink(FogId2ModuleMap &SinkId2Mod, cModule *parentmod, std::vector<format_node> Sink, std::vector<format_xy> XYsink, double scartoX, double scartoY)
    {
        cModuleType *moduleType = cModuleType::get("thesis.src.Sink");

        int count = 0;
        for (format_node i : Sink){

            std::ostringstream os;

            const std::string& indirizzo = i.Indirizzo;
            const char* c_indirizzo = indirizzo.c_str();

            //Creo il nodo
            cModule *mod = moduleType->create(c_indirizzo, parentmod);

            //Inserisco il nodo Fog nella map<ID,Modulo>
            SinkId2Mod[i.ID] = mod;


            //Inserisco l'id per ogni nodo
            cPar& p = mod->par("sinkId").setIntValue(i.ID);





            //Modifico il @display del nodo
            cDisplayString& displayString = mod->getDisplayString();
            os << "p=" << (XYsink.at(count).x - scartoX) << "," << (XYsink.at(count).y - scartoY) << ";i=block/sink;is=n";

            //cout << "X: " << x << "Y: " << y << "\n";

            const std::string& resultsstr = os.str();
            const char* cstr1 = resultsstr.c_str();

            displayString.parse(cstr1);

            count++;
        }
    }

    void TopologyBuilder::buildLinkSourceFog(SourceId2ModuleMap SourceId2Mod, FogId2ModuleMap FogId2Mod, std::vector<format_link> LinkSourceFog)
    {
        for (format_link i : LinkSourceFog){

            cModule *destmod = FogId2Mod.find(i.ID_B)->second;

            //cModule *srcmod = SourceId2Mod.find(i.IDSource)->second;
            if (SourceId2Mod.count(i.ID_A)){
                cModule *srcmod = SourceId2Mod.find(i.ID_A)->second;


                cGate *srcOut = srcmod->gate("out", -1);
                cGate *destIn = destmod->getOrCreateFirstUnconnectedGate("in", 0, false, true);


                cChannelType *channelType = cChannelType::get("ned.DelayChannel");
                cChannel *channel = channelType->create("channel");

                double delay = i.Distanza/100;

                std::ostringstream os;
                os << delay << "ms";

                const std::string& resultsstr = os.str();
                const char* delay_str = resultsstr.c_str();

                //cout << delay_str << "\n";

                channel->par("delay").parse(delay_str);

                srcOut->connectTo(destIn, channel);

                channel->callInitialize();


            }

        }
    }

    void TopologyBuilder::buildLinkFogSink(FogId2ModuleMap FogId2Mod, SinkId2ModuleMap SinkId2Mod, std::vector<format_link> LinkFogSink)
    {
        for (format_link i : LinkFogSink){

            cModule *destmod = SinkId2Mod.find(i.ID_B)->second;

            //cModule *srcmod = SourceId2Mod.find(i.IDSource)->second;
            if (FogId2Mod.count(i.ID_A)){
                cModule *srcmod = FogId2Mod.find(i.ID_A)->second;
                cGate *srcOut = srcmod->gate("out", -1);
                cGate *destIn = destmod->getOrCreateFirstUnconnectedGate("in", 0, false, true);


                cChannelType *channelType = cChannelType::get("ned.DelayChannel");
                cChannel *channel = channelType->create("channel");

                double delay = i.Distanza/100;

                std::ostringstream os;
                os << delay << "ms";

                const std::string& resultsstr = os.str();
                const char* delay_str = resultsstr.c_str();

                //cout << delay_str << "\n";

                channel->par("delay").parse(delay_str);

                srcOut->connectTo(destIn, channel);

                cDisplayString& displayString = srcOut->getDisplayString();
                displayString.parse("ls=red,3,da;");

                channel->callInitialize();

            }

        }
    }




}
