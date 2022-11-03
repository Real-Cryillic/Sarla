from authorization import generate_id, generate_key

format_dict = {
    'id':'',
    'keyword':'',
    'key':'',
    'user':'',
    'host':'',
    'pid':'',
    'build':''
}


def register(data, dictionary):
    job_dictionary = format_dict
    agent_information = data.split(',')

    agent_id = generate_id()

    job_dictionary['id'] = agent_id
    job_dictionary['user'] = agent_information[0]
    job_dictionary['host'] = agent_information[1]
    job_dictionary['pid'] = agent_information[2]
    job_dictionary['build'] = agent_information[3]
    job_dictionary['keyword'] = agent_information[4]

    key = generate_key(job_dictionary['keyword'])

    job_dictionary['key'] = key

    dictionary[job_dictionary['id']] = job_dictionary

    return key