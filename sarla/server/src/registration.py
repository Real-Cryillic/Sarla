from sarla.server.src.authorization import auth


def register(data):
    registration_unformatted = str(b64decode(data))
    registration_unformatted = registration_unformatted[
        2:len(registration_unformatted)]
    registration_unformatted = registration_unformatted[:-1]
    agent_information = registration_unformatted.split(",")

    job = {
        "username": "",
        "hostname": "",
        "process id": 0,
        "version": 0,
        "key": ""
    }

    i = 0
    for x in job:
        job[x] = data[i]
        i += 1

    beacon_key, agent_id = auth(job)

    print("\n")

    return job
