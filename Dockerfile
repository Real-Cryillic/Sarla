FROM python:3.10
WORKDIR /Sarla
RUN apt-get update
COPY . .
RUN python3 -m pip install --upgrade pip
RUN python3 -m pip install pipenv
RUN pipenv install Pipfile
CMD ["pipenv", "run", "python", "sarla.py"]