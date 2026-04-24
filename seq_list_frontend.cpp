#include <algorithm>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <windows.h>

using namespace std;

#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASIBLE -1
#define OVERFLOW -2

typedef int status;
typedef int ElemType;

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

static ListManager g_manager;
static HWND g_hListBox = nullptr;
static HWND g_hLog = nullptr;
static HWND g_hCurrent = nullptr;
static HWND g_hName = nullptr;
static HWND g_hIndex = nullptr;
static HWND g_hValue = nullptr;
static HWND g_hFile = nullptr;
static HWND g_hK = nullptr;

enum ControlId {
    ID_NAME = 101,
    ID_INDEX,
    ID_VALUE,
    ID_FILE,
    ID_K,
    ID_BTN_NEW,
    ID_BTN_SWITCH,
    ID_BTN_REMOVE,
    ID_BTN_INIT,
    ID_BTN_DESTROY,
    ID_BTN_CLEAR,
    ID_BTN_INSERT,
    ID_BTN_DELETE,
    ID_BTN_GET,
    ID_BTN_LOCATE,
    ID_BTN_PRIOR,
    ID_BTN_NEXT,
    ID_BTN_TRAVERSE,
    ID_BTN_SORT,
    ID_BTN_MAXSUM,
    ID_BTN_SUBCNT,
    ID_BTN_SAVE,
    ID_BTN_LOAD,
    ID_BTN_EMPTY,
    ID_BTN_LENGTH,
    ID_BTN_SHOWALL,
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
status ListTraverse(const SqList &L, vector<ElemType> &out);
status MaxSubArray(const SqList &L, ElemType &maxSum);
status SubArrayNum(const SqList &L, ElemType k, long long &count);
status SortList(SqList &L);
status SaveListToFile(const SqList &L, const string &fileName);
status LoadListFromFile(SqList &L, const string &fileName);
status CreateManagedList(ListManager &manager, const string &name);
status RemoveManagedList(ListManager &manager, const string &name);
status SwitchManagedList(ListManager &manager, const string &name);
void DestroyAllManagedLists(ListManager &manager);

static string GetText(HWND hWnd) {
    int len = GetWindowTextLengthA(hWnd);
    string text(len + 1, '\0');
    GetWindowTextA(hWnd, text.data(), len + 1);
    text.resize(len);
    return text;
}

static void SetText(HWND hWnd, const string &text) {
    SetWindowTextA(hWnd, text.c_str());
}

static void AppendLog(const string &msg) {
    int len = GetWindowTextLengthA(g_hLog);
    SendMessageA(g_hLog, EM_SETSEL, len, len);
    SendMessageA(g_hLog, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(msg.c_str()));
    SendMessageA(g_hLog, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>("\r\n"));
}

static int ReadInt(HWND hWnd, bool &ok) {
    string s = GetText(hWnd);
    if (s.empty()) {
        ok = false;
        return 0;
    }
    try {
        size_t idx = 0;
        int v = stoi(s, &idx);
        ok = (idx == s.size());
        return v;
    } catch (...) {
        ok = false;
        return 0;
    }
}

static SqList *GetCurrentList() {
    auto it = g_manager.lists.find(g_manager.currentName);
    if (it == g_manager.lists.end()) {
        return nullptr;
    }
    return it->second;
}

static void RefreshListBox() {
    SendMessageA(g_hListBox, LB_RESETCONTENT, 0, 0);
    SqList *cur = GetCurrentList();
    if (!cur || !cur->initialized) {
        SetText(g_hCurrent, "Current: (none)");
        return;
    }

    string label = "Current: " + g_manager.currentName + "   Length: " + to_string(cur->length);
    SetText(g_hCurrent, label);

    for (int i = 0; i < cur->length; ++i) {
        SendMessageA(g_hListBox, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(to_string(cur->elem[i]).c_str()));
    }
}

status InitList(SqList &L) {
    if (L.initialized) return ERROR;
    L.elem = static_cast<ElemType *>(malloc(LIST_INIT_SIZE * sizeof(ElemType)));
    if (!L.elem) return OVERFLOW;
    L.length = 0;
    L.listsize = LIST_INIT_SIZE;
    L.initialized = true;
    return OK;
}

status DestroyList(SqList &L) {
    if (!L.initialized) return INFEASIBLE;
    free(L.elem);
    L.elem = nullptr;
    L.length = 0;
    L.listsize = 0;
    L.initialized = false;
    return OK;
}

status ClearList(SqList &L) {
    if (!L.initialized) return INFEASIBLE;
    L.length = 0;
    return OK;
}

status ListEmpty(const SqList &L) {
    if (!L.initialized) return INFEASIBLE;
    return L.length == 0 ? TRUE : FALSE;
}

int ListLength(const SqList &L) {
    if (!L.initialized) return -1;
    return L.length;
}

status GetElem(const SqList &L, int i, ElemType &e) {
    if (!L.initialized) return INFEASIBLE;
    if (i < 1 || i > L.length) return ERROR;
    e = L.elem[i - 1];
    return OK;
}

int LocateElem(const SqList &L, ElemType e) {
    if (!L.initialized) return -1;
    for (int i = 0; i < L.length; ++i) {
        if (L.elem[i] == e) return i + 1;
    }
    return 0;
}

status PriorElem(const SqList &L, ElemType cur_e, ElemType &pre_e) {
    if (!L.initialized) return INFEASIBLE;
    for (int i = 1; i < L.length; ++i) {
        if (L.elem[i] == cur_e) {
            pre_e = L.elem[i - 1];
            return OK;
        }
    }
    return ERROR;
}

status NextElem(const SqList &L, ElemType cur_e, ElemType &next_e) {
    if (!L.initialized) return INFEASIBLE;
    for (int i = 0; i < L.length - 1; ++i) {
        if (L.elem[i] == cur_e) {
            next_e = L.elem[i + 1];
            return OK;
        }
    }
    return ERROR;
}

status ListInsert(SqList &L, int i, ElemType e) {
    if (!L.initialized) return INFEASIBLE;
    if (i < 1 || i > L.length + 1) return ERROR;
    if (L.length >= L.listsize) {
        int newSize = L.listsize + LISTINCREMENT;
        ElemType *newBase = static_cast<ElemType *>(realloc(L.elem, newSize * sizeof(ElemType)));
        if (!newBase) return OVERFLOW;
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
    if (!L.initialized) return INFEASIBLE;
    if (L.length == 0 || i < 1 || i > L.length) return ERROR;
    e = L.elem[i - 1];
    for (int j = i; j < L.length; ++j) {
        L.elem[j - 1] = L.elem[j];
    }
    --L.length;
    return OK;
}

status ListTraverse(const SqList &L, vector<ElemType> &out) {
    if (!L.initialized) return INFEASIBLE;
    out.clear();
    for (int i = 0; i < L.length; ++i) out.push_back(L.elem[i]);
    return OK;
}

status MaxSubArray(const SqList &L, ElemType &maxSum) {
    if (!L.initialized) return INFEASIBLE;
    if (L.length == 0) return ERROR;
    ElemType cur = L.elem[0];
    maxSum = L.elem[0];
    for (int i = 1; i < L.length; ++i) {
        cur = max(L.elem[i], cur + L.elem[i]);
        maxSum = max(maxSum, cur);
    }
    return OK;
}

status SubArrayNum(const SqList &L, ElemType k, long long &count) {
    if (!L.initialized) return INFEASIBLE;
    if (L.length == 0) return ERROR;
    unordered_map<long long, long long> prefixCount;
    prefixCount[0] = 1;
    long long prefix = 0;
    count = 0;
    for (int i = 0; i < L.length; ++i) {
        prefix += L.elem[i];
        auto it = prefixCount.find(prefix - k);
        if (it != prefixCount.end()) count += it->second;
        ++prefixCount[prefix];
    }
    return OK;
}

status SortList(SqList &L) {
    if (!L.initialized) return INFEASIBLE;
    sort(L.elem, L.elem + L.length);
    return OK;
}

status SaveListToFile(const SqList &L, const string &fileName) {
    if (!L.initialized) return INFEASIBLE;
    FILE *fp = nullptr;
    fopen_s(&fp, fileName.c_str(), "wb");
    if (!fp) return ERROR;
    fwrite("SQLIST1", 1, 7, fp);
    fwrite(&L.length, sizeof(int), 1, fp);
    fwrite(&L.listsize, sizeof(int), 1, fp);
    fwrite(L.elem, sizeof(ElemType), L.length, fp);
    fclose(fp);
    return OK;
}

status LoadListFromFile(SqList &L, const string &fileName) {
    FILE *fp = nullptr;
    fopen_s(&fp, fileName.c_str(), "rb");
    if (!fp) return ERROR;
    char sig[8] = {0};
    int length = 0, listsize = 0;
    fread(sig, 1, 7, fp);
    fread(&length, sizeof(int), 1, fp);
    fread(&listsize, sizeof(int), 1, fp);
    if (string(sig) != "SQLIST1" || length < 0 || listsize < length) {
        fclose(fp);
        return ERROR;
    }
    if (L.initialized) DestroyList(L);
    L.elem = static_cast<ElemType *>(malloc(listsize * sizeof(ElemType)));
    if (!L.elem) {
        fclose(fp);
        return OVERFLOW;
    }
    L.length = length;
    L.listsize = listsize;
    L.initialized = true;
    fread(L.elem, sizeof(ElemType), L.length, fp);
    fclose(fp);
    return OK;
}

status CreateManagedList(ListManager &manager, const string &name) {
    if (name.empty() || manager.lists.count(name)) return ERROR;
    auto *list = new SqList();
    status ret = InitList(*list);
    if (ret != OK) {
        delete list;
        return ret;
    }
    manager.lists[name] = list;
    if (manager.currentName.empty()) manager.currentName = name;
    return OK;
}

status RemoveManagedList(ListManager &manager, const string &name) {
    auto it = manager.lists.find(name);
    if (it == manager.lists.end()) return ERROR;
    DestroyList(*it->second);
    delete it->second;
    manager.lists.erase(it);
    if (manager.currentName == name) {
        manager.currentName = manager.lists.empty() ? "" : manager.lists.begin()->first;
    }
    return OK;
}

status SwitchManagedList(ListManager &manager, const string &name) {
    if (!manager.lists.count(name)) return ERROR;
    manager.currentName = name;
    return OK;
}

void DestroyAllManagedLists(ListManager &manager) {
    for (auto &kv : manager.lists) {
        DestroyList(*kv.second);
        delete kv.second;
    }
    manager.lists.clear();
    manager.currentName.clear();
}

static void UpdateView() {
    RefreshListBox();
}

static void DoAction(int id) {
    SqList *cur = GetCurrentList();
    bool ok = false;
    string text, name, file;
    int i = 0, val = 0;
    ElemType out = 0;
    long long cnt = 0;

    switch (id) {
    case ID_BTN_NEW:
        name = GetText(g_hName);
        if (CreateManagedList(g_manager, name) == OK) {
            AppendLog("Created list: " + name);
            UpdateView();
        } else {
            AppendLog("Create list failed.");
        }
        break;
    case ID_BTN_SWITCH:
        name = GetText(g_hName);
        if (SwitchManagedList(g_manager, name) == OK) {
            AppendLog("Switched to: " + name);
            UpdateView();
        } else {
            AppendLog("Switch failed.");
        }
        break;
    case ID_BTN_REMOVE:
        name = GetText(g_hName);
        if (RemoveManagedList(g_manager, name) == OK) {
            AppendLog("Removed list: " + name);
            UpdateView();
        } else {
            AppendLog("Remove failed.");
        }
        break;
    case ID_BTN_INIT:
        if (cur && InitList(*cur) == OK) AppendLog("InitList OK."); else AppendLog("InitList failed.");
        UpdateView();
        break;
    case ID_BTN_DESTROY:
        if (cur && DestroyList(*cur) == OK) AppendLog("DestroyList OK."); else AppendLog("DestroyList failed.");
        UpdateView();
        break;
    case ID_BTN_CLEAR:
        if (cur && ClearList(*cur) == OK) AppendLog("ClearList OK."); else AppendLog("ClearList failed.");
        UpdateView();
        break;
    case ID_BTN_INSERT:
        i = ReadInt(g_hIndex, ok);
        val = ReadInt(g_hValue, ok);
        if (cur && ok && ListInsert(*cur, i, val) == OK) AppendLog("Insert OK."); else AppendLog("Insert failed.");
        UpdateView();
        break;
    case ID_BTN_DELETE:
        i = ReadInt(g_hIndex, ok);
        if (cur && ok && ListDelete(*cur, i, out) == OK) {
            AppendLog("Delete OK, removed: " + to_string(out));
        } else {
            AppendLog("Delete failed.");
        }
        UpdateView();
        break;
    case ID_BTN_GET:
        i = ReadInt(g_hIndex, ok);
        if (cur && ok && GetElem(*cur, i, out) == OK) AppendLog("GetElem = " + to_string(out)); else AppendLog("GetElem failed.");
        break;
    case ID_BTN_LOCATE:
        val = ReadInt(g_hValue, ok);
        if (cur && ok) {
            int pos = LocateElem(*cur, val);
            AppendLog(pos > 0 ? ("Locate position = " + to_string(pos)) : "Not found.");
        } else {
            AppendLog("Locate failed.");
        }
        break;
    case ID_BTN_PRIOR:
        val = ReadInt(g_hValue, ok);
        if (cur && ok && PriorElem(*cur, val, out) == OK) AppendLog("Prior = " + to_string(out)); else AppendLog("Prior failed.");
        break;
    case ID_BTN_NEXT:
        val = ReadInt(g_hValue, ok);
        if (cur && ok && NextElem(*cur, val, out) == OK) AppendLog("Next = " + to_string(out)); else AppendLog("Next failed.");
        break;
    case ID_BTN_TRAVERSE: {
        vector<ElemType> vec;
        if (cur && ListTraverse(*cur, vec) == OK) {
            SendMessageA(g_hListBox, LB_RESETCONTENT, 0, 0);
            for (auto v : vec) SendMessageA(g_hListBox, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(to_string(v).c_str()));
            AppendLog("Traverse OK.");
            UpdateView();
        } else {
            AppendLog("Traverse failed.");
        }
        break;
    }
    case ID_BTN_SORT:
        if (cur && SortList(*cur) == OK) AppendLog("Sort OK."); else AppendLog("Sort failed.");
        UpdateView();
        break;
    case ID_BTN_MAXSUM:
        if (cur && MaxSubArray(*cur, out) == OK) AppendLog("MaxSubArray = " + to_string(out)); else AppendLog("MaxSubArray failed.");
        break;
    case ID_BTN_SUBCNT:
        val = ReadInt(g_hK, ok);
        if (cur && ok && SubArrayNum(*cur, val, cnt) == OK) AppendLog("Count = " + to_string(cnt)); else AppendLog("SubArrayNum failed.");
        break;
    case ID_BTN_SAVE:
        file = GetText(g_hFile);
        if (cur && SaveListToFile(*cur, file) == OK) AppendLog("Saved to: " + file); else AppendLog("Save failed.");
        break;
    case ID_BTN_LOAD:
        file = GetText(g_hFile);
        if (cur && LoadListFromFile(*cur, file) == OK) {
            AppendLog("Loaded from: " + file);
            UpdateView();
        } else {
            AppendLog("Load failed.");
        }
        break;
    case ID_BTN_EMPTY:
        if (cur) AppendLog(ListEmpty(*cur) == TRUE ? "List is empty." : "List is not empty."); else AppendLog("No current list.");
        break;
    case ID_BTN_LENGTH:
        if (cur) AppendLog("Length = " + to_string(ListLength(*cur))); else AppendLog("No current list.");
        break;
    case ID_BTN_SHOWALL:
        AppendLog("Managed lists:");
        for (const auto &kv : g_manager.lists) {
            AppendLog(string("- ") + kv.first + (kv.first == g_manager.currentName ? " [current]" : ""));
        }
        if (g_manager.lists.empty()) AppendLog("(none)");
        break;
    default:
        break;
    }
}

static void CreateButton(HWND parent, const char *text, int x, int y, int w, int h, int id) {
    CreateWindowA("BUTTON", text, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                  x, y, w, h, parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
                  GetModuleHandleA(nullptr), nullptr);
}

static void CreateLabel(HWND parent, const char *text, int x, int y, int w, int h) {
    CreateWindowA("STATIC", text, WS_CHILD | WS_VISIBLE, x, y, w, h, parent, nullptr,
                  GetModuleHandleA(nullptr), nullptr);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        CreateLabel(hWnd, "Name", 16, 16, 50, 20);
        g_hName = CreateWindowA("EDIT", "default", WS_CHILD | WS_VISIBLE | WS_BORDER, 70, 12, 120, 24, hWnd,
                                reinterpret_cast<HMENU>(ID_NAME), GetModuleHandleA(nullptr), nullptr);
        CreateLabel(hWnd, "Index", 16, 48, 50, 20);
        g_hIndex = CreateWindowA("EDIT", "1", WS_CHILD | WS_VISIBLE | WS_BORDER, 70, 44, 120, 24, hWnd,
                                 reinterpret_cast<HMENU>(ID_INDEX), GetModuleHandleA(nullptr), nullptr);
        CreateLabel(hWnd, "Value", 16, 80, 50, 20);
        g_hValue = CreateWindowA("EDIT", "0", WS_CHILD | WS_VISIBLE | WS_BORDER, 70, 76, 120, 24, hWnd,
                                 reinterpret_cast<HMENU>(ID_VALUE), GetModuleHandleA(nullptr), nullptr);
        CreateLabel(hWnd, "File", 16, 112, 50, 20);
        g_hFile = CreateWindowA("EDIT", "seq.bin", WS_CHILD | WS_VISIBLE | WS_BORDER, 70, 108, 120, 24, hWnd,
                                reinterpret_cast<HMENU>(ID_FILE), GetModuleHandleA(nullptr), nullptr);
        CreateLabel(hWnd, "K", 16, 144, 50, 20);
        g_hK = CreateWindowA("EDIT", "3", WS_CHILD | WS_VISIBLE | WS_BORDER, 70, 140, 120, 24, hWnd,
                             reinterpret_cast<HMENU>(ID_K), GetModuleHandleA(nullptr), nullptr);
        g_hCurrent = CreateWindowA("STATIC", "Current: default", WS_CHILD | WS_VISIBLE, 16, 176, 300, 20, hWnd, nullptr,
                                   GetModuleHandleA(nullptr), nullptr);

        int bx = 16, by = 208, bw = 110, bh = 28, gap = 8;
        CreateButton(hWnd, "NewList", bx, by, bw, bh, ID_BTN_NEW);
        CreateButton(hWnd, "Switch", bx + bw + gap, by, bw, bh, ID_BTN_SWITCH);
        CreateButton(hWnd, "Remove", bx + (bw + gap) * 2, by, bw, bh, ID_BTN_REMOVE);
        CreateButton(hWnd, "Init", bx + (bw + gap) * 3, by, bw, bh, ID_BTN_INIT);
        CreateButton(hWnd, "Destroy", bx + (bw + gap) * 4, by, bw, bh, ID_BTN_DESTROY);

        by += 36;
        CreateButton(hWnd, "Clear", bx, by, bw, bh, ID_BTN_CLEAR);
        CreateButton(hWnd, "Insert", bx + bw + gap, by, bw, bh, ID_BTN_INSERT);
        CreateButton(hWnd, "Delete", bx + (bw + gap) * 2, by, bw, bh, ID_BTN_DELETE);
        CreateButton(hWnd, "Get", bx + (bw + gap) * 3, by, bw, bh, ID_BTN_GET);
        CreateButton(hWnd, "Locate", bx + (bw + gap) * 4, by, bw, bh, ID_BTN_LOCATE);

        by += 36;
        CreateButton(hWnd, "Prior", bx, by, bw, bh, ID_BTN_PRIOR);
        CreateButton(hWnd, "Next", bx + bw + gap, by, bw, bh, ID_BTN_NEXT);
        CreateButton(hWnd, "Traverse", bx + (bw + gap) * 2, by, bw, bh, ID_BTN_TRAVERSE);
        CreateButton(hWnd, "Sort", bx + (bw + gap) * 3, by, bw, bh, ID_BTN_SORT);
        CreateButton(hWnd, "MaxSum", bx + (bw + gap) * 4, by, bw, bh, ID_BTN_MAXSUM);

        by += 36;
        CreateButton(hWnd, "SubCnt", bx, by, bw, bh, ID_BTN_SUBCNT);
        CreateButton(hWnd, "Save", bx + bw + gap, by, bw, bh, ID_BTN_SAVE);
        CreateButton(hWnd, "Load", bx + (bw + gap) * 2, by, bw, bh, ID_BTN_LOAD);
        CreateButton(hWnd, "Empty?", bx + (bw + gap) * 3, by, bw, bh, ID_BTN_EMPTY);
        CreateButton(hWnd, "Length", bx + (bw + gap) * 4, by, bw, bh, ID_BTN_LENGTH);

        by += 36;
        CreateButton(hWnd, "ShowAll", bx, by, bw, bh, ID_BTN_SHOWALL);

        g_hListBox = CreateWindowA("LISTBOX", nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | WS_VSCROLL,
                                   560, 12, 220, 220, hWnd, nullptr, GetModuleHandleA(nullptr), nullptr);
        g_hLog = CreateWindowA("EDIT", nullptr,
                               WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL |
                                   ES_READONLY | WS_VSCROLL,
                               16, 420, 764, 160, hWnd, nullptr, GetModuleHandleA(nullptr), nullptr);

        CreateManagedList(g_manager, "default");
        g_manager.currentName = "default";
        RefreshListBox();
        AppendLog("Ready.");
        return 0;
    }
    case WM_COMMAND:
        DoAction(LOWORD(wParam));
        return 0;
    case WM_DESTROY:
        DestroyAllManagedLists(g_manager);
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcA(hWnd, msg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const char CLASS_NAME[] = "SeqListFrontendWindow";
    WNDCLASSA wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    RegisterClassA(&wc);

    HWND hWnd = CreateWindowA(CLASS_NAME, "Sequential List GUI", WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, CW_USEDEFAULT, 820, 640,
                              nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) {
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessageA(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return static_cast<int>(msg.wParam);
}