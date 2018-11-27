#include <fstream>
#include <string>
#include <limits>
#include "interpreter.h"
#include "storytokenizer.h"
using namespace std;

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
    // cout << " About to display passage..." << endl;
    cout << endl;
    displayPassage(passIndex);

    if(display_links.size() == 0) break;

    cout << "Your choice: ";
    cin >> selection;
    while(cin.fail() || selection < 1 || selection > display_links.size())
    {

      cout << "Invalid choice" << endl << "Try Again: ";
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      cin >> selection;
    }

    passIndex = selectionToIndex(selection-1);

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

  bool ifbool, displayNextBlock, gotobool;
  ifbool = displayNextBlock = gotobool = false;
  // cout << "Displaying passage..." << endl;
  while(ptok.hasNextSection())
  {
    
    SectionToken temp = ptok.nextSection();
    string name, text = temp.getText();

    // cout << text << endl;
    
    int j = 0;

    switch(temp.getType())
    {
      case TEXT:
      {
        cout << text;
        break;
      }

      case SET:
      {
        string str = temp.getText();
        for (unsigned int i = str.find ('$'); i < str.length(); i++)
        {
          if(str[i] == ' ')
          {
             name = str.substr (str.find ('$'), i - str.find ('$'));
             if (str.find("true") != std::string::npos)
             {
                variables[name] = true;
                cout << "Set " << name << " to true" << endl;
             }
             else
             {
                variables[name] = false;
                cout << "Set " << name << " to false" << endl;
             }
             break;
          }
        }break;
      }

      case IF:
      {
	ifbool = false;
        name =  text.substr(text.find('$'), text.find("is") - text.find('$') - 1);
        // cout << '[' << name << ']' << endl;
        if(name[name.size()-1] == ' ') name.pop_back();
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
      }
      
      case ELSEIF:
      {
        name =  text.substr(text.find('$'), text.find("is") - text.find('$') - 1);
        if(name[name.size()-1] == ' ') name.pop_back();
        // cout << '[' << name << ']' << endl;
        if(ifbool)
        {
          displayNextBlock = false;
          break;
        }
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
      }
      
      case ELSE:
      {
        if(ifbool)
        {
          displayNextBlock = false;
          break;
        }
        displayNextBlock = true;
        break;
      }

      // still need to fix LINK and finish with the BLOCK and GOTO
      case LINK:
      {
        if(text.find(LINK_SEPARATOR) != string::npos)
        {
          name = text.substr(2, text.find(LINK_SEPARATOR)-2);
          display_links.push_back(name);
          cout << name << " ";
          name = text.substr(text.find(LINK_SEPARATOR)+5, (text.find(LINK_END)-text.find(LINK_SEPARATOR))-5 );
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
      }

      case GOTO:
      {
        name = text.substr(text.find(GOTO_NAME_START)+6, text.find(COMMAND_END) - text.find(GOTO_NAME_START)-12);
        // cout << "Going to " << name << endl;
        gotobool = true;
        while(passages.at(j++).getName() != name){}
        display_links.clear();
        actual_links.clear();
        displayPassage(j-1);
        break;
      }

      case BLOCK:
      {
        if(displayNextBlock)
        {
          displayPassage_block(temp);
        }
        break;
      }
        
      

      default:
      {
        // cout << temp.getText() << endl;
        cout << " [ERROR] ";
        break;
      } 
        

    }
    if(gotobool) break;
  }
  cout << endl << "---MAP VALUES---" << endl;
  //display values in map for debugging
  for (std::pair<std::string, bool> element : variables)
  {
	   std::cout << element.first << " :: " << element.second << std::endl;
  }
  cout << endl;
  if(!gotobool)
  {
    cout << endl;
    displayLinks();
  }
}

int Interpreter::selectionToIndex(int n)
{
  int i = 0;
  // cout << "actual_links.at(n) : " << actual_links.at(n) << endl;
  while(passages.at(i++).getName() != actual_links.at(n)){}
  // cout << "passages.at(i).getName() : " << passages.at(i).getName() << endl;
  return i-1;
}

