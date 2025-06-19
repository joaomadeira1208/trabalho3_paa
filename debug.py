class TreeNode:
    def __init__(self, label):
        self.label = label
        self.children = []

def post_order(node, order, llds):
    if not node.children:
        idx = len(order)
        order.append(node)
        llds[idx] = idx
        return idx
    leftmost = None
    for child in node.children:
        idx = post_order(child, order, llds)
        if leftmost is None:
            leftmost = idx
    idx = len(order)
    order.append(node)
    llds[idx] = leftmost
    return idx

def compute_ted(t1, t2):
    T1_nodes, T2_nodes = [], []
    lld1, lld2 = {}, {}

    post_order(t1, T1_nodes, lld1)
    post_order(t2, T2_nodes, lld2)

    n1, n2 = len(T1_nodes), len(T2_nodes)
    treedist = [[0] * n2 for _ in range(n1)]

    def compute(i, j):
        m = i - lld1[i] + 2
        n = j - lld2[j] + 2
        fd = [[0] * n for _ in range(m)]

        for di in range(1, m):
            fd[di][0] = fd[di - 1][0] + 3
        for dj in range(1, n):
            fd[0][dj] = fd[0][dj - 1] + 3

        for di in range(1, m):
            for dj in range(1, n):
                i1 = lld1[i] + di - 1
                j1 = lld2[j] + dj - 1
                if lld1[i1] == lld1[i] and lld2[j1] == lld2[j]:
                    cost = 0 if T1_nodes[i1].label == T2_nodes[j1].label else 2
                    fd[di][dj] = min(
                        fd[di - 1][dj] + 3,
                        fd[di][dj - 1] + 3,
                        fd[di - 1][dj - 1] + cost
                    )
                    treedist[i1][j1] = fd[di][dj]
                else:
                    fd[di][dj] = min(
                        fd[di - 1][dj] + 1,
                        fd[di][dj - 1] + 1,
                        fd[lld1[i1] - lld1[i]][lld2[j1] - lld2[j]] + treedist[i1][j1]
                    )

    # Correto: computar apenas para pares (i, j) onde i e j são keyroots
    keyroots1 = get_keyroots(lld1)
    keyroots2 = get_keyroots(lld2)

    for i in keyroots1:
        for j in keyroots2:
            compute(i, j)

    return treedist[n1 - 1][n2 - 1]


def get_keyroots(llds):
    rev = {}
    for i, l in llds.items():
        rev[l] = i  # pega o mais à direita para cada lld
    return sorted(rev.values())


d = TreeNode('d')
b = TreeNode('b')
f = TreeNode('f')
a = TreeNode('a')
c = TreeNode('c')
e = TreeNode('e')
g = TreeNode('g')

d.children = [b, f]
b.children = [a, c]
f.children = [e, g]


f = TreeNode('f')
e = TreeNode('e')
g = TreeNode('g')
x = TreeNode('x')

f.children = [e, g]
e.children = [x]

print("TED =", compute_ted(d, f))  # Deve imprimir: TED = 2
