/**
 * GB28181
 * 公共安全视频监控联网系统信息传输、交换、控制技术要求
 * 
 * 在生产环境中请以相关的厂家实现为标准，有的厂家甚至在不同的地区还会有一些区别
 */

#ifndef _GB28181_STD_H_
#define _GB28181_STD_H_

#define GB28181_CTNTYPE_MANSCDP "Application/MANSCDP+xml"
#define GB28181_CTNTYPE_MANSRTSP "Application/MANSRTSP"

#define GB28181_get_industry_type_from_id(gbid) GB28181_get_code_from_id(gbid, 8, 10)
#define GB28181_get_device_type_from_id(gbid) GB28181_get_code_from_id(gbid, 10, 13)
#define GB28181_get_net_type_from_id(gbid) GB28181_get_code_from_id(gbid, 13, 14)

/**
 * 行业类型
 */
typedef enum {
    /* 00~11 政府机关 */
    GB28181_INDUSTRY_SS_ROAD          = 0, //社会治安路面接入
    GB28181_INDUSTRY_SS_COMMUNITY     = 1, //社会治安社区接入
    GB28181_INDUSTRY_SS_INTERNAL      = 2, //社会治安内部接入
    GB28181_INDUSTRY_SS_OTHER         = 3, //社会治安其他接入
    GB28181_INDUSTRY_TRAFFIC_ROAD     = 4, //交通路面接入
    GB28181_INDUSTRY_TRAFFIC_GATE     = 5, //交通卡口接入
    GB28181_INDUSTRY_TRAFFIC_INTERNAL = 6, //交通内部接入
    GB28181_INDUSTRY_TRAFFIC_OTHER    = 7, //交通其他接入
    GB28181_INDUSTRY_CITY_MANAGEMENT  = 8, //城市管理接入
    GB28181_INDUSTRY_HEP              = 9, //卫生环保接入
    GB28181_INDUSTRY_CIQ              = 10,//商检海关接入
    GB28181_INDUSTRY_EDU              = 11,//教育部门接入
    /* 12~39 预留1 */
    GB28181_INDUSTRY_RESERVED1,
    /* 40~79 企业/事业单位 */
    GB28181_INDUSTRY_NONGLINMUYU   = 40, //农林牧渔业接入
    GB28181_INDUSTRY_MINING        = 41, //采矿企业接入
    GB28181_INDUSTRY_MANUFACTURING = 42, //制造企业接入
    GB28181_INDUSTRY_YEJIN         = 43, //冶金企业接入
    GB28181_INDUSTRY_DIANLI        = 44, //电力企业接入
    GB28181_INDUSTRY_RANQI         = 45, //燃气企业接入
    GB28181_INDUSTRY_BUILDING      = 46, //建筑企业接入
    GB28181_INDUSTRY_LOGISTICS     = 47, //物流企业接入
    GB28181_INDUSTRY_POSTAL        = 48, //邮政企业接入
    GB28181_INDUSTRY_IT            = 49, //信息企业接入
    GB28181_INDUSTRY_ZHUSUCANYIN   = 50, //住宿和餐饮业接入
    GB28181_INDUSTRY_JINRONG       = 51, //金融企业接入
    GB28181_INDUSTRY_REALESTATE    = 52, //房地产业接入
    GB28181_INDUSTRY_BUSINESS      = 53, //商务服务业接入
    GB28181_INDUSTRY_SHUILI        = 54, //水利企业接入
    GB28181_INDUSTRY_YULE          = 55, //娱乐企业接入
    GB28181_INDUSTRY_RESERVED2,
    /* 80~89 居民自建 */
    GB28181_INDUSTRY_RESERVED3 = 80,
    /* 90~99 其他主体 */
    GB28181_INDUSTRY_RESERVED4 = 90
} GB28181_industry_type_e;

/**
 * 国标编码规则A中的设备类型，第11~13位
 */
