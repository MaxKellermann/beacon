// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2019 Max Kellermann <max.kellermann@gmail.com>,
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "pg/Connection.hxx"
#include "util/PrintException.hxx"
#include "util/StringCompare.hxx"
#include "config.h"

#ifdef HAVE_LIBSYSTEMD
#include <systemd/sd-daemon.h>
#endif

#include <fastcgi.h>
#include <fcgiapp.h>

#include <string>

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

static void
NotFound(FCGX_Stream *out) noexcept
{
	FCGX_PutS("Status: 404 Not Found\n"
		  "Content-Type: text/plain\n"
		  "\n"
		  "Not found\n", out);
}

static void
Run(Pg::Connection &db,
    FCGX_Stream *in, FCGX_Stream *out, FCGX_Stream *err,
    FCGX_ParamArray envp) noexcept
{
	(void)in;
	(void)err;
	(void)envp;

	const char *const path_info = FCGX_GetParam("PATH_INFO", envp);
	if (path_info == nullptr) {
		NotFound(out);
		return;
	}

	char *endptr;
	const uint64_t key = strtoull(path_info, &endptr, 10);
	if (endptr == path_info || (*endptr != 0 && !StringIsEqual(endptr, ".gpx"))) {
		NotFound(out);
		return;
	}

	auto result = db.ExecuteParams(false,
				       "SELECT ST_AsText(location) FROM fixes WHERE key=$1 ORDER BY time LIMIT 16384",
				       key);
	FCGX_PutS("Content-Type: application/gpx+xml\n"
		  "\n"
		  "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
		  "<gpx xmlns=\"http://www.topografix.com/GPX/1/1\"\n"
		  "  creator=\"beacon\" version=\"1.0\">\n"
		  "<trk><trkseg>\n",
		  out);

	for (const auto &row : result) {
		const char *wkt = row.GetValue(0);
		const char *p = StringAfterPrefix(wkt, "POINT(");
		if (p == nullptr)
			continue;

		double longitude = strtod(p, &endptr);
		if (endptr == p || *endptr != ' ')
			continue;

		p = endptr + 1;
		double latitude = strtod(p, &endptr);
		if (endptr == p || *endptr != ')')
			continue;

		FCGX_FPrintF(out,
			     "<trkpt lat=\"%f\" lon=\"%f\"></trkpt>\n",
			     latitude, longitude);

	}

	FCGX_PutS("</trkseg></trk></gpx>\n", out);
}

int
main(int, const char *const*) noexcept
try {
#ifdef HAVE_LIBSYSTEMD
	/* support systemd socket activation by copying systemd's fd
	   to stdin */
	int n = sd_listen_fds(true);
	if (n > 0) {
		if (n != 1)
			throw "Too many systemd fds";

		if (dup3(SD_LISTEN_FDS_START, FCGI_LISTENSOCK_FILENO, O_CLOEXEC) < 0)
			throw "dup2() failed";
		close(SD_LISTEN_FDS_START);
	}
#endif

	const char *_db = "dbname=beacon"; // TODO make configurable
	Pg::Connection db(_db);

	FCGX_Init();
	FCGX_Request request;
	FCGX_InitRequest(&request, 0, 0);

#ifdef HAVE_LIBSYSTEMD
	/* tell systemd we're ready */
	sd_notify(0, "READY=1");
#endif

	while (FCGX_Accept_r(&request) == 0)
		Run(db, request.in, request.out, request.err, request.envp);

	return EXIT_SUCCESS;
} catch (...) {
	PrintException(std::current_exception());
	return EXIT_FAILURE;
}
