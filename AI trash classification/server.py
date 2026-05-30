from flask import Flask, request, jsonify, render_template_string
from datetime import datetime

app = Flask(__name__)

# 🟢 تخزين النتائج
history = []

# 🖥️ HTML UI داخل الكود
HTML_PAGE = """
<!DOCTYPE html>
<html>
<head>
    <title>Recycle Dashboard</title>

    <style>
        body {
            font-family: Arial;
            background: #f4f4f4;
            text-align: center;
        }

        h1 {
            color: #2c3e50;
        }

        table {
            margin: auto;
            border-collapse: collapse;
            width: 80%;
            background: white;
        }

        th, td {
            border: 1px solid #ddd;
            padding: 10px;
        }

        th {
            background: #27ae60;
            color: white;
        }

        tr:hover {
            background: #f1f1f1;
        }
    </style>
</head>

<body>

    <h1>♻️ Recycle Classifier Dashboard</h1>

    <table>
        <tr>
            <th>Label</th>
            <th>Confidence</th>
            <th>Message</th>
            <th>Time</th>
        </tr>

        {% for item in data %}
        <tr>
            <td>{{ item.label }}</td>
            <td>{{ item.confidence }}</td>
            <td>{{ item.message }}</td>
            <td>{{ item.time }}</td>
        </tr>
        {% endfor %}

    </table>

</body>
</html>
"""

# 🟢 الصفحة الرئيسية
@app.route("/")
def home():
    return render_template_string(HTML_PAGE, data=history)

# 🟢 استقبال البيانات
@app.route("/receive", methods=["POST"])
def receive():
    data = request.get_json()

    print("🔥 REQUEST RECEIVED:", data)

    # إضافة وقت
    data["time"] = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    history.append(data)

    return jsonify({"status": "ok"})


if __name__ == "__main__":
    app.run(debug=True, host="0.0.0.0", port=5000)