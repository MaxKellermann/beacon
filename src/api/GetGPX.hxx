// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright Max Kellermann <max.kellermann@gmail.com>

#pragma once

#include <fcgiapp.h>

namespace Pg { class Connection; }

void
HandleGPX(Pg::Connection &db,
	  const char *path_info,
	  FCGX_Stream *out, FCGX_ParamArray envp) noexcept;
