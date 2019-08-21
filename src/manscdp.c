/**
 * 监控报警联网系统控制描述协议
 * MANSCDP: Monitoringand Alarming Network System Control Description Protocol
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

static int seq_num = 1000000;

manscdp_item_cb manscdp_got_catalog_item = NULL;
manscdp_item_cb manscdp_got_file_item = NULL;

char *cvt_charset(const char *, const char *, const char *);
const char *xml_fmt_cb(mxml_node_t *, int);
int manscdp_notify_keepalive(MANSCDP *);
int manscdp_response_catalog_item(MANSCDP *);
static void manscdp_build_control_cmd(MANSCDP *);

MANSCDP *
manscdp_new(manscdp_type_e type, const char *cmdtype, const char *device_id)
{
    int err, sn;
    mxml_node_t *xml, *root, *CmdType, *SN, *DeviceID;
    MANSCDP *manscdp;

    // xml = mxmlNewXML("1.0"); //使用这个函数会带上 encoding="utf-8"
    xml = mxmlNewElement(NULL, "?xml version=\"1.0\"?"); //GB28181推荐视同GB2312编码格式
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
            type = MANSCDP_TYPE_UNSUPPORTED;
            mxmlDelete(xml);
            root = NULL;
            xml = NULL;
            break;
    }

    if (root) {
        sn = ++seq_num;
        CmdType = mxmlNewElement(root, "CmdType");
        mxmlNewText(CmdType, 0, cmdtype);
        SN = mxmlNewElement(root, "SN");
        mxmlNewInteger(SN, sn);

        if (device_id) {
            DeviceID = mxmlNewElement(root, "DeviceID");
            mxmlNewText(DeviceID, 0, device_id);
        }
    }

    manscdp = (MANSCDP *)malloc(sizeof(MANSCDP));
    if (manscdp == NULL) { // Cannot allocate memory
        mxmlDelete(xml);
        return NULL;
    }
    memset(manscdp, 0, sizeof(MANSCDP));
    manscdp->type = type;
    manscdp->sn = sn;
    manscdp->xml = xml;

    return manscdp;
}

MANSCDP *
manscdp_parse(const char *strxml)
{
    char *ustrxml = cvt_charset(strxml, "gbk", "utf-8");
    const char *strtype, *strsn;
    MANSCDP *manscdp;
    manscdp_type_e mt;
    mxml_node_t *top, *root;

    top = mxmlLoadString(NULL, ustrxml, MXML_OPAQUE_CALLBACK);
    root = mxmlGetFirstChild(top);
    while (root != NULL && mxmlGetType(root) != MXML_ELEMENT) {
        root = mxmlWalkNext(root, top, MXML_NO_DESCEND);
    }

    if (root == NULL) {
        // "Load xml string error"
        fprintf(stderr, "MANSCDP parse error: incorrect xml\n%s\n", strxml);
        goto failure_of_parsing;
    }

    strtype = mxmlGetElement(root);
    if (strcmp(strtype, "Response") == 0) {
        mt = MANSCDP_TYPE_RESPONSE;
    }
    else if (strcmp(strtype, "Control") == 0) {
        mt = MANSCDP_TYPE_CONTROL;
    }
    else if (strcmp(strtype, "Notify") == 0) {
        mt = MANSCDP_TYPE_NOTIFY;
    }
    else if (strcmp(strtype, "Query") == 0) {
        mt = MANSCDP_TYPE_QUERY;
    }
    else { //unsupported
        fprintf(stderr, "MANSCDP parse error: unsupported type\n");
        mxmlSaveFile(top, stderr, MXML_NO_CALLBACK);
        goto failure_of_parsing;
    }

    strsn = mxmlGetOpaque(mxmlFindPath(root, "SN"));

    manscdp = (MANSCDP *)malloc(sizeof(MANSCDP));
    if (manscdp == NULL) {
        fprintf(stderr, "MANSCDP parse error: cannot allocate memory\n");
        goto failure_of_parsing;
    }
    memset(manscdp, 0, sizeof(MANSCDP));
    manscdp->type = mt;
    manscdp->oxml = ustrxml;
    manscdp->xml = top;
    manscdp->sn = strsn == NULL ? 0 : atoi(strsn);

    return manscdp;

failure_of_parsing:
    if (ustrxml != NULL) free(ustrxml);
    mxmlDelete(top);
    return NULL;
}

void manscdp_free(MANSCDP *manscdp)
{
    if (manscdp == NULL) return;

    if (manscdp->oxml != NULL) free(manscdp->oxml);

    if (manscdp->xml != NULL) mxmlDelete(manscdp->xml);

    free(manscdp);
}

const char *
manscdp_get_node_attr(MANSCDP *manscdp, const char *xpath, const char *name)
{
    if (manscdp == NULL || manscdp->xml == NULL) return NULL;

    mxml_node_t *node = xpath == NULL ?
        manscdp->xml : mxmlFindPath(manscdp->xml, xpath);

    return mxmlElementGetAttr(node, name);
}

const char *
manscdp_get_node_text(MANSCDP *manscdp, const char *xpath)
{
    if (manscdp == NULL || manscdp->xml == NULL || xpath == NULL) return NULL;
    return mxmlGetOpaque(mxmlFindPath(manscdp->xml, xpath));
}

int manscdp_set_node(MANSCDP *manscdp, const char *xpath, char *name, char *value)
{
    if (manscdp == NULL || manscdp->xml == NULL) return -1;

    int err = -1;
    mxml_node_t *top, *root, *node;
    top = manscdp->xml;
    if (xpath) {
        root = mxmlFindPath(top, xpath);
    }
    else {
        root = mxmlGetFirstChild(top); //去掉xml头
        while (root != NULL && mxmlGetType(root) != MXML_ELEMENT) { //确认root存在并且是一个元素
            root = mxmlWalkNext(root, top, MXML_NO_DESCEND);
        }
    }

    if (root) {
        node = mxmlFindPath(root, name);
        if (node) {
            mxmlSetText(node, 0, value);
        }
        else {
            node = mxmlNewElement(root, name);
            mxmlNewText(node, 0, value);
        }

        err = 0;
    }

    return err;
}

int manscdp_infile(MANSCDP *manscdp, const char *xpath, FILE *fd)
{
    if (manscdp == NULL || manscdp->xml == NULL) return -1;

    mxml_node_t *node = xpath == NULL ?
        manscdp->xml : mxmlFindPath(manscdp->xml, xpath);

    return mxmlSaveFile(node, fd, mxml_save_cb);
}

int manscdp_to_str(MANSCDP *manscdp, char *buf, int bufsize, char **newstr)
{
    if (manscdp == NULL || manscdp->xml == NULL) return -1;

    int len = 0;
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
    else item_fields = NULL;
    
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

int manscdp_device_control_PTZ(MANSCDP *manscdp, int act, int zoom, int tilt, int pan)
{
    if (manscdp == NULL || manscdp->xml == NULL) return -1;

    unsigned char *cc = manscdp->cmd;

    if ((act & 0xc0) > 0
        || (act & 0x30) == 0x30   //缩放
        || (act & 0x12) == 0x12   //上下
        || (act & 0x03) == 0x03)  //左右
    {
        // assert 错误动作参数
        return -2;
    }

    cc[3] = act;
    cc[4] = tilt & 0xff;
    cc[5] = pan & 0xff;
    cc[6] = (zoom & 0x0f) << 4;

    char cmd[32] = {0};

    manscdp_build_control_cmd(manscdp);

    sprintf(cmd, "%02X%02X%02X%02X%02X%02X%02X%02X",
        cc[0], cc[1], cc[2], cc[3], cc[4], cc[5], cc[6], cc[7]);

    return manscdp_set_node(manscdp, "PTZCmd", cmd, "Control");
}

/**
 * 建立第1, 2, 8字节内容
 * @param manscdp
 */
static void
manscdp_build_control_cmd(MANSCDP *manscdp)
{
    int fixed = 0xA5, ccver = 0;
    unsigned char *cc = manscdp->cmd;
    cc[0] = fixed; //固定内容
    cc[1] = (ccver << 4) | (((fixed >> 4) + (fixed & 0x0F) + ccver) % 16); //版本信息和校验值
    cc[7] = (cc[0] + cc[1] + cc[2] + cc[3] + cc[4] + cc[5] + cc[6]) % 256;
}
