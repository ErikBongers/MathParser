#pragma once

enum class ValueType : char { NONE = '_', NUMBER = 'N', TIMEPOINT = 'T', DURATION = 'D', CALENDAR = 'C', LIST = 'L', FUNCTION = 'F', FORMATTED_STRING = 'S' };

std::string to_string(ValueType type);