typedef enum GB28181_device_type_a {
    /* 111~130 前端主设备 */
    GB28181_DEVICE_DVR                 = 111, //DVR
    GB28181_DEVICE_VIDEO_SVR           = 112, //视频服务器
    GB28181_DEVICE_ENCODER             = 113, //编码器
    GB28181_DEVICE_DECODER             = 114, //解码器
    GB28181_DEVICE_VIDEO_SWITCH_MATRIX = 115, //视频切换矩阵
    GB28181_DEVICE_AUDIO_SWITCH_MATRIX = 116, //音频切换矩阵
    GB28181_DEVICE_ALARM_CTRL          = 117, //报警控制器
    GB28181_DEVICE_NVR                 = 118, //网络视频录像机（NVR）
    GB28181_DEVICE_HVR                 = 119, //混合硬盘录像机（HVR）
    GB28181_DEVICE_VIDEOSTITCH         = 121, //画面分割器设备
    GB28181_DEVICE_HVR2                = 130, //混合硬盘录像机（HVR）
    /* 131~199 前端外围设备 */
    GB28181_DEVICE_CAMERA              = 131, //摄像机
    GB28181_DEVICE_IPC                 = 132, //网络摄像机
    GB28181_DEVICE_MONITOR             = 133, //显示器
    GB28181_DEVICE_ALARMINPUT          = 134, //报警输入设备（如红外、烟感、门禁等报警设备）
    GB28181_DEVICE_ALARMOUTPUT         = 135, //报警输出设备(如警灯、警铃等设备)
    GB28181_DEVICE_AUDIOINPUT          = 136, //语音输入设备
    GB28181_DEVICE_AUDIOOUTPUT         = 137, //语音输出设备
    GB28181_DEVICE_MOBILE              = 138, //移动传输设备
    /* 200~299 平台设备 */
    GB28181_DEVICE_SIPSVR         = 200, //中心信令控制服务器
    GB28181_DEVICE_WEBSVR         = 201, //Web应用服务器
    GB28181_DEVICE_MEDIASVR       = 202, //媒体分发服务器
    GB28181_DEVICE_PROXYSVR       = 203, //代理服务器
    GB28181_DEVICE_SAFESVR        = 204, //安全服务器
    GB28181_DEVICE_ALARMSVR       = 205, //报警服务器
    GB28181_DEVICE_DBSVR          = 206, //数据库服务器
    GB28181_DEVICE_GISSVR         = 207, //GIS服务器
    GB28181_DEVICE_ADMINSVR       = 208, //管理服务器
    GB28181_DEVICE_GATEWAY        = 209, //接入网关
    GB28181_DEVICE_MSS            = 210, //媒体存储服务器
    GB28181_DEVICE_SSG            = 211, //信令安全路由网关
    GB28181_DEVICE_VOD            = 212, //
    GB28181_DEVICE_UPDATECENTER   = 213, //中心升级服务器
    GB28181_DEVICE_UPDATESVR      = 214,
    GB28181_DEVICE_BUSINESS_GROUP = 215, //业务分组  角色
    GB28181_DEVICE_VIRTUAL_GROUP  = 216, //虚拟分组  资源组
    GB28181_DEVICE_EXTENDPLATFORM = 220,
    /* 300~399 中心用户 */
    GB28181_DEVICE_CENTERUSER = 300,
    /* 400~499 终端用户 */
    GB28181_DEVICE_CLIENT = 400,
    /* 500~599 平台外接服务器 */
    GB28181_DEVICE_EXTEND            = 500,
    GB28181_DEVICE_CAM_GROUP         = 501,
    GB28181_DEVICE_MON_GROUP         = 503,
    GB28181_DEVICE_VIDEOSTITCH_GROUP = 521,
    GB28181_DEVICE_OTHER_GROUP       = 522, //其他资源组，包含报警输入输出，音频输入输出等
    /* 600~999 扩展 */
    GB28181_DEVICE_RESERVED = 600
} GB28181_device_type_e;


