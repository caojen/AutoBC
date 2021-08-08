#pragma once

#include "error.hpp"

namespace autobc {

    class file_not_valid: public ltl::not_a_ltl {
        virtual char const * what() const noexcept {
          return "Not a valid File";
        }
    };

}