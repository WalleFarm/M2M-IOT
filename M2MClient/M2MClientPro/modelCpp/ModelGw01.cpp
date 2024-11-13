#include "ModelGw01.h"

ModelGw01::ModelGw01(QObject *parent) : BaseModel(parent)
{
    m_secTickets=0;
    checkTimer = new QTimer(this);
    checkTimer->setInterval(1*1000);//心跳检测
    checkTimer->start(); 
    connect(checkTimer, SIGNAL(timeout()),this,SLOT(slotCheckTimeout())); 
}

void ModelGw01::slotCheckTimeout(void)
{
        m_secTickets++;
        if(m_onlineState>0)//离线检测
        {
            if(m_secTickets - m_onlineTime>40)
            {
                m_onlineState=DEV_STATE_OFF_LINE;       
            }
            else
            {
                m_onlineState=DEV_STATE_ON_LINE;
            }
            emit siqUpdateOnlineState(m_onlineState);   
        }

}

QByteArray ModelGw01::takeModelPassword(u8 index)
{
    static u8 passwd_table[5][16]={
        0x9D, 0x53, 0x09, 0xBF, 0x75, 0x28, 0xDE, 0x94, 0x4A, 0xFD, 0xB3, 0x69, 0x1F, 0xD2, 0x88, 0x3E, 
        0xF4, 0xAA, 0x5D, 0x13, 0xC9, 0x7F, 0x31, 0xE7, 0x9D, 0x53, 0x06, 0xBC, 0x72, 0x28, 0xDB, 0x91,
        0x47, 0xFD, 0xB3, 0x66, 0x1C, 0xD2, 0x88, 0x3B, 0xF1, 0xB5, 0x75, 0x39, 0xFA, 0xBE, 0x7E, 0x42,
        0x03, 0xC7, 0x88, 0x4B, 0x0C, 0xD0, 0x91, 0x54, 0x15, 0xD9, 0x9A, 0x5E, 0x21, 0xE2, 0xA6, 0x67, 
        0x2A, 0xEB, 0xAF, 0x70, 0x34, 0xF4, 0xB8, 0x79, 0x3D, 0xFD, 0xC1, 0x82, 0x46, 0x06, 0xCA, 0x8B, 
    };
    if(index>=5)index=0;
    
    QByteArray ba;
    ba.setRawData((char*)&passwd_table[index][0], 16);
    return ba;
    
}


void ModelGw01::showModel(QObject *parent)
{
    hideModel();
    m_modelParent=parent; 
    if(m_modelEngine==nullptr)
    {
        m_modelEngine=new QQmlApplicationEngine(this);
        m_modelEngine->rootContext()->setContextProperty("theModelGw01", this);
        m_modelEngine->rootContext()->setContextProperty("theCenterMan", this->parent());
    }
    m_modelEngine->load("qrc:/qmlRC/modelQml/GW01/ModelGw01.qml");
}

void ModelGw01::hideModel(void)
{
    if(m_modelEngine)
        delete m_modelEngine;
    m_modelEngine=nullptr;
    m_modelParent=nullptr;
}


void ModelGw01::showSimple(QObject *parent)
{
    m_simpleParent=parent; 
    if(m_simpleEngine==nullptr)
    {
        m_simpleEngine=new QQmlApplicationEngine(this);
        m_simpleEngine->rootContext()->setContextProperty("theModelGw01", this);
        m_simpleEngine->rootContext()->setContextProperty("theCenterMan", this->parent());
    }
    m_simpleEngine->load("qrc:/qmlRC/modelQml/GW01/SimpleGw01.qml");
}

void ModelGw01::hideSimple(void)
{
    if(m_simpleEngine)
        delete m_simpleEngine;
    m_simpleEngine=nullptr;
    m_simpleParent=nullptr;
}

