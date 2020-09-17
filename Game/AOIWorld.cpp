#include "AOIWorld.h"

using namespace std;

//通过构造函数指定矩形的大小和分割粒度
AOIWorld::AOIWorld(int _x_begin, int _x_end, int _y_begin, int _y_end,
                   int _x_count, int _y_count) : x_begin(_x_begin),
                                                 x_end(_x_end),
                                                 y_begin(_y_begin),
                                                 y_end(_y_end),
                                                 x_count(_x_count),
                                                 y_count(_y_count)
{
    //计算每个网格宽度
    x_width = (x_end - x_begin) / x_count;
    y_width = (y_end - y_begin) / y_count;

    //创建格子们
    for (int i = 0; i < x_count * y_count; i++)
    {
        Grid tmp;
        m_grids.push_back(tmp);
    }
}

AOIWorld::~AOIWorld()
{
}

//获取周围玩家
std::list<Player *> AOIWorld::GetSrdPlayers(Player *_player)
{
    list<Player *> ret;
    //计算所属编号
    int grid_id = (_player->GetX() - x_begin) / x_width + (_player->GetY()) / y_width * x_count;
    //判断具体情况，取出邻居网格的玩家们

    //计算当前网格横着数和纵着数是第几个
    int x_index = grid_id % x_count;
    int y_index = grid_id / x_count;

    if (x_index > 0 && y_index > 0)
    {
        auto &cur_list = m_grids[grid_id - 1 - x_count].m_players;
        ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
    }
    if (y_index > 0)
    {
        auto &cur_list = m_grids[grid_id - x_count].m_players;
        ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
    }
    if (x_index < x_count - 1 && y_index > 0)
    {
        auto &cur_list = m_grids[grid_id - x_count + 1].m_players;
        ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
    }
    if (x_index > 0)
    {
        auto &cur_list = m_grids[grid_id - 1].m_players;
        ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
    }
    auto &cur_list = m_grids[grid_id].m_players;
    ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
    if (x_index < x_count - 1)
    {
        auto &cur_list = m_grids[grid_id + 1].m_players;
        ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
    }
    if (x_index > 0 && y_index < y_count - 1)
    {
        auto &cur_list = m_grids[grid_id - 1 + x_count].m_players;
        ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
    }
    if (y_index < y_count - 1)
    {
        auto &cur_list = m_grids[grid_id + x_count].m_players;
        ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
    }
    if (x_index < x_count - 1 && y_index < y_count - 1)
    {
        auto &cur_list = m_grids[grid_id + 1 + x_count].m_players;
        ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
    }

    return ret;
}

//添加玩家到AOI网格
bool AOIWorld::AddPlayer(Player *_player)
{
    //计算所属网格号
    int grid_id = (_player->GetX() - x_begin) / x_width + (_player->GetY()) / y_width * x_count;

    //添加到该网格中
    m_grids[grid_id].m_players.push_back(_player);
    return true;
}

//摘除玩家
void AOIWorld::DelPlayer(Player *_player)
{
    int grid_id = (_player->GetX() - x_begin) / x_width + (_player->GetY()) / y_width * x_count;
    m_grids[grid_id].m_players.remove(_player);
}