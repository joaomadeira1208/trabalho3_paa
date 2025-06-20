#include <iostream>
#include "Tree.h"
#include <algorithm>
#include <chrono>
#include <unordered_set>
#include <random>
#include <ctime>
#include <sstream>
#include <functional>
#include <sys/resource.h>
#include <fstream>

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

// Algoritmo de Zhang & Shasha
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

int compararArvoresGuloso(TreeNode *n1, TreeNode *n2)
{
    if (!n1 && !n2)
        return 0;

    if (!n1 && n2)
    {
        int custo = 3;
        for (auto *filho : n2->children)
            custo += compararArvoresGuloso(nullptr, filho);
        return custo;
    }

    if (n1 && !n2)
    {
        int custo = 3;
        for (auto *filho : n1->children)
            custo += compararArvoresGuloso(filho, nullptr);
        return custo;
    }

    int custo = (n1->label != n2->label) ? 2 : 0;

    int tam1 = n1->children.size();
    int tam2 = n2->children.size();
    int comum = min(tam1, tam2);

    for (int i = 0; i < comum; ++i)
        custo += compararArvoresGuloso(n1->children[i], n2->children[i]);

    for (int i = comum; i < tam1; ++i)
        custo += compararArvoresGuloso(n1->children[i], nullptr);

    for (int i = comum; i < tam2; ++i)
        custo += compararArvoresGuloso(nullptr, n2->children[i]);

    return custo;
}
// Algoritmo guloso
int tedGuloso(Tree &a1, Tree &a2)
{
    return compararArvoresGuloso(a1.getRoot(), a2.getRoot());
}

void gerarArvoreAleatoriaComString(Tree &t, int maxNos, const std::string &prefixo)
{
    if (maxNos <= 0)
        return;

    static std::mt19937 rng(time(nullptr));
    std::uniform_int_distribution<int> filhosDistrib(0, 3);
    std::uniform_int_distribution<int> letraDistrib('a', 'z');

    int id = 0;
    int limite = maxNos;
    std::ostringstream oss;

    std::function<void(int)> gerarSubarvore;

    gerarSubarvore = [&](int profundidade)
    {
        if (id >= limite)
            return;

        char letra = static_cast<char>(letraDistrib(rng));
        oss << letra;

        int filhos = filhosDistrib(rng);
        if (filhos > 0 && id + filhos < limite)
        {
            oss << "(";
            for (int i = 0; i < filhos; ++i)
            {
                if (i > 0)
                    oss << " ";
                ++id;
                gerarSubarvore(profundidade + 1);
            }
            oss << ")";
        }
    };

    ++id;
    gerarSubarvore(0);

    std::string treeStr = oss.str();
    t.buildFromString(treeStr);
}

int getMemoryUsageInBytes()
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss * 1024;
}

