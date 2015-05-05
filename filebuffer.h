#ifndef FILEBUFFER_H
#define FILEBUFFER_H

#include <string>

class FileBuffer final
{
private:

  struct Impl;
  Impl* mImpl;

  void CloseNoThrow() noexcept;

public:
  FileBuffer();
  FileBuffer(const FileBuffer&) = delete; // can't copy a buffer (only one instance owns the mapped file)
  FileBuffer(FileBuffer&& buffer) noexcept; // can move a buffer
  ~FileBuffer() noexcept;
  FileBuffer& operator= (const FileBuffer&) = delete; // can't assign from another buffer
  FileBuffer& operator= (FileBuffer&& buffer) noexcept; // can move assign

  FileBuffer& operator<< (const std::string& string) noexcept;

  template <typename T>
  FileBuffer& operator<< (const T& number) noexcept;

  template <typename T>
  void Serialise(const T& message);

  void Close();
};

template <typename T>
void FileBuffer::Serialise(const T& message)
{
  *this << message.GetMessageType();
  message.Serialise(*this);
}

#endif // FILEBUFFER_H
