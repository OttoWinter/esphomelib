#ifndef ESPHOMELIB_LOG_H
#define ESPHOMELIB_LOG_H

#include <cassert>
#include <cstdarg>
#include <string>

// avoid esp-idf redefining our macros
#include "esphomelib/esphal.h"

#ifdef ARDUINO_ARCH_ESP32
  #include "esp_err.h"
#endif

#define ESPHOMELIB_LOG_LEVEL_NONE 0
#define ESPHOMELIB_LOG_LEVEL_ERROR 1
#define ESPHOMELIB_LOG_LEVEL_WARN 2
#define ESPHOMELIB_LOG_LEVEL_INFO 3
#define ESPHOMELIB_LOG_LEVEL_DEBUG 4
#define ESPHOMELIB_LOG_LEVEL_VERBOSE 5
#define ESPHOMELIB_LOG_LEVEL_VERY_VERBOSE 6

#ifndef ESPHOMELIB_LOG_LEVEL
  #define ESPHOMELIB_LOG_LEVEL ESPHOMELIB_LOG_LEVEL_DEBUG
#endif

#define ESPHOMELIB_LOG_COLOR_BLACK   "30"
#define ESPHOMELIB_LOG_COLOR_RED     "31" //ERROR
#define ESPHOMELIB_LOG_COLOR_GREEN   "32" //INFO
#define ESPHOMELIB_LOG_COLOR_YELLOW  "33" //WARNING
#define ESPHOMELIB_LOG_COLOR_BLUE    "34"
#define ESPHOMELIB_LOG_COLOR_MAGENTA "35" //CONFIG
#define ESPHOMELIB_LOG_COLOR_CYAN    "36" //DEBUG
#define ESPHOMELIB_LOG_COLOR_GRAY    "37" //VERBOSE
#define ESPHOMELIB_LOG_COLOR_WHITE   "38"

#define ESPHOMELIB_LOG_COLOR(COLOR)  "\033[0;" COLOR "m"
#define ESPHOMELIB_LOG_BOLD(COLOR)   "\033[1;" COLOR "m"

#ifndef ESPHOMELIB_LOG_NO_COLORS
  #define ESPHOMELIB_LOG_COLOR_E       ESPHOMELIB_LOG_BOLD(ESPHOMELIB_LOG_COLOR_RED)
  #define ESPHOMELIB_LOG_COLOR_W       ESPHOMELIB_LOG_COLOR(ESPHOMELIB_LOG_COLOR_YELLOW)
  #define ESPHOMELIB_LOG_COLOR_I       ESPHOMELIB_LOG_COLOR(ESPHOMELIB_LOG_COLOR_GREEN)
  #define ESPHOMELIB_LOG_COLOR_C       ESPHOMELIB_LOG_COLOR(ESPHOMELIB_LOG_COLOR_MAGENTA)
  #define ESPHOMELIB_LOG_COLOR_D       ESPHOMELIB_LOG_COLOR(ESPHOMELIB_LOG_COLOR_CYAN)
  #define ESPHOMELIB_LOG_COLOR_V       ESPHOMELIB_LOG_COLOR(ESPHOMELIB_LOG_COLOR_GRAY)
  #define ESPHOMELIB_LOG_COLOR_VV       ESPHOMELIB_LOG_COLOR(ESPHOMELIB_LOG_COLOR_WHITE)
  #define ESPHOMELIB_LOG_RESET_COLOR   "\033[0m"
#else
  #define ESPHOMELIB_LOG_COLOR_E
  #define ESPHOMELIB_LOG_COLOR_W
  #define ESPHOMELIB_LOG_COLOR_I
  #define ESPHOMELIB_LOG_COLOR_C
  #define ESPHOMELIB_LOG_COLOR_D
  #define ESPHOMELIB_LOG_COLOR_V
  #define ESPHOMELIB_LOG_COLOR_VV
  #define ESPHOMELIB_LOG_RESET_COLOR
#endif

int esp_log_printf_(int level, const char *tag, const char *format, ...) __attribute__ ((format (printf, 3, 4)));
int esp_log_vprintf_(int level, const char *tag, const char *format, va_list args);
int esp_idf_log_vprintf_(const char *format, va_list args);

#define ESPHOMELIB_SHORT_LOG_FORMAT(tag, letter, format)  ESPHOMELIB_LOG_COLOR_ ## letter format ESPHOMELIB_LOG_RESET_COLOR
#define ESPHOMELIB_LOG_FORMAT(tag, letter, format)  ESPHOMELIB_LOG_COLOR_ ## letter "[" #letter "][%s:%03u]: " format ESPHOMELIB_LOG_RESET_COLOR, tag, __LINE__

