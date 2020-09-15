/* 监控报警联网系统控制描述协议
 * MANSCDP(Monitoringand Alarming Network System Control Description Protocol)
 * Copyright (C) 2019-2020,
 * 
 * 主要是XML处理
 */

#ifndef _GB28181_MANSCDP_H_
#define _GB28181_MANSCDP_H_

#define MANSCDP_BUFSIZE 2048

#define MANSCDP_CMDTYPE_ALARM           "Alarm"
#define MANSCDP_CMDTYPE_BROADCAST       "Broadcast"
#define MANSCDP_CMDTYPE_CATALOG         "Catalog"
#define MANSCDP_CMDTYPE_CONFIGDOWNLOAD  "ConfigDownload"
#define MANSCDP_CMDTYPE_DEVICECONFIG    "DeviceConfig"
#define MANSCDP_CMDTYPE_DEVICECONTROL   "DeviceControl"
#define MANSCDP_CMDTYPE_DEVICEINFO      "DeviceInfo"
#define MANSCDP_CMDTYPE_DEVICESTATUS    "DeviceStatus"
#define MANSCDP_CMDTYPE_KEEPALIVE       "Keepalive"
#define MANSCDP_CMDTYPE_MEDIASTATUS     "MediaStatus"
#define MANSCDP_CMDTYPE_MOBILEPOSITION  "MobilePosition"
#define MANSCDP_CMDTYPE_PRESETQUERY     "PresetQuery"
#define MANSCDP_CMDTYPE_RECORDINFO      "RecordInfo"

#define MANSCDP_PTZ_ZOOM_OUT    0x20 /* 画面缩小 */
#define MANSCDP_PTZ_ZOOM_IN     0x10 /* 画面放大 */
#define MANSCDP_PTZ_UP          0x08 /* 云台向上 */
#define MANSCDP_PTZ_DOWN        0x04 /* 云台向下 */
#define MANSCDP_PTZ_LEFT        0x02 /* 云台向左 */
#define MANSCDP_PTZ_RIGHT       0x01 /* 云台向右 */

#define MANSCDP_FI_FOCUS_IN     0x42 /* 聚焦拉近 */
#define MANSCDP_FI_FOCUS_OUT    0x41 /* 聚焦拉远 */
#define MANSCDP_FI_IRIS_UP      0x44 /* 光圈放大 */
#define MANSCDP_FI_IRIS_DOWN    0x48 /* 光圈缩小 */

#define MANSCDP_CHECKPOINT_ADD  0x81 /* 设置预置位 */
#define MANSCDP_CHECKPOINT_SET  0x82 /* 调用预置位 */
#define MANSCDP_CHECKPOINT_DEL  0x83 /* 删除预置位 */

#define MANSCDP_CRUISE_ADD      0x84 /* 加入巡航点 */
#define MANSCDP_CRUISE_DEL      0x85 /* 删除巡航点 */
#define MANSCDP_CRUISE_SPEED    0x86 /* 设置巡航速度 */
#define MANSCDP_CRUISE_DURATION 0x87 /* 设置巡航停留时间 */
#define MANSCDP_CRUISE_START    0x88 /* 开始巡航 */

#define MANSCDP_SCAN_START      0x89 /* 开始自动扫描 */
#define MANSCDP_SCAN_LEFT       0x89 /* 设置自动扫描左边界 */
#define MANSCDP_SCAN_RIGHT      0x89 /* 设置自动扫描右边界 */
#define MANSCDP_SCAN_SPEED      0x8A /* 设置自动扫描速度 */

#define MANSCDP_AUXSWITCH_ON    0x8C /* 打开辅助开关 */
#define MANSCDP_AUXSWITCH_OFF   0x8D /* 关闭辅助开关 */

#define manscdp2str(manscdp)\
        manscdp2string(manscdp, (char[MANSCDP_BUFSIZE]){0}, MANSCDP_BUFSIZE)
