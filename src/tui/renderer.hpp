#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace visu {

class Renderer {
private:
  struct TerminalSize {
    std::uint16_t rows;
    std::uint16_t cols;
  };

  struct Cell {
    char glyph = ' ';
    std::string ansi_prefix = "\033[0m";
  };

  TerminalSize      m_frame_size;
  std::size_t       m_frame_count;
  bool              m_running;
  std::vector<Cell> m_frame_data;

  TerminalSize get_term_size();
  void         init_frame(std::string& frame);
  void         draw_frame();
  void         update_cell(std::uint16_t pos_x, std::uint16_t pos_y, char c, const std::string& color = "\033[0m");

public:
  Renderer() : m_frame_size({0, 0}), m_frame_count(0), m_running(false) {}

  void start();

};

}
