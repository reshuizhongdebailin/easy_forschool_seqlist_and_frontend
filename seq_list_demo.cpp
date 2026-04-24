#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

/* -------- page 10 on textbook -------- */
#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASIBLE -1
#define OVERFLOW -2

typedef int status;
typedef int ElemType;

/* ------- page 22 on textbook ------- */
#define LIST_INIT_SIZE 100
#define LISTINCREMENT 10

struct SqList {
    ElemType *elem;
    int length;
    int listsize;
    bool initialized;

    SqList() : elem(nullptr), length(0), listsize(0), initialized(false) {}
};

struct ListManager {
    unordered_map<string, SqList *> lists;
    string currentName;
};

status InitList(SqList &L);
status DestroyList(SqList &L);
status ClearList(SqList &L);
status ListEmpty(const SqList &L);
int ListLength(const SqList &L);
status GetElem(const SqList &L, int i, ElemType &e);
int LocateElem(const SqList &L, ElemType e);
status PriorElem(const SqList &L, ElemType cur_e, ElemType &pre_e);
status NextElem(const SqList &L, ElemType cur_e, ElemType &next_e);
status ListInsert(SqList &L, int i, ElemType e);
status ListDelete(SqList &L, int i, ElemType &e);
status ListTraverse(const SqList &L);

status MaxSubArray(const SqList &L, ElemType &maxSum);
status SubArrayNum(const SqList &L, ElemType k, long long &count);
status SortList(SqList &L);
status SaveListToFile(const SqList &L, const string &fileName);
status LoadListFromFile(SqList &L, const string &fileName);

status CreateManagedList(ListManager &manager, const string &name);
status RemoveManagedList(ListManager &manager, const string &name);
status SwitchManagedList(ListManager &manager, const string &name);
void PrintAllManagedLists(const ListManager &manager);
SqList *GetCurrentList(const ListManager &manager);
void DestroyAllManagedLists(ListManager &manager);

void PrintMenu(const string &currentName);
void ClearInputBuffer();

status InitList(SqList &L) {
    if (L.initialized) {
        return ERROR;
    }

    L.elem = static_cast<ElemType *>(malloc(LIST_INIT_SIZE * sizeof(ElemType)));
    if (!L.elem) {
        return OVERFLOW;
    }

    L.length = 0;
    L.listsize = LIST_INIT_SIZE;
    L.initialized = true;
    return OK;
}

status DestroyList(SqList &L) {
    if (!L.initialized) {
        return INFEASIBLE;
    }

    free(L.elem);
    L.elem = nullptr;
    L.length = 0;
    L.listsize = 0;
    L.initialized = false;
    return OK;
}

status ClearList(SqList &L) {
    if (!L.initialized) {
        return INFEASIBLE;
    }

    L.length = 0;
    return OK;
}

status ListEmpty(const SqList &L) {
    if (!L.initialized) {
        return INFEASIBLE;
    }
    return (L.length == 0) ? TRUE : FALSE;
}

int ListLength(const SqList &L) {
    if (!L.initialized) {
        return -1;
    }
    return L.length;
}

status GetElem(const SqList &L, int i, ElemType &e) {
    if (!L.initialized) {
        return INFEASIBLE;
    }
    if (i < 1 || i > L.length) {
        return ERROR;
    }

    e = L.elem[i - 1];
    return OK;
}

int LocateElem(const SqList &L, ElemType e) {
    if (!L.initialized) {
        return -1;
    }

    for (int i = 0; i < L.length; ++i) {
        if (L.elem[i] == e) {
            return i + 1;
        }
    }
    return 0;
}

status PriorElem(const SqList &L, ElemType cur_e, ElemType &pre_e) {
    if (!L.initialized) {
        return INFEASIBLE;
    }

    for (int i = 1; i < L.length; ++i) {
        if (L.elem[i] == cur_e) {
            pre_e = L.elem[i - 1];
            return OK;
        }
    }
    return ERROR;
}

status NextElem(const SqList &L, ElemType cur_e, ElemType &next_e) {
    if (!L.initialized) {
        return INFEASIBLE;
    }

    for (int i = 0; i < L.length - 1; ++i) {
        if (L.elem[i] == cur_e) {
            next_e = L.elem[i + 1];
            return OK;
        }
    }
    return ERROR;
}

