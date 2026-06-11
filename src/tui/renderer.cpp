#include "renderer.hpp"
#include <cstdint>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <csignal>

#ifdef _WIN32
  
#else
  #include <sys/ioctl.h>
  #include <unistd.h>
#endif

namespace visu {

volatile sig_atomic_t g_terminal_resized = false;

#define ANSI_CURSOR_00 "\033[H" // places cursor at (1,1)
#define ANSI_CURSOR_HIDE "\033[?25l"
#define ANSI_CURSOR_SHOW "\033[?25h"
#define ANSI_CLEAR_SCREEN "\033[2J"
#define ANSI_DEFAULT_COLOR "\033[0m"
#define DEFAULT_WIN_SIZE {24, 80}

void sigwinch_handler(int) {
  g_terminal_resized = true;
}

void Renderer::start() {
  m_frame_size = get_term_size();
  m_frame_data = std::vector<Cell>(m_frame_size.cols * m_frame_size.rows);
  m_running = true;

#ifndef _WIN32
  std::signal(SIGWINCH, sigwinch_handler);
#endif

  std::cout << ANSI_CURSOR_HIDE << ANSI_CLEAR_SCREEN << std::flush;

  while (m_running) {
#ifndef _WIN32
    if (g_terminal_resized) {
      m_frame_size = get_term_size();
      m_frame_data.assign(m_frame_size.cols * m_frame_size.rows, Cell{});

      std::cout << ANSI_CLEAR_SCREEN << std::flush;
    }
#endif


    draw_frame();
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
  }

  std::cout << ANSI_CURSOR_SHOW << std::flush;
}

void Renderer::init_frame(std::string& frame) {
  frame += ANSI_CURSOR_00;
}

void Renderer::update_cell(std::uint16_t pos_x, std::uint16_t pos_y, char c, const std::string& color) {
  if (pos_x < m_frame_size.cols && pos_y < m_frame_size.rows)
    m_frame_data[pos_y * m_frame_size.cols + pos_x] = {c, color};
}

void Renderer::draw_frame() {
  std::string frame;
  frame.reserve(m_frame_size.cols * m_frame_size.rows * 10);

  init_frame(frame);

  std::string curr_col = ANSI_DEFAULT_COLOR;

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

  frame += ANSI_DEFAULT_COLOR;
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
