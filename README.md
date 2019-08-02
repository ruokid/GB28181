# GB28181

- 本项目是协议层工具，仅生成和解析信令
- 项目中使用了一个第三方库：mxml
---

使用示例
```
// 添加事件入口，第三个参数是回调函数指针
manscdp_on(MANSCDP_TYPE_NOTIFY, MANSCDP_CMDTYPE_KEEPALIVE, NULL);

MANSCDP *manscdp = manscdp_parse(strxml);

// 触发事件
manscdp_fire(manscdp);

manscdp_free(manscdp);
```

当Catalog回调传入NULL时会使用默认的回调函数`manscdp_got_catalog_items`

同时如果`manscdp_got_catalog_item`被赋值，那么它将会根据Item个数被多次调用

---
### 参考文献
[1]《GBT 28181-2016 公共安全视频监控联网系统信息传输、交换、控制技术要求》

[2]《28181平台对接接口详解》

[3] [https://www.msweet.org/mxml/mxml.html](https://www.msweet.org/mxml/mxml.html)
