#define KXVER 3
#include <iostream>
#include <string>
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <fstream>
#include "include/k.h"
#include "include/kdb_time.h"



using namespace std;

int main(int argc, char *argv[]) {

    // find host:port
    if(argc<5) {
        printf("progname {host} {port} {table} {output_file}\n");
        exit(1);
    }
    string host(argv[1]);
    int port = atoi(argv[2]);
    string table(argv[3]);
    string output_file(argv[4]);

    // connect
    int handle = khpu(S(host.c_str()), port, S("MarketSaver:x"));

    if ((!handle) || (handle < 0)) {
        printf("Cannot connect\n");
        exit(1);
    } else {
        printf("connected %s:%i\n", host.c_str(), port);
    }

    ofstream fout;
    fout.open(output_file, ios::out|ios::app);
    // subscribe to trade table
    string sub_cmd = ".u.sub[`" + table + ";`]" ;
    K r = k(handle, S(sub_cmd.c_str()), (K) 0);
    if (!r) {
        printf("Network Error\n");
        perror("Network");
        exit(1);
    } else {
        printf("subscribed trade table\n");
    }

    // process ticks
    K tbl, colData, colNames;
    int nCols, nRows, row, col;

    while(1) {
        r = k(handle,(S)0);
        if(r) {
            if(r->t == 0) {
                // r is 3 item list of format (".u.upd"; `trade; updateTable)
                if (!strcmp(kK(r)[1]->s, table.c_str())) {

                    // latest table of trade data, type 98
                    // means use ->k to access dictionary
                    tbl = kK(r)[2]->k;

                    // in dictionary tbl,
                    //     [0] is symbol list of column names
                    //     [1] is columns of data
                    colNames = kK(tbl)[0];
                    colData = kK(tbl)[1];
                    nCols = colNames->n;
                    nRows = kK(colData)[0]->n;

                    for (int row = 0; row < nRows; row++) {
                        for (int col = 0; col < nCols; col++) {
                            K obj = kK(colData)[col];
                            if (col > 0) {
                                fout << ",";
                            }
                            switch(obj->t)
                            {
                                case(0): { // handle a list of char vectors
                                    K x = kK(obj)[row];
                                    if (10 == x->t) {
                                        int i;
                                        for (i = 0; i < xn; i++) {
                                            fout << kG(x)[i];
                                        }
                                    }
                                    // else printf("type %d not supported by this client", obj->t);
                                    break;
                                }
                                case(1):{
                                    fout << kG(obj)[row];
                                    break;
                                }
                                case(4): {
                                    fout << kG(obj)[row];
                                    break;
                                }
                                case(5): {
                                    fout << kH(obj)[row];
                                    break;
                                }
                                case(6): {
                                    fout << kI(obj)[row];
                                    break;
                                }
                                case(7): {
                                    fout << kJ(obj)[row];
                                    break;
                                }
                                case(8): {
                                    fout << kE(obj)[row];
                                    break;
                                }
                                case(9): {
                                    fout << kF(obj)[row];
                                    break;
                                }
                                case(11): {
                                    fout << kS(obj)[row];
                                    break;
                                }
                                case(16): {
                                    // TODO
                                    fout << kJ(obj)[row];
                                    break;
                                }
                                default: {
                                    // printf("type %d not supported by this client", obj->t);
                                    break;
                                }

                            }
                            // string val(kS(obj)[row]);
                            // line += val;
                        }
                        fout << endl;
                    }
                }
            }
        }
        r0(r); // decrement reference count, free memory
    }

    fout.close();
    kclose(handle);
    return 0;
}