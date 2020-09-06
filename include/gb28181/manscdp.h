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

// PTZ指令
#define MANSCDP_PTZ_ZOOM_OUT    0x20
#define MANSCDP_PTZ_ZOOM_IN     0x10
#define MANSCDP_PTZ_UP          0x08
#define MANSCDP_PTZ_DOWN        0x04
#define MANSCDP_PTZ_LEFT        0x02
#define MANSCDP_PTZ_RIGHT       0x01
// FI指令
#define MANSCDP_FI
// 预置位指令
#define MANSCDP_PRESET
// 巡航指令
#define MANSCDP_CRUISE
// 扫描指令
#define MANSCDP_SCAN
// 辅助开关控制指令
#define MANSCDP_AUXSWITCH

#define manscdp2str(manscdp) \
        manscdp2string(manscdp, (char[MANSCDP_BUFSIZE]){0}, MANSCDP_BUFSIZE)
#define manscdp_device_control_GuardCmd(manscdp, set) \
        manscdp_set_node(manscdp, "Control", "GuardCmd", set == 0 ? "ResetGuard" : "SetGuard")
#define manscdp_device_control_RecordCmd(manscdp, record) \
        manscdp_set_node(manscdp, "Control", "RecordCmd", record == 0 ? "StopRecord" : "Record")
#define manscdp_device_control_TeleBoot(manscdp) \
        manscdp_set_node(manscdp, "Control", "TeleBoot", "Boot")
#define manscdp_device_control_AlarmCmd(manscdp) \
        manscdp_set_node(manscdp, "Control", "AlarmCmd", "ResetAlarm")
#define manscdp_device_control_IFameCmd(manscdp) \
        manscdp_set_node(manscdp, "Control", "IFameCmd", "Send")

#define manscdp_notify_keepalive(id) \
        manscdp_new(MANSCDP_TYPE_NOTIFY, MANSCDP_CMDTYPE_KEEPALIVE, id)

typedef struct MANSCDP MANSCDP;

/**
 * 单项回调函数
 * @param MANSCDP
 * @param 内部xml对象，记录当前处理项
 * @param 字段值数组
 * @param 字段个数
 */
typedef void (*manscdp_item_cb)(MANSCDP *, void *, const char **, int);
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
 * 监控报警联网系统控制描述协议
 * Monitoringand Alarming Network System Control Description Protocol
 */
struct MANSCDP {
    manscdp_type_e type;    /* 协议类型 */
    const char *cmdtype;
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
int manscdp_infile(MANSCDP *manscdp, const char *xpath, FILE *fd);

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
    manscdp_to_str(manscdp, buf, bufsize, NULL);
    return buf;
}

/**
 * got items回调
 * @param manscdp MANSCDP
 * @return 返回单项回调运行次数
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
 * 
 * @param manscdp MANSCDP
 * @return 0 is success
 */
int manscdp_Control_DeviceControl(MANSCDP *manscdp);

/**
 * 云台控制
 * @param manscdp MANSCDP
 * @param act 行为，使用 MANSCDP_PTZ_* 相关宏指令
 * @param zoom 缩放值。取值范围：0~15
 * @param tilt 垂直运动值。取值范围：0~255
 * @param pan 水平运动值。取值范围：0~255
 * @return 0 is success
 * 
 * @example
 *  1.单项操作（放大）
 *  manscdp_control_PTZ(manscdp, MANSCDP_PTZ_ZOOM_IN, 5, 0, 0);
 *  2.混合操作（左上运动 + 放大）
 *  manscdp_control_PTZ(manscdp,
 *      MANSCDP_PTZ_ZOOM_IN|MANSCDP_PTZ_UP|MANSCDP_PTZ_LEFT,
 *      5, 180, 200);
 * 
 * @deprecated 请使用 manscdp_control_PTZ(use manscdp_control_PTZ functions instead)
 */
int manscdp_device_control_PTZ(MANSCDP *manscdp, int act, int zoom, int tilt, int pan);
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
 *  manscdp_control_PTZ(manscdp, 0, 0, 5);
 *  2.左上运动 + 放大（混合操作）
 *  manscdp_control_PTZ(manscdp, -90, 180, 10);
 */
int manscdp_control_PTZ(MANSCDP *manscdp, int pan, int tilt, int zoom);

/**
 * 控制聚焦与光圈
 * @param manscdp MANSCDP
 * @param foucus 聚焦速度 （-255~255）
 * @param iris 光圈速度   （-255~255）
 * @return 0 is success
 */
int manscdp_control_FI(MANSCDP *manscdp, int focus, int iris);

/**
 * 预置位操作
 * @param manscdp MANSCDP
 * @return 0 is success
 */
int manscdp_control_CheckPoint(MANSCDP *manscdp);

#ifdef __cplusplus
}//extern "C"
#endif

#endif//_GB28181_MANSCDP_H_