int ModelGw01::setRawData(u32 app_id, u32 dev_sn, u8 pack_num, u8 msg_type, u8 *msg_buff, u16 msg_len)
{
    if(dev_sn!=m_devSn)
        return 0;
    
    u8 *pData=msg_buff;
    msg_len=msg_len;
    if(m_upPackNum==pack_num)
        return 0;
    m_upPackNum=pack_num;
    m_appID=app_id;
//    qDebug()<<"msg_type="<<msg_type;
    switch(msg_type)
    {
        case GW01_CMD_DATA://状态数据
        {
            u32 gw_time=pData[0]<<24 | pData[1]<<16 | pData[2]<<8 | pData[3];
            pData+=4;
            u32 now_time=QDateTime::currentDateTime().toTime_t();
            int det_time=now_time-gw_time;
            if(abs(det_time)>300)
            {
                qDebug("ack time!");
                setGwTime(now_time);
            }
            break;
        }
        case GW01_CMD_NODE_DATA://节点数据
        {
            u32 node_sn=pData[0]<<24 | pData[1]<<16 | pData[2]<<8 | pData[3];
            pData+=4;
            u16 rssi_u16=pData[0]<<8 | pData[1];
            pData+=2;
            u16 snr_u16=pData[0]<<8 | pData[1];
            pData+=2;
            u32 freq=pData[0]<<24 | pData[1]<<16 | pData[2]<<8 | pData[3];
            pData+=4;
            u8 sf=pData[0];
            pData+=1;
            u8 bw=pData[0];
            pData+=1;
            u8 up_pack_num=pData[0];
            pData+=1;
            u8 node_cmd=pData[0];
            pData+=1;
            int rssi=rssi_u16-1000;
            int snr=snr_u16-100;
            qDebug("node_sn=0x%08X,  rssi=%ddbm,  snr=%ddbm", node_sn, rssi, snr);

            switch(node_cmd)
            {
                case 0x01:
                {
                    u16 temp_val=pData[0]<<8 | pData[1];
                    pData+=2;
                    u32 node_time=pData[0]<<24 | pData[1]<<16 | pData[2]<<8 | pData[3];
                    pData+=4;
                    u16 pack_id=pData[0]<<8 | pData[1];
                    pData+=2;
                    float temp_f=(temp_val-1000)/10.f;
                    qDebug("temp_val=%.1fC\n", temp_f);
                    qDebug("node_time=%us\n", node_time);
                    Node01DataListStruct *pNode01=addNode01(node_sn);
                    if(pNode01)
                    {
                        Node01DataUnionStruct data_union;
                        data_union.rssi=rssi;
                        data_union.snr=snr;
                        data_union.temp=temp_val;
                        data_union.pack_id=pack_id;
                        data_union.time_stamp=QDateTime::currentDateTime().toTime_t();//当前时间戳,不要用节点上报的时间戳,不一定准确
                        data_union.freq=freq;
                        data_union.sf=sf;
                        data_union.bw=bw;
                        data_union.up_pack_num=up_pack_num;
                        pNode01->data_list.append(data_union);
                        QString sn_str=QString::asprintf("%08X", node_sn);
                        QString rssi_str=QString::asprintf("%d", rssi);
                        QString snr_str=QString::asprintf("%d", snr);
                        QString data_str=QString::asprintf("%.1f℃,%d,",  temp_f, pack_id);
                        data_str+=QString::asprintf("(%.1f,%d,%d),", freq/1000000.f, sf, bw);
                        data_str+=QDateTime::currentDateTime().toString("hh:mm:ss");
                        emit siqUpdateNode01Data(sn_str, rssi_str, snr_str, data_str);
                    }
                    break;
                }                
            
            }
            break;
        }
        case GW01_CMD_NODE_LIST:
        {
            
            break;
        }
        case GW01_CMD_NODE_DOWN://设置结果返回
        {
            u32 node_sn=pData[0]<<24 | pData[1]<<16 | pData[2]<<8 | pData[3];
            pData+=4;
            u8 result=pData[0];
            pData+=1;
            QString sn_str=QString::asprintf("节点%08X ", node_sn);
            switch(result)
            {
                case NwkMasterDownResultAddOK:
                {
                    emit siqShowMsg(sn_str+"添加成功,等待发送!");
                    break;
                }   
                case NwkMasterDownResultFull:
                {
                    emit siqShowMsg(sn_str+" 缓冲区已满!");
                    break;
                }
                case NwkMasterDownResultErrSn:
                {
                    emit siqShowMsg(sn_str+" 设备不存在!");
                    break;
                }   
                case NwkMasterDownResultTimeOut:
                {
                    emit siqShowMsg(sn_str+" 发送超时!");
                    break;
                }   
                case NwkMasterDownResultSuccess:
                {
                    emit siqShowMsg(sn_str+"下发成功!");
                    break;
                }   
                default:
                {
                    emit siqShowMsg(sn_str+" 未知错误!");
                    break;
                }
            }
            break;
        }        
    }
    QDateTime current_date_time = QDateTime::currentDateTime();
    m_updateTime=current_date_time.toString("hh:mm:ss");

    m_onlineTime=m_secTickets;
    m_onlineState=DEV_STATE_ON_LINE;
    return 0;
}

//新增节点
ModelGw01::Node01DataListStruct *ModelGw01::addNode01(u32 node_sn)
{
    Node01DataListStruct *pNode01=findNode01(node_sn);
    if(pNode01==nullptr)
    {
        Node01DataListStruct tag_node;
        tag_node.node_sn=node_sn;
        m_node01DataList.append(tag_node);
        pNode01=&m_node01DataList.last();
    }
    return pNode01;
}

