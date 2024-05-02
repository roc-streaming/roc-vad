/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <CLI/CLI.hpp>

namespace rocvad {

class HelpFormatter : public CLI::Formatter
{
public:
    HelpFormatter();

    std::string make_expanded(const CLI::App* sub) const override;
};

} // namespace rocvad
