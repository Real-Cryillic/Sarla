from base64 import b64decode


def register(data):
    registration_unformatted = str(b64decode(data))
    registration_unformatted = registration_unformatted[
        2 : len(registration_unformatted)
    ]
    registration_unformatted = registration_unformatted[:-1]
    agent_information = registration_unformatted.split(",")

    job = {"Username": "", "Hostname": "", "Process ID": 0, "Version": 0}
    i = 0
    for x in job:
        job[x] = agent_information[i]
        i += 1

    print("\n")

    return job
