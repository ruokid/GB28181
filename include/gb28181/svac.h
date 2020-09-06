/* 安全防范监控数字视音频编码
 * SVAC(Surveillance Video and Audio Coding)
 * Copyright (C) 2019-2020,
 *
 * 参考：
 *  《安全防范监控数字视音频编解码技术标准》
 * 
 * 说起来网上搜索的文章不少都说SVAC是和H265对标，甚至在安防这块是更好的标准，
 * 各种应该大力推广云云，可为什么找不有用的技术资料呢？
 */

#ifndef _GB28181_SVAC_H_
#define _GB28181_SVAC_H_

typedef struct {
    int flags;
#define SVAC_FLAG_TIME 1    //是否包含时间信息
#define SVAC_FLAG_SURVEIL   //是否包含监控信息
#define SVAC_FLAG_ALARM     //是否包含报警信息
#define SVAC_FLAG_ROI       //是否包含ROI信息

    //图像信息
    unsigned short extension_length;   //本扩展语法的长度
    unsigned short position_idc;       //图像的来源位置
    unsigned int camera_idc;           //图像来源的摄像机标识
    //时间信息
    unsigned short time_year;          //年
    unsigned short time_month;         //月
    unsigned short time_day;           //日
    unsigned short time_hour;          //时
    unsigned short time_minute;        //分
    unsigned short time_second;        //秒
    unsigned short time_fractional;    //秒的分数信息，以1/16384秒为单位
    unsigned short time_ref_date_flag; //为1表示包括年月日的信息
    unsigned short FrameNum;           //帧数，i帧置0，p帧逐帧加1

    //扩展事件信息
    unsigned short Surveil_event_num[6];   //扩展事件数
    unsigned short Surveil_event_ID[6];    //扩展事件ID

    //报警事件信息
    unsigned char Surveil_alert_num;           //报警事件数目
    unsigned char Surveil_alert_ID[16];        //报警事件信息
    unsigned char Surveil_alert_region[16];    //位置编号信息
    unsigned char Surveil_alert_frame_num[16]; //摄像头编号信息

    //ROI信息
    unsigned char ROINum;              //图像包含ROI数目
    unsigned char ROISkipMode;         //ROI背景跳过模式
    unsigned char ROI_SVC_SkipMode;    //ROI_SVC背景跳过模式

    struct {
        unsigned int   roi_infor;   //ROI 区域信息
        unsigned short top;         //ROI 顶边坐标 [y_min]
        unsigned short left;        //ROI 左边坐标 [x_min]
        unsigned short bot;         //ROI 底边坐标 [y_max]
        unsigned short right;       //ROI 右边坐标 [x_max]
    } ROIRect[4], ROI_SVC_ElRect[4];//基本层ROI矩形信息；SVC增强层ROI矩形信息，SVC下有效

    //元数据信息
    unsigned int  VideoID;      //设备ID
    unsigned int  DeviceType;   //设备类型，枪机、球机等
    unsigned int  Application;  //监控场景，室内、室外
    unsigned char VideoAddr[32];//监控地点
    unsigned char Time[32];     //监控时间
    unsigned int  Resolution;   //分辨率
    unsigned int  FrameRate;    //
} SVAC_EXT_DATA;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}//extern "C"
#endif

#endif//_GB28181_SVAC_H_
