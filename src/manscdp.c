/* 监控报警联网系统控制描述协议
 * MANSCDP(Monitoringand Alarming Network System Control Description Protocol)
 * Copyright (C) 2019-2020,
 */

#include <stdio.h>
#include "gb28181/std.h"
#include "gb28181/manscdp.h"
#include "mxml.h"

#ifdef MINISIZE
#define mxml_save_cb MXML_NO_CALLBACK
#else
#define mxml_save_cb xml_fmt_cb
#endif

#define MANSCDP_PTZCMD_FIXED 0xA5   /* 首字节固定值 */
#define MANSCDP_PTZCMD_VER 0        /* 本标准的版本号是1.0，版本信息为0 */
/**
 * 建立第1, 2, 8字节内容
 * @param cc 指令码(command code)
 * @param ver 版本信息 MANSCDP_PTZCMD_VER
 * 
 * @note
 *  字节1: MANSCDP_PTZCMD_FIXED
 *  字节2: 为版本信息和校验信息，校验信息 = (字节1的高4位 + 字节1的低4位 + 字节2的高4位) % 16
 *  字节8: 校验和
 */
#define MANSCDP_PTZCMD_CHECKSUM(cc, ver) {\
    cc[0] = MANSCDP_PTZCMD_FIXED;\
    cc[1] = (MANSCDP_PTZCMD_VER << 4) | (((MANSCDP_PTZCMD_FIXED >> 4) + (MANSCDP_PTZCMD_FIXED & 0x0F) + (MANSCDP_PTZCMD_VER >> 4)) % 16);\
    cc[7] = (cc[0] + cc[1] + cc[2] + cc[3] + cc[4] + cc[5] + cc[6]) % 256;\
}

#define mxmlFindXPath(manscdp, xpath) (manscdp ?\
    (xpath ? mxmlFindPath(manscdp->xml, xpath) : manscdp->xml) : NULL)

static int seq_num = 100000;

manscdp_item_cb manscdp_got_catalog_item = NULL;
manscdp_item_cb manscdp_got_file_item = NULL;

char *cvt_charset(const char *, const char *, const char *);
const char *xml_fmt_cb(mxml_node_t *, int);
mxml_node_t *mxmlGetRoot(mxml_node_t *);

MANSCDP *manscdp_new(manscdp_type_e type, const char *cmdtype, const char *device_id)
{
    int err, sn;
    mxml_node_t *xml, *root, *CmdType, *SN, *DeviceID;
    MANSCDP *manscdp;

    // xml = mxmlNewXML("1.0"); //使用这个函数会包含 encoding="utf-8" 属性
    // xml = mxmlNewElement(NULL, "?xml version=\"1.0\"?"); //不使用encoding属性
    xml = mxmlNewElement(NULL, "?xml version=\"1.0\" encoding=\"gb2312\"?"); //文档中推荐使用GB2312编码格式
    if (xml == NULL) return NULL;

    switch (type) {
        case MANSCDP_TYPE_CONTROL:
            root = mxmlNewElement(xml, "Control");
            break;

        case MANSCDP_TYPE_NOTIFY:
            root = mxmlNewElement(xml, "Notify");
            break;

        case MANSCDP_TYPE_QUERY:
            root = mxmlNewElement(xml, "Query");
            break;

        case MANSCDP_TYPE_RESPONSE:
            root = mxmlNewElement(xml, "Response");
            break;

        default:
            mxmlDelete(xml);
            return NULL;
    }

    if (root) {
        sn = ++seq_num;
        CmdType = mxmlNewElement(root, "CmdType");
        SN = mxmlNewElement(root, "SN");
        DeviceID = mxmlNewElement(root, "DeviceID");
        mxmlNewText(CmdType, 0, cmdtype);
        mxmlNewInteger(SN, sn);
        mxmlNewText(DeviceID, 0, device_id);
    }

    if ((manscdp = malloc(sizeof(MANSCDP))) == NULL) { // Cannot allocate memory
        mxmlDelete(xml);
        return NULL;
    }
    memset(manscdp, 0, sizeof(MANSCDP));
    manscdp->type = type;
    manscdp->sn = sn;
    manscdp->xml = xml;

    return manscdp;
}

