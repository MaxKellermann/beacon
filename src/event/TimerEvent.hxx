// SPDX-License-Identifier: BSD-2-Clause
// Copyright CM4all GmbH
// author: Max Kellermann <max.kellermann@ionos.com>

#pragma once

#include "FineTimerEvent.hxx"

/**
 * This is a transitional alias.  Use #FineTimerEvent or
 * #CoarseTimerEvent instead.
 */
using TimerEvent = FineTimerEvent;
