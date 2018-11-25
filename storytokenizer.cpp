#include "storytokenizer.h"

bool StoryTokenizer::hasNextPassage() const
{
  return story.find(PASSAGE_START_TAG, pos) != string::npos;
}

PassageToken StoryTokenizer::nextPassage()
{
  unsigned int next = story.find(PASSAGE_START_TAG, pos);
  unsigned int start = story.find(PASSAGE_START_TAG_CLOSE, next) + 1;
  unsigned int end = story.find(PASSAGE_END_TAG, start);

  if (next == string::npos || end == string::npos)
    return PassageToken();
  else
  {
    pos = end;
    unsigned int namestart = story.find(PASSAGE_NAME_START, next) + PASSAGE_NAME_START.size();
    unsigned int nameend = story.find(PASSAGE_NAME_END, namestart);
    if (namestart == string::npos || nameend == string::npos)
      return PassageToken(story.substr(start, end - start));
    else
      return PassageToken(story.substr(namestart, nameend - namestart), story.substr(start, end - start));
  }
}

bool PassageTokenizer::hasNextSection() const
{
  return pos < pass.size();
}

SectionToken PassageTokenizer::nextSection()
{
  unsigned int start, end;
  SectionToken ret;

  if (pos >= pass.size())
  {
    lastsect = TEXT;
    return ret;
  }

  //Handle blocks separately
  start = pass.find(BLOCK_START, pos);
  if ((lastsect == IF || lastsect == ELSEIF || lastsect == ELSE) && start != string::npos)
  {
    end = start+1;
    unsigned int level = 1;
    while (level > 0 && end < pass.size())
    {
      if (pass.substr(end, BLOCK_START.size()) == BLOCK_START)
        level++;
      else if (pass.substr(end, BLOCK_START.size()) == BLOCK_END)
        level--;
      end++;
    }

    if (level == 0)
      lastsect = BLOCK;
    else
      lastsect = TEXT; //Treat a bad block as text?
    ret = SectionToken(pass.substr(start, end - start), BLOCK);
    pos = end;
    return ret;
  }
  else
  {
    unsigned int nextset = pass.find(SET_START, pos);
    unsigned int nextgoto = pass.find(GOTO_START, pos);
    unsigned int nextlink = pass.find(LINK_START, pos);
    unsigned int nextif = pass.find(IF_START, pos);
    unsigned int nextelseif = pass.find(ELSEIF_START, pos);
    unsigned int nextelse = pass.find(ELSE_START, pos);

    unsigned int nexttok = nextset;
    if (nextgoto < nexttok)
      nexttok = nextgoto;
    if (nextgoto < nexttok)
      nexttok = nextgoto;
    if (nextlink < nexttok)
      nexttok = nextlink;
    if (nextif < nexttok)
      nexttok = nextif;
    if (nextelseif < nexttok)
      nexttok = nextelseif;
    if (nextelse < nexttok)
      nexttok = nextelse;

    end = string::npos;
    lastsect = TEXT;
    if (nexttok > pos)
      end = nexttok;
    else if (nexttok == nextlink)
    {
      end = pass.find(LINK_END, nextlink) + LINK_END.size();
      lastsect = LINK;
    }
    else if (nexttok != string::npos)
    {
      end = pass.find(COMMAND_END, pos) + 1;
      if (nexttok == nextgoto)
        lastsect = GOTO;
      else if (nexttok == nextset)
        lastsect = SET;
      else if (nexttok == nextif)
        lastsect = IF;
      else if (nexttok == nextelseif)
        lastsect = ELSEIF;
      else if (nexttok == nextelse)
        lastsect = ELSE;
      else
        lastsect = TEXT;
    }

    if (end != string::npos)
      ret = SectionToken(pass.substr(pos, end - pos), lastsect);
    else
      ret = SectionToken(pass.substr(pos), lastsect);
    pos = end;
    return ret;
  }
}

//debug
string PassageToken::getText()
{
  return this->pass;
}