void Interpreter::displayPassage_block(SectionToken stok)
{
  // cout << "---MAP VALUES---" << endl;
  // //display values in map for debugging
  // for (std::pair<std::string, bool> element : variables)
  // {
	//    std::cout << element.first << " :: " << element.second << std::endl;
  // }
  // cout << endl << endl;

  string str = stok.getText();
  PassageTokenizer ptok(str.substr(1, str.length()-2));

  bool ifbool, displayNextBlock, gotobool;
  ifbool = displayNextBlock = gotobool = false;

  while(ptok.hasNextSection())
  {
    SectionToken temp = ptok.nextSection();
    string name, text = temp.getText();

    int j = 0;
    // cout << endl << temp.getText() << endl;

    switch(temp.getType())
    {
      case TEXT:
      {
        cout << text;
        break;
      }

      case SET:
      {
        string str = temp.getText();
        for (unsigned int i = str.find ('$'); i < str.length(); i++)
        {
          if(str[i] == ' ')
          {
             name = str.substr (str.find ('$') + 1, i - str.find ('$') - 2);
             if (str.find("true") != std::string::npos)
             {
                variables[name] = true;
             }
             else
             {
                variables[name] = false;
             }
             break;
          }
        }break;
      }

      case IF:
      {
	ifbool = false;
        name =  text.substr(text.find('$'), text.find("is") - text.find('$'));
        if(name[name.size()-1] == ' ') name.pop_back();
        // cout << "[" <<  name << "]"<< endl;
        if(text.find("true") != string::npos)
        {
          if(variables[name] == true)
          {
            displayNextBlock = true;
            ifbool = true;
            // cout << "DIsplaying next block..  " << endl;
          }
        }
        else
        {
          if(variables[name] == false)
          {
            displayNextBlock = true;
            ifbool = true;
            // cout << "DIsplaying next block..  " << endl;
          }
        }
        break;
      }

      case ELSEIF:
      {
        name =  text.substr(text.find('$'),  text.find("is") - text.find('$'));
        if(name[name.size()-1] == ' ') name.pop_back();
        cout << name << endl;
        if(ifbool)
        {
          displayNextBlock = false;
          break;
        }
        if(text.find("true") != string::npos)
        {
          if(variables[name] == true)
          {
            displayNextBlock = true;
            ifbool = true;
            // cout << "DIsplaying next block..  " << endl;
          }
        }
        else
        {
          if(variables[name] == false)
          {
            displayNextBlock = true;
            ifbool = true;
            // cout << "DIsplaying next block..  " << endl;
          }
        }
        break;
      }

      case ELSE:
      {
        if(ifbool)
        {
          displayNextBlock = false;
          break;
        }
        displayNextBlock = true;
        // cout << "DIsplaying next block..  " << endl;
        break;
      }

      // still need to fix LINK and finish with the BLOCK and GOTO
      case LINK:
      {
        if(text.find(LINK_SEPARATOR) != string::npos)
        {
          name = text.substr(2, text.find(LINK_SEPARATOR)-2);
          display_links.push_back(name);
          cout << name << " ";
          name = text.substr(text.find(LINK_SEPARATOR)+5, (text.find(LINK_END)-text.find(LINK_SEPARATOR))-5 );
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
      }

      case BLOCK:
      {
        if(displayNextBlock)
        {
          displayPassage_block(temp);
        }
        break;
      }

      case GOTO:
      {
        name = text.substr(text.find(GOTO_NAME_START)+6, text.find(COMMAND_END) - text.find(GOTO_NAME_START)-12);
        // cout << "Going to " << name << endl;
        gotobool = true;
        while(passages.at(j++).getName() != name){}
        display_links.clear();
        actual_links.clear();
        displayPassage(j-1);
        break;
      }

      default:
      {
        cout << temp.getText() << endl;
        cout << " [ERROR] ";
        break;
      }
    }
  }

  // cout << endl << "---MAP VALUES---" << endl;
  // //display values in map for debugging
  // for (std::pair<std::string, bool> element : variables)
  // {
  //    std::cout << element.first << " :: " << element.second << std::endl;
  // }
  // cout << endl << endl;
}


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

