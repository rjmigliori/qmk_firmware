#include "communication.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "../../util_comm.h"
#include <wchar.h>

Communication::Communication()
{
    if (hid_init() != 0)
    {
        std::cerr << "Error: Unable to run hid_init()" << std::endl;
        exit(-1);
    };
}

Communication::~Communication()
{
    hid_exit();
}

std::vector<std::string> Communication::scan()
{
    QMutexLocker locker(&mutex);
    std::vector<std::string> ret;
    //printf("Scanning\n");
    hid_device_info *enu = hid_enumerate(0x0481, 0x0002);
    hid_device_info *devinfo = enu;
    while (devinfo != NULL)
    {
        if ((NULL != devinfo->manufacturer_string) &&
            (NULL != devinfo->product_string) &&
            (0==wcscmp(devinfo->manufacturer_string, L"Tom Wong-Cornall")) &&
            (0==wcscmp(devinfo->product_string, L"ibm-capsense-usb")))
        {
            if (devinfo->interface_number == 1)
            {
                ret.push_back(std::string(devinfo->path) + XWHATSIT_ENDING_STRING);
            }
        } else {
            if (devinfo->interface_number == 1)
            {
                ret.push_back(devinfo->path);
            }
        }
        devinfo = devinfo->next;
    }
    hid_free_enumeration(enu);
    return ret;
}

Device *Communication::open(std::string path)
{
    return new Device(path, mutex);
}