/**
 * 报警布防/撤防
 * @param manscdp MANSCDP
 * @param set 0撤防，1布防
 */
#define manscdp_Control_GuardCmd(manscdp, set)\
        manscdp_set_node(manscdp, "Control", "GuardCmd", set == 0 ? "ResetGuard" : "SetGuard")
/**
 * 录像控制
 * @param manscdp MANSCDP
 * @param record 0停止录像，1开始录像
 */
#define manscdp_Control_RecordCmd(manscdp, record)\
        manscdp_set_node(manscdp, "Control", "RecordCmd", record == 0 ? "StopRecord" : "Record")
/**
 * 远程启动
 * @param manscdp MANSCDP
 */
#define manscdp_Control_TeleBoot(manscdp)\
        manscdp_set_node(manscdp, "Control", "TeleBoot", "Boot")
/**
 * 报警复位
 * @param manscdp MANSCDP
 */
#define manscdp_Control_AlarmCmd(manscdp)\
        manscdp_set_node(manscdp, "Control", "AlarmCmd", "ResetAlarm")
/**
 * 强制关键帧
 * @param manscdp MANSCDP
 */
#define manscdp_Control_IFameCmd(manscdp)\
        manscdp_set_node(manscdp, "Control", "IFameCmd", "Send")

typedef struct MANSCDP MANSCDP;

/**
 * 单项回调函数
 * @param MANSCDP
 * @param 内部xml对象，记录当前处理项
 * @param 字段值数组
 * @param 字段个数
 */
typedef void (*manscdp_item_cb)(MANSCDP *, void *, const char **, int);
/**
 * 单项回调函数2
 * @param MANSCDP
 * @param 字段数组
 * @param 值数组
 */
typedef void (*manscdp_item_cb2)(MANSCDP *, const char **, const char **, int);
extern manscdp_item_cb manscdp_got_catalog_item; //目录项回调
extern manscdp_item_cb manscdp_got_file_item; //录像文件回调

/**
 * 协议类型
 */
typedef enum {
    MANSCDP_TYPE_UNSUPPORTED,
    MANSCDP_TYPE_RESPONSE,
    MANSCDP_TYPE_CONTROL,
    MANSCDP_TYPE_NOTIFY,
    MANSCDP_TYPE_QUERY
} manscdp_type_e;
// const char *manscdp_type_es[] = { "Response", "Control", "Notify", "Query" };

/**
 * PTZCmd类型
 */
enum {
    MANSCDP_PTZCMD_PTZ = 1,     //PTZ
    MANSCDP_PTZCMD_FI,          //聚焦、光圈
    MANSCDP_PTZCMD_CHECKPOINT,  //预置位
    MANSCDP_PTZCMD_CRUISE,      //巡航
    MANSCDP_PTZCMD_SCAN,        //扫描
    MANSCDP_PTZCMD_AUXSWITCH    //辅助开关
};

/**
 * 监控报警联网系统控制描述协议
 * Monitoringand Alarming Network System Control Description Protocol
 */
