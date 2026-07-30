#include <iostream>
#include <random>
#include <fstream>
#include <vector>

using namespace std;


static std::random_device rd;
static std::mt19937 gen(rd());

int NumGen() {
    std::uniform_int_distribution<> distr(1, 100);
    int prob = distr(gen);
    if (prob <= 35) {
        return 1;
    } else {
        return 0;
    }
}

int TileCounter(const vector<vector<int>>& map, int i, int j,int ID) {
    int Tile_Count = 0;
    
    for (int y = i - 1; y <= i + 1; y++) {
        for (int x = j - 1; x <= j + 1; x++) {
            if (y == i && x == j) continue;

			if (y < 0 || y >= 65 || x < 0 || x >= 65) {
                if (ID == 1) {
                    Tile_Count++;
                }
            }
			else if (map[y][x] == ID) {
                Tile_Count++;
            }
        }
    }
    return Tile_Count;
}


bool IsNearSpawn(int x, int y, const vector<pair<int, int>>& spawns) {
    int safeRadius = 3;
    for (const auto& spawn : spawns) {
        int dx = x - spawn.first;
        int dy = y - spawn.second;
        if ((dx * dx) + (dy * dy) <= (safeRadius * safeRadius)) {
            return true;
        }
    }
    return false;
}

int main() {
    vector<pair<int, int>> spawn_point;
    spawn_point.emplace_back(10, 32);
    spawn_point.emplace_back(54, 32);
    spawn_point.emplace_back(21, 13);
    spawn_point.emplace_back(43, 13);
    spawn_point.emplace_back(21, 51);
    spawn_point.emplace_back(43, 51);

    ofstream fout;
    int number = 3;
    string file_path = "../resources/Map/map" + to_string(number) + ".txt";
    fout.open(file_path);

    vector<vector<int>> map(65, vector<int>(65, 0));
    vector<vector<int>> temp_map(65, vector<int>(65, 0));

    for (size_t i = 0; i < 65; i++) {
        for (size_t j = 0; j < 65; j++) {
            if (IsNearSpawn(j, i, spawn_point)) {
                map[i][j] = 0;
            } else {
                map[i][j] = NumGen();
            }
        }
    }

    for (int p = 0; p < 4; p++) {
        for (size_t i = 0; i < 65; i++) {
            for (size_t j = 0; j < 65; j++) {
                
                if (IsNearSpawn(j, i, spawn_point)) {
                    temp_map[i][j] = 0;
                } else {
                    if (TileCounter(map, i, j, 1) >= 4) {
                        temp_map[i][j] = 1;
                    } else {
                        temp_map[i][j] = 0;
                    }
                }
                
            }
        }
        map = temp_map; 
    }

    for (size_t i = 0; i < 65; i++) {
        for (size_t j = 0; j < 65; j++) {
            if (map[i][j] == 0 && !IsNearSpawn(j, i, spawn_point)) {
                std::uniform_int_distribution<> distr(1, 100);
                int prob = distr(gen);
                
                if (prob >= 10 && prob <= 30) {
                    map[i][j] = 2; 
                } 
                else if (prob > 30 && prob <= 40) {
                    map[i][j] = 3; 
                }
            }
        }
    }

    int curr_ID = 2;
    for(size_t k = 0; k < 3; k++){
        temp_map = map;
        for(size_t i = 0; i < 65; i++){
            for(size_t j = 0; j < 65; j++){
                
                if (IsNearSpawn(j, i, spawn_point)) {
                    temp_map[i][j] = 0;
                }
                else if(map[i][j] == 0 || map[i][j] == curr_ID){                
                    if(TileCounter(map, i, j, curr_ID) >= 4){
                        temp_map[i][j] = curr_ID;
                    }
                    else if(TileCounter(map, i, j, curr_ID) < 2){
                        temp_map[i][j] = 0;
                    }
                }       
            }
        }
        map = temp_map;
    }   

    curr_ID = 3;
    for(size_t k = 0; k < 3; k++){
        temp_map = map;
        for(size_t i = 0; i < 65; i++){
            for(size_t j = 0; j < 65; j++){
                
                if (IsNearSpawn(j, i, spawn_point)) {
                    temp_map[i][j] = 0;
                }
                else if(map[i][j] == 0 || map[i][j] == curr_ID){                
                    if(TileCounter(map, i, j, curr_ID) >= 4){
                        temp_map[i][j] = curr_ID;
                    }
                    else if(TileCounter(map, i, j, curr_ID) < 2){
                        temp_map[i][j] = 0;
                    }
                }       
            }
        }
        map = temp_map;
    }   

    for (size_t i = 0; i < 65; i++) {
        for (size_t j = 0; j < 65; j++) {
            if (i == 0 || j == 0 || i == 64 || j == 64) {
                fout << 1 << (j == 64 ? "" : " ");
            } else {
                fout << map[i][j] << (j == 64 ? "" : " ");
            }
        }
        fout << "\n";
    }
    
    fout.close();
    return 0;
}