#pragma once

#include <iostream>
# define LOG_ME_THIS(msg) \
  std::cout << __FILE__ << ":" \
            << __LINE__ <<  " in " \
            << __func__ << ": " << msg << std::endl
