#include <string>
#include <thread>

#include "filebuffer.h"
#include "messages.h"

using namespace std;

void SendMessages()
{
  // TODO implement multiple message types
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


  // TODO write sample program to read (to verify)

  // TODO journalling (just do a mumap and create new buffer)
}

