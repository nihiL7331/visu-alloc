#include "renderer.hpp"
#include <cstdint>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>

#ifdef _WIN32
  
#else
  #include <sys/ioctl.h>
  #include <unistd.h>
#endif

namespace visu {

void Renderer::start() {
  m_frame_size = get_term_size();
  m_frame_data = std::vector<Cell>(m_frame_size.cols * m_frame_size.rows);
  m_running = true;

  while (m_running) {
    draw_frame();
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
  }
}

#define CURSOR_00 "\033[H" // places cursor at (1,1)
#define DEFAULT_COLOR "\033[0m"
#define DEFAULT_WIN_SIZE {24, 80}

void Renderer::init_frame(std::string& frame) {
  frame += CURSOR_00;
}

void Renderer::update_cell(std::uint16_t pos_x, std::uint16_t pos_y, char c, const std::string& color) {
  if (pos_x >= 0 && pos_x < m_frame_size.cols && pos_y >= 0 && pos_y < m_frame_size.rows)
    m_frame_data[pos_y * m_frame_size.cols + pos_x] = {c, color};
}

void Renderer::draw_frame() {
  std::string frame;
  frame.reserve(m_frame_size.cols * m_frame_size.rows * 10);

  init_frame(frame);

  std::string curr_col = DEFAULT_COLOR;

  for (std::uint16_t pos_y = 0; pos_y < m_frame_size.rows; ++pos_y) {
    for (std::uint16_t pos_x = 0; pos_x < m_frame_size.cols; ++pos_x) {
      const Cell& cell = m_frame_data[pos_y * m_frame_size.cols + pos_x];

      if (cell.ansi_prefix != curr_col) {
        frame += cell.ansi_prefix;
        curr_col = cell.ansi_prefix;
      }

      frame += cell.glyph;
    }

    frame += '\n';
  }

  std::cout << frame << std::flush;
}

Renderer::TerminalSize Renderer::get_term_size() {
#ifdef _WIN32
  std::cout << "unimplemented" << std::endl;
#else
  struct winsize w;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
    return {
      .rows = 24,
      .cols = 80,
    };
  }

  return {
    .rows = w.ws_row,
    .cols = w.ws_col,
  };
#endif
}

}
