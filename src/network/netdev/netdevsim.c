/* SPDX-License-Identifier: LGPL-2.1+ */

#include "netdevsim.h"

const NetDevVTable netdevsim_vtable = {
        .object_size = sizeof(NetDevSim),
        .sections = NETDEV_COMMON_SECTIONS,
        .create_type = NETDEV_CREATE_INDEPENDENT,
        .generate_mac = true,
};
