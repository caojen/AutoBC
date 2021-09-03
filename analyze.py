import os

root="./experiment1.3"
dirs = []
for r, dirs, files in os.walk(root):  
  print(r)
  print(dirs) #当前路径下所有子目录
  break

for dir in dirs:
  e = root + "/" + dir
  contrasties = []

  random_found = 0     # 实验搜索过的公式
  random_results = 0    # 实验找到的修复
  random_time = 0      # 实验修复的时间
  random_diff = 0      # 实验修复公式中语义不同的

  ref_found = 0
  ref_results = 0
  ref_time = 0
  ref_diff = 0

  top_count = 0
  ref_top1 = 0
  ref_top2 = 0
  ref_top3 = 0

  ref_m_top1 = 0
  ref_m_top2 = 0
  ref_m_top3 = 0

  for r, dirs, files in os.walk(e):
    contrasties = [e + "/" + i for i in dirs]
    break

  for contrasty in contrasties:
    random_result = contrasty + "/" + "random_result.txt"
    ref_result = contrasty + "/" + "ref_result.txt"
    rank_result = contrasty + "/" + "rank_result.txt"
    rank_m_result = contrasty + "/" + "rank_m_result.txt"

    if os.path.isfile(random_result):
      random_content = open(random_result, "r").read()
      random_content_lines = random_content.split("\n")
      for random_content_line in random_content_lines:
        parts = random_content_line.split("=")
        if len(parts) != 2:
          continue
        if parts[0] == "found":
          random_found += int(parts[1])
        elif parts[0] == "result":
          random_results += int(parts[1])
        elif parts[0] == "time":
          random_time += float(parts[1])
        elif parts[0] == "dif":
          random_diff += int(parts[1])
    
    if os.path.isfile(ref_result):
      ref_content = open(ref_result, "r").read()
      ref_content_lines = ref_content.split("\n")
      for ref_content_line in ref_content_lines:
        parts = ref_content_line.split("=")
        if len(parts) != 2:
          continue
        if parts[0] == "found":
          ref_found += int(parts[1])
        elif parts[0] == "result":
          ref_results += int(parts[1])
        elif parts[0] == "time":
          ref_time += float(parts[1])
        elif parts[0] == "dif":
          ref_diff += int(parts[1])

    if os.path.isfile(rank_result):
      content = open(rank_result, "r").read()
      lines = content.split("\n")
      idx = 1
      for line in lines:
        if len(line) == 0:
          continue
        if line[0] == '+':
          parts = line.split("=")
          if len(parts) != 2:
            continue
          if parts[0] == "+Ref_Solver_Count":
            if idx == 1:
              ref_top1 += float(parts[1])
            elif idx == 2:
              ref_top2 += float(parts[1])
            else:
              ref_top3 += float(parts[1])
            idx += 1
            top_count += 1
    
    if os.path.isfile(rank_m_result):
      content = open(rank_m_result, "r").read()
      lines = content.split("\n")
      idx = 1
      for line in lines:
        if len(line) == 0:
          continue
        if line[0] == '+':
          parts = line.split("=")
          if len(parts) != 2:
            continue
          if parts[0] == "+Ref_Solver_Count":
            if idx == 1:
              ref_m_top1 += float(parts[1])
            elif idx == 2:
              ref_m_top2 += float(parts[1])
            else:
              ref_m_top3 += float(parts[1])
            idx += 1
      
  if top_count > 0:
    top_count = top_count / 3
    random_found = random_found / top_count
    random_results = random_results / top_count
    random_time = random_time / top_count
    random_diff = random_diff / top_count

    ref_found = ref_found / top_count
    ref_results = ref_results / top_count
    ref_time = ref_time / top_count
    ref_diff = ref_diff / top_count
    print(e, 'rand', random_found, random_results, random_time, random_diff)
    print(e, 'ref ', ref_found, ref_results, ref_time, ref_diff)

    print(e, 'ref rank top', ref_top1 / top_count, ref_top2 / top_count, ref_top3 / top_count)
    print(e, 'ref rank m t', ref_m_top1 / top_count, ref_m_top2 / top_count, ref_m_top3 / top_count)
    print(e, 'rnd rank top', 1 - ref_top1 / top_count, 1 - ref_top2 / top_count, 1 - ref_top3 / top_count)
    print(e, 'rnd rank m t', 1 - ref_m_top1 / top_count, 1 - ref_m_top2 / top_count, 1 - ref_m_top3 / top_count)
  else:
    print(e, 'no result')
