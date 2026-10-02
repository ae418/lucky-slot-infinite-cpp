#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <fstream>
#include <windows.h>
#include <set>
#include <ctime>
using namespace std;

enum Rarity { COMMON, UNCOMMON, RARE, EPIC, LEGENDARY };
struct Item { string name; Rarity rarity; long long price; };

long long coins = 500;
long long jackpot = 10000;
int pity_epic = 0;
int pity_legendary = 0;
mt19937 rng(time(0));
vector<Item> bag;
set<string> collected;

string rarity_name(Rarity r) {
    switch(r) {
        case COMMON: return "普通";
        case UNCOMMON: return "罕见";
        case RARE: return "稀有";
        case EPIC: return "史诗";
        case LEGENDARY: return "传说";
    }
    return "";
}

int rarity_color(Rarity r) {
    switch(r) {
        case COMMON: return 8;
        case UNCOMMON: return 2;
        case RARE: return 9;
        case EPIC: return 5;
        case LEGENDARY: return 14;
    }
    return 7;
}

Rarity roll_rarity(int box_cost) {
    if(pity_legendary >= 200) return LEGENDARY;
    if(pity_epic >= 80) return EPIC;
    double roll = uniform_real_distribution<double>(0,1)(rng);
    double leg_p = box_cost==200 ? 0.08 : (box_cost==50 ? 0.03 : 0.01);
    if(roll < leg_p) return LEGENDARY;
    if(roll < leg_p + 0.07) return EPIC;
    if(roll < leg_p + 0.07 + 0.2) return RARE;
    if(roll < leg_p + 0.07 + 0.2 + 0.3) return UNCOMMON;
    return COMMON;
}

Item roll_item(Rarity r) {
    static vector<string> names[5] = {
        {"青铜匕首","铁枪","皮手套","木盾","陶碗"},
        {"银军刀","迷彩步枪","重护甲","皮靴","银戒指"},
        {"黄金沙鹰","屠龙刀","暗影匕首","冰晶法杖","狙击枪"},
        {"龙狙","深红之网","死神镰刀","凤凰权杖","冰霜剑"},
        {"龙纹传说","永恒之枪","世界之巅","神之庇护","屠龙神话"}
    };
    int idx = uniform_int_distribution<int>(0, names[r].size()-1)(rng);
    Item it;
    it.name = names[r][idx];
    it.rarity = r;
    it.price = 10 * (1 << r) * (1 + idx);
    return it;
}

void do_spin() {
    cout << "\n选择宝箱：1.普通箱(10金币) 2.高级箱(50金币) 3.至尊箱(200金币) > ";
    int c; cin >> c;
    int cost = c==1?10:(c==2?50:200);
    if(coins < cost) { cout << "金币不够！" << endl; Sleep(1000); return; }
    coins -= cost;
    jackpot += cost * 0.3;
    pity_epic++; pity_legendary++;
    
    Rarity r = roll_rarity(cost);
    if(r == EPIC) pity_epic = 0;
    if(r == LEGENDARY) { pity_legendary = 0; coins += jackpot; jackpot = 10000; }
    
    Item it = roll_item(r);
    bag.push_back(it);
    collected.insert(it.name);
    
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), rarity_color(r));
    cout << "\n🎉 你抽中了：" << it.name << " [" << rarity_name(r) << "] 价值" << it.price << "金币" << endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << "当前金币：" << coins << "  |  公共奖池：" << jackpot << endl;
    Sleep(1500);
}

void show_bag() {
    cout << "\n===== 背包（共" << bag.size() << "个道具）=====" << endl;
    for(size_t i=0;i<bag.size();i++) {
        cout << i+1 << ". " << bag[i].name << " [" << rarity_name(bag[i].rarity) << "] 价值" << bag[i].price << endl;
    }
    cout << "输入序号卖出道具，输入0返回 > ";
    int s; cin >> s;
    if(s>0 && (size_t)s <= bag.size()) {
        coins += bag[s-1].price;
        cout << "卖出成功，获得" << bag[s-1].price << "金币" << endl;
        bag.erase(bag.begin()+s-1);
        Sleep(1000);
    }
}

void save() {
    ofstream f("save.ini");
    f << coins << endl << jackpot << endl << pity_epic << endl << pity_legendary << endl;
}

int main() {
    SetConsoleOutputCP(936);
    ifstream f("save.ini");
    if(f.is_open()) f >> coins >> jackpot >> pity_epic >> pity_legendary;
    
    while(true) {
        system("cls");
        cout << "╔════════════════════════╗" << endl;
        cout << "║  幸运开箱·无尽版  v1.0  ║" << endl;
        cout << "╚════════════════════════╝" << endl;
        cout << "💰 金币：" << coins << "  |  🎰 奖池：" << jackpot << endl;
        cout << "1. 开箱抽奖\n2. 背包卖道具\n3. 存档退出\n> ";
        int op; cin >> op;
        if(op == 1) do_spin();
        else if(op == 2) show_bag();
        else if(op == 3) { save(); cout << "已保存存档，下次继续玩！" << endl; break; }
    }
    return 0;
}