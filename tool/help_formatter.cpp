/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "help_formatter.hpp"

#include <sstream>

namespace rocvad {

HelpFormatter::HelpFormatter()
{
    label("REQUIRED", "[required]");
}

std::string HelpFormatter::make_expanded(const CLI::App* sub) const
{
    std::stringstream out;

    out << "\n" << sub->get_group() << ":\n";
    for (const CLI::Option* opt : sub->get_options()) {
        if (opt == sub->get_help_ptr() || opt == sub->get_help_all_ptr()) {
            continue;
        }
        if (opt->get_group().empty()) {
            continue;
        }
        out << make_option(opt, false);
    }

    return out.str();
}

} // namespace rocvad
