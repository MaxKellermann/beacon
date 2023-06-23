// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright Max Kellermann <max.kellermann@gmail.com>

#pragma once

#include <fcgiapp.h>

namespace Pg { class Connection; }

void
HandleRequest(Pg::Connection &db,
	      FCGX_Stream *in, FCGX_Stream *out, FCGX_Stream *err,
	      FCGX_ParamArray envp) noexcept;
