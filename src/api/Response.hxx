// SPDX-License-Identifier: BSD-2-Clause
// Copyright Max Kellermann <max.kellermann@gmail.com>

#pragma once

#include <fcgiapp.h>

namespace Json { class Value; }

void
NotFound(FCGX_Stream *out) noexcept;

void
SendResponse(FCGX_Stream *out, const Json::Value &root) noexcept;
