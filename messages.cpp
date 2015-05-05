#include "messages.h"

const std::string& TraderKeyLoginMessage::GetTraderName() const
{
  return mTraderName;
}

void TraderKeyLoginMessage::SetTraderName(const std::string& value)
{
  mTraderName = value;
}

uint32_t TraderKeyLoginMessage::GetTraderIndex() const
{
  return mTraderIndex;
}

void TraderKeyLoginMessage::SetTraderIndex(const uint32_t value)
{
  mTraderIndex = value;
}

float TraderKeyLoginMessage::GetFooFactor() const
{
  return mFooFactor;
}

void TraderKeyLoginMessage::SetFooFactor(float value)
{
  mFooFactor = value;
}

MessageType TraderKeyLoginMessage::GetMessageType() const
{
  return MessageType::TraderKeyLogin;
}

void TraderKeyLoginMessage::Serialise(FileBuffer& buffer) const
{
  buffer << mTraderName;
  buffer << mTraderIndex;
  buffer << mFooFactor;
}

MessageType OrderInsertMessage::GetMessageType() const
{
  return MessageType::OrderInsert;
}

void OrderInsertMessage::Serialise(FileBuffer& buffer) const
{
  buffer << mVolume;
  buffer << mPrice;
}


uint32_t OrderInsertMessage::GetVolume() const
{
  return mVolume;
}

void OrderInsertMessage::SetVolume(const uint32_t value)
{
  mVolume = value;
}

double OrderInsertMessage::GetPrice() const
{
  return mPrice;
}

void OrderInsertMessage::SetPrice(double value)
{
  mPrice = value;
}
