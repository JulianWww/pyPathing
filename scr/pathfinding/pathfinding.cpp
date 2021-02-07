// pathfinding.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "node_Graph.h"

int main()
{
    node_Graph* g = new node_Graph({ { { 1,1,1,1,1,1,1,1 },
                                       { 1,1,1,1,1,1,1,1 },
                                       { 1,1,1,1,1,1,1,1 },
                                       { 1,1,1,1,1,1,1,1 },
                                       { 1,1,1,1,1,1,1,1 },
                                       { 1,1,1,1,1,1,1,1 },
                                       { 1,1,1,1,1,1,1,1 },
                                       { 1,1,1,1,1,1,1,1 }},

                                  { { 1,1,1,1,1,1,1,1 },
                                    { 1,1,1,1,1,1,1,1 },
                                    { 1,1,1,1,1,1,1,1 },
                                    { 1,1,1,1,1,1,1,1 },
                                    { 1,1,1,1,1,1,1,1 },
                                    { 1,1,1,1,1,1,1,1 },
                                    { 1,1,1,1,1,1,1,1 },
                                    { 1,1,1,1,1,1,1,1 }} }, { 4, 2}, 2);

    //g->build_hpA(3)
    auto r = g->Astar({ 0,0,0 }, { 0,6,6 }, 5);
    g->cleanUp();

    
    // auto c = g->Astar(1, 9);

    return 3;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file



/*
next step cython setup files
*/