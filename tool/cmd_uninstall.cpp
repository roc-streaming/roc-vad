/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_uninstall.hpp"
#include "build_info.hpp"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <unistd.h>
#include <filesystem>
#include <string>

using namespace rocvad;

CmdUninstall::CmdUninstall(CLI::App& parent)
{
    register_command(parent.add_subcommand("uninstall", "uninstall driver and tool"));
}

bool CmdUninstall::execute(const Environment& env)
{
    if (geteuid() != 0) {
        spdlog::error("should be run with root privileges\ntry \"sudo {} uninstall\"",
            BuildInfo::tool_name);
        return false;
    }

    int n_ok = 0, n_warn = 0, n_err = 0;

    for (const std::string path : {BuildInfo::driver_bundle_path, BuildInfo::tool_path}) {
        if (path == BuildInfo::tool_path && n_err != 0) {
            spdlog::warn(
                "can't remove \"{}\": Skipping because of previois errors", path);
            n_warn++;
            continue;
        }

        std::error_code err;
        const auto n_removed = std::filesystem::remove_all(path, err);

        if (n_removed != 0 && !err) {
            fmt::println("removed {}", path);
            n_ok++;
        } else if (n_removed == 0 && !err) {
            spdlog::warn("can't remove \"{}\": Not found", path);
            n_warn++;
        } else if (err) {
            spdlog::error("can't remove \"{}\": {}", path, err.message());
            n_err++;
        }
    }

    if (n_ok == 0) {
        fmt::println("nothing removed");
    } else if (n_err == 0) {
        fmt::println("everything removed");
    } else {
        fmt::println("completed with errors");
    }

    return (n_err == 0);
}