status ListInsert(SqList &L, int i, ElemType e) {
    if (!L.initialized) {
        return INFEASIBLE;
    }
    if (i < 1 || i > L.length + 1) {
        return ERROR;
    }

    if (L.length >= L.listsize) {
        int newSize = L.listsize + LISTINCREMENT;
        ElemType *newBase =
            static_cast<ElemType *>(realloc(L.elem, newSize * sizeof(ElemType)));
        if (!newBase) {
            return OVERFLOW;
        }
        L.elem = newBase;
        L.listsize = newSize;
    }

    for (int j = L.length - 1; j >= i - 1; --j) {
        L.elem[j + 1] = L.elem[j];
    }
    L.elem[i - 1] = e;
    ++L.length;
    return OK;
}

status ListDelete(SqList &L, int i, ElemType &e) {
    if (!L.initialized) {
        return INFEASIBLE;
    }
    if (L.length == 0) {
        return ERROR;
    }
    if (i < 1 || i > L.length) {
        return ERROR;
    }

    e = L.elem[i - 1];
    for (int j = i; j < L.length; ++j) {
        L.elem[j - 1] = L.elem[j];
    }
    --L.length;
    return OK;
}

status ListTraverse(const SqList &L) {
    if (!L.initialized) {
        return INFEASIBLE;
    }

    cout << "\n----------- all elements -----------\n";
    for (int i = 0; i < L.length; ++i) {
        cout << L.elem[i] << " ";
    }
    cout << "\n--------------- end ----------------\n";
    return L.length;
}

status MaxSubArray(const SqList &L, ElemType &maxSum) {
    if (!L.initialized) {
        return INFEASIBLE;
    }
    if (L.length == 0) {
        return ERROR;
    }

    ElemType cur = L.elem[0];
    maxSum = L.elem[0];
    for (int i = 1; i < L.length; ++i) {
        cur = max(L.elem[i], cur + L.elem[i]);
        maxSum = max(maxSum, cur);
    }
    return OK;
}

status SubArrayNum(const SqList &L, ElemType k, long long &count) {
    if (!L.initialized) {
        return INFEASIBLE;
    }
    if (L.length == 0) {
        return ERROR;
    }

    unordered_map<long long, long long> prefixCount;
    prefixCount[0] = 1;
    long long prefix = 0;
    count = 0;

    for (int i = 0; i < L.length; ++i) {
        prefix += L.elem[i];
        if (prefixCount.find(prefix - k) != prefixCount.end()) {
            count += prefixCount[prefix - k];
        }
        ++prefixCount[prefix];
    }
    return OK;
}

status SortList(SqList &L) {
    if (!L.initialized) {
        return INFEASIBLE;
    }

    sort(L.elem, L.elem + L.length);
    return OK;
}

status SaveListToFile(const SqList &L, const string &fileName) {
    if (!L.initialized) {
        return INFEASIBLE;
    }

    ofstream out(fileName, ios::binary);
    if (!out.is_open()) {
        return ERROR;
    }

    const char magic[8] = {'S', 'Q', 'L', 'I', 'S', 'T', '1', '\0'};
    out.write(magic, sizeof(magic));
    out.write(reinterpret_cast<const char *>(&L.length), sizeof(L.length));
    out.write(reinterpret_cast<const char *>(&L.listsize), sizeof(L.listsize));
    out.write(reinterpret_cast<const char *>(L.elem),
              static_cast<streamsize>(L.length * sizeof(ElemType)));

    return out.good() ? OK : ERROR;
}

status LoadListFromFile(SqList &L, const string &fileName) {
    ifstream in(fileName, ios::binary);
    if (!in.is_open()) {
        return ERROR;
    }

    char magic[8] = {0};
    int length = 0;
    int listsize = 0;

    in.read(magic, sizeof(magic));
    in.read(reinterpret_cast<char *>(&length), sizeof(length));
    in.read(reinterpret_cast<char *>(&listsize), sizeof(listsize));
    if (!in.good()) {
        return ERROR;
    }

    const string sig(magic);
    if (sig != "SQLIST1") {
        return ERROR;
    }
    if (length < 0 || listsize < length) {
        return ERROR;
    }

    if (L.initialized == TRUE) {
        //如果 L 里面本来已经有数据，就不能直接覆盖，否则会造成内存泄漏
        DestroyList(L);
    }
    L.elem = static_cast<ElemType *>(malloc(listsize * sizeof(ElemType)));
    if (!L.elem) {
        return OVERFLOW;
    }

    L.length = length;
    L.listsize = listsize;
    L.initialized = true;

    in.read(reinterpret_cast<char *>(L.elem),
            static_cast<streamsize>(L.length * sizeof(ElemType)));
    if (!in.good()) {
        DestroyList(L);
        return ERROR;
    }
    return OK;
}

