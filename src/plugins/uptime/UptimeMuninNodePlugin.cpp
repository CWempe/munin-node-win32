/* This file is part of munin-node-win32
 * Copyright (C) 2006-2007 Jory Stone (jcsston@jory.info)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "StdAfx.h"
#include "UptimeMuninNodePlugin.h"

DWORDLONG GetTickCount_64(void)
{
  // Use highres timer for all operations on Windows
  // The Timer starts at system boot and runs (on a Intel Quad core)
  // with 14 million ticks per second. So it won't overflow for
  // 35000 years.

  // Convert hires ticks to milliseconds
  static double tickFactor;
  _LARGE_INTEGER li;

  static bool first = true;
  if (first) {
    // calculate the conversion factor for the highres timer
    QueryPerformanceFrequency(&li);
    tickFactor = 1000.0 / li.QuadPart;
    first = false;
  }

  QueryPerformanceCounter(&li);
  return (DWORDLONG)(li.QuadPart * tickFactor);
}

UptimeMuninNodePlugin::UptimeMuninNodePlugin()
{
  GetTickCount64 = (PROCTICKCOUNT)GetProcAddress(GetModuleHandle(_T("Kernel32")), "GetTickCount64");
  if (!GetTickCount64) {
    GetTickCount64 = (PROCTICKCOUNT)GetTickCount_64;
  }
}

UptimeMuninNodePlugin::~UptimeMuninNodePlugin()
{

}

int UptimeMuninNodePlugin::GetConfig(char *buffer, int len) 
{
  strncpy(buffer, "graph_title Uptime\n"
    "graph_category system\n"
    "graph_args --base 1000 -l 0\n"
    "graph_vlabel uptime in days\n"
    "uptime.label uptime\n"
    "uptime.draw AREA\n"
    ".\n", len);

  return 0;
}

int UptimeMuninNodePlugin::GetValues(char *buffer, int len)
{ 
  DWORDLONG tick = GetTickCount64();
  double napok = tick / 86400000.0f ; // 86400 * 1000
  _snprintf(buffer, len, "uptime.value %.2f\n.\n", napok);
  return 0;
}
