#include "Config.h"

namespace cli {

const std::string Config::short_prefix = "-";

const std::string Config::long_prefix = "--";

const std::string Config::long_arg_start = "=";

const std::string Config::terminator = "--";

const unsigned Config::name_offset = 4;

const unsigned Config::description_offset = 12;

}; // namespace cli;
