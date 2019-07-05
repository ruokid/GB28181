/**
 * GB28181中SDP扩展的两个描述字段f和y
 * f描述了音视频格式相关参数。使用f字段时应保证视频和音频参数的结构完整性，即在任何时候f字段的结构都应是完整的结构
 *  f=v/编码格式/分辨率/帧率/码率类型/码率大小a/编码格式/码率大小/采样率
 * 
 * y描述了时效和SSRC，0为实时，1为录像
 * 第1位表示是否为历史 + 第2位至第6位取20位SIP监控域ID之中的4到8位 + 第7位至第10位是SSRC的后4位
 * 例如：Invite 12345678901234567890, SSRC: 0x063497bd(104110013)
 *  y=0456780013
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
    int acodec; //音频编码格式
    int abr; //音频码率
    int asr; //音频采样率
    int vcodec; //视频编码格式
    int vfbl; //视频分辨率
    int fps; //帧率
    int vbr_type; //视频码率
    int vbr; //kbps
} GB28181_SDP_f_t;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 附加f字段
 * @param sdp
 * @param sdp_f
 * @return 0 is success
 * 
 * @example
 *  GB28181_SDP_f_t sdp_f = {GB28181_SDP_F_ACODEC_G7231, GB28181_SDP_F_BR_32}
 * 
 *  GB28181_SDP_append_f(sdp, &sdp_f);
 *  f=v/////a/2/6/
 */
int GB28181_SDP_append_f(char *sdp, GB28181_SDP_f_t *sdp_f);

#ifdef __cplusplus
}//extern "C"
#endif

#endif//_GB28181_SDP_H_
