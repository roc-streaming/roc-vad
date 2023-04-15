/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "parse.hpp"

#include <spdlog/spdlog.h>

#include <limits>
#include <stdexcept>

namespace rocvad {

bool parse_index(const std::string& in, index_t& out)
{
    try {
        const long value = std::stol(in);

        if (value <= 0 || value > std::numeric_limits<uint32_t>::max()) {
            spdlog::error("invalid device index \"{}\": out of range", in);
            return false;
        }

        out = (index_t)value;
        return true;
    }
    catch (std::exception& e) {
        spdlog::error("invalid device index \"{}\": not a number", in);
        return false;
    }
}

} // namespace rocvad
