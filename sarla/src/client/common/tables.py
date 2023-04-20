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
            "Address",
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

        print(row)

        order = [4, 8, 3, 7, 6, 1, 5, 0, 2]
        row = [row[i] for i in order]

        if check_time(row[8]) != True:
            row[8] = colored(row[8], "red")
        else:
            row[8] = colored(row[8], "green")

        table_data.append(row)

    return SingleTable(table_data)
