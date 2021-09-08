#pragma once

class CommandBuffer
{
private:
  char *buffer;
  char *parameter;
  int index;
  int size;
  int commandLength;

public:
  CommandBuffer(int);
  ~CommandBuffer();
  void push_back(char);
  void clear();
  void parse();
  bool compareCommand(const char *);
  int getParsedParameter();
  const char *getParameter() const;
  const char *getCommand() const;
};