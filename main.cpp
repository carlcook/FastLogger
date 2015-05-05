#include <iostream>
#include <thread>

#include "filebuffer.h"

using namespace std;

class Message
{
public:
  std::string mTraderName;
  int mTraderIndex;
  float mFooFactor;

  int GetMessageType() const
  {
    return 1;
  }

  void Serialise(FileBuffer& buffer) const
  {
    buffer << mTraderName;
    buffer << mTraderIndex;
    buffer << mFooFactor;
  }
};

void SendMessages()
{
  FileBuffer buffer;
  Message message;
  message.mTraderName = "CarlCook";
  message.mTraderIndex = 1;
  message.mFooFactor = 12.57;
  buffer.Serialise(message);
}

int main()
{
  std::thread worker1(SendMessages);
  std::thread worker2(SendMessages);
  worker1.join();
  worker2.join();


  // write sample program to read (to verify)

  // journalling (just do a mumap and create new buffer)
}

