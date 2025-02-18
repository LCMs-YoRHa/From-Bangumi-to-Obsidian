import time
import Bangumi
import re

# 利用正则筛除文件名内不受支持的字符
def sanitize_filename(filename):
    sanitized = re.sub(r'[\\/*?:"<>|]', "", filename)
# 确保检测后文件以.md结尾
    if not sanitized.endswith(".md"):
        sanitized += ".md"
    return sanitized

# 读取用户user_id和token
def read_credentials(file_path):
    with open(file_path, 'r', encoding='utf-8') as file:
        lines = file.readlines()
        user_id = lines[0].strip()
        token = lines[1].strip()
    return user_id, token

# 获取用户全部收藏条目id
def extract_subject_ids(collection_data):
    return [item['subject_id'] for item in collection_data.get('data', [])]

def write_subject_ids_to_file(subject_ids, file_name):
    with open(file_name, 'w', encoding='utf-8') as f:
        f.write(','.join(map(str, subject_ids)))

def fetch_and_write_all_subject_ids(user_id, token, subject_type=2, type=None, limit=30, offset=0):
    all_subject_ids = []
    while True:
        collection_data = Bangumi.get_all_collection_ids(user_id, token, subject_type=subject_type, type=type, limit=limit, offset=offset)
        if 'data' not in collection_data or not collection_data['data']:
            break
        all_subject_ids.extend(extract_subject_ids(collection_data))
        offset += limit
    write_subject_ids_to_file(all_subject_ids, 'subject_ids.txt')
    print("保存为同目录下subject_ids.txt")

# 写入单个条目数据
def write_extended_subject_data(user_id,subject_id,token):
    # 请求数据
    user_subject_data = Bangumi.get_user_collection(user_id, subject_id, token)
    subject_data = Bangumi.getsubject(subject_id,user_id)
    staff_data = Bangumi.getsubjectpersons(subject_id,user_id)
    characters_data = Bangumi.getsubjectcharacters(subject_id,user_id)
    relations_data = Bangumi.getsubjectrelations(subject_id,user_id)

    # 生成文件名
    file_name = sanitize_filename(subject_data.get("name_cn") or subject_data.get("name", "default") + ".md")

    # 提取基本数据
    name = subject_data.get("name", "")
    episodes = subject_data.get("total_episodes", "")
    version =subject_data.get("platform", "")
    score = subject_data.get("rating", {}).get("score", "")
    person_score = user_subject_data.get("rate", "")
    summary = subject_data.get("summary", "")
    img_link = subject_data.get("images", {}).get("large", "")
    broadcast_date = subject_data.get("date", "")
    watch_date= user_subject_data.get("updated_at", "")
    person_comment = user_subject_data.get("comment", "")
    director = ""
    studio = ""

    # 观看状态映射
    status_mapping = {
        1: "在看",
        2: "看过",
        3: "想看",
        4: "搁置",
        5: "抛弃"
    }
    watch_status = status_mapping.get(user_subject_data.get("subject_type", 0), "未知")

    for entry in subject_data.get("infobox", []):
        k = entry.get("key", "")
        v = entry.get("value", "")
        if k == "导演":
            director = v
        elif k == "製作":
            studio = v

    # 合并用户个人及公开标签
    subject_tags = {t["name"] for t in subject_data.get("tags", [])}
    user_tags = set(user_subject_data.get("tags", []))
    combined_tags = list(subject_tags.union(user_tags))

    # 写入文件
    with open(file_name, 'w', encoding='utf-8') as f:
        f.write("---\n")
        f.write(f"原名: {name}\n")
        f.write(f"话数: {episodes}\n")
        f.write(f"版本: {version}\n")
        f.write(f"导演: {director}\n")
        f.write(f"tags: {', '.join(combined_tags)}\n")
        f.write(f"动画制作: {studio}\n")
        f.write(f"收录评分: {score}\n")
        f.write(f"个人评分: {person_score}\n")
        f.write(f"放送开始: {broadcast_date}\n")
        f.write(f"观看状态: {watch_status}\n")
        f.write(f"收藏日期: {watch_date}\n")
        f.write(f"个人评论: {person_comment}\n")
        f.write(f"URL: https://bangumi.tv/subject/{subject_id}\n")

        f.write("---\n\n")

        f.write("## 剧情梗概:\n")
        f.write(f"{summary}\n\n")

        f.write("---\n\n")

        f.write("## 制作人员:\n")
        f.write('<table style="table-layout: fixed; width: 100%;">\n')
        for i, staff in enumerate(staff_data):
            if isinstance(staff, dict):
                if i % 4 == 0:
                    f.write('<tr>\n')
                f.write('<td style="width: 25%; text-align: center; vertical-align: top;">\n')
                f.write(f"{staff.get('relation', '')}<br>{staff.get('name', '')}\n")
                image = staff.get('images', {}).get('grid', '')
                if image:
                    f.write(f'<br><img src="{image}" style="max-width: 100px; max-height: 100px;">\n')
                f.write('</td>\n')
                if (i + 1) % 4 == 0:
                    f.write('</tr>\n')
        f.write('</table>\n\n')

        f.write("---\n\n")

        f.write("## 出演角色:\n")
        f.write('<table style="table-layout: fixed; width: 100%;">\n')
        for i, character in enumerate(characters_data):
            if isinstance(character, dict):
                if i % 3 == 0:
                    f.write('<tr>\n')
                f.write('<td style="width: 33.33%; text-align: center; vertical-align: top;">\n')
                actor_name = character.get('actors', [{}])[0].get('name', '') if character.get('actors') else ''
                f.write(f"{character.get('relation', '')}<br>{character.get('name', '')}<br>配音: {actor_name}\n")
                char_image = character.get('images', {}).get('grid', '')
                actor_image = character.get('actors', [{}])[0].get('images', {}).get('grid', '') if character.get('actors') else ''
                if char_image:
                    f.write(f'<br><img src="{char_image}" style="max-width: 100px; max-height: 100px;">\n')
                if actor_image:
                    f.write(f'<br><img src="{actor_image}" style="max-width: 100px; max-height: 100px;">\n')
                f.write('</td>\n')
                if (i + 1) % 3 == 0:
                    f.write('</tr>\n')
        f.write('</table>\n\n')

        f.write("---\n\n")

        f.write("## 关联条目:\n")
        for relation in relations_data:
            if isinstance(relation, dict):
                name_cn = relation.get("name_cn") or relation.get("name") or "default"
                f.write(f'{relation.get("relation", "")} - [[{name_cn}]]\n')

        f.write("---\n\n")

        f.write(f'<img src="{img_link}" alt="{subject_data.get("name_cn", "")}" style="max-width: 100%;">\n')
        f.flush()

# 逐个写入条目
def process_subject_ids(user_id, token):
    with open('subject_ids.txt', 'r', encoding='utf-8') as f:
        subject_ids = f.read().split(',')
    for count, subject_id in enumerate(subject_ids, start=1):
        subject_data = Bangumi.getsubject(subject_id,user_id)
        name_cn = subject_data.get("name_cn", "default")
        print(f"正在写入第{count}篇: {name_cn}")
        write_extended_subject_data(user_id, subject_id, token)
        time.sleep(0.2)  # 防止请求过快