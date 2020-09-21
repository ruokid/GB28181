# GB28181

- 本项目是协议层工具，仅生成和解析信令
- 项目中使用了一个第三方库：mxml
---

### 使用示例
```
#include "gb28181/std.h"
#include "gb28181/manscdp.h"

char *body;
MANSCDP *manscdp;
manscdp = manscdp_new(MANSCDP_TYPE_QUERY, MANSCDP_CMDTYPE_CATALOG, "设备/网关ID");

body = manscdp2str(manscdp);
/* <?xml version="1.0"?>
 * <Query>
 *     <CmdType>Catalog</CmdType>
 *     <SN>1000001</SN>
 *     <DeviceID>设备/网关ID</DeviceID>
 * </Query>
 */

manscdp_set_node(manscdp, "Query", "StartTime", "2019-08-28T00:00:00");

int slen = manscdp_to_str(manscdp, NULL, 0, &body);
/* body=>
 * <?xml version="1.0"?>
 * <Query>
 *     <CmdType>Catalog</CmdType>
 *     <SN>1000001</SN>
 *     <DeviceID>设备/网关ID</DeviceID>
 *     <StartTime>2019-08-28T00:00:00</StartTime>
 * </Query>
 */
 free(body);

manscdp_get_node_text(manscdp, "Query/CmdType"); //=> Catalog

manscdp_Control_PTZ(manscdp, -90, 180, 10);
/* <?xml version="1.0" encoding="GB2312"?>
 * <Control>
 *   <CmdType>DeviceControl</CmdType>
 *   <DeviceID>设备/网关ID</DeviceID>
 *   <PTZCmd>A50F001A5AB4A07C</PTZCmd>
 *   <SN>100001</SN>
 * </Control>
 */

```

---
### 参考文献
[1]《GBT 28181-2016 公共安全视频监控联网系统信息传输、交换、控制技术要求》

[2]《28181平台对接接口详解》

[3] [https://www.msweet.org/mxml/mxml.html](https://www.msweet.org/mxml/mxml.html)
