/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <aspl/Tracer.hpp>

namespace rocvad {

// Handles trace messages from libASPL.
class Tracer : public aspl::Tracer
{
public:
    Tracer();

protected:
    bool ShouldIgnore(const Operation& operation) override;

    void Print(const char* message) override;
};

} // namespace rocvad
