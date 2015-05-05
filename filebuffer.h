#ifndef FILEBUFFER_H
#define FILEBUFFER_H

#include <string>
#include <memory>

class FileBuffer final
{
private:
  /// data members
  struct Impl;
  std::unique_ptr<Impl> mImpl;

  /// useful helper methods
  void CloseNoThrow() noexcept;

public:
  /// constructors
  FileBuffer(); // once constructed, file has been mapped
  FileBuffer(const FileBuffer&) = delete; // copying a buffer makes no sense
  FileBuffer(FileBuffer&& buffer) noexcept; // transferring ownership is allowed
  ~FileBuffer() noexcept;
  FileBuffer& operator= (const FileBuffer&) = delete; // can't copy by way of assignment (copies are not allowed)
  FileBuffer& operator= (FileBuffer&& buffer) noexcept; // can move by way of assignment

  /// serialisation routines that messages will call within their serialisation routine
  FileBuffer& operator<< (const std::string& string) noexcept;
  template <typename T> FileBuffer& operator<< (const T& number) noexcept;

  /// call this to serialise an entire message (sort of a template method pattern)
  template <typename T> void Serialise(const T& message);

  /// ask the buffer to flush to disk and clean up
  void Close();
};

template <typename T>
void FileBuffer::Serialise(const T& message)
{
  *this << message.GetMessageType();
  message.Serialise(*this);
}

#endif // FILEBUFFER_H
