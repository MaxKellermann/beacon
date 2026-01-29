// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright Max Kellermann <max.kellermann@gmail.com>

#pragma once

#include <fcgiapp.h>

namespace Beacon { class ApiDatabase; }

void
HandleGPX(Beacon::ApiDatabase &db,
	  const char *path_info,
	  FCGX_Stream *out, FCGX_ParamArray envp) noexcept;
