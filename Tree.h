#ifndef TREE_H
#define TREE_H

#include <unordered_map>
#include <vector>
#include "TreeNode.h"

class Tree
{
private:
    std::unordered_map<int, TreeNode *> nodes;
    TreeNode *root = nullptr;

public:
    ~Tree();

    TreeNode *addNode(int id, const std::string &label);
    void addEdge(int parentId, const std::string &parentLabel,
                 int childId, const std::string &childLabel);
    void setRoot(int id, const std::string &label);
    TreeNode *getRoot() const;

    void postOrder(TreeNode *node, std::vector<TreeNode *> &order, std::vector<int> &lld);
    void buildFromString(const std::string &s);
};

#endif