#if ESPHOMELIB_LOG_LEVEL >= ESPHOMELIB_LOG_LEVEL_VERY_VERBOSE
  #define esph_log_vv(tag, format, ...) esp_log_printf_(ESPHOMELIB_LOG_LEVEL_VERY_VERBOSE, tag, ESPHOMELIB_LOG_FORMAT(tag, VV, format), ##__VA_ARGS__)

  #define ESPHOMELIB_LOG_HAS_VERY_VERBOSE
#else
  #define esph_log_vv(tag, format, ...)
#endif

#if ESPHOMELIB_LOG_LEVEL >= ESPHOMELIB_LOG_LEVEL_VERBOSE
  #define esph_log_v(tag, format, ...) esp_log_printf_(ESPHOMELIB_LOG_LEVEL_VERBOSE, tag, ESPHOMELIB_LOG_FORMAT(tag, V, format), ##__VA_ARGS__)

  #define ESPHOMELIB_LOG_HAS_VERBOSE
#else
  #define esph_log_v(tag, format, ...)
#endif

#if ESPHOMELIB_LOG_LEVEL >= ESPHOMELIB_LOG_LEVEL_DEBUG
  #define esph_log_d(tag, format, ...) esp_log_printf_(ESPHOMELIB_LOG_LEVEL_DEBUG, tag, ESPHOMELIB_LOG_FORMAT(tag, D, format), ##__VA_ARGS__)

  #define esph_log_config(tag, format, ...) esp_log_printf_(ESPHOMELIB_LOG_LEVEL_DEBUG, tag, ESPHOMELIB_LOG_FORMAT(tag, C, format), ##__VA_ARGS__)

  #define ESPHOMELIB_LOG_HAS_DEBUG
  #define ESPHOMELIB_LOG_HAS_CONFIG
#else
  #define esph_log_d(tag, format, ...)

  #define esph_log_config(tag, format, ...)
#endif

#if ESPHOMELIB_LOG_LEVEL >= ESPHOMELIB_LOG_LEVEL_INFO
  #define esph_log_i(tag, format, ...) esp_log_printf_(ESPHOMELIB_LOG_LEVEL_INFO, tag, ESPHOMELIB_LOG_FORMAT(tag, I, format), ##__VA_ARGS__)

  #define ESPHOMELIB_LOG_HAS_INFO
#else
  #define esph_log_i(tag, format, ...)
#endif

#if ESPHOMELIB_LOG_LEVEL >= ESPHOMELIB_LOG_LEVEL_WARN
  #define esph_log_w(tag, format, ...) esp_log_printf_(ESPHOMELIB_LOG_LEVEL_WARN, tag, ESPHOMELIB_LOG_FORMAT(tag, W, format), ##__VA_ARGS__)

  #define ESPHOMELIB_LOG_HAS_WARN
#else
  #define esph_log_w(tag, format, ...)
#endif

#if ESPHOMELIB_LOG_LEVEL >= ESPHOMELIB_LOG_LEVEL_ERROR
  #define esph_log_e(tag, format, ...) esp_log_printf_(ESPHOMELIB_LOG_LEVEL_ERROR, tag, ESPHOMELIB_LOG_FORMAT(tag, E, format), ##__VA_ARGS__)

  #define ESPHOMELIB_LOG_HAS_ERROR
#else
  #define esph_log_e(tag, format, ...)
#endif

#ifdef ESP_LOGE
  #undef ESP_LOGE
#endif
#ifdef ESP_LOGW
  #undef ESP_LOGW
#endif
#ifdef ESP_LOGI
  #undef ESP_LOGI
#endif
#ifdef ESP_LOGD
  #undef ESP_LOGD
#endif
#ifdef ESP_LOGV
  #undef ESP_LOGV
#endif

#define ESP_LOGE(tag, ...)  esph_log_e(tag, __VA_ARGS__)
#define ESP_LOGW(tag, ...)  esph_log_w(tag, __VA_ARGS__)
#define ESP_LOGI(tag, ...)  esph_log_i(tag, __VA_ARGS__)
#define ESP_LOGD(tag, ...)  esph_log_d(tag, __VA_ARGS__)
#define ESP_LOGV(tag, ...)  esph_log_v(tag, __VA_ARGS__)
#define ESP_LOGVV(tag, ...)  esph_log_vv(tag, __VA_ARGS__)
#define ESP_LOGCONFIG(tag, ...)  esph_log_config(tag, __VA_ARGS__)

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0')
#define YESNO(b) ((b) ? "YES" : "NO")
#define ONOFF(b) ((b) ? "ON" : "OFF")

#endif //ESPHOMELIB_LOG_H
