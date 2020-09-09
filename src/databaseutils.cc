#include "databaseutils.h"
#include <iostream>

namespace thesis {


using Node = std::vector<format_node>;
using Link = std::vector<format_link>;

static int callback_source(void *ptr, int argc, char **argv, char **azColName){
    int i;

    format_node Source1;
    Node* source = static_cast<Node*>(ptr);

    //printf("%d\n", random);

    for(i=0; i<argc; i++){
        std::string temp = azColName[i];
        if (temp == "ID"){
            Source1.ID = atoi(argv[i]);
        }if (temp == "Indirizzo"){
            Source1.Indirizzo = argv[i];
        }if (temp == "Latitudine"){
            Source1.Latitudine = atof(argv[i]);
        }
        if (temp == "Longitudine"){
            Source1.Longitudine = atof(argv[i]);
        }
        if (temp == "Periferia"){
            Source1.Periferia = argv[i];
        }
    }
    source->push_back(Source1);
    return 0;
}


static int callback_fog(void *ptr, int argc, char **argv, char **azColName){
    int i;

    format_node Fog1;
    Node* fog = static_cast<Node*>(ptr);

    for(i=0; i<argc; i++){
        std::string temp = azColName[i];
        if (temp == "ID"){
            Fog1.ID = atoi(argv[i]);
        }if (temp == "Indirizzo"){
            Fog1.Indirizzo = argv[i];
        }if (temp == "Latitudine"){
            Fog1.Latitudine = atof(argv[i]);
        }
        if (temp == "Longitudine"){
            Fog1.Longitudine = atof(argv[i]);
        }
        if (temp == "Periferia"){
            Fog1.Periferia = argv[i];
        }
    }

    fog->push_back(Fog1);

    return 0;
}


static int callback_sink(void *ptr, int argc, char **argv, char **azColName){
    int i;

    format_node Sink1;
    Node* sink = static_cast<Node*>(ptr);

    for(i=0; i<argc; i++){
        std::string temp = azColName[i];
        if (temp == "ID"){
            Sink1.ID = atoi(argv[i]);
        }if (temp == "Indirizzo"){
            Sink1.Indirizzo = argv[i];
        }if (temp == "Latitudine"){
            Sink1.Latitudine = atof(argv[i]);
        }
        if (temp == "Longitudine"){
            Sink1.Longitudine = atof(argv[i]);
        }
        if (temp == "Periferia"){
            Sink1.Periferia = argv[i];
        }
    }

    sink->push_back(Sink1);

    return 0;
}




static int callback_linksourcefog(void *ptr, int argc, char **argv, char **azColName){
    int i;

    format_link Link1;
    Link* link = static_cast<Link*>(ptr);

    for(i=0; i<argc; i++){
        std::string temp = azColName[i];
        if (temp == "IDSource"){
            Link1.ID_A = atoi(argv[i]);
        }if (temp == "IDFog"){
            Link1.ID_B = atoi(argv[i]);
        }if (temp == "Distanza"){
            Link1.Distanza = atof(argv[i]);
        }
    }

    link->push_back(Link1);

    return 0;
}




static int callback_linkfogsink(void *ptr, int argc, char **argv, char **azColName){
    int i;

    format_link Link1;
    Link* link = static_cast<Link*>(ptr);

    for(i=0; i<argc; i++){
        std::string temp = azColName[i];
        if (temp == "IDFog"){
            Link1.ID_A = atoi(argv[i]);
        }if (temp == "IDSink"){
            Link1.ID_B = atoi(argv[i]);
        }if (temp == "Distanza"){
            Link1.Distanza = atof(argv[i]);
        }
    }

    link->push_back(Link1);

    return 0;
}






void Functions::Database::returnFog(std::vector<format_node>& vect_source, std::vector<format_node>& vect_fog, std::vector<format_node>& vect_sink , std::vector<format_link>& vect_linksourcefog, std::vector<format_link>& vect_linkfogsink, const char* database_name){

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql_source, *sql_fog, *sql_sink, *sql_linksourcefog, *sql_linkfogsink;


    srand (time(NULL));

    /* Open database */
    rc = sqlite3_open(database_name, &db);

    /* Create SQL statement */
    sql_source = (char*)"SELECT ID,Indirizzo,Latitudine,Longitudine,Periferia from Source";
    sql_fog = (char*)"SELECT ID,Indirizzo,Latitudine,Longitudine,Periferia from Fog";
    sql_sink = (char*)"SELECT ID,Indirizzo,Latitudine,Longitudine,Periferia from Sink";
    sql_linksourcefog = (char*)"SELECT IDSource,IDFog,Distanza from LinkSourceFog";
    sql_linkfogsink = (char*)"SELECT IDFog,IDSink,Distanza from LinkFogSink";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql_source, callback_source, &vect_source, &zErrMsg);
    rc = sqlite3_exec(db, sql_fog, callback_fog, &vect_fog, &zErrMsg);
    rc = sqlite3_exec(db, sql_sink, callback_sink, &vect_sink, &zErrMsg);
    rc = sqlite3_exec(db, sql_linksourcefog, callback_linksourcefog, &vect_linksourcefog, &zErrMsg);
    rc = sqlite3_exec(db, sql_linkfogsink, callback_linkfogsink, &vect_linkfogsink, &zErrMsg);

    sqlite3_close(db);
}

}