MANSCDP *manscdp_parse(const char *xmlstr)
{
    int i;
    char *uxmlstr, encoding[64] = "gbk";
    const char *sp;
    manscdp_type_e mt;
    mxml_node_t *xml, *root;
    MANSCDP *manscdp;

    if (xmlstr == NULL) return NULL;

    sp = strstr(xmlstr, "encoding=\"");
    if (sp) {
        strncpy(encoding, sp + 10, 63);
        for (i = 0; i < 63; i++) {
            if (encoding[i] == '"') {
                encoding[i] = '\0';
                break;
            }
        }
    }

    // uxmlstr = cvt_charset(xmlstr, encoding, "utf-8"); //有时候gb2312转码会错误
    uxmlstr = cvt_charset(xmlstr, "gbk", "utf-8");
    xml = mxmlLoadString(NULL, uxmlstr, MXML_OPAQUE_CALLBACK);
    root = mxmlGetRoot(xml);

    if (uxmlstr) free(uxmlstr);

    if (root == NULL) {
        // "Load xml string error"
        fprintf(stderr, "MANSCDP parse error: incorrect xml\n%s\n", xmlstr);
        mxmlDelete(xml);
        return NULL;
    }

    sp = mxmlGetElement(root);
    if (strcmp(sp, "Response") == 0) {
        mt = MANSCDP_TYPE_RESPONSE;
    }
    else if (strcmp(sp, "Control") == 0) {
        mt = MANSCDP_TYPE_CONTROL;
    }
    else if (strcmp(sp, "Notify") == 0) {
        mt = MANSCDP_TYPE_NOTIFY;
    }
    else if (strcmp(sp, "Query") == 0) {
        mt = MANSCDP_TYPE_QUERY;
    }
    else { //unsupported
        fprintf(stderr, "MANSCDP parse error: unsupported type\n");
        mxmlSaveFile(xml, stderr, MXML_NO_CALLBACK);
        mxmlDelete(xml);
        return NULL;
    }

    if ((manscdp = malloc(sizeof(MANSCDP)))) {
        memset(manscdp, 0, sizeof(MANSCDP));
        manscdp->type = mt;
        manscdp->cmdtype = mxmlGetOpaque(mxmlFindPath(root, "CmdType"));
        manscdp->device_id = mxmlGetOpaque(mxmlFindPath(root, "DeviceID"));
        manscdp->xml = xml;

        sp = mxmlGetOpaque(mxmlFindPath(root, "SN"));
        manscdp->sn = sp == NULL ? -1 : atoi(sp);
    }

    return manscdp;
}

void manscdp_free(MANSCDP *manscdp)
{
    if (manscdp == NULL) return;

    mxmlDelete(manscdp->xml);
    manscdp->xml = NULL;

    free(manscdp);
}

const char *manscdp_get_node_attr(MANSCDP *manscdp, const char *xpath, const char *name)
{
    mxml_node_t *node = mxmlFindXPath(manscdp, xpath);

    return mxmlElementGetAttr(node, name);
}

const char *manscdp_get_node_text(MANSCDP *manscdp, const char *xpath)
{
    return mxmlGetOpaque(mxmlFindXPath(manscdp, xpath));
}

int manscdp_set_node(MANSCDP *manscdp, const char *xpath, char *name, char *value)
{
    if (manscdp == NULL || manscdp->xml == NULL) return -1;

    int err = -1;
    mxml_node_t *top, *root, *node;
    top = manscdp->xml;
    if (xpath != NULL) {
        root = mxmlFindPath(top, xpath);
    }
    else {
        root = mxmlGetRoot(top);
    }

    if (root != NULL) {
        node = mxmlFindPath(root, name);
        if (node) {
            err = mxmlSetText(node, 0, value);
        }
        else {
            if ((node = mxmlNewElement(root, name)) != NULL) {
                err = mxmlNewText(node, 0, value) == NULL;
            }
        }
    }

    return err;
}

int manscdp_infile(MANSCDP *manscdp, const char *xpath, void *fd)
{
    mxml_node_t *node = mxmlFindXPath(manscdp, xpath);
    if (node == NULL) return -1;

    return mxmlSaveFile(node, fd, mxml_save_cb);
}

int manscdp_to_str(MANSCDP *manscdp, char *buf, int bufsize, char **newstr)
{
    if (manscdp == NULL || manscdp->xml == NULL) return -1;

    int len = 0;

    if (manscdp->device_id) {
        manscdp_set_node(manscdp, NULL, "DeviceID", (char *)manscdp->device_id);
    }

    if (newstr != NULL) {
        *newstr = mxmlSaveAllocString(manscdp->xml, mxml_save_cb);
        len = strlen(*newstr);
    }
    else {
        len = mxmlSaveString(manscdp->xml, buf, bufsize, mxml_save_cb);
    }

    return len;
}

