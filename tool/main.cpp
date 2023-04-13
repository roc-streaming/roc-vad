/*
 * Copyright (c) Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cmd_root.hpp"

#include <grpc/impl/codegen/log.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <CLI11.hpp>

#include <cstdlib>
#include <memory>

using namespace rocvad;

namespace {

void spdlog_init(const Environment& env)
{
    auto sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>(env.color_mode);
    sink->set_color(spdlog::level::critical, sink->red);
    sink->set_color(spdlog::level::err, sink->red);
    sink->set_color(spdlog::level::warn, sink->yellow);
    sink->set_color(spdlog::level::info, sink->green);
    sink->set_color(spdlog::level::debug, sink->white);
    sink->set_color(spdlog::level::trace, sink->white);

    auto logger = std::make_shared<spdlog::logger>("console", sink);
    spdlog::set_default_logger(logger);

    spdlog::set_level(env.log_level);
    spdlog::set_pattern("%^%l:%$ %v");
}

void grpc_init()
{
    gpr_set_log_verbosity(GPR_LOG_SEVERITY_DEBUG);
    gpr_set_log_function([](gpr_log_func_args* args) {
        spdlog::log(spdlog::level::trace,
            "(gpr:{}) {}",
            args->severity == GPR_LOG_SEVERITY_ERROR  ? "e"
            : args->severity == GPR_LOG_SEVERITY_INFO ? "i"
                                                      : "d",
            std::string(args->message));
    });
}

} // namespace

int main(int argc, char** argv)
{
    CLI::App app{"Roc Virtual Audio Device control tool"};

    int verbosity = 0;
    bool force_color = false, force_no_color = false;

    app.add_flag("-v,--verbose",
        verbosity,
        "increase verbosity level (may be used multiple times)");

    app.add_flag("-C", force_color, "always use colored output");
    app.add_flag("-n", force_no_color, "never use colored output");

    auto cmd = std::make_shared<CmdRoot>(app);

    try {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError& e) {
        return app.exit(e);
    }

    if (!force_color) {
        if (auto str = getenv("FORCE_COLOR"); str && atoi(str) > 0) {
            force_color = true;
        }
    }

    if (!force_no_color) {
        if (auto str = getenv("NO_COLOR"); str && *str) {
            force_no_color = true;
        }
    }

    Environment env;

    switch (verbosity) {
    case 0:
        env.log_level = spdlog::level::warn;
        break;
    case 1:
        env.log_level = spdlog::level::info;
        break;
    case 2:
        env.log_level = spdlog::level::debug;
        break;
    default:
        env.log_level = spdlog::level::trace;
        break;
    }

    env.color_mode = force_no_color ? spdlog::color_mode::never
                     : force_color  ? spdlog::color_mode::always
                                    : spdlog::color_mode::automatic;

    spdlog_init(env);
    grpc_init();

    const int code = cmd->execute(env) ? EXIT_SUCCESS : EXIT_FAILURE;

    cmd.reset();

    spdlog::shutdown();

    return code;
}
