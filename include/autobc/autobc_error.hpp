#pragma once

#include "error.hpp"

namespace autobc {

    class file_not_valid: public ltl::not_a_ltl {
        virtual char const * what() const noexcept {
          return "Not a valid File";
        }
    };

    class output_line_too_less: public std::exception {
        virtual char const * what() const noexcept {
          return "Likelyhood.jar return too less lines";
        }
    };

    class output_line_format_error: public std::exception {
        virtual char const * what() const noexcept {
          return "Line Format Unknown";
        }
    };
}
