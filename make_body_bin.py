import struct

# Step 1: UTF 字符串 ("helloworld")
utf_str = "<cons_sys_code></cons_sys_code><cons_node_id></cons_node_id><cons_flow_no></cons_flow_no><cons_tran_id></cons_tran_id><cons_tran_time></cons_tran_time>".encode("utf-8")

# Step 2: XML 字符串
xml_str = b'<SERVICE_CODE attr="s,30">01001000001</SERVICE_CODE><SERVICE_SCENE attr="s,2">00</SERVICE_SCENE>'

# Step 3: 开始拼接二进制
body = bytearray()

# 2 字节 UTF 长度（big-endian）
body += struct.pack(">H", len(utf_str))

# UTF 字节
body += utf_str

# 4 字节 XML 长度（big-endian）
body += struct.pack(">I", len(xml_str))

# XML 字节
body += xml_str

# Step 4: 写入 body.bin
with open("body.bin", "wb") as f:
    f.write(body)

print("生成完毕，body.bin 已创建。")
print(f"总长度: {len(body)} 字节")
