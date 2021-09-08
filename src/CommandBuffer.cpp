#include "CommandBuffer.h"
#include <stdlib.h>

CommandBuffer::CommandBuffer(int size) : buffer(new char[size]), size(size), index(0), commandLength(0)
{
  this->size = size;
  for (int i = 0; i < size; i++) {
    buffer[i] = 0;
  }
  parameter = nullptr;
}

CommandBuffer::~CommandBuffer()
{
  delete[] buffer;
}

void CommandBuffer::push_back(char c)
{
  if (index < size) {
    if (c == '(') {
      c = 0;
      parameter = &buffer[index] + 1;
    }
    if (c == ')') {
      c = 0;
    }
    buffer[index++] = c;
  }
}

void CommandBuffer::clear()
{
  while (index > 0) {
    buffer[--index] = 0;
  }
  parameter = nullptr;
}

void CommandBuffer::parse()
{
  for (int i = 0; i < index; i++) {
    if (buffer[i] == '(' || buffer[i] == ')') {
      buffer[i] = 0;
    }
  }
}

bool CommandBuffer::compareCommand(const char *arr)
{
  for (int i = 0; buffer[i] != 0 && arr[i] != 0; i++) {
    if (buffer[i] != arr[i]) {
      return false;
    }
  }
  return true;
}

int CommandBuffer::getParsedParameter()
{
  if (parameter != nullptr) {
    int count = 0;
    for (int i = 0; parameter[i] != 0; i++) {
      if (parameter[i] != '-' && (parameter[i] < '0' || parameter[i] > '9')) {
        if (parameter[i] == '-') {
          count++;
        }
        return -1;
      }
    }
    if (count > 1) {
      return -1;
    }
    return atoi(parameter);
  }
  return -1;
}

const char *CommandBuffer::getParameter() const
{
  return parameter;
}

const char *CommandBuffer::getCommand() const
{
  return buffer;
}
