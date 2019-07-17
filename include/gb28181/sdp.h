/**
 * 符合[RFC4566]并且可以在GB28181中使用的SDP内容
 * ================================================
 *  v=0
 *  o=国标ID 0 0 IN IP4 创建会话的计算机的地址
 *  s=Play
 *  c=IN IP4 流媒体接受者的地址
 *  t=0 0
 *  m=video 流媒体接受者端口号 RTP/AVP 96 97 98
 *  a=rtpmap:96 PS/90000
 *  a=rtpmap:97 MPEG4/90000
 *  a=rtpmap:98 H264/90000
 *  a=recvonly
 * ================================================
 * 
 * o字段的国标ID可以填写媒体所有者（平台网关ID、媒体服务器ID）的ID或者媒体创建者（前端设备ID）的ID
 * 这里建议填写媒体所有者的ID，因为SIP的Invite请求地址中是使用的设备ID，并且官方文档的示例中使用的是一个媒体服务器的ID。
 * 
 * s字段，这里与[RFC4566]中不同的是GB28181中限定了只能是以下值
 * Play代表实时点播；Playback代表历史回放；Download代表文件下载；Talk代表语音对讲。
 * 
 * GB28181中扩展的两个描述字段f和y
 * f字段描述了音视频格式相关参数。使用f字段时应保证视频和音频参数的结构完整性，即在任何时候f字段的结构都应是完整的结构
 *  f=v/编码格式/分辨率/帧率/码率类型/码率大小a/编码格式/码率大小/采样率
 * 
 * y字段描述了时效和SSRC，0为实时，1为历史
 * 第1位表示是否为历史 + 第2位至第6位取20位SIP监控域ID之中的4到8位 + 第7位至第10位是SSRC的后4位
 * 例如：Invite 12345678901234567890, SSRC: 0x063497bd(104110013)
 *  y=0456780013
 * 注意：某些厂家的实现为时效标识直接和SSRC的组合
 *  y=0104110013
 * 建议不要使用y字段。。。
 */

#ifndef _GB28181_SDP_H_
#define _GB28181_SDP_H_

#define GB28181_SDP_F_ACODEC_G711 1
#define GB28181_SDP_F_ACODEC_G7231 2
#define GB28181_SDP_F_ACODEC_G729 3
#define GB28181_SDP_F_ACODEC_G7221 4

#define GB28181_SDP_F_BR_5_3 1 //5.3kbps (注: G.723.1 中使用)
#define GB28181_SDP_F_BR_6_3 2 //6.3kbps (注: G.723.1 中使用)
#define GB28181_SDP_F_BR_8 3 //8kbps (注: G.729 中使用)
#define GB28181_SDP_F_BR_16 4 //16kbps (注: G.722.1 中使用)
#define GB28181_SDP_F_BR_24 5 //24kbps (注: G.722.1 中使用)
#define GB28181_SDP_F_BR_32 6 //32kbps (注: G.722.1 中使用)
#define GB28181_SDP_F_BR_48 7 //48kbps (注: G.722.1 中使用)
#define GB28181_SDP_F_BR_64 8 //64kbps (注: G.711 中使用)

#define GB28181_SDP_F_SR_8 1 //8kHz (注: G.711/G.723.1/G.729 中使用)
#define GB28181_SDP_F_SR_14 2 //14kHz (注: G.722.1 中使用)
#define GB28181_SDP_F_SR_16 3 //16kHz (注: G.722.1 中使用)
#define GB28181_SDP_F_SR_32 4 //32kHz (注: G.722.1 中使用)

#define GB28181_SDP_F_VCODEC_MPEG4 1
#define GB28181_SDP_F_VCODEC_H264 2
#define GB28181_SDP_F_VCODEC_SVAC 3
#define GB28181_SDP_F_VCODEC_3GP 4

#define GB28181_SDP_F_FBL_QCIF 1
#define GB28181_SDP_F_FBL_CIF 2
#define GB28181_SDP_F_FBL_4CIF 3 
#define GB28181_SDP_F_FBL_D1 4
#define GB28181_SDP_F_FBL_720P 5
#define GB28181_SDP_F_FBL_1080P_I 6

#define GB28181_SDP_F_BR_CBR 1 //固定码率
#define GB28181_SDP_F_BR_VBR 2 //可变码率

typedef struct {
    int vcodec, fbl, fps, vbrt, vbr; //视频编码格式、分辨率、帧率、码率类型、码率(kbps)
    int acodec, abr, asr; //音频编码格式、码率、采样率
} GB28181_SDP_f_t;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 附加f字段和y字段
 * @param sdp_f 将f字段中的内容附加到基本sdp消息结尾
 * @param sdp 
 * @return 0 is success. The resulting should be freed with free()
 */
char *GB28181_SDP_append_f(GB28181_SDP_f_t *sdp_f, char *sdp);

/**
 * 生产sdp消息
 * @param id
 * @param ip
 * @param port
 * @param start
 * @param end
 * @param sdp
 * @param size
 * @return strlen(sdp)
 */
int GB28181_SDP_build(char *id, char *ip, char *port, int start, int end, char *sdp, int size);

#ifdef __cplusplus
}//extern "C"
#endif

#endif//_GB28181_SDP_H_
