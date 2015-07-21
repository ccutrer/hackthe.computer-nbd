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
  std::string variable1;
  std::string variable2;
  bool is_variable;
  uint64_t integer;
  std::vector<Statement> substatements;
};

std::string parseVariable(char * &stuff)
{
  char * begin = stuff;
  while ( (*stuff >= 'a' && *stuff <= 'z') ||
          (*stuff >= 'A' && *stuff <= 'Z') ||
          (*stuff >= '0' && *stuff <= '9'))
          ++stuff;
  return std::string(begin, stuff - begin);
}

uint64_t parseInteger(char * &stuff)
{
  return strtoull(stuff, &stuff, 10);
}

void parseValue(char * &stuff, std::string &variable, uint64_t &integer, bool &is_variable)
{
  if (*stuff >= '0' && *stuff <= '9') {
    is_variable = false;
    integer = parseInteger(stuff);
  } else {
    is_variable = true;
    variable = parseVariable(stuff);
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

void parse(std::vector<Statement> &statements, char * &stuff)
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
      parseValue(stuff, stmt.variable1, stmt.integer, stmt.is_variable);
      parse(stmt.substatements, stuff);
      ++stuff;
    } else if (*stuff == '!') {
      ++stuff;
      stmt.op = Statement::Print;
      parseValue(stuff, stmt.variable1, stmt.integer, stmt.is_variable);
    } else {
      stmt.variable1 = parseVariable(stuff);
      stmt.op = parseOp(stuff);
      parseValue(stuff, stmt.variable2, stmt.integer, stmt.is_variable);
    }
          statements.push_back(stmt);

  }
}

uint64_t evalValue(const std::string &variable, const uint64_t &integer, const bool &is_variable, map<string, uint64_t> &variables)
{
  if(is_variable) {
    return variables[variable];
  } else {
    return integer;
  }
}

void eval(const vector<Statement> &statements, map<string, uint64_t> &variables)
{
  for(vector<Statement>::const_iterator it = statements.begin(); it != statements.end(); ++it)
  {
    switch(it->op)
    {
      case Statement::Print:
        cout << evalValue(it->variable1, it->integer, it->is_variable, variables) << endl;
        break;
      case Statement::IfZero:
          if (evalValue(it->variable1, it->integer, it->is_variable, variables) == 0ull) {
            eval(it->substatements, variables);
          }
          break;
      case Statement::Assign:
        variables[it->variable1] = evalValue(it->variable2, it->integer, it->is_variable, variables);
        break;
      case Statement::Increment:
        variables[it->variable1] += evalValue(it->variable2, it->integer, it->is_variable, variables);
        break;
      case Statement::Decrement:
        variables[it->variable1] -= evalValue(it->variable2, it->integer, it->is_variable, variables);
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
    char *prog = (char *)program.c_str();
    parse(statements, prog);
    map<string, uint64_t> variables;
    eval(statements, variables);
  return 0;
}