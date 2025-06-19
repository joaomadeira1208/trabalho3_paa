#include "Tree.h"
#include <iostream>
#include <stack>
#include <cctype>
#include <sstream>

Tree::~Tree()
{
    for (auto &pair : nodes)
    {
        delete pair.second;
    }
}

TreeNode *Tree::addNode(int id, const std::string &label)
{
    if (nodes.count(id) == 0)
    {
        nodes[id] = new TreeNode(id, label);
    }
    return nodes[id];
}

void Tree::addEdge(int parentId, const std::string &parentLabel,
                   int childId, const std::string &childLabel)
{
    TreeNode *parent = addNode(parentId, parentLabel);
    TreeNode *child = addNode(childId, childLabel);
    parent->children.push_back(child);
}

void Tree::setRoot(int id, const std::string &label)
{
    root = addNode(id, label);
}

TreeNode *Tree::getRoot() const
{
    return root;
}

void Tree::postOrder(TreeNode *node, std::vector<TreeNode *> &order, std::vector<int> &lld)
{
    if (node == nullptr)
        return;

    int index_esquerda = -1;

    for (TreeNode *child : node->children)
    {
        postOrder(child, order, lld);

        if (index_esquerda == -1)
        {
            index_esquerda = lld[order.size() - 1];
        }
    }
    int index_atual = order.size();
    order.push_back(node);

    if (index_esquerda == -1)
    {
        index_esquerda = index_atual;
    }
    lld.push_back(index_esquerda);
}

void Tree::buildFromString(const std::string &s)
{
    // Limpa a árvore atual
    for (auto &pair : nodes)
        delete pair.second;
    nodes.clear();
    root = nullptr;

    std::stack<std::pair<int, std::string>> parentStack; // (id, label)
    std::stack<int> idStack;

    int nextId = 1;
    size_t pos = 0;

    while (pos < s.size())
    {
        while (pos < s.size() && isspace(s[pos]))
            ++pos;

        if (pos >= s.size())
            break;

        if (s[pos] == '(')
        {
            idStack.push(parentStack.top().first);
            ++pos;
        }
        else if (s[pos] == ')')
        {
            if (!idStack.empty())
                idStack.pop();
            ++pos;
        }
        else
        {
            // Lê o label (palavra até espaço ou parêntese)
            size_t start = pos;
            while (pos < s.size() && !isspace(s[pos]) && s[pos] != '(' && s[pos] != ')')
                ++pos;
            std::string label = s.substr(start, pos - start);

            int currentId = nextId++;
            TreeNode *node = addNode(currentId, label);

            if (root == nullptr)
            {
                root = node;
            }

            if (!idStack.empty())
            {
                int parentId = idStack.top();
                std::string parentLabel = nodes[parentId]->label;
                addEdge(parentId, parentLabel, currentId, label);
            }

            parentStack.push({currentId, label});
        }
    }
}