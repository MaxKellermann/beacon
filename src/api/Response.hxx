// SPDX-License-Identifier: BSD-2-Clause
// Copyright Max Kellermann <max.kellermann@gmail.com>

#pragma once

#include <fcgiapp.h>
#include <nlohmann/json_fwd.hpp>

void
NotFound(FCGX_Stream *out) noexcept;

void
SendResponse(FCGX_Stream *out, const nlohmann::json &root) noexcept;
