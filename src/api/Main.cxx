// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright Max Kellermann <max.kellermann@gmail.com>

#include "Handler.hxx"
#include "pg/Connection.hxx"
#include "util/PrintException.hxx"
#include "config.h"

#ifdef HAVE_LIBSYSTEMD
#include <systemd/sd-daemon.h>
#endif

#include <fastcgi.h>
#include <fcgiapp.h>

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

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
		HandleRequest(db, request.in, request.out, request.err,
			      request.envp);

	return EXIT_SUCCESS;
} catch (...) {
	PrintException(std::current_exception());
	return EXIT_FAILURE;
}
