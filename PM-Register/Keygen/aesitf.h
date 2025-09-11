#ifndef AESITF_H
#define AESITF_H

#include <QByteArray>
#include "qaesencryption.h"

// AES 数据结构
struct sAesItf
{
    QAESEncryption::Aes level;       // AES密钥长度
    QAESEncryption::Mode mode;       // 加密模式
    QAESEncryption::Padding padding; // 填充方式
    QByteArray key;                  // 密钥
    QByteArray iv;                   // 偏移量

    sAesItf()
        : level(QAESEncryption::AES_128)
        , mode(QAESEncryption::ECB)
        , padding(QAESEncryption::PKCS7)
    {
        key = QByteArray::fromHex("00112233445566778899aabbccddeeff");
        iv  = QByteArray::fromHex("");
    }
};

// AES 辅助函数
namespace AESHelper {

inline QString levelToStr(QAESEncryption::Aes level) {
    switch (level) {
    case QAESEncryption::AES_128: return "AES-128";
    case QAESEncryption::AES_192: return "AES-192";
    case QAESEncryption::AES_256: return "AES-256";
    default: return "Unknown";
    }
}

inline QString modeToStr(QAESEncryption::Mode mode) {
    switch (mode) {
    case QAESEncryption::ECB: return "ECB";
    case QAESEncryption::CBC: return "CBC";
    case QAESEncryption::CFB: return "CFB";
    case QAESEncryption::OFB: return "OFB";
    default: return "Unknown";
    }
}

inline QString paddingToStr(QAESEncryption::Padding padding) {
    switch (padding) {
    case QAESEncryption::ZERO:  return "ZeroPadding";
    case QAESEncryption::PKCS7: return "PKCS7";
    default: return "Unknown";
    }
}

} // namespace AESHelper

#endif // AESITF_H
