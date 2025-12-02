import sys
import struct
import os

def main():
    if len(sys.argv) < 2:
        print("用法: python txt2bin.py 文件名.txt")
        return

    txt_path = sys.argv[1]

    if not os.path.exists(txt_path):
        print(f"文件不存在: {txt_path}")
        return

    # 自动生成输出名，例如 a.txt → a.bin
    base, _ = os.path.splitext(txt_path)
    bin_path = base + ".bin"

    # 读取文本
    with open(txt_path, "r", encoding="utf-8") as f:
        text = f.read()

    # 转成UTF-8字节
    body = text.encode("utf-8")

    # 计算长度
    length = len(body)

    # 长度头（大端 4 字节）
    header = struct.pack(">I", length)

    # 写入输出
    with open(bin_path, "wb") as f:
        f.write(header + body)

    print(f"成功生成: {bin_path}  (长度={length} 字节)")

if __name__ == "__main__":
    main()
