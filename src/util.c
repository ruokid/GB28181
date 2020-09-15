/* gb28181/util.c
 * Copyright (C) 2019-2020,
 */

#include <iconv.h>
#include "mxml.h"

#define EOXE "\n" //end of xml element

void *mallocz(size_t size)
{
    void *p = malloc(size);

    memset(p, 0, size);

    return p;
}

/**
 * 格式化输出xml字符串的回调函数，用于mxmlSaveAllocString
 * @param node
 * @param w
 * @return
 */
const char *xml_fmt_cb(mxml_node_t *node, int w)
{
    char sp[1024] = {0};
    int deep = 0;
    mxml_node_t *child, *parent;

    parent = mxmlGetParent(node);
    while ((parent = mxmlGetParent(parent))) deep++;
    sprintf(sp, "%*s", deep * 4, "");

    switch (w) {
        case MXML_WS_BEFORE_OPEN:
            break;

        case MXML_WS_AFTER_OPEN:
            child = mxmlGetFirstChild(node);
            if (child == NULL) {
                return EOXE;
            }
            while (child && mxmlGetType(child) != MXML_ELEMENT) {
                child = mxmlWalkNext(child, node, MXML_NO_DESCEND);
            }
            return child == NULL ? NULL : EOXE;

        case MXML_WS_BEFORE_CLOSE:
            child = mxmlGetFirstChild(node);
            while (child && mxmlGetType(child) != MXML_ELEMENT) {
                child = mxmlWalkNext(child, node, MXML_NO_DESCEND);
            }
            if (child) {
                break;
            }
            return NULL;

        case MXML_WS_AFTER_CLOSE:
            return EOXE;

        default:
            return NULL;
    }
    const char *ret = sp;

    return ret;
}

/**
 * 使用 iconv 转换字符集
 * @param src
 * @param fromcode
 * @param tocode
 * @return str
 *
 * The resulting should be freed with free()
 */
char *cvt_charset(const char *src, const char *fromcode, const char *tocode)
{
    iconv_t cd;
    size_t srclen = strlen(src), dstlen = srclen * 2, cvtlen;
    char *inbuf, *outbuf, *ip, *op;
    inbuf = (char *)mallocz(srclen + 1);
    if (inbuf == NULL) return NULL;
    outbuf = (char *)mallocz(dstlen);
    if (outbuf == NULL) {
        free(inbuf);
        return NULL;
    }
    cd = iconv_open(tocode, fromcode);
    if (cd == NULL) {
        free(inbuf);
        free(outbuf);
        return NULL;
    }
    ip = inbuf;
    op = outbuf;
    strcpy(inbuf, src);
    cvtlen = iconv(cd, &inbuf, &srclen, &outbuf, &dstlen);
    iconv_close(cd);
    free(ip);

    return op;
}

/**
 * 获取根元素
 * @param xml
 * @return NULL is error
 */
mxml_node_t *mxmlGetRoot(mxml_node_t *xml)
{
    mxml_node_t *root = mxmlGetFirstChild(xml);
    while (root != NULL && mxmlGetType(root) != MXML_ELEMENT) {
        root = mxmlWalkNext(root, xml, MXML_NO_DESCEND);
    }

    return root;
}
