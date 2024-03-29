// SPDX-License-Identifier: BSD-2-Clause
// Copyright CM4all GmbH
// author: Max Kellermann <mk@cm4all.com>

#ifndef EVENT_LOOP_HXX
#define EVENT_LOOP_HXX

#include "Features.h"
#include "Chrono.hxx"
#include "TimerWheel.hxx"
#include "system/EpollFD.hxx"
#include "time/ClockCache.hxx"
#include "util/IntrusiveList.hxx"

#ifndef NO_FINE_TIMER_EVENT
#include "TimerList.hxx"
#endif // NO_FINE_TIMER_EVENT

#ifndef NDEBUG
#include "util/BindMethod.hxx"
#endif

class DeferEvent;
class SocketEvent;

/**
 * A non-blocking I/O event loop.
 */
class EventLoop final
{
	EpollFD poll_backend;

	TimerWheel coarse_timers;

#ifndef NO_FINE_TIMER_EVENT
	TimerList timers;
#endif // NO_FINE_TIMER_EVENT

	using DeferList = IntrusiveList<DeferEvent>;

	DeferList defer;

	/**
	 * This is like #defer, but gets invoked when the loop is idle.
	 */
	DeferList idle;

	using SocketList = IntrusiveList<SocketEvent>;

	/**
	 * A list of scheduled #SocketEvent instances, without those
	 * which are ready (these are in #ready_sockets).
	 */
	SocketList sockets;

	/**
	 * A list of #SocketEvent instances which have a non-zero
	 * "ready_flags" field and need to be dispatched.
	 */
	SocketList ready_sockets;

#ifndef NDEBUG
	using PostCallback = BoundMethod<void() noexcept>;
	PostCallback post_callback = nullptr;
#endif

	bool quit;

	/**
	 * True when the object has been modified and another check is
	 * necessary before going to sleep via PollGroup::ReadEvents().
	 */
	bool again;

	ClockCache<std::chrono::steady_clock> steady_clock_cache;
	ClockCache<std::chrono::system_clock> system_clock_cache;

public:
	EventLoop();
	~EventLoop() noexcept;

	EventLoop(const EventLoop &other) = delete;
	EventLoop &operator=(const EventLoop &other) = delete;

#ifndef NDEBUG
	/**
	 * Set a callback function which will be invoked each time an even
	 * has been handled.  This is debug-only and may be used to inject
	 * regular debug checks.
	 */
	void SetPostCallback(PostCallback new_value) noexcept {
		post_callback = new_value;
	}
#endif

	const auto &GetSteadyClockCache() const noexcept {
		return steady_clock_cache;
	}

	const auto &GetSystemClockCache() const noexcept {
		return system_clock_cache;
	}

	/**
	 * Caching wrapper for std::chrono::steady_clock::now().  The
	 * real clock is queried at most once per event loop
	 * iteration, because it is assumed that the event loop runs
	 * for a negligible duration.
	 */
	[[gnu::pure]]
	const auto &SteadyNow() const noexcept {
		return steady_clock_cache.now();
	}

	/**
	 * Caching wrapper for std::chrono::system_clock::now().  The
	 * real clock is queried at most once per event loop
	 * iteration, because it is assumed that the event loop runs
	 * for a negligible duration.
	 */
	[[gnu::pure]]
	const auto &SystemNow() const noexcept {
		return system_clock_cache.now();
	}

	void FlushClockCaches() noexcept {
		steady_clock_cache.flush();
		system_clock_cache.flush();
	}

	void Break() noexcept {
		quit = true;
	}

	bool IsEmpty() const noexcept {
		return coarse_timers.IsEmpty() &&
#ifndef NO_FINE_TIMER_EVENT
			timers.IsEmpty() &&
#endif // NO_FINE_TIMER_EVENT
			defer.empty() && idle.empty() &&
			sockets.empty() && ready_sockets.empty();
	}

	bool AddFD(int fd, unsigned events, SocketEvent &event) noexcept;
	bool ModifyFD(int fd, unsigned events, SocketEvent &event) noexcept;
	bool RemoveFD(int fd, SocketEvent &event) noexcept;

	/**
	 * Remove the given #SocketEvent after the file descriptor
	 * has been closed.  This is like RemoveFD(), but does not
	 * attempt to use #EPOLL_CTL_DEL.
	 */
	void AbandonFD(SocketEvent &event) noexcept;

	void Insert(CoarseTimerEvent &t) noexcept;

#ifndef NO_FINE_TIMER_EVENT
	void Insert(FineTimerEvent &t) noexcept;
#endif // NO_FINE_TIMER_EVENT

	void AddDefer(DeferEvent &e) noexcept;
	void AddIdle(DeferEvent &e) noexcept;

	void Run() noexcept;

private:
	void RunDeferred() noexcept;

	/**
	 * Invoke one "idle" #DeferEvent.
	 *
	 * @return false if there was no such event
	 */
	bool RunOneIdle() noexcept;

	/**
	 * Invoke all expired #TimerEvent instances and return the
	 * duration until the next timer expires.  Returns a negative
	 * duration if there is no timeout.
	 */
	Event::Duration HandleTimers() noexcept;

	/**
	 * Call epoll_wait() and pass all returned events to
	 * SocketEvent::SetReadyFlags().
	 *
	 * @return true if one or more sockets have become ready
	 */
	bool Wait(Event::Duration timeout) noexcept;

	void RunPost() noexcept {
#ifndef NDEBUG
		if (post_callback)
			post_callback();
#endif
	}
};

#endif
