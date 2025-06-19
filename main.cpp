#include <iostream>
#include "Tree.h"
#include <algorithm>
#include <chrono>
#include <unordered_set>

using namespace std;

vector<int> obterRaizesChave(const vector<int> &lld)
{
    vector<int> keyroots;
    unordered_set<int> vistos;

    for (int i = lld.size() - 1; i >= 0; --i)
    {
        if (vistos.find(lld[i]) == vistos.end())
        {
            keyroots.push_back(i);
            vistos.insert(lld[i]);
        }
    }

    reverse(keyroots.begin(), keyroots.end());
    return keyroots;
}

void zhang_calcularDistanciaSubarvore(
    int &indiceA1,
    int &indiceA2,
    vector<vector<int>> &matrizDistanciaArvore,
    const vector<int> &lldA1,
    const vector<int> &lldA2,
    const vector<TreeNode *> &nosA1,
    const vector<TreeNode *> &nosA2)
{
    int i1 = lldA1[indiceA1];
    int j1 = lldA2[indiceA2];
    int m = indiceA1 - i1 + 1;
    int n = indiceA2 - j1 + 1;

    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));

    for (int i = 1; i <= m; ++i)
        dp[i][0] = dp[i - 1][0] + 3;

    for (int j = 1; j <= n; ++j)
        dp[0][j] = dp[0][j - 1] + 3;

    for (int i = 1; i <= m; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            int nodeA = i1 + i - 1;
            int nodeB = j1 + j - 1;

            if (lldA1[nodeA] == i1 && lldA2[nodeB] == j1)
            {
                int custo = (nosA1[nodeA]->label == nosA2[nodeB]->label) ? 0 : 2;
                dp[i][j] = min({dp[i - 1][j] + 3,
                                dp[i][j - 1] + 3,
                                dp[i - 1][j - 1] + custo});
                matrizDistanciaArvore[nodeA][nodeB] = dp[i][j];
            }
            else
            {
                int a = (lldA1[nodeA] - i1);
                int b = (lldA2[nodeB] - j1);
                dp[i][j] = min({dp[i - 1][j] + 3,
                                dp[i][j - 1] + 3,
                                dp[a][b] + matrizDistanciaArvore[nodeA][nodeB]});
            }
        }
    }
}

int zhang_calcularTED(Tree &arvore1, Tree &arvore2)
{
    vector<int> lld1, lld2;
    vector<TreeNode *> posOrdem1, posOrdem2;

    arvore1.postOrder(arvore1.getRoot(), posOrdem1, lld1);
    arvore2.postOrder(arvore2.getRoot(), posOrdem2, lld2);

    int tam1 = posOrdem1.size();
    int tam2 = posOrdem2.size();

    vector<vector<int>> distanciaArvore(tam1, vector<int>(tam2, 0));

    vector<int> raizesChave1 = obterRaizesChave(lld1);
    vector<int> raizesChave2 = obterRaizesChave(lld2);

    for (int i : raizesChave1)
    {
        for (int j : raizesChave2)
        {
            zhang_calcularDistanciaSubarvore(i, j, distanciaArvore, lld1, lld2, posOrdem1, posOrdem2);
        }
    }

    return distanciaArvore[tam1 - 1][tam2 - 1];
}

void exemplo(Tree &a1, Tree &a2)
{
    a1.setRoot(4, "d");
    a1.addEdge(4, "d", 2, "b");
    a1.addEdge(4, "d", 6, "f");
    a1.addEdge(2, "b", 1, "a");
    a1.addEdge(2, "b", 3, "c");
    a1.addEdge(6, "f", 5, "e");
    a1.addEdge(6, "f", 7, "g");

    a2.setRoot(6, "f");
    a2.addEdge(6, "f", 5, "e");
    a2.addEdge(6, "f", 7, "g");
    a2.addEdge(5, "e", 99, "x");
}

int main()
{
    Tree a1, a2;
    a1.buildFromString("d(b(a c) f(e g))");
    a2.buildFromString("f(e(x) g)");

    using namespace std::chrono;

    auto inicio = high_resolution_clock::now();

    int result = zhang_calcularTED(a1, a2);

    auto fim = high_resolution_clock::now();
    auto duracao = duration_cast<microseconds>(fim - inicio).count();

    vector<TreeNode *> ordem1, ordem2;
    vector<int> lld1, lld2;
    a1.postOrder(a1.getRoot(), ordem1, lld1);
    a2.postOrder(a2.getRoot(), ordem2, lld2);
    size_t memBytes = sizeof(int) * ordem1.size() * ordem2.size();
    double memMB = memBytes / (1024.0 * 1024.0);

    cout << "TED: " << result << endl;
    cout << "Tempo de execução: " << duracao << " μs" << endl;
    cout << "Uso estimado de memória (matriz DP): " << memMB << " MB" << endl;

    return 0;
}