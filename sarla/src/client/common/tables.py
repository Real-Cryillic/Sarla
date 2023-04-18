import datetime
from terminaltables import SingleTable
from termcolor import colored


def check_time(time_string):
    current_time = datetime.datetime.now().strftime("%H:%M:%S")
    end = datetime.datetime.strptime(current_time, "%H:%M:%S")
    start = end - datetime.timedelta(seconds=30)
    beacon_time_object = datetime.datetime.strptime(time_string, "%H:%M:%S")

    if start <= beacon_time_object <= end:
        return True
    else:
        return False


def create_table(data):
    table_data = [
        [
            "Agent ID",
            "Username",
            "Hostname",
            "Process ID",
            "Process Name",
            "System Arch",
            "Key",
            "Last Check-In",
        ]
    ]

    for key in data:
        row = []
        for subkey, values in key.items():
            if subkey == "_id":
                pass
            else:
                row.append(values)

        order = [3, 7, 2, 6, 5, 0, 4, 1]
        row = [row[i] for i in order]

        if check_time(row[7]) != True:
            row[7] = colored(row[7], "red")
        else:
            row[7] = colored(row[7], "green")

        table_data.append(row)

    return SingleTable(table_data)