int manscdp_got_items(MANSCDP *manscdp)
{
    if (manscdp == NULL || manscdp->xml == NULL) return -1;

    const char *cmdtype, *list_path, *sumnum, **item_fields, **item_values;
    int i, n, sum, field_count;
    mxml_node_t *top, *SumNum, *ItemList, *Item;

    top = manscdp->xml;
    cmdtype = mxmlGetOpaque(mxmlFindPath(top, "*/CmdType"));
    if (cmdtype == NULL) return -1;

    if (strcmp(cmdtype, "Catalog") == 0) {
        list_path = "*/DeviceList";
        item_fields = GB28181_catalog_item_fields;
    }
    else if (strcmp(cmdtype, "RecordInfo") == 0) {
        list_path = "*/RecordList";
        item_fields = GB28181_file_item_fields;
    }
    else {
        list_path = NULL;
        item_fields = NULL;
    }

    if (item_fields == NULL) return -1;

    for (field_count = 0; item_fields[field_count] != NULL; field_count++);

    n = 0;
    SumNum = mxmlFindPath(top, "*/SumNum");
    sumnum = mxmlGetOpaque(SumNum);
    sum = sumnum == NULL ? 0 : atoi(sumnum);
    ItemList = mxmlFindPath(top, list_path);
    Item = mxmlFindElement(ItemList, top, "Item", NULL, NULL, MXML_NO_DESCEND);
    while (Item != NULL) {
        if (mxmlGetType(Item) == MXML_ELEMENT && strcmp("Item", mxmlGetElement(Item)) == 0) {
            if (manscdp_got_catalog_item != NULL) {
                item_values = malloc(field_count * sizeof(item_fields[0]));
                if (item_values != NULL) {
                    for (i = 0; i < field_count; i++) {
                        item_values[i] = mxmlGetOpaque(mxmlFindPath(Item, item_fields[i]));
                    }
                    manscdp_got_catalog_item(manscdp, Item, item_values, field_count);
                    free(item_values);
                }
            }
            n++;
        }
        Item = mxmlWalkNext(Item, top, MXML_NO_DESCEND);
    }

    return n;
}

int manscdp_get_items(MANSCDP *manscdp, manscdp_item_cb2 cb)
{
    const char *sp, **item_fields, *item_values[34];
    int i, n, sum, field_count;
    mxml_node_t *root, *SumNum, *ItemList, *Item;

    if (manscdp == NULL || manscdp->xml == NULL) return -1;

    root = mxmlGetRoot(manscdp->xml);
    if (strcmp(manscdp->cmdtype, "Catalog") == 0) {
        ItemList = mxmlFindPath(root, "DeviceList");
        item_fields = GB28181_catalog_item_fields;
    }
    else if (strcmp(manscdp->cmdtype, "RecordInfo") == 0) {
        ItemList = mxmlFindPath(root, "RecordList");
        item_fields = GB28181_file_item_fields;
    }
    else {
        ItemList = NULL;
        return -1;
    }

    if (cb == NULL) return 0;
    
    sp = mxmlElementGetAttr(mxmlGetParent(ItemList), "Num");
    sp = mxmlGetOpaque(mxmlFindPath(root, "SumNum"));
    sum = sp == NULL ? 0 : atoi(sp);
    n = 0;

    Item = mxmlFindElement(ItemList, manscdp->xml, "Item", NULL, NULL, MXML_NO_DESCEND);
    while (Item != NULL) {
        if (mxmlGetType(Item) == MXML_ELEMENT && strcmp("Item", mxmlGetElement(Item)) == 0) {
            for (i = 0; item_fields[i]; i++) {
                item_values[i] = mxmlGetOpaque(mxmlFindPath(Item, item_fields[i]));
            }
            cb(manscdp, item_fields, item_values, i);
            n++;
        }
        Item = mxmlWalkNext(Item, manscdp->xml, MXML_NO_DESCEND);
    }

    return n;
}

