#!/usr/bin/env python3
"""
TA 内部工具：批量生成校园导航必做功能的测试数据。

不要发给学生。

输出位置：上一级目录的 测试数据_v2/
  small_cases/case_04/    （TIMED_SHORTEST 专项）
  medium_cases/...        （~100 节点）
  large_cases/...         （~1000 节点）
  sample_ecnu/answer.txt  （补齐迁移数据的答案）

运行方式：python3 generate_cases.py
"""

import os
import random
import math
from pathlib import Path
import networkx as nx


CATS = ['Teaching', 'Dining', 'Dormitory', 'Sports', 'Medical', 'Other']

TIME_WINDOWS = {
    'wide':       ('00:00', '23:59'),
    'morning':    ('06:00', '12:00'),
    'afternoon':  ('13:00', '18:00'),
    'night':      ('18:00', '23:00'),
    'noon':       ('11:00', '14:00'),
}


# -----------------------------------------------------------
# 写文件
# -----------------------------------------------------------
def write_case(out_dir, places, roads, commands, answers):
    out_dir = Path(out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    with open(out_dir / 'places.csv', 'w') as f:
        f.write('place_id,display_name,category,stay_time,open_time,close_time\n')
        for p in places:
            f.write(','.join(str(x) for x in p) + '\n')
    with open(out_dir / 'roads.csv', 'w') as f:
        f.write('from_id,to_id,distance,walk_time,status\n')
        for r in roads:
            f.write(','.join(str(x) for x in r) + '\n')
    with open(out_dir / 'command.txt', 'w') as f:
        f.write('\n'.join(commands) + '\n')
    with open(out_dir / 'answer.txt', 'w') as f:
        f.write('\n'.join(answers) + '\n')


# -----------------------------------------------------------
# Graph helpers
# -----------------------------------------------------------
def build_graph(places, roads, only_open=True, time=None):
    G = nx.Graph()
    for pid, name, cat, stay, ot, ct in places:
        if time is not None and not (ot <= time <= ct):
            continue
        G.add_node(pid, stay_time=stay, open_time=ot, close_time=ct, category=cat,
                   display_name=name)
    for u, v, d, w, s in roads:
        if only_open and s != 'open':
            continue
        if u not in G or v not in G:
            continue
        G.add_edge(u, v, distance=d, walk_time=w, status=s)
    return G


# -----------------------------------------------------------
# 答案生成器（每条命令对应一个）
# -----------------------------------------------------------
def ans_load():
    return 'OK'


def ans_query_place(places, pid):
    pd = {p[0]: p for p in places}
    if pid not in pd:
        return 'ERROR place_not_found'
    p = pd[pid]
    return f'PLACE {p[0]} {p[1]} {p[2]} {p[3]} {p[4]} {p[5]}'


def ans_query_category(places, cat):
    ids = sorted(p[0] for p in places if p[2] == cat)
    if not ids:
        return f'CATEGORY {cat} 0'
    return f'CATEGORY {cat} {len(ids)} ' + ' '.join(ids)


def ans_adj(places, roads, pid):
    pd = {p[0]: p for p in places}
    if pid not in pd:
        return 'ERROR place_not_found'
    nbrs = []
    for u, v, d, w, s in roads:
        if u == pid:
            nbrs.append((v, d, w, s))
        elif v == pid:
            nbrs.append((u, d, w, s))
    nbrs.sort(key=lambda x: x[0])
    if not nbrs:
        return f'ADJ {pid} 0'
    parts = ' '.join(f'{n}:{d}:{w}:{s}' for n, d, w, s in nbrs)
    return f'ADJ {pid} {len(nbrs)} {parts}'


def ans_components(places, roads):
    G = build_graph(places, roads)
    if G.number_of_nodes() == 0:
        return 'COMPONENTS 0 SIZES'
    sizes = sorted((len(c) for c in nx.connected_components(G)), reverse=True)
    return f'COMPONENTS {len(sizes)} SIZES ' + ' '.join(map(str, sizes))


def ans_shortest(places, roads, src, dst, mode):
    """按 distance 或 walk_time 求最短路径（必做版本）"""
    pd = {p[0]: p for p in places}
    if src not in pd or dst not in pd:
        return 'ERROR place_not_found'
    G = build_graph(places, roads)
    if src not in G or dst not in G:
        return 'NO_PATH'
    weight = 'distance' if mode == 'DIST' else 'walk_time'
    try:
        path = nx.shortest_path(G, src, dst, weight=weight)
        cost = nx.shortest_path_length(G, src, dst, weight=weight)
        return f'PATH {mode} {cost} NODES ' + ' '.join(path)
    except nx.NetworkXNoPath:
        return 'NO_PATH'


def ans_timed_shortest(places, roads, src, dst, time, mode):
    """时刻约束最短路径，按 distance 或 walk_time"""
    pd = {p[0]: p for p in places}
    if src not in pd or dst not in pd:
        return 'ERROR place_not_found'
    if not (pd[src][4] <= time <= pd[src][5]):
        return 'NO_PATH'
    if not (pd[dst][4] <= time <= pd[dst][5]):
        return 'NO_PATH'
    G = build_graph(places, roads, time=time)
    if src not in G or dst not in G:
        return 'NO_PATH'
    weight = 'distance' if mode == 'DIST' else 'walk_time'
    try:
        path = nx.shortest_path(G, src, dst, weight=weight)
        cost = nx.shortest_path_length(G, src, dst, weight=weight)
        return f'PATH {mode} {cost} NODES ' + ' '.join(path)
    except nx.NetworkXNoPath:
        return 'NO_PATH'


def ans_must_pass(places, roads, src, dst, mode, waypoints):
    """必经点路径，按 distance 或 walk_time"""
    pd = {p[0]: p for p in places}
    if src not in pd or dst not in pd or any(wp not in pd for wp in waypoints):
        return 'ERROR place_not_found'
    G = build_graph(places, roads)
    weight = 'distance' if mode == 'DIST' else 'walk_time'
    seq = [src] + list(waypoints) + [dst]
    full_path = []
    full_cost = 0
    for i in range(len(seq) - 1):
        if seq[i] not in G or seq[i + 1] not in G:
            return 'NO_PATH'
        try:
            seg = nx.shortest_path(G, seq[i], seq[i + 1], weight=weight)
            seg_cost = nx.shortest_path_length(G, seq[i], seq[i + 1], weight=weight)
            full_cost += seg_cost
            if full_path:
                seg = seg[1:]
            full_path.extend(seg)
        except nx.NetworkXNoPath:
            return 'NO_PATH'
    return f'PATH {mode} {full_cost} NODES ' + ' '.join(full_path)


# ---- 拓展任务 1：分层图最短路径 ----
def ans_shortest_layered(places, roads, src, dst, K):
    """分层图 Dijkstra：最多用 K 张加速通行券（仅供拓展任务 1 使用）"""
    pd = {p[0]: p for p in places}
    if src not in pd or dst not in pd:
        return 'ERROR place_not_found'
    G = build_graph(places, roads)
    if src not in G or dst not in G:
        return 'NO_PATH'

    import heapq
    INF = float('inf')
    dist = {(src, 0): 0}
    prev = {}
    pq = [(0, src, 0)]
    while pq:
        d, v, k = heapq.heappop(pq)
        if d > dist.get((v, k), INF):
            continue
        for u in G[v]:
            wt = G[v][u]['walk_time']
            nd = d + wt
            if nd < dist.get((u, k), INF):
                dist[(u, k)] = nd
                prev[(u, k)] = (v, k, False)
                heapq.heappush(pq, (nd, u, k))
            if k < K:
                nd2 = d + math.ceil(wt / 3)
                if nd2 < dist.get((u, k + 1), INF):
                    dist[(u, k + 1)] = nd2
                    prev[(u, k + 1)] = (v, k, True)
                    heapq.heappush(pq, (nd2, u, k + 1))

    best_k = None
    best_d = INF
    for kk in range(K + 1):
        if dist.get((dst, kk), INF) < best_d:
            best_d = dist[(dst, kk)]
            best_k = kk
    if best_k is None:
        return 'NO_PATH'

    path = []
    fast_edges = []
    cur = (dst, best_k)
    while cur != (src, 0):
        v, k = cur
        path.append(v)
        if cur not in prev:
            return 'NO_PATH'
        pv, pk, used = prev[cur]
        if used:
            a, b = sorted((pv, v))
            fast_edges.append((a, b))
        cur = (pv, pk)
    path.append(src)
    path.reverse()
    fast_edges.sort()

    s = f'PATH {best_d} K_USED {best_k} NODES ' + ' '.join(path)
    s += f' FAST {len(fast_edges)}'
    if fast_edges:
        s += ' ' + ' '.join(f'{a}-{b}' for a, b in fast_edges)
    return s


def ans_mst(places, roads):
    G = build_graph(places, roads)
    if G.number_of_nodes() == 0:
        return 'DISCONNECTED'
    if not nx.is_connected(G):
        return 'DISCONNECTED'
    T = nx.minimum_spanning_tree(G, weight='distance')
    edges = []
    total = 0
    for u, v, d in T.edges(data=True):
        a, b = sorted((u, v))
        edges.append((a, b, d['distance']))
        total += d['distance']
    edges.sort()
    return f'MST {total} EDGES ' + ' '.join(f'{a}-{b}:{w}' for a, b, w in edges)


def ans_critical(places, roads):
    G = build_graph(places, roads)
    aps = sorted(nx.articulation_points(G))
    bridges = sorted((min(u, v), max(u, v)) for u, v in nx.bridges(G))
    s = f'CRITICAL NODES {len(aps)}'
    if aps:
        s += ' ' + ' '.join(aps)
    s += f' EDGES {len(bridges)}'
    if bridges:
        s += ' ' + ' '.join(f'{a}-{b}' for a, b in bridges)
    return s


# -----------------------------------------------------------
# 拓扑生成器
# -----------------------------------------------------------
def make_place(pid, time_mode='wide', stay=None):
    cat = random.choice(CATS)
    if stay is None:
        stay = random.randint(0, 60)
    ot, ct = TIME_WINDOWS[time_mode]
    return (pid, f'Place_{pid}', cat, stay, ot, ct)


def gen_chain(n, time_mode='wide'):
    places = [make_place(f'P{i:04d}', time_mode) for i in range(1, n + 1)]
    roads = []
    for i in range(1, n):
        d = random.randint(50, 300)
        w = max(1, math.ceil(d / 80.0))
        roads.append((f'P{i:04d}', f'P{i + 1:04d}', d, w, 'open'))
    return places, roads


def gen_ring(n, time_mode='wide'):
    places, roads = gen_chain(n, time_mode)
    d = random.randint(50, 300)
    w = max(1, math.ceil(d / 80.0))
    roads.append((f'P{n:04d}', 'P0001', d, w, 'open'))
    return places, roads


def gen_grid(rows, cols, time_mode='wide'):
    pid = lambda r, c: f'P{(r * cols + c + 1):04d}'
    places = [make_place(pid(r, c), time_mode) for r in range(rows) for c in range(cols)]
    roads = []
    for r in range(rows):
        for c in range(cols):
            if c + 1 < cols:
                d = random.randint(50, 300)
                w = max(1, math.ceil(d / 80.0))
                roads.append((pid(r, c), pid(r, c + 1), d, w, 'open'))
            if r + 1 < rows:
                d = random.randint(50, 300)
                w = max(1, math.ceil(d / 80.0))
                roads.append((pid(r, c), pid(r + 1, c), d, w, 'open'))
    return places, roads


def gen_random_sparse(n, m, time_mode='wide', closed_ratio=0.0):
    """n 节点 m 条边的稀疏图，保证连通。"""
    places = [make_place(f'P{i:04d}', time_mode) for i in range(1, n + 1)]
    roads = []
    perm = list(range(1, n + 1))
    random.shuffle(perm)
    edge_set = set()
    for i in range(1, n):
        a, b = perm[i - 1], perm[i]
        if a > b:
            a, b = b, a
        edge_set.add((a, b))
    while len(edge_set) < m:
        a = random.randint(1, n)
        b = random.randint(1, n)
        if a == b:
            continue
        if a > b:
            a, b = b, a
        edge_set.add((a, b))
    for a, b in sorted(edge_set):
        d = random.randint(50, 300)
        w = max(1, math.ceil(d / 80.0))
        s = 'closed' if random.random() < closed_ratio else 'open'
        roads.append((f'P{a:04d}', f'P{b:04d}', d, w, s))
    return places, roads


def gen_random_mixed_time(n, m, closed_ratio=0.0):
    """同 random_sparse，但时间窗混合。"""
    places = []
    for i in range(1, n + 1):
        # 70% wide, 10% morning, 10% afternoon, 5% night, 5% noon
        r = random.random()
        if r < 0.70:
            tm = 'wide'
        elif r < 0.80:
            tm = 'morning'
        elif r < 0.90:
            tm = 'afternoon'
        elif r < 0.95:
            tm = 'night'
        else:
            tm = 'noon'
        places.append(make_place(f'P{i:04d}', tm))
    # edges as in gen_random_sparse
    roads = []
    perm = list(range(1, n + 1))
    random.shuffle(perm)
    edge_set = set()
    for i in range(1, n):
        a, b = perm[i - 1], perm[i]
        if a > b:
            a, b = b, a
        edge_set.add((a, b))
    while len(edge_set) < m:
        a = random.randint(1, n)
        b = random.randint(1, n)
        if a == b:
            continue
        if a > b:
            a, b = b, a
        edge_set.add((a, b))
    for a, b in sorted(edge_set):
        d = random.randint(50, 300)
        w = max(1, math.ceil(d / 80.0))
        s = 'closed' if random.random() < closed_ratio else 'open'
        roads.append((f'P{a:04d}', f'P{b:04d}', d, w, s))
    return places, roads


# -----------------------------------------------------------
# 旧数据转换器（cases.zip 里的旧 schema → 新 schema）
# -----------------------------------------------------------
# 旧 nodes.csv: name,type,weight
# 旧 edges.csv: node,node,weight
# 新 places.csv: place_id,display_name,category,stay_time,open_time,close_time
# 新 roads.csv: from_id,to_id,distance,walk_time,status

OLD_TYPE_MAP = {
    '1': 'Teaching',
    '2': 'Dining',
    '3': 'Dormitory',
    '4': 'Sports',
    '5': 'Library',
    '6': 'Other',
}


def convert_old_case(old_dir, time_mode='wide'):
    """读 old_dir 里的 nodes.csv + edges.csv（旧 schema），转换成新 schema 的 places + roads"""
    old_dir = Path(old_dir)
    name_to_pid = {}
    places = []
    pid_idx = 0
    with open(old_dir / 'nodes.csv') as f:
        first = True
        for line in f:
            line = line.strip()
            if not line:
                continue
            if first:
                first = False
                if line.startswith('name'):
                    continue
            parts = line.split(',')
            if len(parts) < 3:
                continue
            name, type_str, weight_str = parts[0], parts[1], parts[2]
            pid_idx += 1
            pid = f'P{pid_idx:04d}'
            name_to_pid[name] = pid
            cat = OLD_TYPE_MAP.get(type_str, 'Other')
            stay = int(weight_str)
            ot, ct = TIME_WINDOWS[time_mode]
            # display_name：替换空格为 _ 以便 CSV 安全
            display = name.replace(' ', '_').replace(',', '_')
            places.append((pid, display, cat, stay, ot, ct))

    roads = []
    with open(old_dir / 'edges.csv') as f:
        first = True
        for line in f:
            line = line.strip()
            if not line:
                continue
            if first:
                first = False
                if line.startswith('node'):
                    continue
            parts = line.split(',')
            if len(parts) < 3:
                continue
            n1, n2, weight_str = parts[0], parts[1], parts[2]
            if n1 not in name_to_pid or n2 not in name_to_pid:
                continue
            distance = int(weight_str)
            walk_time = max(1, math.ceil(distance / 80.0))
            roads.append((name_to_pid[n1], name_to_pid[n2], distance, walk_time, 'open'))

    return places, roads


# -----------------------------------------------------------
# 命令分发器（用于把 commands 列表映射到 answers）
# -----------------------------------------------------------
def run_commands(places, roads, commands):
    """根据命令列表跑 networkx 算法，返回每条命令对应的 answer。
    QUIT 不产生输出，跳过。"""
    answers = []
    # 注意：本函数把图当作只读的；维护类命令（ADD/DELETE/UPDATE/CLOSE/OPEN）
    # 会真正修改 P/R，影响后续命令；再次 LOAD 会恢复到输入的初始数据。
    P = list(places)
    R = list(roads)
    for line in commands:
        line = line.strip()
        if not line:
            continue
        parts = line.split()
        cmd = parts[0]
        if cmd == 'QUIT':
            continue
        elif cmd == 'LOAD':
            P = list(places)
            R = list(roads)
            answers.append('OK')
        elif cmd == 'SAVE':
            answers.append('OK')
        elif cmd == 'QUERY_PLACE':
            answers.append(ans_query_place(P, parts[1]))
        elif cmd == 'QUERY_CATEGORY':
            answers.append(ans_query_category(P, parts[1]))
        elif cmd == 'ADJ':
            answers.append(ans_adj(P, R, parts[1]))
        elif cmd == 'ADD_PLACE':
            pid = parts[1]
            if any(p[0] == pid for p in P):
                answers.append('ERROR place_already_exists')
            else:
                P.append((pid, parts[2], parts[3], int(parts[4]), parts[5], parts[6]))
                answers.append('OK')
        elif cmd == 'DELETE_PLACE':
            pid = parts[1]
            if not any(p[0] == pid for p in P):
                answers.append('ERROR place_not_found')
            else:
                P = [p for p in P if p[0] != pid]
                R = [r for r in R if r[0] != pid and r[1] != pid]
                answers.append('OK')
        elif cmd == 'UPDATE_PLACE':
            pid, field, value = parts[1], parts[2], parts[3]
            idx = next((i for i, p in enumerate(P) if p[0] == pid), -1)
            if idx == -1:
                answers.append('ERROR place_not_found')
            elif field not in ('display_name', 'category', 'stay_time', 'open_time', 'close_time'):
                answers.append('ERROR invalid_field')
            else:
                p = list(P[idx])
                fields = {'display_name': 1, 'category': 2, 'stay_time': 3,
                          'open_time': 4, 'close_time': 5}
                col = fields[field]
                p[col] = int(value) if field == 'stay_time' else value
                P[idx] = tuple(p)
                answers.append('OK')
        elif cmd == 'ADD_ROAD':
            u, v = parts[1], parts[2]
            if not any(p[0] == u for p in P) or not any(p[0] == v for p in P):
                answers.append('ERROR place_not_found')
            elif any((r[0] == u and r[1] == v) or (r[0] == v and r[1] == u) for r in R):
                answers.append('ERROR road_already_exists')
            else:
                R.append((u, v, int(parts[3]), int(parts[4]), parts[5]))
                answers.append('OK')
        elif cmd == 'DELETE_ROAD':
            u, v = parts[1], parts[2]
            new_R = [r for r in R if not ((r[0] == u and r[1] == v) or (r[0] == v and r[1] == u))]
            if len(new_R) == len(R):
                answers.append('ERROR road_not_found')
            else:
                R = new_R
                answers.append('OK')
        elif cmd == 'UPDATE_ROAD':
            u, v, field, value = parts[1], parts[2], parts[3], parts[4]
            idx = next((i for i, r in enumerate(R)
                        if (r[0] == u and r[1] == v) or (r[0] == v and r[1] == u)), -1)
            if idx == -1:
                answers.append('ERROR road_not_found')
            elif field not in ('distance', 'walk_time', 'status'):
                answers.append('ERROR invalid_field')
            else:
                r = list(R[idx])
                fields = {'distance': 2, 'walk_time': 3, 'status': 4}
                col = fields[field]
                r[col] = int(value) if field in ('distance', 'walk_time') else value
                R[idx] = tuple(r)
                answers.append('OK')
        elif cmd == 'CLOSE_ROAD':
            u, v = parts[1], parts[2]
            idx = next((i for i, r in enumerate(R)
                        if (r[0] == u and r[1] == v) or (r[0] == v and r[1] == u)), -1)
            if idx == -1:
                answers.append('ERROR road_not_found')
            else:
                r = list(R[idx])
                r[4] = 'closed'
                R[idx] = tuple(r)
                answers.append('OK')
        elif cmd == 'OPEN_ROAD':
            u, v = parts[1], parts[2]
            idx = next((i for i, r in enumerate(R)
                        if (r[0] == u and r[1] == v) or (r[0] == v and r[1] == u)), -1)
            if idx == -1:
                answers.append('ERROR road_not_found')
            else:
                r = list(R[idx])
                r[4] = 'open'
                R[idx] = tuple(r)
                answers.append('OK')
        elif cmd == 'COMPONENTS':
            answers.append(ans_components(P, R))
        elif cmd == 'SHORTEST':
            # SHORTEST <from> <to> <DIST|TIME>
            answers.append(ans_shortest(P, R, parts[1], parts[2], parts[3]))
        elif cmd == 'TIMED_SHORTEST':
            # TIMED_SHORTEST <from> <to> <time> <DIST|TIME>
            answers.append(ans_timed_shortest(P, R, parts[1], parts[2], parts[3], parts[4]))
        elif cmd == 'MUST_PASS':
            # MUST_PASS <from> <to> <DIST|TIME> <k> <p1>...<pk>
            src, dst, mode = parts[1], parts[2], parts[3]
            k = int(parts[4])
            wp = parts[5:5 + k]
            answers.append(ans_must_pass(P, R, src, dst, mode, wp))
        elif cmd == 'SHORTEST_K':
            # 拓展任务 1：分层图 SHORTEST_K <from> <to> <K>
            answers.append(ans_shortest_layered(P, R, parts[1], parts[2], int(parts[3])))
        elif cmd == 'MST':
            answers.append(ans_mst(P, R))
        elif cmd == 'CRITICAL':
            answers.append(ans_critical(P, R))
        else:
            answers.append('ERROR unknown_command')
    return answers


# -----------------------------------------------------------
# Cases
# -----------------------------------------------------------
def reprocess(dir_path, label):
    """从 dir 读 csv + command.txt，重算 answer.txt"""
    dir_path = Path(dir_path)
    if not (dir_path / 'places.csv').exists():
        return
    P, R = [], []
    with open(dir_path / 'places.csv') as f:
        for i, line in enumerate(f):
            line = line.strip()
            if not line:
                continue
            if i == 0 and line.startswith('place_id'):
                continue
            parts = line.split(',')
            P.append((parts[0], parts[1], parts[2], int(parts[3]), parts[4], parts[5]))
    with open(dir_path / 'roads.csv') as f:
        for i, line in enumerate(f):
            line = line.strip()
            if not line:
                continue
            if i == 0 and line.startswith('from_id'):
                continue
            parts = line.split(',')
            R.append((parts[0], parts[1], int(parts[2]), int(parts[3]), parts[4]))
    with open(dir_path / 'command.txt') as f:
        commands = [l.rstrip() for l in f]
    answers = run_commands(P, R, commands)
    with open(dir_path / 'answer.txt', 'w') as f:
        f.write('\n'.join(answers) + '\n')
    print(f'  {label}/answer.txt ok (regenerated)')


def main():
    base = Path(__file__).resolve().parent.parent / '测试数据_v2'
    cases_zip_extracted = Path('/tmp/cases_extract')  # 旧 cases.zip 解压目录

    # =====================================================================
    # 必做任务的测试数据
    # =====================================================================
    必做 = base / '必做'

    # ---------- small_cases ----------
    # case_04 由代码生成（TIMED_SHORTEST 专项）
    random.seed(20250503)
    places = [
        ('P0001', 'Place_P0001', 'Teaching',  20, '00:00', '23:59'),
        ('P0002', 'Place_P0002', 'Dining',    30, '06:00', '12:00'),  # morning
        ('P0003', 'Place_P0003', 'Library',   25, '00:00', '23:59'),
        ('P0004', 'Place_P0004', 'Sports',    40, '13:00', '18:00'),  # afternoon
        ('P0005', 'Place_P0005', 'Other',     10, '00:00', '23:59'),
        ('P0006', 'Place_P0006', 'Bar',       35, '18:00', '23:00'),  # night
        ('P0007', 'Place_P0007', 'Other',      5, '00:00', '23:59'),
        ('P0008', 'Place_P0008', 'Dorm',       0, '00:00', '23:59'),
    ]
    roads = [
        ('P0001', 'P0002', 100, 2, 'open'),
        ('P0001', 'P0003', 200, 3, 'open'),
        ('P0002', 'P0004', 150, 2, 'open'),
        ('P0003', 'P0004',  80, 1, 'open'),
        ('P0003', 'P0005', 250, 4, 'open'),
        ('P0004', 'P0006', 120, 2, 'open'),
        ('P0005', 'P0006', 160, 2, 'open'),
        ('P0005', 'P0007',  90, 1, 'open'),
        ('P0006', 'P0008', 300, 4, 'open'),
        ('P0007', 'P0008', 110, 2, 'open'),
    ]
    commands = [
        'LOAD places.csv roads.csv',
        'TIMED_SHORTEST P0001 P0008 10:00 DIST',
        'TIMED_SHORTEST P0001 P0006 10:00 DIST',
        'TIMED_SHORTEST P0001 P0006 19:00 DIST',
        'TIMED_SHORTEST P0001 P0008 09:00 TIME',
        'TIMED_SHORTEST P0006 P0001 20:00 DIST',
        'TIMED_SHORTEST P0001 P0006 05:00 DIST',
        'QUIT',
    ]
    answers = run_commands(places, roads, commands)
    write_case(必做 / 'small_cases' / 'case_04', places, roads, commands, answers)
    print('  必做/small_cases/case_04 ok')

    # case_01..03 重新跑（手工编辑了 command.txt）
    reprocess(必做 / 'small_cases' / 'case_01', '必做/small_cases/case_01')
    reprocess(必做 / 'small_cases' / 'case_02', '必做/small_cases/case_02')
    reprocess(必做 / 'small_cases' / 'case_03', '必做/small_cases/case_03')

    # ---------- medium_cases (~100 节点) ----------
    medium = 必做 / 'medium_cases'

    # 直接复用旧 cases.zip 里的 100 节点拓扑，转换 schema
    OLD_100_CASES = [
        ('chain_flower_100', 'chain_flower_100'),
        ('grid_100',         'grid_100'),
        ('ring_100',         'ring_100'),
        ('star_100',         'star_100'),
        ('random_sparse_100', 'random_sparse_100'),
    ]
    for old_name, new_name in OLD_100_CASES:
        src_dir = cases_zip_extracted / old_name
        if not (src_dir / 'nodes.csv').exists():
            print(f'  WARN: 跳过 {old_name}（找不到旧数据）')
            continue
        places, roads = convert_old_case(src_dir)
        # 命令清单：覆盖必做核心算法
        n = len(places)
        end = f'P{n:04d}'
        mid = f'P{(n // 2):04d}'
        commands = [
            'LOAD places.csv roads.csv',
            'COMPONENTS',
            f'SHORTEST P0001 {end} DIST',
            f'SHORTEST P0001 {end} TIME',
            f'MUST_PASS P0001 {end} DIST 1 {mid}',
            'MST',
            'CRITICAL',
            'QUIT',
        ]
        answers = run_commands(places, roads, commands)
        write_case(medium / new_name, places, roads, commands, answers)
        print(f'  必做/medium_cases/{new_name} ok（来自旧 cases.zip）')

    # 我们自己造一个混合时间窗 100 节点 case，用于测 TIMED_SHORTEST
    random.seed(1005)
    places, roads = gen_random_mixed_time(100, 200, closed_ratio=0.05)
    commands = [
        'LOAD places.csv roads.csv',
        'COMPONENTS',
        'SHORTEST P0001 P0050 DIST',
        'TIMED_SHORTEST P0001 P0050 10:00 DIST',
        'TIMED_SHORTEST P0001 P0050 15:00 TIME',
        'TIMED_SHORTEST P0001 P0050 20:00 DIST',
        'MST',
        'QUIT',
    ]
    answers = run_commands(places, roads, commands)
    write_case(medium / 'mixed_time_100', places, roads, commands, answers)
    print('  必做/medium_cases/mixed_time_100 ok')

    # ---------- large_cases (~10000 节点 等) ----------
    large = 必做 / 'large_cases'

    # 自造 1000 节点稀疏图（chain）
    random.seed(2001)
    places, roads = gen_chain(1000)
    commands = [
        'LOAD places.csv roads.csv',
        'COMPONENTS',
        'SHORTEST P0001 P1000 DIST',
        'SHORTEST P0001 P1000 TIME',
        'MST',
        'CRITICAL',
        'QUIT',
    ]
    answers = run_commands(places, roads, commands)
    write_case(large / 'chain_1000', places, roads, commands, answers)
    print('  必做/large_cases/chain_1000 ok')

    # 用旧 random_sparse_10000（10000 节点）作为大规模压力测试
    src_dir = cases_zip_extracted / 'random_sparse_10000'
    if (src_dir / 'nodes.csv').exists():
        places, roads = convert_old_case(src_dir)
        commands = [
            'LOAD places.csv roads.csv',
            'COMPONENTS',
            'SHORTEST P0001 P5000 DIST',
            'SHORTEST P0001 P10000 TIME',
            'MST',
            'QUIT',
        ]
        answers = run_commands(places, roads, commands)
        write_case(large / 'random_sparse_10000', places, roads, commands, answers)
        print('  必做/large_cases/random_sparse_10000 ok（来自旧 cases.zip）')

    # ---------- sample_ecnu ----------
    reprocess(必做 / 'sample_ecnu', '必做/sample_ecnu')

    # =====================================================================
    # 拓展任务 1 的测试数据（分层图最短路径，命令名 SHORTEST_K）
    # =====================================================================
    layered = base / '拓展_分层图'

    # ---------- small ----------
    # 经典贪心反例：
    #   普通路径 ABCD：walk=(10,10,10) 共 30
    #   绕路 AXD：walk=(5,50) 共 55
    # K=0: ABCD 胜（30 < 55）
    # K=1: AXD 中 XD 用券 → 5 + ceil(50/3)=17 = 22  反超 ABCD（最佳省 6 = 24）
    # K=2: ABCD 中两条边用券 → 4+4+10 = 18 反超 AXD（不到 19）
    # 这强迫学生用分层图，否则会卡在"对原图最短路用券"得到 24
    places = [
        ('P0001', 'A', 'Teaching', 0, '00:00', '23:59'),
        ('P0002', 'B', 'Teaching', 0, '00:00', '23:59'),
        ('P0003', 'C', 'Teaching', 0, '00:00', '23:59'),
        ('P0004', 'D', 'Teaching', 0, '00:00', '23:59'),
        ('P0005', 'X', 'Teaching', 0, '00:00', '23:59'),
    ]
    roads = [
        ('P0001', 'P0002', 100,  10, 'open'),  # A-B
        ('P0002', 'P0003', 100,  10, 'open'),  # B-C
        ('P0003', 'P0004', 100,  10, 'open'),  # C-D
        ('P0001', 'P0005',  50,   5, 'open'),  # A-X 短
        ('P0005', 'P0004', 500,  50, 'open'),  # X-D 步行慢但用券超划算
    ]
    commands = [
        'LOAD places.csv roads.csv',
        'SHORTEST_K P0001 P0004 0',  # 应得 30 via ABCD
        'SHORTEST_K P0001 P0004 1',  # 应得 22 via AXD（贪心错样例）
        'SHORTEST_K P0001 P0004 2',  # 应得 18 via ABCD 用 2 张券
        'SHORTEST_K P0001 P0004 3',  # 应得 12 via ABCD 全部用券
        'QUIT',
    ]
    answers = run_commands(places, roads, commands)
    write_case(layered / 'small' / 'case_layered_01', places, roads, commands, answers)
    print('  拓展_分层图/small/case_layered_01 ok')

    # ---------- medium (~100 节点，用 random_sparse_100 拓扑) ----------
    src_dir = cases_zip_extracted / 'random_sparse_100'
    if (src_dir / 'nodes.csv').exists():
        places, roads = convert_old_case(src_dir)
        commands = [
            'LOAD places.csv roads.csv',
            'SHORTEST_K P0001 P0050 0',
            'SHORTEST_K P0001 P0050 3',
            'SHORTEST_K P0001 P0050 5',
            'SHORTEST_K P0001 P0050 10',
            'SHORTEST_K P0001 P0100 0',
            'SHORTEST_K P0001 P0100 5',
            'QUIT',
        ]
        answers = run_commands(places, roads, commands)
        write_case(layered / 'medium' / 'random_sparse_100', places, roads, commands, answers)
        print('  拓展_分层图/medium/random_sparse_100 ok（来自旧 cases.zip）')

    # 同时给 grid_100 一份
    src_dir = cases_zip_extracted / 'grid_100'
    if (src_dir / 'nodes.csv').exists():
        places, roads = convert_old_case(src_dir)
        commands = [
            'LOAD places.csv roads.csv',
            'SHORTEST_K P0001 P0100 0',
            'SHORTEST_K P0001 P0100 3',
            'SHORTEST_K P0001 P0100 8',
            'QUIT',
        ]
        answers = run_commands(places, roads, commands)
        write_case(layered / 'medium' / 'grid_100', places, roads, commands, answers)
        print('  拓展_分层图/medium/grid_100 ok（来自旧 cases.zip）')

    # ---------- large (~1000 节点) ----------
    random.seed(40001)
    places, roads = gen_random_sparse(1000, 2000)
    commands = [
        'LOAD places.csv roads.csv',
        'SHORTEST_K P0001 P0500 0',
        'SHORTEST_K P0001 P0500 5',
        'SHORTEST_K P0001 P1000 0',
        'SHORTEST_K P0001 P1000 10',
        'QUIT',
    ]
    answers = run_commands(places, roads, commands)
    write_case(layered / 'large' / 'random_sparse_1000', places, roads, commands, answers)
    print('  拓展_分层图/large/random_sparse_1000 ok')


if __name__ == '__main__':
    main()