enum GB28181_device_type_b {
    /* 00~19 数字视音频设备类型码 */
    GB28181_DEVICE_DV_ENCODER   = 0, //数字视频编码设备(不带本地存储)
    GB28181_DEVICE_DV_RECORDER  = 1, //数字视频录像设备(带本地存储)
    GB28181_DEVICE_DV_DECODER   = 2, //数字视频解码设备
    GB28181_DEVICE_DV_RESERVED,      //预留1(数字视音频设备)
    /* 20～39 服务器设备类型码 */
    GB28181_DEVICE_MNASVR       = 20, //监控联网管理服务器
    GB28181_DEVICE_V_PROXYSVR   = 21, //视频代理服务器
    GB28181_DEVICE_WEBINSVR     = 22, //Web应用服务器
    GB28181_DEVICE_VCRMSVR      = 23, //录像管理服务器
    GB28181_DEVICE_SVR_RESERVED,      //预留2(服务器设备)
    /* 40～59 其他数字设备类型码 */
    GB28181_DEVICE_NET_MATRIX       = 40, //网络数字矩阵
    GB28181_DEVICE_NET_CONTROLLER   = 41, //网络控制器
    GB28181_DEVICE_NET_ALERTOR      = 42, //网络报警主机
    GB28181_DEVICE_OTHER_RESERVED,        //预留3(其他数字设备)
    /* 60～74 模拟视音频设备类型码 */
    GB28181_DEVICE_SIM_CAM          = 60, //模拟摄像机
    GB28181_DEVICE_SIM_VIDEO_MATRIX = 61, //视频模拟矩阵
    GB28181_DEVICE_SIM_RESERVED,          //预留4(模拟视音频设备)
    /* 75～89 其他模拟设备类型码 */
    GB28181_DEVICE_SIM_CONTROLLER = 75, //模拟控制器
    GB28181_DEVICE_SIM_ALERTOR    = 76, //模拟报警主机
    GB28181_DEVICE_SIM_RESERVED4,       //预留4(其他模拟设备)
    /* 90～99 用户类型 */
    GB28181_DEVICE_U_RESERVED = 90 //
};

/**
 * 国标编码规则A中的网络类型，第14位
 */
typedef enum {
    GB28181_NET_MAPN,        //监控报警专网(monitoringand alarming private network)
    GB28181_NET_MAPN1,
    GB28181_NET_MAPN2,
    GB28181_NET_MAPN3,
    GB28181_NET_MAPN4,
    GB28181_NET_PSI,         //公安信息网(public security information)
    GB28181_NET_GOV,         //政务网
    GB28181_NET_INTERNET,    //英特网
    GB28181_NET_SOCIALRES,   //社会资源接入网
    GB28181_NET_RESERVED
} GB28181_net_type_e;

/**
 * 设备目录项
 */
typedef struct {
    char *DeviceID;             //*设备/区域编码
    char *Name;                 //*设备/区域名称
    char *Address;              //地址
    char *StartTime;            //开始时间
    char *EndTime;              //截止时间
    int Secrecy;                //*保密属性，缺省为0; 0不涉密，1涉密
} GB28181_catalog_item_t;

/**
 * 文件目录项
 */
typedef struct {
    char *DeviceID;             //*设备/区域编码
    char *Name;                 //*设备/区域名称
    char *Address;              //录像地址
    char *StartTime;            //录像开始时间
    char *EndTime;              //录像截止时间
    int Secrecy;                //*保密属性，缺省为0; 0不涉密，1涉密
    char *Type;                 //录像产生类型。time、alarm或manual
    char *RecorderID;           //录像触发者ID
    char *FileSize;             //录像文件大小，单位：Byte
} GB28181_file_item_t;

/**
 * 目录项字段
 */
extern const char *GB28181_catalog_item_fields[];
/**
 * 文件项字段
 */
extern const char *GB28181_file_item_fields[];

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 从国标ID中获取子字符
 */
int GB28181_get_code_from_id(const char *iid, int start, int end);

#ifdef __cplusplus
}//extern "C"
#endif

#endif//_GB28181_STD_H_
