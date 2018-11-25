#include <fstream>
#include <string>
#include "interpreter.h"
#include "storytokenizer.h"
using namespace std;

//Constructor
Interpreter::Interpreter(string str)
{
  filename = str;

  //Loads the contents of if.html into the interpreter member called "raw_story".
  string raw_story;
  ifstream story;
  story.open(filename);

  if(!story.is_open())
  {
    cout << "Unable to open file." << endl;
    exit(0);
  }

  string line;
  getline(story, line);
  while ((story.good()) && (line != "</html>"))
  {
    raw_story += line + '\n';
    getline(story, line);
  }

  //Puts all the passages into vector of PassageItems, so they can be accessed more easily.
  StoryTokenizer st(raw_story);
  while(st.hasNextPassage())
  {
    this->passages.push_back(st.nextPassage());
  }
}

//This function starts the interpreter
void Interpreter::start()
{
  int passIndex = 0;
  unsigned int selection;
  cout << "The Interpretor is starting" << endl;

  cout << endl;
  string output_text;

  while(1)
  {
    // while(selection < 1 || selection > passageItems.at(selection).number_of_selections())
    // {
    //   cout << "Invalid selection. Try again" << endl;
    //   cin >> selection;
    // }
    // cout << "\n\nYour choices: " << endl;

    // for(unsigned int z = 0; z < passageItems.size(); z++)
    // {
    //   if(passageItems.at(z).getName() == temp.getLinkAtIndex(selection-1))
    //   {
    //     passageItems.at(z).displayText();
    //     cout << output_text << endl;
    //     break;
    //   }
    // }

    displayPassage(passIndex);
    cout << "Your choice: ";
    cin >> selection;
    while(selection < 1 || selection > display_links.size())
    {
      cout << "Invalid choice" << endl << "Try Again: ";
      cin >> selection;
    }
    // Need to make something that takes the selection and turns it into the index in passages

    display_links.clear();
    actual_links.clear();
  }
};

void Interpreter::displayLinks()
{
  for(unsigned int i = 0; i < display_links.size(); i++)
  {
    cout << i+1 << ". " << display_links.at(i) << endl;
  }
};

void Interpreter::displayPassage(int n)
{
  PassageTokenizer ptok(passages.at(n).getText());

  bool ifbool, displayNextBlock; //gotobool;
  ifbool = displayNextBlock = false;

  while(ptok.hasNextSection())
  {
    SectionToken temp = ptok.nextSection();
    string name, text = temp.getText();


    switch(temp.getType())
    {
      case TEXT:
        cout << text;
        break;

      case SET:
        name = text.substr(5, text.find("to")-5);
        if(text.find("true") != string::npos) variables[name] = true;
        else variables[name] = false;
        break;

      case IF:
        name = text.substr(4, text.find("is")-4);
        if(text.find("true") != string::npos)
        {
          if(variables[name] == true)
          {
            displayNextBlock = true;
            ifbool = true;
          }
        }
        else
        {
          if(variables[name] == false)
          {
            displayNextBlock = true;
            ifbool = true;
          }
        }
        break;

      case ELSEIF:
        name = text.substr(9, text.find("is")-9);
        if(ifbool) break;
        if(text.find("true") != string::npos)
        {
          if(variables[name] == true)
          {
            displayNextBlock = true;
            ifbool = true;
          }
        }
        else
        {
          if(variables[name] == false)
          {
            displayNextBlock = true;
            ifbool = true;
          }
        }
        break;

      case ELSE:
        if(ifbool) break;
        displayNextBlock = true;
        break;

      // still need to fix LINK and finish with the BLOCK and GOTO
      case LINK:
        if(text.find(LINK_SEPARATOR) != string::npos)
        {
          name = text.substr(2, text.find(LINK_SEPARATOR)-2);
          display_links.push_back(name);
          cout << name << " ";
          name = text.substr(text.find(LINK_SEPARATOR), (text.find(LINK_END)-text.find(LINK_SEPARATOR)) );
          actual_links.push_back(name);

        }
        else
        {
          name = text.substr(2, text.find(LINK_END)-2);
          cout << name << " ";
          display_links.push_back(name);
          actual_links.push_back(name);
        }
        break;

      default:
        cout << " [ERROR] ";
        break;
    }
  }
  cout << endl;
  displayLinks();
}
