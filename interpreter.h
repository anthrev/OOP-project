#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <map>
#include "storytokenizer.h"
#include "passageitem.h"
using namespace std;

class Interpreter
{
  private:
    string filename;
    vector<PassageToken> passages;
    vector<string> display_links;
    vector<string> actual_links;
    unordered_map<string, bool> variables;
  public:
    Interpreter() {filename = "none";}//give all member variables some default value
    Interpreter(string str);
    void start();
    void displayPassage(int);
    void displayLinks();
};

#endif
