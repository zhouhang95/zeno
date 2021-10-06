#include <cstdio>
#include <string>
#include <vector>
#include <set>
#include <any>


struct Node {
    std::string kind;
    int xorder = 0;
    std::vector<int> deps;
    std::any value = (int)0;
};


std::vector<Node> nodes;


/*void init() {
    if a and b have a common trivial output, then should link.

    0 = a
    1 = b
    2 = c 0 1

    0 = a
    1 = b
    2 = c
    3 = d 0 1
    4 = e 1 2
    5 = if 3 4
}*/


void sortexec(std::vector<int> &tolink, std::set<int> &visited) {
    std::sort(tolink.begin(), tolink.end(), [&] (int i, int j) {
        return nodes[i].xorder < nodes[j].xorder;
    });
    for (auto idx: tolink) {
        if (!visited.contains(idx)) {
            visited.insert(idx);
            printf("%d\n", idx);
        }
    }
}

std::any resolve(int idx);

void touch(int idx, std::vector<int> &tolink, std::set<int> &visited) {
    if (idx == -1) return;
    if (nodes[idx].kind == "if") {
        auto cond = resolve(nodes[idx].deps[0]);
        if (std::any_cast<int>(cond)) {
            return touch(nodes[idx].deps[1], tolink, visited);
        } else {
            return touch(nodes[idx].deps[2], tolink, visited);
        }
    }
    for (auto dep: nodes[idx].deps) {
        touch(dep, tolink, visited);
    }
    tolink.push_back(idx);
}

std::any resolve(int idx) {
    if (idx == -1) return {};
    std::vector<int> tolink;
    std::set<int> visited;
    touch(idx, tolink, visited);
    sortexec(tolink, visited);
    return nodes[idx].value;
}

int main() {
    nodes.resize(5);
    nodes[0] = {"float", 100, {}};
    nodes[1] = {"float", 200, {}};
    nodes[2] = {"float", 300, {}};
    nodes[3] = {"iff", 400, {2, 0, 1}};
    nodes[4] = {"test", 500, {3, 0}};

    resolve(4);
    return 0;
}
