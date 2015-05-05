#ifndef MESSAGES_H
#define MESSAGES_H

#include "filebuffer.h"

class Message
{
public:
  std::string mTraderName;
  uint32_t mTraderIndex;
  float mFooFactor;

  // TODO declare all ctors

  // TODO public getters/setters, private fields

  // TODO enum for message type
  int GetMessageType() const;

  void Serialise(FileBuffer& buffer) const;
};

#endif // MESSAGES_H
