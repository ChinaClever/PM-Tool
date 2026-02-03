#include "powersystemdata.h"
#include <QMutex>
#include <QJsonObject>

extern PowerSystemData m_systemData;
extern void debugPrintSystemData(PowerSystemData&) ;
/* MPro批量发送相关 */
extern QMap<QString, PowerSystemData> AMap;
extern QMap<QString, PowerSystemData> BMap;
extern QMap<QString, PowerSystemData> CMap;
extern QMap<QString, PowerSystemData> DMap;

extern QVector<QQueue<QJsonObject>> ProBulkJQs;     // Processing-Bulk JSON Queues
extern QMutex ProBulkJQMutexes[4];

extern int mp_sendTime;

extern void envInc(PowerSystemData&);
extern void bitInc(PowerSystemData&);

extern int MpCnt;
extern int MpCntt;
extern int MpCntEr;
extern int Anum,Bnum,Cnum,Dnum;
