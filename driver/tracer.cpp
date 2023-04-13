/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "tracer.hpp"

#include <spdlog/spdlog.h>

namespace rocvad {

Tracer::Tracer()
    : aspl::Tracer(Mode::Custom, Style::Hierarchical)
{
}

bool Tracer::ShouldIgnore(const Operation& operation)
{
    return operation.Flags & Flags::Readonly;
}

void Tracer::Print(const char* message)
{
    spdlog::trace("{}", std::string(message));
}

} // namespace rocvad
