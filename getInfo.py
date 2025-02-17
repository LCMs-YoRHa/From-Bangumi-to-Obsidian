import Bangumi

def write_extended_subject_data(subject_id):
    # Fetch all relevant data
    subject_data = Bangumi.getsubject(subject_id)
    staff_data = Bangumi.getsubjectpersons(subject_id)
    characters_data = Bangumi.getsubjectcharacters(subject_id)
    relations_data = Bangumi.getsubjectrelations(subject_id)

    # Prepare file name
    file_name = subject_data.get("name_cn", "default") + ".md"

    # Extract basic subject info
    name = subject_data.get("name", "")
    episodes = subject_data.get("total_episodes", "")
    version =subject_data.get("platform", "")
    score = subject_data.get("rating", {}).get("score", "")
    summary = subject_data.get("summary", "")
    img_link = subject_data.get("images", {}).get("large", "")
    tags_list = [t["name"] for t in subject_data.get("tags", [])]
    broadcast_date = subject_data.get("date", "")
    director = ""
    studio = ""

    for entry in subject_data.get("infobox", []):
        k = entry.get("key", "")
        v = entry.get("value", "")
        if k == "导演":
            director = v
        elif k == "製作":
            studio = v

    # Write data to the Markdown file
    with open(file_name, 'w', encoding='utf-8') as f:
        f.write("---\n")
        f.write(f"原名: {name}\n")
        f.write(f"话数: {episodes}\n")
        f.write(f"版本: {version}\n")
        f.write(f"导演: {director}\n")
        f.write(f"tags: {', '.join(tags_list)}\n")
        f.write(f"动画制作: {studio}\n")
        f.write(f"收录评分: {score}\n")
        f.write(f"放送开始: {broadcast_date}\n")
        f.write(f"URL: https://bangumi.tv/subject/{subject_id}\n")

        f.write("---\n\n")

        f.write("## 剧情梗概:\n")
        f.write(f"{summary}\n\n")
        f.write("---\n\n")


        f.write("## 制作人员:\n")
        f.write('<table style="table-layout: fixed; width: 100%;">\n')
        for i, staff in enumerate(staff_data):
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
            name_cn = relation.get("name_cn") or relation.get("name") or "default"
            f.write(f'{relation.get("relation", "")} - [[{name_cn}]]\n')
        f.write("---\n\n")
        f.write(f'<img src="{img_link}" alt="{subject_data.get("name_cn", "")}" style="max-width: 100%;">\n')
        f.close()


subject_id = 233
write_extended_subject_data(subject_id)