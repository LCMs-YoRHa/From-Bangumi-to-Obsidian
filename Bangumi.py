import json
import requests

# 获取条目信息
def getsubject(subject_id):
    url = 'https://api.bgm.tv/v0/subjects/' + str(subject_id)
    headers = {
        "accept": "application/json",
        "User-Agent": "LacYor/my-private-project"
    }
    response = requests.get(url, headers=headers)
    return json.loads(response.text)

# 获取条目职员表
def getsubjectpersons(subject_id):
    url = 'https://api.bgm.tv/v0/subjects/' + str(subject_id) + '/persons'
    headers = {
        "accept": "application/json",
        "User-Agent": "LacYor/my-private-project"
    }
    response = requests.get(url, headers=headers)
    return json.loads(response.text)

# 获取条目角色表
def getsubjectcharacters(subject_id):
    url = 'https://api.bgm.tv/v0/subjects/' + str(subject_id) + '/characters'
    headers = {
        "accept": "application/json",
        "User-Agent": "LacYor/my-private-project"
    }
    response = requests.get(url, headers=headers)
    return json.loads(response.text)

# 获取条目关联
def getsubjectrelations(subject_id):
    url = 'https://api.bgm.tv/v0/subjects/' + str(subject_id) + '/subjects'
    headers = {
        "accept": "application/json",
        "User-Agent": "LacYor/my-private-project"
    }
    response = requests.get(url, headers=headers)
    return json.loads(response.text)