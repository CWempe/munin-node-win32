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
#include "IfMuninNodePlugin.h"

IfMuninNodePlugin::IfMuninNodePlugin()
{

}

IfMuninNodePlugin::~IfMuninNodePlugin()
{

}

int IfMuninNodePlugin::GetConfig(char *buffer, int len)
{
  int ret = 0;
  ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO)*100; // hope nowhere are more adapters than 100
  PIP_ADAPTER_INFO pAdapterInfo = new IP_ADAPTER_INFO[sizeof (IP_ADAPTER_INFO)];
  PIP_ADAPTER_INFO pAdapter = NULL;
  DWORD dwRetVal = 0;

  ret = _snprintf(buffer, len, "graph_title network traffic\n"
    "graph_args --base 1000\n"
    "graph_vlabel bits in (-) / out (+) per ${graph_period}\n"
    "graph_category network\n"
    "graph_info This graph shows the traffic of the network interface. Please note that the traffic is shown in bits per second, not bytes. "
    // "IMPORTANT: Since the data source for this plugin use 32bit counters, this plugin is really unreliable and unsuitable for most 100Mb (or faster) interfaces, where bursts are expected to exceed 50Mbps. This means that this plugin is usuitable for most production environments. To avoid this problem, use the ip_ plugin instead."
    "\n");
  buffer += ret;
  len -= ret;
  if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
    pAdapter = pAdapterInfo;
    while (pAdapter) {
      ret = _snprintf(buffer, len,  "down_%d.label receive\n"
        "down_%d.type COUNTER\n"
        "down_%d.graph no\n"
        "down_%d.cdef down_%d,8,*\n"
        "up_%d.label %s\n"
        "up_%d.type COUNTER\n"
        "up_%d.negative down_%d\n"
        "up_%d.cdef up_%d,8,*\n",
        pAdapter->Index, pAdapter->Index, pAdapter->Index, pAdapter->Index, pAdapter->Index,
        pAdapter->Index, pAdapter->Description,
        pAdapter->Index, pAdapter->Index, pAdapter->Index, pAdapter->Index,	pAdapter->Index);
      buffer += ret;
      len -= ret;
/*
      PMIB_IFROW pMibIfRow;
      pMibIfRow = (MIB_IFROW*) malloc(sizeof(MIB_IFROW));
      pMibIfRow->dwIndex = pAdapter->Index;
      if (GetIfEntry(pMibIfRow) == NO_ERROR) {
      printf("%s\n", pMibIfRow->bDescr);
      }
      free(pMibIfRow);
*/
      pAdapter = pAdapter->Next;
    }
  }
  if (pAdapterInfo)
    delete [] pAdapterInfo;

  strncat(buffer, ".\n", len);
  return 0;
};

int IfMuninNodePlugin::GetValues(char *buffer, int len)
{
  int ret = 0;
  ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO)*100; // hope nowhere are more adapters than 100
  PIP_ADAPTER_INFO pAdapterInfo = new IP_ADAPTER_INFO[sizeof (IP_ADAPTER_INFO)];
  PIP_ADAPTER_INFO pAdapter = NULL;
  DWORD dwRetVal = 0;

  if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
    pAdapter = pAdapterInfo;
    while (pAdapter) {
      PMIB_IFROW pMibIfRow;
      pMibIfRow = (MIB_IFROW*) malloc(sizeof(MIB_IFROW));
      pMibIfRow->dwIndex = pAdapter->Index;
      if (GetIfEntry(pMibIfRow) == NO_ERROR) {
        ret = _snprintf(buffer, len,  "down_%d.value %u\n"
          "up_%d.value %u\n",
          pAdapter->Index, pMibIfRow->dwInOctets,
          pAdapter->Index, pMibIfRow->dwOutOctets);
        buffer += ret;
        len -= ret;
      }
      free(pMibIfRow);
      pAdapter = pAdapter->Next;
    }
  }
  if (pAdapterInfo)
    delete [] pAdapterInfo;
  strncat(buffer, ".\n", len);
  return 0;
};
