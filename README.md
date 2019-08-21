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
---
### 参考文献
[1]《GBT 28181-2016 公共安全视频监控联网系统信息传输、交换、控制技术要求》

[2]《28181平台对接接口详解》

[3] [https://www.msweet.org/mxml/mxml.html](https://www.msweet.org/mxml/mxml.html)
