#ifndef TREENODE_H
#define TREENODE_H

#include <vector>
#include <string>

struct TreeNode
{
    int id;
    std::vector<TreeNode *> children;
    std::string label;

    TreeNode(int id, std::string label) : id(id), label(label) {}
};

#endif