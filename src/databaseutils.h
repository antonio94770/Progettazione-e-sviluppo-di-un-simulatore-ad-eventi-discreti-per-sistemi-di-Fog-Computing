#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <vector>
#include <sqlite3.h>
#include <omnetpp.h>

#ifndef __DATABASEUTILS_H__
#define __DATABASEUTILS_H__

using namespace std;

namespace thesis {

struct format_node{
    int ID;
    std::string Indirizzo;
    double Latitudine;
    double Longitudine;
    std::string Periferia;
};

struct format_link{
    int ID_A;
    int ID_B;
    float Distanza;
};

struct format_xy{
    double x;
    double y;
};

namespace Functions{
    class Database{
    public:
        void returnFog(std::vector<format_node>& vect_source, std::vector<format_node>& vect_fog, std::vector<format_node>& vect_sink, std::vector<format_link>& vect_linksourcefog, std::vector<format_link>& vect_linkfogsink, const char* database_name);
    };
}

}

#endif