status CreateManagedList(ListManager &manager, const string &name) {
    if (name.empty() || manager.lists.find(name) != manager.lists.end()) {
        return ERROR;
    }

    SqList *newList = new SqList();
    status ret = InitList(*newList);
    if (ret != OK) {
        delete newList;
        return ret;
    }

    manager.lists[name] = newList;
    if (manager.currentName.empty()) {
        manager.currentName = name;
    }
    return OK;
}

status RemoveManagedList(ListManager &manager, const string &name) {
    auto it = manager.lists.find(name);
    if (it == manager.lists.end()) {
        return ERROR;
    }

    DestroyList(*(it->second));
    delete it->second;
    manager.lists.erase(it);

    if (manager.currentName == name) {
        if (manager.lists.empty()) {
            manager.currentName.clear();
        } else {
            manager.currentName = manager.lists.begin()->first;
        }
    }
    return OK;
}

status SwitchManagedList(ListManager &manager, const string &name) {
    if (manager.lists.find(name) == manager.lists.end()) {
        return ERROR;
    }

    manager.currentName = name;
    return OK;
}

void PrintAllManagedLists(const ListManager &manager) {
    cout << "\n------ managed SqLists ------\n";
    if (manager.lists.empty()) {
        cout << "(none)\n";
    } else {
        for (const auto &kv : manager.lists) {
            cout << kv.first;
            if (kv.first == manager.currentName) {
                cout << "  [current]";
            }
            cout << "\n";
        }
    }
    cout << "-----------------------------\n";
}

SqList *GetCurrentList(const ListManager &manager) {
    auto it = manager.lists.find(manager.currentName);
    if (it == manager.lists.end()) {
        return nullptr;
    }
    return it->second;
}

void DestroyAllManagedLists(ListManager &manager) {
    for (auto &kv : manager.lists) {
        DestroyList(*kv.second);
        delete kv.second;
    }
    manager.lists.clear();
    manager.currentName.clear();
}

void PrintMenu(const string &currentName) {
    cout << "\n\nMenu for Linear Table On Sequence Structure\n";
    cout << "Current List: " << (currentName.empty() ? "(none)" : currentName)
         << "\n";
    cout << "---------------------------------------------\n";
    cout << " 1.  InitList            11. ListDelete\n";
    cout << " 2.  DestroyList         12. ListTraverse\n";
    cout << " 3.  ClearList           13. MaxSubArray\n";
    cout << " 4.  ListEmpty           14. SubArrayNum(sum = k)\n";
    cout << " 5.  ListLength          15. SortList\n";
    cout << " 6.  GetElem             16. SaveListToFile\n";
    cout << " 7.  LocateElem          17. LoadListFromFile\n";
    cout << " 8.  PriorElem           18. CreateManagedList\n";
    cout << " 9.  NextElem            19. SwitchManagedList\n";
    cout << "10.  ListInsert          20. RemoveManagedList\n";
    cout << "21.  PrintAllManagedLists\n";
    cout << " 0.  Exit\n";
    cout << "---------------------------------------------\n";
    cout << "Please choose [0~21]: ";
}

void ClearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    ListManager manager;
    CreateManagedList(manager, "default");
    manager.currentName = "default";

    int op = -1;
    while (true) {
        PrintMenu(manager.currentName);
        if (!(cin >> op)) {
            ClearInputBuffer();
            cout << "Invalid input.\n";
            continue;
        }

        if (op == 0) {
            break;
        }

        SqList *cur = GetCurrentList(manager);
        if (!cur && op >= 1 && op <= 17) {
            cout << "No current list. Please create/switch one first.\n";
            continue;
        }

        switch (op) {
        case 1: {
            status ret = InitList(*cur);
            cout << ((ret == OK) ? "InitList success.\n" : "InitList failed.\n");
            break;
        }
        case 2: {
            status ret = DestroyList(*cur);
            cout << ((ret == OK) ? "DestroyList success.\n"
                                 : "DestroyList failed.\n");
            break;
        }
        case 3: {
            status ret = ClearList(*cur);
            cout << ((ret == OK) ? "ClearList success.\n" : "ClearList failed.\n");
            break;
        }
        case 4: {
            status ret = ListEmpty(*cur);
            if (ret == TRUE) {
                cout << "List is empty.\n";
            } else if (ret == FALSE) {
                cout << "List is NOT empty.\n";
            } else {
                cout << "ListEmpty failed.\n";
            }
            break;
        }
        case 5: {
            int len = ListLength(*cur);
            if (len >= 0) {
                cout << "ListLength = " << len << "\n";
            } else {
                cout << "ListLength failed.\n";
            }
            break;
        }
        case 6: {
            int i = 0;
            ElemType e = 0;
            cout << "Input i: ";
            cin >> i;
            status ret = GetElem(*cur, i, e);
            if (ret == OK) {
                cout << "GetElem success, e = " << e << "\n";
            } else {
                cout << "GetElem failed.\n";
            }
            break;
        }
        case 7: {
            ElemType e = 0;
            cout << "Input target e: ";
            cin >> e;
            int pos = LocateElem(*cur, e);
            if (pos > 0) {
                cout << "LocateElem success, position = " << pos << "\n";
            } else if (pos == 0) {
                cout << "Element not found.\n";
            } else {
                cout << "LocateElem failed.\n";
            }
            break;
        }
        case 8: {
            ElemType cur_e = 0;
            ElemType pre_e = 0;
            cout << "Input cur_e: ";
            cin >> cur_e;
            status ret = PriorElem(*cur, cur_e, pre_e);
            if (ret == OK) {
                cout << "PriorElem success, pre_e = " << pre_e << "\n";
            } else {
                cout << "PriorElem failed.\n";
            }
            break;
        }
        case 9: {
            ElemType cur_e = 0;
            ElemType next_e = 0;
            cout << "Input cur_e: ";
            cin >> cur_e;
            status ret = NextElem(*cur, cur_e, next_e);
            if (ret == OK) {
                cout << "NextElem success, next_e = " << next_e << "\n";
            } else {
                cout << "NextElem failed.\n";
            }
            break;
        }
        case 10: {
            int i = 0;
            ElemType e = 0;
            cout << "Input i and e: ";
            cin >> i >> e;
            status ret = ListInsert(*cur, i, e);
            cout << ((ret == OK) ? "ListInsert success.\n"
                                 : "ListInsert failed.\n");
            break;
        }
        case 11: {
            int i = 0;
            ElemType e = 0;
            cout << "Input i: ";
            cin >> i;
            status ret = ListDelete(*cur, i, e);
            if (ret == OK) {
                cout << "ListDelete success, deleted e = " << e << "\n";
            } else {
                cout << "ListDelete failed.\n";
            }
            break;
        }
        case 12: {
            status ret = ListTraverse(*cur);
            if (ret == INFEASIBLE) {
                cout << "ListTraverse failed.\n";
            }
            break;
        }
        case 13: {
            ElemType ans = 0;
            status ret = MaxSubArray(*cur, ans);
            if (ret == OK) {
                cout << "MaxSubArray = " << ans << "\n";
            } else {
                cout << "MaxSubArray failed.\n";
            }
            break;
        }
        case 14: {
            ElemType k = 0;
            long long ans = 0;
            cout << "Input k: ";
            cin >> k;
            status ret = SubArrayNum(*cur, k, ans);
            if (ret == OK) {
                cout << "SubArrayNum = " << ans << "\n";
            } else {
                cout << "SubArrayNum failed.\n";
            }
            break;
        }
        case 15: {
            status ret = SortList(*cur);
            cout << ((ret == OK) ? "SortList success.\n" : "SortList failed.\n");
            break;
        }
        case 16: {
            string fileName;
            cout << "Input file name: ";
            cin >> fileName;
            status ret = SaveListToFile(*cur, fileName);
            cout << ((ret == OK) ? "SaveListToFile success.\n"
                                 : "SaveListToFile failed.\n");
            break;
        }
        case 17: {
            string fileName;
            cout << "Input file name: ";
            cin >> fileName;
            status ret = LoadListFromFile(*cur, fileName);
            cout << ((ret == OK) ? "LoadListFromFile success.\n"
                                 : "LoadListFromFile failed.\n");
            break;
        }
        case 18: {
            string name;
            cout << "Input new list name: ";
            cin >> name;
            status ret = CreateManagedList(manager, name);
            cout << ((ret == OK) ? "CreateManagedList success.\n"
                                 : "CreateManagedList failed.\n");
            break;
        }
        case 19: {
            string name;
            cout << "Input list name to switch: ";
            cin >> name;
            status ret = SwitchManagedList(manager, name);
            cout << ((ret == OK) ? "SwitchManagedList success.\n"
                                 : "SwitchManagedList failed.\n");
            break;
        }
        case 20: {
            string name;
            cout << "Input list name to remove: ";
            cin >> name;
            status ret = RemoveManagedList(manager, name);
            cout << ((ret == OK) ? "RemoveManagedList success.\n"
                                 : "RemoveManagedList failed.\n");
            break;
        }
        case 21: {
            PrintAllManagedLists(manager);
            break;
        }
        default:
            cout << "Invalid option.\n";
            break;
        }
    }

    DestroyAllManagedLists(manager);
    cout << "Bye.\n";
    return 0;
}