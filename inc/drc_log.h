#ifndef DRC_LOG_H
#define DRC_LOG_H

#include "drc_log_cfg.h"

#if DRC_LOG_ERROR_EN
  #define DRC_LOG_ERROR(...) fprintf(stderr, __VA_ARGS__);
#else
  #define DRC_LOG_ERROR(...)
#endif

#if DRC_LOG_INFO_EN
  #define DRC_LOG_INFO(...) printf(__VA_ARGS__);
#else
  #define DRC_LOG_INFO(...)
#endif

#if DRC_LOG_DEBUG_EN
  #define DRC_LOG_DEBUG(...) printf(__VA_ARGS__);
#else
  #define DRC_LOG_DEBUG(...)
#endif

#endif