int manscdp_device_config_Basic(MANSCDP *manscdp, char *name, int exp, int interval, int count)
{
    if (manscdp == NULL || manscdp->xml == NULL) return -1;

    mxml_node_t *top, *root, *node;
    top = manscdp->xml;
    root = mxmlFindPath(top, "Control");
    root = mxmlNewElement(root, "BasicParam");
    if (name != NULL) {
        node = mxmlNewElement(root, "Name");
        mxmlNewText(node, 0, name);
    }
    if (exp > 0) {
        node = mxmlNewElement(root, "Expiration");
        mxmlNewInteger(node, exp);
    }
    if (interval > 0) {
        node = mxmlNewElement(root, "HeartBeatInterval");
        mxmlNewInteger(node, interval);
    }
    if (count > 0) {
        node = mxmlNewElement(root, "HeartBeatCount");
        mxmlNewInteger(node, count);
    }

    return 0;
}

int manscdp_Control_DeviceControl(MANSCDP *manscdp, int type, int byte4, int byte5, int byte6, int byte7, int addr)
{
    if (manscdp == NULL || manscdp->xml == NULL) return -1;

    int act = 0, B4 = byte5, B5 = byte6, B6 = byte7;
    char cmd[17] = { 0 };
    unsigned char cc[8] = { 0 };

    switch (type) {
        case MANSCDP_PTZCMD_PTZ:
            if (B4 > 0) {
                act |= MANSCDP_PTZ_RIGHT;
            }
            else if (B4 < 0) {
                act |= MANSCDP_PTZ_LEFT;
                B4 = -B4;
            }

            if (B5 > 0) {
                act |= MANSCDP_PTZ_UP;
            }
            else if (B5 < 0) {
                act |= MANSCDP_PTZ_DOWN;
                B5 = -B5;
            }

            if (B6 > 0) {
                act |= MANSCDP_PTZ_ZOOM_IN;
            }
            else if (B6 < 0) {
                act |= MANSCDP_PTZ_ZOOM_OUT;
                B6 = -B6;
            }
            break;

        case MANSCDP_PTZCMD_FI:
            if (B4 > 0) {
                act |= MANSCDP_FI_FOCUS_OUT;
            }
            else if (B4 < 0) {
                act |= MANSCDP_FI_FOCUS_IN;
                B4 = -B4;
            }

            if (B5 > 0) {
                act |= MANSCDP_FI_IRIS_UP;
            }
            else if (B5 < 0) {
                act |= MANSCDP_FI_IRIS_DOWN;
                B5 = -B5;
            }
            break;

        case MANSCDP_PTZCMD_CHECKPOINT:
            act = byte4 & 0xff;
            break;

        case MANSCDP_PTZCMD_CRUISE:
            break;

        case MANSCDP_PTZCMD_SCAN:
            break;

        case MANSCDP_PTZCMD_AUXSWITCH:
            if (B5 > 0) {
                act |= MANSCDP_AUXSWITCH_ON;
            }
            else if (B5 < 0) {
                act |= MANSCDP_AUXSWITCH_OFF;
                B5 = -B5;
            }
            break;

        default:
            return -2;
    }

    // 字节1：固定值
    cc[0] = MANSCDP_PTZCMD_FIXED;
    // 字节2：高4位是版本信息，低4位是校验位；校验位 = (字节1的高4位 + 字节1的低4位 + 字节2的高4位) % 16
    cc[1] = (MANSCDP_PTZCMD_VER << 4) |
        (((MANSCDP_PTZCMD_FIXED >> 4) + (MANSCDP_PTZCMD_FIXED & 0x0F) + (MANSCDP_PTZCMD_VER >> 4)) % 16);
    // 字节3：地址的低8位
    cc[2] = addr & 0xff;
    // 字节4：指令码
    cc[3] = act;
    // 字节5：数据1
    cc[4] = B4 & 0xff;
    // 字节6：数据2
    cc[5] = B5 & 0xff;
    // 字节7：高4位是数据3，低4位是地址的高4位
    cc[6] = ((B6 & 0x0f) << 4) | ((addr & 0xf0) >> 4);
    // 字节8：前面的第1~7字节的算术和的低8位，即算术和对256取模后的结果
    cc[7] = (cc[0] + cc[1] + cc[2] + cc[3] + cc[4] + cc[5] + cc[6]) % 256;

    sprintf(cmd, "%02X%02X%02X%02X%02X%02X%02X%02X",
        cc[0], cc[1], cc[2], cc[3], cc[4], cc[5], cc[6], cc[7]);

    return manscdp_set_node(manscdp, "Control", "PTZCmd", cmd);
}
