/**
 * 监控报警联网系统控制描述协议
 * MANSCDP: Monitoringand Alarming Network System Control Description Protocol
 */

#include <stdio.h>
#include "gb28181/std.h"
#include "gb28181/manscdp.h"
#include "mxml.h"

typedef struct manscdp_event_s {
    const char *cmdtype;
    int (*handler)(MANSCDP *);
    void *pa;
    struct manscdp_event_s *next;
} manscdp_event_t;

static int manscdp_got_catalog_items(MANSCDP *);

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
    /*33*/"SVCTimeSupportMode"
};
static manscdp_event_t *manscdp_events[] = { NULL, NULL, NULL, NULL, NULL };

void *mallocz(size_t);

int manscdp_on(manscdp_type_e type, char *cmdtype, int (*handler)(MANSCDP *))
{
    int isappend = 1;
    manscdp_event_t *e = manscdp_events[type], *new_event;
    while (e != NULL && e->next != NULL) {
        if (strcmp(e->cmdtype, cmdtype) == 0) {
            isappend = 0;
            break;
        }
        e = e->next;
    }

    if (isappend) {
        new_event = (manscdp_event_t *)mallocz(sizeof(manscdp_event_t));
        new_event->cmdtype = cmdtype;
        new_event->handler = handler;
        if (e == NULL) manscdp_events[type] = new_event;
        else e->next = new_event;
        e = new_event;
    }
    else {
        e->handler = handler;
    }

    if (strcmp(MANSCDP_CMDTYPE_CATALOG, cmdtype) == 0)
        if (type != MANSCDP_TYPE_QUERY) new_event->handler = &manscdp_got_catalog_items;

    return isappend;
}

int manscdp_fire(MANSCDP *manscdp)
{
    int retval = -1;
    const char *cmdtype;

    if (manscdp == NULL || manscdp->xml == NULL) return retval;

    cmdtype = mxmlGetText(mxmlFindPath(manscdp->xml, "*/CmdType"), 0);

    if (cmdtype == NULL) return retval;

    manscdp_event_t *e = manscdp_events[manscdp->type];
    while (e != NULL && strcmp(e->cmdtype, cmdtype)) e = e->next;
    if (e != NULL && e->handler != NULL) retval = e->handler(manscdp);

    return retval;
}

/**
 * catalog回调
 * @param manscdp
 * @return 返回单项回调运行次数
 */
static int manscdp_got_catalog_items(MANSCDP *manscdp)
{
    if (manscdp == NULL || manscdp->xml == NULL) return -1;

    const char *sumnum, **item_values;
    int i, n, sum, field_count;
    mxml_node_t *top, *SumNum, *DeviceList, *Item;

    n = 0;
    field_count = sizeof(GB28181_catalog_item_fields) / sizeof(GB28181_catalog_item_fields[0]);
    top = manscdp->xml;
    SumNum = mxmlFindPath(top, "*/SumNum");
    sumnum = mxmlGetText(SumNum, 0);
    sum = sumnum == NULL ? 0 : atoi(sumnum);
    DeviceList = mxmlFindPath(top, "*/DeviceList");
    Item = mxmlFindElement(DeviceList, top, "Item", NULL, NULL, MXML_NO_DESCEND);
    while (Item != NULL) {
        if (mxmlGetType(Item) == MXML_ELEMENT && strcmp("Item", mxmlGetElement(Item)) == 0) {
            if (manscdp_got_catalog_item != NULL) {
                item_values = malloc(sizeof(GB28181_catalog_item_fields));
                if (item_values != NULL) {
                    for (i = 0; i < field_count; i++) {
                        item_values[i] = mxmlGetText(mxmlFindPath(Item, GB28181_catalog_item_fields[i]), 0);
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
