#include "datapacket.h"

sDataPacket::sDataPacket()
{
    dev = new sDevData;
    pro = new sProgress();
}

sDataPacket *sDataPacket::build()
{
    static sDataPacket* sington = nullptr;
    if(sington == nullptr)
        sington = new sDataPacket();
    return sington;
}

void sDataPacket::init()
{
    dev->dt.user = "";
    dev->dt.keylength = "";
    dev->dt.paddingMode = "";
    dev->dt.paddingMode = "";
    dev->dt.iv = "";
    dev->dt.key = "";
    dev->dt.customer = "";
    dev->dt.activationCode = "";
    dev->dt.sn = "";
    dev->dt.licenseFile = "";
    dev->dt.dateEnd = "";
}
