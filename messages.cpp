#include "messages.h"

int Message::GetMessageType() const
{
  return 1;
}

void Message::Serialise(FileBuffer& buffer) const
{
  buffer << mTraderName;
  buffer << mTraderIndex;
  buffer << mFooFactor;
}
