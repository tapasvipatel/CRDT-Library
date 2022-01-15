#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "postgresql/libpq-fe.h"

void delete_table(PGconn *conn, std::string tableName) {

    std::string command = "DROP TABLE IF EXISTS " + tableName;
    PGresult *res = PQexec(conn, command.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQfinish(conn);
        exit(1); 
    }

    PQclear(res);

}

void create_set_double_data_table(PGconn *conn) {

    PGresult *res = PQexec(conn, "CREATE TABLE doublesetdata(globalid INT, objectid INT, value REAL)");

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQfinish(conn);
        exit(1); 
    }

    PQclear(res);

}

void create_set_int_data_table(PGconn *conn) {

    PGresult *res = PQexec(conn, "CREATE TABLE integersetdata(globalid INT, objectid INT, value INT)");

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQfinish(conn);
        exit(1); 
    }

    PQclear(res);

}

void create_crdt_gcounter_table(PGconn *conn) {

    PGresult *res = PQexec(conn, "CREATE TABLE gcounter(objectid INT PRIMARY KEY, value INT)");

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQfinish(conn);
        exit(1); 
    }

    PQclear(res);

}

void create_crdt_gset_table(PGconn *conn) {

    PGresult *res = PQexec(conn, "CREATE TABLE gset(objectid INT PRIMARY KEY, numelements INT)");

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQfinish(conn);
        exit(1); 
    }

    PQclear(res);

}

void create_crdt_pncounter_table(PGconn *conn) {

    PGresult *res = PQexec(conn, "CREATE TABLE pncounter(objectid INT PRIMARY KEY, value INT)");

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQfinish(conn);
        exit(1); 
    }

    PQclear(res);

}

void create_crdt_2pset_table(PGconn *conn) {

    PGresult *res = PQexec(conn, "CREATE TABLE twopset(objectid INT PRIMARY KEY, numelements INT)");

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQfinish(conn);
        exit(1); 
    }

    PQclear(res);
}

int get_pncounter_value(PGconn *conn, std::string id) {

    std::string command = "SELECT * FROM pncounter WHERE objectid = " + id;
    PGresult *res = PQexec(conn, command.c_str());
    int value = 0;

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {

        std::cout << "ObjectID " << id << " does not exist in the pncounter table." << std::endl;
        PQclear(res);
        PQfinish(conn);
        exit(1);

    }  

    value = std::stoi(std::string(PQgetvalue(res, 0, 1)));
    PQclear(res);
    return value;
}

void update_pncounter(PGconn *conn, std::string id, std::string newValue) {

    std::string command = "UPDATE pncounter SET value = '" + newValue + "' WHERE objectid = " + id;

    PGresult *res = PQexec(conn, command.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {

        std::cout << "ObjectID " << id << " does not exist in the pncounter table." << std::endl;
        PQclear(res);
        PQfinish(conn);
        exit(1);

    }  

    PQclear(res);

}

void create_pncounter(PGconn *conn, int initialValue) {

    PGresult *res = PQexec(conn, "SELECT * FROM pncounter");

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {

        std::cout << "No data in pncounter table." << std::endl;
        PQclear(res);
        PQfinish(conn);
        exit(1);

    }  

    std::string objectID = std::to_string(PQntuples(res));
    std::string payload = std::to_string(initialValue);
    std::string command = "INSERT INTO pncounter (objectid, value) VALUES(" + objectID + "," + payload + ")";
    
    PQclear(res);

    res = PQexec(conn, command.c_str());
        
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQfinish(conn);
        exit(1);     
    }

    PQclear(res);

}

void create_gcounter(PGconn *conn, int initialValue) {

    PGresult *res = PQexec(conn, "SELECT * FROM gcounter");

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {

        std::cout << "No data in gcounter table." << std::endl;
        PQclear(res);
        PQfinish(conn);
        exit(1);

    }  

    std::string objectID = std::to_string(PQntuples(res));
    std::string payload = std::to_string(initialValue);
    std::string command = "INSERT INTO gcounter (objectid, value) VALUES(" + objectID + "," + payload + ")";

    PQclear(res);

    res = PQexec(conn, command.c_str());
        
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQfinish(conn);
        exit(1);     
    }

    PQclear(res);

}

int get_num_gsets(PGconn *conn) {

    PGresult *res = PQexec(conn, "SELECT * FROM gset");    
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {

        std::cout << "No data in gset table." << std::endl;
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }    

    return PQntuples(res);

}

void create_crdt_table(PGconn *conn, std::string name) {

    PGresult *res = PQexec(conn, "INSERT INTO counters (objectid, value) VALUES(1,5)");
        
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQfinish(conn);
        exit(1);     
    }
    
    PQclear(res);

}

int main(int argc, char **argv) {
    
    if (argc != 3) {
        std::cout << "Usage: ./driver postgresqlUserName databaseName" << std::endl;
        exit(1);
    }

    std::string connectionString = "user=" + std::string(argv[1]) + " dbname=" + std::string(argv[2]);

    PGconn *conn = PQconnectdb(connectionString.c_str());
    int ver = 0;

    if (PQstatus(conn) == CONNECTION_BAD) {
        
        fprintf(stderr, "Connection to database failed: %s\n",
            PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
    else {

        std::cout << "Connected to database successfully!" << std::endl;
        ver = PQserverVersion(conn);
        std::cout << "Postgresql Server version: " << ver << std::endl;
    }

    // Temporary testing
    create_crdt_pncounter_table(conn);

    create_pncounter(conn, 5);
    update_pncounter(conn, std::to_string(0), std::to_string(2));

    create_pncounter(conn, 7);
    update_pncounter(conn, std::to_string(1), std::to_string(4));

    std::cout << "Value = " << get_pncounter_value(conn, std::to_string(1)) << std::endl; // should be 4

    PQfinish(conn);
    exit(1);

    return 0;

}
