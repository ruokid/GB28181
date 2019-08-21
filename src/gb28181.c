/*
 * GB28181
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gb28181/std.h"
#include "gb28181/manscdp.h"
#include "gb28181/sdp.h"

const char *GB28181_catalog_item_fields[] = {
    /* 0*/"DeviceID",
    /* 1*/"Name",
    /* 2*/"Manufacturer",
    /* 3*/"Model",
    /* 4*/"Owner",
    /* 5*/"CivilCode",
    /* 6*/"Block",
    /* 7*/"Address",
    /* 8*/"Parental",
    /* 9*/"ParentID",
    /*10*/"SafetyWay",
    /*11*/"RegisterWay",
    /*12*/"CertNum",
    /*13*/"Certifiable",
    /*14*/"ErrCode",
    /*15*/"EndTime",
    /*16*/"Secrecy",
    /*17*/"IPAddress",
    /*18*/"Port",
    /*19*/"Password",
    /*20*/"Status",
    /*21*/"Longitude",
    /*22*/"Latitude",
    /*23*/"PTZType",
    /*24*/"PositionType",
    /*25*/"RoomType",
    /*26*/"UseType",
    /*27*/"SupplyLightType",
    /*28*/"DirectionType",
    /*29*/"Resolution",
    /*30*/"BusinessGroupID",
    /*31*/"DownloadSpeed",
    /*32*/"SVCSpaceSupportMode",
    /*33*/"SVCTimeSupportMode",
    NULL
};
const char *GB28181_file_item_fields[] = {
    "DeviceID",
    "Name",
    "FilePath",
    "Address",
    "StartTime",
    "EndTime",
    "Secrecy",
    "Type",
    "RecorderID",
    "FileSize",
    NULL
};

int GB28181_get_code_from_id(const char *gbid, int start, int end)
{
	int itype = 0,
		gbidlen = strlen(gbid),
		dv = end - start;
	char subcode[20] = {0};

	if (gbidlen >= end && start >= 0 && dv > 0) {
		strncpy(subcode, gbid + start, dv);
		itype = atoi(subcode);
	}

	return itype;
}

char *GB28181_SDP_append_f(GB28181_SDP_f_t *sdp_f, char *sdp)
{
    if (sdp_f == NULL || sdp == NULL) return NULL;

    char *newsdp, f[100] = {0}, *fp = f;
    int i, j, l1, l2;

    l1 = strlen(sdp);
    if (l1 > 2 && strcmp("\r\n", sdp + l1 - 2) == 0) l1 = l1 - 2;

    fp = fp + sprintf(fp, "\r\nf=v/");

    if (sdp_f->vcodec > 0) {
        fp = fp + sprintf(fp, "%d", sdp_f->vcodec);
    }
    fp = fp + sprintf(fp, "/");
    if (sdp_f->fbl > 0) {
        fp = fp + sprintf(fp, "%d", sdp_f->fbl);
    }
    fp = fp + sprintf(fp, "/");
    if (sdp_f->fps > 0) {
        fp = fp + sprintf(fp, "%d", sdp_f->fps);
    }
    fp = fp + sprintf(fp, "/");
    if (sdp_f->vbrt > 0) {
        fp = fp + sprintf(fp, "%d", sdp_f->vbrt);
    }
    fp = fp + sprintf(fp, "/");
    if (sdp_f->vbr > 0) {
        fp = fp + sprintf(fp, "%d", sdp_f->vbr);
    }
    fp = fp + sprintf(fp, "a/");
    if (sdp_f->acodec > 0) {
        fp = fp + sprintf(fp, "%d", sdp_f->acodec);
    }
    fp = fp + sprintf(fp, "/");
    if (sdp_f->abr > 0) {
        fp = fp + sprintf(fp, "%d", sdp_f->abr);
    }
    fp = fp + sprintf(fp, "/");
    if (sdp_f->asr > 0) {
        fp = fp + sprintf(fp, "%d", sdp_f->asr);
    }

    l2 = l1 + strlen(f) + 1;

    newsdp = (char *)malloc(l2);
    memset(newsdp, 0, l2);
    strncpy(newsdp, sdp, l1);
    strcat(newsdp, f);

    printf("%s\n", newsdp);

    return newsdp;
}

int GB28181_SDP_build(char *id, char *ip, char *port, int start, int end, char *sdp, int size)
{
    char *s = "Play";
    int n = 0;
    if (end > 0) s = "Playback";

    snprintf(sdp, size, 
        "v=0\r\n"
        "o=%s 0 0 IN IP4 %s\r\n"
        "s=%s\r\n"
        "c=IN IP4 %s\r\n"
        "t=%d %d\r\n"
        "m=video %s RTP/AVP 96 97 98\r\n"
        "a=rtpmap:96 PS/90000\r\n"
        "a=rtpmap:97 MPEG4/90000\r\n"
        "a=rtpmap:98 H264/90000\r\n"
        "a=recvonly",
        id, ip, s, ip, start, end, port);

    while (sdp[n] != '\0') n++;

    return n;
}
