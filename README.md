# GB28181

- 本项目是协议层工具，仅生成和解析信令
- 项目中使用了一个第三方库：mxml
---
```
gcc -std=c99 \
-o libgb28181.so \
-I ${workspaceFolder}/include \
-I ${workspaceFolder}/third/mxml-3.0/include \
-liconv \
${workspaceFolder}/third/mxml-3.0/lib/libmxml.a \
${workspaceFolder}/src/gb28181.c \
${workspaceFolder}/src/manscdp.c \
${workspaceFolder}/src/util.c
```
使用示例
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

body = manscdp2str(manscdp);
/* <?xml version="1.0"?>
 * <Query>
 *     <CmdType>Catalog</CmdType>
 *     <SN>1000001</SN>
 *     <DeviceID>设备/网关ID</DeviceID>
 *     <StartTime>2019-08-28T00:00:00</StartTime>
 * </Query>
 */

manscdp_get_node_text(manscdp, "Query/CmdType"); //=> Catalog

```

---
### 参考文献
[1]《GBT 28181-2016 公共安全视频监控联网系统信息传输、交换、控制技术要求》

[2]《28181平台对接接口详解》

[3] [https://www.msweet.org/mxml/mxml.html](https://www.msweet.org/mxml/mxml.html)
