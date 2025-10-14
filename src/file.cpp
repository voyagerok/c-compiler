#include "file.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

file::file(std::string_view path) {
  int fd = open(path.data(), O_RDONLY);
  if (fd == -1) {
    throw std::runtime_error("Failed to open file");
  }
  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    close(fd);
    throw std::runtime_error("Failed to get file size");
  }
  m_size = sb.st_size;
  m_addr = static_cast<char *>(
      mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
  if (m_addr == MAP_FAILED) {
    close(fd);
    throw std::runtime_error("Failed to map file to memory");
  }
  close(fd);
}

file::~file() {
  if (m_addr) {
    munmap(m_addr, m_size);
  }
}

char file::get() {
  if (is_eof())
    return '\0';
  return *m_pos++;
}
