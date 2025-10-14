#ifndef FILE_H
#define FILE_H

#include <string_view>

class file {
public:
  explicit file(std::string_view path);
  explicit file(char *data, size_t size)
      : m_addr(data), m_pos(data), m_size(size) {}
  ~file();

  bool is_eof() const { return m_pos >= m_addr + m_size; }
  char peek() const { return is_eof() ? '\0' : *m_pos; }
  char get();
  char *data() const { return m_pos; }

private:
  char *m_addr = nullptr;
  char *m_pos = nullptr;
  size_t m_size = 0;
};

#endif // FILE_H