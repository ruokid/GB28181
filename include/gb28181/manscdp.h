/**
 * 监控报警联网系统控制描述协议
 * MANSCDP: Monitoringand Alarming Network System Control Description Protocol
 */

#ifndef _GB28181_MANSCDP_H_
#define _GB28181_MANSCDP_H_

#define MANSCDP_MAX_STRING_SIZE 1024

#define MANSCDP_CMDTYPE_ALARM "Alarm"
#define MANSCDP_CMDTYPE_BROADCAST "Broadcast"
#define MANSCDP_CMDTYPE_CATALOG "Catalog"
#define MANSCDP_CMDTYPE_CONFIGDOWNLOAD "ConfigDownload"
#define MANSCDP_CMDTYPE_DEVICECONFIG "DeviceConfig"
#define MANSCDP_CMDTYPE_DEVICECONTROL "DeviceControl"
#define MANSCDP_CMDTYPE_DEVICEINFO "DeviceInfo"
#define MANSCDP_CMDTYPE_DEVICESTATUS "DeviceStatus"
#define MANSCDP_CMDTYPE_KEEPALIVE "Keepalive"
#define MANSCDP_CMDTYPE_MEDIASTATUS "MediaStatus"
#define MANSCDP_CMDTYPE_MOBILEPOSITION "MobilePosition"
#define MANSCDP_CMDTYPE_PRESETQUERY "PresetQuery"
#define MANSCDP_CMDTYPE_RECORDINFO "RecordInfo"

// PTZ指令
#define MANSCDP_PTZ_ZOOM_OUT 0x20
#define MANSCDP_PTZ_ZOOM_IN 0x10
#define MANSCDP_PTZ_UP 0x08
#define MANSCDP_PTZ_DOWN 0x04
#define MANSCDP_PTZ_LEFT 0x02
#define MANSCDP_PTZ_RIGHT 0x01
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
        manscdp2string(manscdp, (char[MANSCDP_MAX_STRING_SIZE]){0}, MANSCDP_MAX_STRING_SIZE)
#define manscdp2str2(manscdp) \
        manscdp2string(manscdp, (char[MANSCDP_MAX_STRING_SIZE * 2]){0}, MANSCDP_MAX_STRING_SIZE * 2)
#define manscdp_device_control_GuardCmd(manscdp, set) \
        manscdp_set_node(manscdp, "GuardCmd", set == 0 ? "ResetGuard" : "SetGuard", "Control");
#define manscdp_device_control_RecordCmd(manscdp, record) \
        manscdp_set_node(manscdp, "RecordCmd", record == 0 ? "StopRecord" : "Record", "Control");
#define manscdp_device_control_TeleBoot(manscdp) \
        manscdp_set_node(manscdp, "TeleBoot", "Boot", "Control");
#define manscdp_device_control_AlarmCmd(manscdp) \
        manscdp_set_node(manscdp, "AlarmCmd", "ResetAlarm", "Control");
#define manscdp_device_control_IFameCmd(manscdp) \
        manscdp_set_node(manscdp, "IFameCmd", "Send", "Control");

typedef struct MANSCDP MANSCDP;

/**
 * 单项回调函数
 * @param MANSCDP对象
 * @param 内部xml对象，记录当前处理项
 * @param 字段值数组
 * @param 字段个数
 */
typedef void (*manscdp_item_cb)(MANSCDP *, void *, const char **, int);

extern manscdp_item_cb manscdp_got_catalog_item;
extern manscdp_item_cb manscdp_got_file_item;

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

/**
 * 监控报警联网系统控制描述协议
 * Monitoringand Alarming Network System Control Description Protocol
 */
struct MANSCDP {
    manscdp_type_e type;
    struct {
        char *username;
        char *host;
        char *port;
    } via;
    int sn;
    char *oxml; //解析时接收到的utf8编码的源字符串
    struct _mxml_node_s *xml; //内部xml结构体，请不要修改这个指针
    unsigned char cmd[8]; //指令
};

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 创建MANSCDP
 * @param type
 * @param cmdtype
 * @param device_id
 * @return NULL is cannot allocate memory or incorrect type
 * 
 * The resulting should be freed with manscdp_free()
 */
MANSCDP *manscdp_new(manscdp_type_e type, const char *cmdtype, const char *device_id);

/**
 * 解析字符串创建一个MANSCDP
 * @param strxml
 * @return NULL is parse failed
 * 
 * The resulting should be freed with manscdp_free()
 */
MANSCDP *manscdp_parse(const char *strxml);

/**
 * 释放MANSCDP
 * @param manscdp
 */
void manscdp_free(MANSCDP *manscdp);

/**
 * 设置事件（回调）
 * @param type
 * @param cmdtype
 * @param handler
 * @return 0表示更新，1表示添加
 * 
 * 注意，catalog和recodeinfo比较特殊，需要赋值单项回调函数
 */
int manscdp_on(manscdp_type_e type, char *cmdtype, int (*handler)(MANSCDP *));

/**
 * 触发已注册对应的事件
 * @param manscdp
 * @return 0 is success
 */
int manscdp_fire(MANSCDP *manscdp);

/**
 * 获取节点指定的属性值
 * @param manscdp
 * @param xpath
 * @param name
 * @return
 */
const char *manscdp_get_node_attr(MANSCDP *manscdp, const char *xpath, const char *name);

/**
 * 获取节点的文本值
 * @param manscdp
 * @param xpath
 * @return
 */
const char *manscdp_get_node_text(MANSCDP *manscdp, const char *xpath);

/**
 * 添加节点到指定位置，如果该位置已存在同名节点则修改值
 * @param manscdp
 * @param xpath If NULL add to root
 * @param name
 * @param value
 * @return 0 is success
 */
int manscdp_set_node(MANSCDP *manscdp, const char *xpath, char *name, char *value);

/**
 * 将MANSCDP保存文件
 * @param manscdp
 * @param xpath If xpath is NULL, save all
 * @param fd
 */
int manscdp_infile(MANSCDP *manscdp, const char *xpath, FILE *fd);

/**
 * 将MANSCDP转换成字符串
 * @param manscdp
 * @param buf
 * @param bufsize
 * @param newstr 如果使用了此参数，buf和bufsize将被忽略，并且需要手动释放
 * @return the total number of bytes that would
 * 
 * @example
 *  1.不使用newstr
 *  char buf[1024] = {0};
 *  manscdp_to_str(manscdp, buf, 1024, NULL);
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
 * 设备配置
 * @param manscdp
 * @param name 设备名称
 * @param exp 注册过期时间
 * @param interval 心跳间隔时间
 * @param count 心跳超时次数
 * @return 0 is success
 */
int manscdp_device_config_Basic(MANSCDP *manscdp, char *name, int exp, int interval, int count);

/**
 * 云台控制
 * @param manscdp
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
 */
int manscdp_device_control_PTZ(MANSCDP *manscdp, int act, int zoom, int tilt, int pan);

#ifdef __cplusplus
}//extern "C"
#endif

#endif//_GB28181_MANSCDP_H_