void executarExperimentosComArvoresAleatorias()
{
    using namespace std::chrono;

    std::ofstream csv("resultados.csv");
    csv << "Iteracao,TED_Guloso,TED_Zhang,Tempo_Guloso(us),Tempo_Zhang(us),"
        << "Mem_Estimada_Guloso(MB),Mem_Real_Guloso(MB),"
        << "Mem_Estimada_Zhang(MB),Mem_Real_Zhang(MB),TED_Diferente\n";

    for (int iter = 1; iter <= 10000; ++iter)
    {
        Tree a1, a2;
        gerarArvoreAleatoriaComString(a1, 32, "a");
        gerarArvoreAleatoriaComString(a2, 32, "b");

        cout << "==================== Iteração " << iter << " ====================" << endl;

        int mem_before_guloso = getMemoryUsageInBytes();
        auto inicio_1 = high_resolution_clock::now();
        int custo_guloso = tedGuloso(a1, a2);
        auto fim_1 = high_resolution_clock::now();
        int mem_after_guloso = getMemoryUsageInBytes();
        auto duracao_1 = duration_cast<microseconds>(fim_1 - inicio_1).count();

        vector<TreeNode *> ordem1, ordem2;
        vector<int> lld1, lld2;
        a1.postOrder(a1.getRoot(), ordem1, lld1);
        a2.postOrder(a2.getRoot(), ordem2, lld2);

        int memBytesGuloso = sizeof(TreeNode *) * ordem1.size() + sizeof(TreeNode *) * ordem2.size();
        double memMBGulosoEstimada = memBytesGuloso / (1024.0 * 1024.0);
        double memMBGulosoReal = (mem_after_guloso - mem_before_guloso) / (1024.0 * 1024.0);

        cout << "[GULOSO]" << endl;
        cout << "TED: " << custo_guloso << endl;
        cout << "Tempo de execução: " << duracao_1 << " μs" << endl;
        cout << "Uso estimado de memória (apenas vetores): " << memMBGulosoEstimada << " MB" << endl;
        cout << "Uso real de memória: " << memMBGulosoReal << " MB" << endl;

        vector<int> raizesChave1 = obterRaizesChave(lld1);
        vector<int> raizesChave2 = obterRaizesChave(lld2);
        long long memoriaDP = 0;
        for (int i : raizesChave1)
        {
            for (int j : raizesChave2)
            {
                int i1 = lld1[i];
                int j1 = lld2[j];
                int m = i - i1 + 1;
                int n = j - j1 + 1;
                memoriaDP += static_cast<long long>(m + 1) * (n + 1) * sizeof(int);
            }
        }

        int mem_before_zhang = getMemoryUsageInBytes();
        auto inicio_2 = high_resolution_clock::now();
        int custo_zhang = zhang_calcularTED(a1, a2);
        auto fim_2 = high_resolution_clock::now();
        int mem_after_zhang = getMemoryUsageInBytes();
        auto duracao_2 = duration_cast<microseconds>(fim_2 - inicio_2).count();

        double memMBZhangEstimada = memoriaDP / (1024.0 * 1024.0);
        double memMBZhangReal = (mem_after_zhang - mem_before_zhang) / (1024.0 * 1024.0);

        cout << "[ZHANG & SHASHA]" << endl;
        cout << "TED: " << custo_zhang << endl;
        cout << "Tempo de execução: " << duracao_2 << " μs" << endl;
        cout << "Uso estimado de memória (somando DPs internas): " << memMBZhangEstimada << " MB" << endl;
        cout << "Uso real de memória: " << memMBZhangReal << " MB" << endl;

        bool diferentes = (custo_guloso != custo_zhang);

        csv << iter << ","
            << custo_guloso << ","
            << custo_zhang << ","
            << duracao_1 << ","
            << duracao_2 << ","
            << memMBGulosoEstimada << ","
            << memMBGulosoReal << ","
            << memMBZhangEstimada << ","
            << memMBZhangReal << ","
            << (diferentes ? 1 : 0) << "\n";

        cout << "===================================================" << endl;
    }

    csv.close();
}

/*
 * Para testar com árvores aleatórias, utilize a função:
 *     executarExperimentosComArvoresAleatorias();
 *
 * Ela gera árvores aleatórias, roda os algoritmos,
 * mede tempo de execução, uso estimado e real de memória,
 * e salva os resultados em um arquivo CSV.
 *
 * Pode mudar o tamanho máximo da árvore: (gerarArvoreAleatoriaComString(a2, 32, "b");) -> neste caso 32
 * Pode mudar o número de iterações: (for (int iter = 1; iter <= 10000; ++iter)) -> neste caso 10.0000
 * Intrução: apenas chamar a função dentro da main()
 */

int main()
{
    using namespace std::chrono;

    Tree T1, T2;
    T1.buildFromString("d(b(a c) f(e g))");
    T2.buildFromString("f(e(x) g)");

    auto inicio_guloso = high_resolution_clock::now();
    int tedG = tedGuloso(T1, T2);
    auto fim_guloso = high_resolution_clock::now();
    auto duracao_guloso = duration_cast<microseconds>(fim_guloso - inicio_guloso).count();

    auto inicio_zhang = high_resolution_clock::now();
    int tedZ = zhang_calcularTED(T1, T2);
    auto fim_zhang = high_resolution_clock::now();
    auto duracao_zhang = duration_cast<microseconds>(fim_zhang - inicio_zhang).count();

    cout << "[GULOSO]" << endl;
    cout << "TED: " << tedG << endl;
    cout << "Tempo: " << duracao_guloso << " μs" << endl;

    cout << "[ZHANG & SHASHA]" << endl;
    cout << "TED: " << tedZ << endl;
    cout << "Tempo: " << duracao_zhang << " μs" << endl;

    cout << "=====================================================================" << endl;

    // executarExperimentosComArvoresAleatorias();

    return 0;
}
