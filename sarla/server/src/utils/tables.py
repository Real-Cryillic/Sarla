from terminaltables import SingleTable
from sarla.server.src.utils import settings


def create_table(data):
    table_data = [
        ['Agent ID', 'Keyword', 'Key', 'Username', 'Hostname', 'Process ID', 'Build']
    ]

    for key in data:
        row = []
        for subkey in data[key]:
            row.append(data[key][subkey])
        table_data.append(row)

    table = SingleTable(table_data)
    settings.agent_table = table