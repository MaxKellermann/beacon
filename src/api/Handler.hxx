// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright Max Kellermann <max.kellermann@gmail.com>

#pragma once

#include <fcgiapp.h>

namespace Beacon { class ApiDatabase; }

void
HandleRequest(Beacon::ApiDatabase &db,
	      FCGX_Stream *in, FCGX_Stream *out, FCGX_Stream *err,
	      FCGX_ParamArray envp) noexcept;
