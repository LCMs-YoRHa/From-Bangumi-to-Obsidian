import json
import requests

# 获取用户个人全部收藏条目id
def get_all_collection_ids(user_id, token, subject_type=2, type=None, limit=30, offset=0):
    url = f'https://api.bgm.tv/v0/users/{user_id}/collections'
    headers = {
        "accept": "application/json",
        "User-Agent": f"{user_id}/my-private-project",
        "Authorization": f"Bearer {token}"
    }
    params = {
        "subject_type": subject_type,
        "limit": limit,
        "offset": offset
    }
    if type is not None:
        params["type"] = type
    response = requests.get(url, headers=headers, params=params)
    return json.loads(response.text)

# 获取用户个人单个条目收藏信息
def get_user_collection(user_id, subject_id, token):
    url = f'https://api.bgm.tv/v0/users/{user_id}/collections/{subject_id}'
    headers = {
        "accept": "application/json",
        "User-Agent": f"{user_id}/my-private-project",
        "Authorization": f"Bearer {token}"
    }
    response = requests.get(url, headers=headers)
    return json.loads(response.text)

# 获取条目公共信息
def getsubject(subject_id,user_id):
    url = 'https://api.bgm.tv/v0/subjects/' + str(subject_id)
    headers = {
        "accept": "application/json",
        "User-Agent": f"{user_id}/my-private-project"
    }
    response = requests.get(url, headers=headers)
    return json.loads(response.text)

# 获取条目职员表
def getsubjectpersons(subject_id,user_id):
    url = 'https://api.bgm.tv/v0/subjects/' + str(subject_id) + '/persons'
    headers = {
        "accept": "application/json",
        "User-Agent": f"{user_id}/my-private-project"
    }
    response = requests.get(url, headers=headers)
    return json.loads(response.text)

# 获取条目角色表
def getsubjectcharacters(subject_id,user_id):
    url = 'https://api.bgm.tv/v0/subjects/' + str(subject_id) + '/characters'
    headers = {
        "accept": "application/json",
        "User-Agent": f"{user_id}/my-private-project"
    }
    response = requests.get(url, headers=headers)
    return json.loads(response.text)

# 获取条目关联项目
def getsubjectrelations(subject_id,user_id):
    url = 'https://api.bgm.tv/v0/subjects/' + str(subject_id) + '/subjects'
    headers = {
        "accept": "application/json",
        "User-Agent": f"{user_id}/my-private-project"
    }
    response = requests.get(url, headers=headers)
    return json.loads(response.text)