struct MANSCDP {
    manscdp_type_e type;    /* 协议类型 */
    const char *cmdtype;    /* CmdType */
    const char *device_id;  /* 国标编码 */
    struct {
        char *username;
        char *host;
        char *port;
    } from;
    int sn;                 /* 序号 */
    void *xml;              /* 内部xml结构体，请不要修改这个指针 */
    // unsigned char cmd[8];   /* @deprecated 指令数据（PTZ、FI、预置位、巡航、扫描、辅助开关） */
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 创建MANSCDP
 * @param type 类型：Response, Control, Notify, Query
 * @param cmdtype 宏MANSCDP_CMDTYPE_*
 * @param device_id 国标编码
 * @return NULL is cannot allocate memory or incorrect type
 * 
 * @note
 *  The resulting should be freed with manscdp_free()
 */
MANSCDP *manscdp_new(manscdp_type_e type, const char *cmdtype, const char *device_id);
#define MANSCDP_RESPONSE(CMDTYPE) manscdp_new(MANSCDP_TYPE_RESPONSE, CMDTYPE, "")
#define MANSCDP_CONTROL(CMDTYPE) manscdp_new(MANSCDP_TYPE_CONTROL, CMDTYPE, "")
#define MANSCDP_NOTIFY(CMDTYPE) manscdp_new(MANSCDP_TYPE_NOTIFY, CMDTYPE, "")
#define MANSCDP_QUERY(CMDTYPE) manscdp_new(MANSCDP_TYPE_QUERY, CMDTYPE, "")

/**
 * 解析字符串创建一个MANSCDP
 * @param xmlstr XML字符串
 * @return NULL is parse failed
 * 
 * @note
 *  The resulting should be freed with manscdp_free()
 */
MANSCDP *manscdp_parse(const char *xmlstr);

/**
 * 释放MANSCDP
 * @param manscdp MANSCDP
 */
void manscdp_free(MANSCDP *manscdp);

/**
 * 获取节点指定的属性值
 * @param manscdp MANSCDP
 * @param xpath XPath
 * @param name 属性名
 * @return 属性值(attribute value)
 */
const char *manscdp_get_node_attr(MANSCDP *manscdp, const char *xpath, const char *name);

/**
 * 获取节点的文本值
 * @param manscdp MANSCDP
 * @param xpath XPath
 * @return 节点内容
 */
const char *manscdp_get_node_text(MANSCDP *manscdp, const char *xpath);

/**
 * 添加节点到指定位置，如果该位置已存在同名节点则修改值
 * @param manscdp MANSCDP
 * @param xpath if this is NULL add to root
 * @param name 节点名称
 * @param value 节点值
 * @return 0 is success
 */
int manscdp_set_node(MANSCDP *manscdp, const char *xpath, char *name, char *value);

/**
 * 将MANSCDP保存文件
 * @param manscdp MANSCDP
 * @param xpath if this is NULL, save all
 * @param fd 文件句柄
 * @return 0 is success
 */
int manscdp_infile(MANSCDP *manscdp, const char *xpath, void *fd);

/**
 * 将MANSCDP转换成字符串
 * @param manscdp MANSCDP
 * @param buf 用来保存字符串的缓冲区
 * @param bufsize 缓冲区长度
 * @param newstr 如果使用了此参数，buf和bufsize将被忽略，并且需要手动释放
 * @return length
 * 
 * @example
 *  1.不使用newstr
 *  char buf[1024] = {0};
 *  manscdp_to_str(manscdp, buf, sizeof(buf), NULL);
 *  2.使用newstr
 *  char *newstr;
 *  manscdp_to_str(manscdp, NULL, 0, &newstr);
 *  free(newstr);
 */
int manscdp_to_str(MANSCDP *manscdp, char *buf, int bufsize, char **newstr);
static inline char *manscdp2string(MANSCDP *manscdp, char *buf, int bufsize)
{
    manscdp_to_str(manscdp, buf, bufsize, 0);
    return buf;
}

/**
 * got items回调
 * @param manscdp MANSCDP
 * @return 返回单项回调运行次数
 * 
 * @deprecated 请使用manscdp_get_items
 */
int manscdp_got_items(MANSCDP *manscdp);
/**
 * 遍历项
 * @param manscdp MANSCDP
 * @param cb 四个参数
 * @return 返回单项回调运行次数
 */
int manscdp_get_items(MANSCDP *manscdp, manscdp_item_cb2 cb);
/**
 * 设备配置
 * @param manscdp MANSCDP
 * @param name 设备名称
 * @param exp 注册过期时间
 * @param interval 心跳间隔时间
 * @param count 心跳超时次数
 * @return 0 is success
 */
int manscdp_device_config_Basic(MANSCDP *manscdp, char *name, int exp, int interval, int count);

/**
 * 前端设备控制
 * @param manscdp MANSCDP
 * @param type 指令类型：PTZ/FI/预置位/巡航/扫描/辅助开关控制
 * @param byte4 指令码
 * @param byte5 数据1，第5字节
 * @param byte6 数据2，第6字节
 * @param byte7 数据3，第7字节高4位
 * @param addr 地址
 * @return 0 is success
 * 
 * @note
 *  byte5, byte6, byte7这三个虽然是字节数据内容但是作为参数是有符号的，用来判断方向
 */
int manscdp_Control_DeviceControl(MANSCDP *manscdp, int type, int byte4, int byte5, int byte6, int byte7, int addr);
/**
 * 云台控制
 * @param manscdp MANSCDP
 * @param pan  水平运动(X轴)（左 -255~255 右）
 * @param tilt 垂直运动(Y轴)（下 -255~255 上）
 * @param zoom 缩放操作    （缩小 -15~15 放大）
 * @return 0 is success
 * 
 * @example
 *  1.放大（单项操作）
 *  manscdp_Control_PTZ(manscdp, 0, 0, 5);
 *  2.左上运动 + 放大（混合操作）
 *  manscdp_Control_PTZ(manscdp, -90, 180, 10);
 */
#define manscdp_Control_PTZ(manscdp, pan, tilt, zoom)\
        manscdp_Control_DeviceControl(manscdp, MANSCDP_PTZCMD_PTZ, 0, pan, tilt, zoom, 0)
/**
 * 控制聚焦与光圈
 * @param manscdp MANSCDP
 * @param foucus 聚焦速度 （拉近 -255~255 拉远）
 * @param iris 光圈速度   （缩小 -255~255 放大）
 * @return 0 is success
 * 
 * @example
 *  1.拉近
 *  manscdp_Control_FI(manscdp, 128, 0);
 *  2.拉远 + 光圈缩小
 *  manscdp_Control_FI(manscdp, -128, -64);
 */
#define manscdp_Control_FI(manscdp, focus, iris)\
        manscdp_Control_DeviceControl(manscdp, MANSCDP_PTZCMD_FI, 0, focus, iris, 0, 0)
/**
 * 预置位操作
 * @param manscdp MANSCDP
 * @param act 使用MANSCDP_CHECKPOINT_*宏指令
 * @param index 1~255
 * @return 0 is success
 * 
 * @example
 *  1.设置一个预置位
 *  manscdp_Control_CheckPoint(manscdp, MANSCDP_CHECKPOINT_ADD, 1);
 *  2.调用预置位
 *  manscdp_Control_CheckPoint(manscdp, MANSCDP_CHECKPOINT_SET, 1);
 *  3.删除预置位
 *  manscdp_Control_CheckPoint(manscdp, MANSCDP_CHECKPOINT_DEL, 1);
 */
#define manscdp_Control_CheckPoint(manscdp, act, index)\
        manscdp_Control_DeviceControl(manscdp, MANSCDP_PTZCMD_CHECKPOINT, act, 0, index, 0, 0)

/**
 * 辅助开关控制
 * @param manscdp MANSCDP
 * @param no 辅助开关编号，取值为1表示雨刷控制（关闭 -255~255 打开）
 * @return 0 is success
 * 
 * @example
 *  1.打开雨刷
 *  manscdp_Control_AUXSwtich(manscdp, 1);
 *  2.关闭雨刷
 *  manscdp_Control_AUXSwtich(manscdp, -1);
 */
#define manscdp_Control_AUXSwtich(manscdp, no)\
        manscdp_Control_DeviceControl(manscdp, MANSCDP_PTZCMD_AUXSWITCH, 0, no, 0, 0, 0)

#ifdef __cplusplus
}//extern "C"
#endif

#endif//_GB28181_MANSCDP_H_
