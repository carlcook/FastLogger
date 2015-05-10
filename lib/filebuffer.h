#ifndef FILEBUFFER_H
#define FILEBUFFER_H

#include <memory>
#include <string>

class FileBuffer final
{
private:
  /// data members
  struct Impl;
  std::unique_ptr<Impl> mImpl;

  /// useful helper methods
  void CloseNoThrow() noexcept;
  char*& GetInternalBuffer();
  void WriteTimestamp();

public:
  /// constructors
  FileBuffer(); // once constructed, file has been mapped
  FileBuffer(const FileBuffer&) = delete; // copying a buffer makes no sense
  FileBuffer(FileBuffer&& buffer) noexcept; // transferring ownership is allowed
  virtual ~FileBuffer() noexcept;
  FileBuffer& operator= (const FileBuffer&) = delete; // can't copy by way of assignment (copies are not allowed)
  FileBuffer& operator= (FileBuffer&& buffer) noexcept; // can move by way of assignment

  /// serialisation routines that messages will call within their serialisation routine
  template <typename T> FileBuffer& operator<< (const T& type) noexcept;

  /// call this to serialise an entire message (sort of a template method pattern)
  template <typename T> void Serialise(const T& message);

  /// ask the buffer to flush to disk and clean up
  void Close();
};

template <typename T>
void FileBuffer::Serialise(const T& message)
{
  // TODO journalling (just do a munmap and create new buffer), otherwise we will segfault
  // Maybe ask for serialised size, and if not enough space, flush out (or use hardcoded limit)

  if (mImpl)
  {
    WriteTimestamp();
    message.Serialise(GetInternalBuffer());
  }
}

#endif // FILEBUFFER_H
