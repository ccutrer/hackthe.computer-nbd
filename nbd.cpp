#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <cstdint>
#include <fstream>

using namespace std;

struct Statement
{
    enum Op {
      Print,
      IfZero,
      Assign,
      Increment,
      Decrement
    };

  Statement()
  {
    is_variable = false;
    integer = 0ull;
  }

  Op op;
  int64_t *variable1;
  int64_t *variable2;
  bool is_variable;
  int64_t integer;
  std::vector<Statement> substatements;
};

int64_t *parseVariable(char * &stuff, map<string, int64_t> &variables)
{
  char * begin = stuff;
  while ( (*stuff >= 'a' && *stuff <= 'z') ||
          (*stuff >= 'A' && *stuff <= 'Z') ||
          (*stuff >= '0' && *stuff <= '9'))
          ++stuff;
  return &variables[std::string(begin, stuff - begin)];
}

int64_t parseInteger(char * &stuff)
{
  return strtoll(stuff, &stuff, 10);
}

void parseValue(char * &stuff, int64_t *&variable, int64_t &integer, bool &is_variable, map<string, int64_t> &variables)
{
  if (*stuff >= '0' && *stuff <= '9') {
    is_variable = false;
    integer = parseInteger(stuff);
  } else {
    is_variable = true;
    variable = parseVariable(stuff, variables);
  }
}

Statement::Op parseOp(char * &stuff)
{
  char *begin = stuff;
  ++stuff;
  if(*stuff == '=') ++stuff;
  switch(*begin)
  {
    case '+': return Statement::Increment; break;
    case '-': return Statement::Decrement; break;
    case '=': return Statement::Assign; break;
  }
}

void parse(std::vector<Statement> &statements, char * &stuff, map<string, int64_t> &variables)
{
  while(true)
  {
    Statement stmt;
    if(!*stuff) break;
    if(*stuff == '}') break;
    if(*stuff == ';') {
      ++stuff;
      continue;
    } else if (*stuff == '?') {
      ++stuff;
      stmt.op = Statement::IfZero;
      parseValue(stuff, stmt.variable1, stmt.integer, stmt.is_variable, variables);
      parse(stmt.substatements, stuff, variables);
      ++stuff;
    } else if (*stuff == '!') {
      ++stuff;
      stmt.op = Statement::Print;
      parseValue(stuff, stmt.variable1, stmt.integer, stmt.is_variable, variables);
    } else {
      stmt.variable1 = parseVariable(stuff, variables);
      stmt.op = parseOp(stuff);
      parseValue(stuff, stmt.variable2, stmt.integer, stmt.is_variable, variables);
    }
    statements.push_back(stmt);
  }
}

int64_t evalValue(const int64_t * const &variable, const int64_t &integer, const bool &is_variable)
{
  if(is_variable) {
    return *variable;
  } else {
    return integer;
  }
}

void eval(const vector<Statement> &statements)
{
  for(vector<Statement>::const_iterator it = statements.begin(); it != statements.end(); ++it)
  {
    switch(it->op)
    {
      case Statement::Print:
        cout << evalValue(it->variable1, it->integer, it->is_variable) << endl;
        break;
      case Statement::IfZero:
          if (evalValue(it->variable1, it->integer, it->is_variable) == 0ull) {
            eval(it->substatements);
          }
          break;
      case Statement::Assign:
        *it->variable1 = evalValue(it->variable2, it->integer, it->is_variable);
        break;
      case Statement::Increment:
        *it->variable1 += evalValue(it->variable2, it->integer, it->is_variable);
        break;
      case Statement::Decrement:
        *it->variable1 -= evalValue(it->variable2, it->integer, it->is_variable);
        break;
    }
  }
}

void
replace(std::string &str, char find, const std::string &replaceWith)
{
    size_t index = str.find(find);
    while (index != std::string::npos) {
        str = str.substr(0, index) + replaceWith + str.substr(index + 1);
        index = str.find(find, index + replaceWith.size());
    }
}

int main(int argc, char **argv)
{
    std::string program, line;
    //fstream file("example1");
    while (true)
    {
      if (cin.eof()) {
        break;
      }
      getline(cin, line);
      program.append(line);
    }
    replace(program, '\n', "");
    replace(program, '\t', "");
    replace(program, '\r', "");
    replace(program, ' ', "");
    vector<Statement> statements;
    map<string, int64_t> variables;
    char *prog = (char *)program.c_str();
    parse(statements, prog, variables);
    eval(statements);
  return 0;
}