//查找节点
ModelGw01::Node01DataListStruct *ModelGw01::findNode01(u32 node_sn)
{
    for(int i=0; i<m_node01DataList.size(); i++)
    {
        Node01DataListStruct *pNode01=&m_node01DataList[i];
        if(pNode01->node_sn==node_sn)
        {
            return pNode01;
        }
    }
    return nullptr;
}

//添加数据单元
void ModelGw01::addNode01DataUnion(u32 node_sn, Node01DataUnionStruct data_union)
{
    Node01DataListStruct *pNode01=findNode01(node_sn);
    if(pNode01)
    {
        pNode01->data_list.append(data_union);
        if(pNode01->data_list.size()>NODE01_DATA_LIST_SIZE)//数量限制
        {
            pNode01->data_list.removeLast();//移除最后一个数据单元
        }
    }
}

QString ModelGw01::makeNode01HistoryDataStr(int index, Node01DataUnionStruct data_union)
{
    QString data_str=QString::asprintf("%d.%d|%ddbm,%.1fC,%d|%d,", index+1, data_union.rssi, data_union.snr, (data_union.temp-1000)/10.f, data_union.pack_id, data_union.up_pack_num);
    data_str+=QString::asprintf("(%.1f,%d,%d),", data_union.freq/1000000.f, data_union.sf, data_union.bw);
    data_str+=QDateTime::fromTime_t(data_union.time_stamp).toString("hh:mm:ss");
    return data_str;
}


void ModelGw01::updateNode01History(QString node_str)
{
    bool ok;
    u32 node_sn=node_str.toUInt(&ok, 16);
     Node01DataListStruct *pNode01=findNode01(node_sn);
     if(pNode01)
     {
         int nSize=pNode01->data_list.size();
         for(int i=0; i<nSize; i++)
         {
             Node01DataUnionStruct data_union=pNode01->data_list.at(i);
            QString data_str=makeNode01HistoryDataStr(i, data_union);
             emit siqAddNode01History(i, data_str);
         }
     }
}

QString ModelGw01::exportNode01History(QString node_str)
{
    QString ack_str;
    bool ok;
    u32 node_sn=node_str.toUInt(&ok, 16);
     Node01DataListStruct *pNode01=findNode01(node_sn);
     if(pNode01)
     {
         QString file_name=node_str+QDateTime::currentDateTime().toString(" yyyy-MM-dd hh.mm.ss")+".txt";
         qDebug()<<file_name;
         ack_str=file_name;
         QFile file(file_name);  
         if(file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))  
         {
             int nSize=pNode01->data_list.size();
             for(int i=0; i<nSize; i++)
             {
                 Node01DataUnionStruct data_union=pNode01->data_list.at(i);
                QString data_str=makeNode01HistoryDataStr(i, data_union);
                data_str+="\n";
                file.write(data_str.toLatin1());
             }
             file.close();
             ack_str+=" 导出成功!";
         }
         else
         {
             qDebug("文件打开失败!");
             ack_str+=" 文件打开失败!";
         }
     }
     else
     {
         ack_str="节点不存在!";
     }
     return ack_str;
}

void ModelGw01::setNode01DownMsg(QString node_str)
{
    bool ok;
    u32 node_sn=node_str.toUInt(&ok, 16);
     
    u8 make_buff[100]={0};
    u16 make_len=0;
    make_buff[make_len++]=node_sn>>24;
    make_buff[make_len++]=node_sn>>16;
    make_buff[make_len++]=node_sn>>8;
    make_buff[make_len++]=node_sn; //节点序列号
    QString time_str=QDateTime::currentDateTime().toString("hh:mm:ss");//下发当前时间
    QByteArray time_ba=time_str.toLatin1();
    u8 data_len=time_ba.size();
    make_buff[make_len++]=data_len;
    memcpy(&make_buff[make_len], time_ba.data(), data_len);
    make_len+=data_len;
    emit sigSendDownMsg(m_appID, m_devSn, m_downPackNum++, GW01_CMD_NODE_DOWN, make_buff, make_len);
}

void ModelGw01::setGwTime(u32 time)
{
    u8 make_buff[100]={0};
    u16 make_len=0;
    make_buff[make_len++]=time>>24;
    make_buff[make_len++]=time>>16;
    make_buff[make_len++]=time>>8;
    make_buff[make_len++]=time; 
    emit sigSendDownMsg(m_appID, m_devSn, m_downPackNum++, GW01_CMD_DATA, make_buff, make_len);
}





