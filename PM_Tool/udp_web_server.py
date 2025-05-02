import socket
import threading
from flask import Flask, jsonify, render_template_string

# 保存最近一次收到的数据

#运行前需先安装flask包
# pip install flask

latest_data = {}

# 创建 Flask 应用
app = Flask(__name__)

# 网页模板（简洁可视化）
html_template = """
<!doctype html>
<html>
    <head>
        <title>UDP JSON 数据接收</title>
        <meta charset="utf-8"/>
        <meta http-equiv="refresh" content="2">  <!-- 每2秒刷新 -->
    </head>
    <body>
        <h2>最近收到的 JSON 数据：</h2>
        <pre>{{ data }}</pre>
    </body>
</html>
"""

@app.route("/")
def index():
    return render_template_string(html_template, data=latest_data)

@app.route("/json")
def get_json():
    return jsonify(latest_data)

# UDP 接收线程
def udp_listener(host='0.0.0.0', port=12345):
    global latest_data
    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_socket.bind((host, port))
    print(f"[UDP监听] 正在监听 {host}:{port} ...")

    while True:
        data, addr = udp_socket.recvfrom(4096)
        print(f"[UDP] 收到来自 {addr} 的数据：{data.decode(errors='ignore')}")
        try:
            import json
            latest_data = json.loads(data.decode('utf-8'))
        except json.JSONDecodeError:
            latest_data = {"error": "接收到的数据不是合法的 JSON"}

# 启动服务
if __name__ == '__main__':
    # 启动 UDP 接收线程
    threading.Thread(target=udp_listener, daemon=True).start()

    # 启动 Flask web 服务器
    app.run(host='127.0.0.1', port=5000)
