#pragma once

enum class ValueType : char { NONE = '_', NUMBER = 'N', TIMEPOINT = 'T', DURATION = 'D', CALENDAR = 'C' };

std::string to_string(ValueType type);
